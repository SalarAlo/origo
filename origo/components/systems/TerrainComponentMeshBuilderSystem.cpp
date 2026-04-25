#include "TerrainComponentMeshBuilderSystem.h"

#include <cmath>
#include <random>
#include <thread>

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/MeshData.h"
#include "origo/assets/Mesh.h"

#include "origo/components/TerrainComponent.h"

#include "origo/core/Logger.h"
#include "origo/core/RID.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayout.h"
#include "origo/renderer/VertexLayoutRegistry.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {
struct HeightSample {
	float Height = 0.0f;
	glm::vec2 Gradient = glm::vec2(0.0f);
};

static glm::vec3 read_position_from_mesh_data(const MeshData& mesh, size_t vertex_index, size_t stride) {
	const size_t offset = vertex_index * stride;
	return { mesh.Vertices[offset + 0], mesh.Vertices[offset + 1], mesh.Vertices[offset + 2] };
}

static void apply_mesh_bounds(AssetHandle handle, const MeshData& mesh, size_t stride) {
	if (mesh.Vertices.empty() || stride < 3)
		return;

	glm::vec3 min_bounds = read_position_from_mesh_data(mesh, 0, stride);
	glm::vec3 max_bounds = min_bounds;

	for (size_t vertex_index = 1; vertex_index < mesh.Vertices.size() / stride; ++vertex_index) {
		const glm::vec3 position = read_position_from_mesh_data(mesh, vertex_index, stride);
		min_bounds = glm::min(min_bounds, position);
		max_bounds = glm::max(max_bounds, position);
	}

	if (auto* mesh_asset = AssetManager::get_instance().get_asset<Mesh>(handle))
		mesh_asset->set_bounds(min_bounds, max_bounds);
}

static void release_terrain_mesh(OptionalAssetHandle& mesh_handle) {
	if (!mesh_handle.has_value())
		return;

	auto& asset_manager = AssetManager::get_instance();
	auto* mesh = asset_manager.get_asset<Mesh>(*mesh_handle);
	if (mesh) {
		if (auto* layout = VertexLayoutRegistry::get(mesh->LayoutId)) {
			GeometryHeapRegistry::free_mesh_range(
			    mesh->HeapId,
			    mesh->Range,
			    layout->get_stride());
		}
	}

	asset_manager.destroy(*mesh_handle);
	mesh_handle.reset();
}

static float hash_noise_2d(int x, int z) {
	const float value = std::sin(static_cast<float>(x) * 12.9898f + static_cast<float>(z) * 78.233f) * 43758.5453f;
	return glm::fract(value);
}

static float remap_noise(float n) {
	return glm::clamp(n * 0.5f + 0.5f, 0.0f, 1.0f);
}

static float build_base_height(
    const TerrainComponent& terrain_component,
    const Noise::Generator& base_generator,
    const Noise::Generator& warp_generator_x,
    const Noise::Generator& warp_generator_z,
    const Noise::Generator& medium_generator,
    const Noise::Generator& ridge_generator,
    const Noise::Generator& fine_generator,
    int x,
    int z) {
	const float sample_x = static_cast<float>(x) * terrain_component.scale;
	const float sample_z = static_cast<float>(z) * terrain_component.scale;

	const float warp_strength = terrain_component.shape_settings.domain_warp_strength * terrain_component.scale;
	const float warped_x = sample_x + warp_generator_x.sample_2d(sample_x * 0.35f, sample_z * 0.35f) * warp_strength;
	const float warped_z = sample_z + warp_generator_z.sample_2d(sample_x * 0.35f, sample_z * 0.35f) * warp_strength;

	const float base = remap_noise(base_generator.sample_2d(warped_x, warped_z));
	const float medium = remap_noise(medium_generator.sample_2d(
	    warped_x * terrain_component.shape_settings.medium_frequency_multiplier,
	    warped_z * terrain_component.shape_settings.medium_frequency_multiplier));
	const float ridge = 1.0f - std::abs(ridge_generator.sample_2d(
	    warped_x * terrain_component.shape_settings.ridge_frequency_multiplier,
	    warped_z * terrain_component.shape_settings.ridge_frequency_multiplier));
	const float fine = remap_noise(fine_generator.sample_2d(
	    warped_x * terrain_component.shape_settings.fine_frequency_multiplier,
	    warped_z * terrain_component.shape_settings.fine_frequency_multiplier));

	const float mountain_mask = glm::smoothstep(0.38f, 0.82f, base);
	const float basin_mask = 1.0f - glm::smoothstep(0.24f, 0.56f, base);

	float height = base * terrain_component.shape_settings.base_strength;
	height += medium * terrain_component.shape_settings.medium_detail_strength;
	height += ridge * terrain_component.shape_settings.ridge_strength * mountain_mask;
	height += (fine - 0.5f) * terrain_component.shape_settings.detail_strength * glm::mix(0.45f, 1.0f, mountain_mask);
	height = glm::mix(height * 0.68f, height, 1.0f - basin_mask * 0.45f);
	height = glm::smoothstep(
	    terrain_component.shape_settings.remap_min,
	    terrain_component.shape_settings.remap_max,
	    glm::clamp(height, 0.0f, 1.0f));
	height = glm::pow(height, terrain_component.contrast);
	return glm::clamp(height, 0.0f, 1.0f);
}

static HeightSample sample_height_and_gradient(
    const std::vector<float>& height_map,
    int size_x,
    int size_z,
    float x,
    float z) {
	x = std::clamp(x, 0.0f, static_cast<float>(size_x - 1));
	z = std::clamp(z, 0.0f, static_cast<float>(size_z - 1));

	const int x0 = std::min(static_cast<int>(x), size_x - 2);
	const int z0 = std::min(static_cast<int>(z), size_z - 2);
	const int x1 = x0 + 1;
	const int z1 = z0 + 1;

	const float tx = x - static_cast<float>(x0);
	const float tz = z - static_cast<float>(z0);

	const float h00 = height_map[z0 * size_x + x0];
	const float h10 = height_map[z0 * size_x + x1];
	const float h01 = height_map[z1 * size_x + x0];
	const float h11 = height_map[z1 * size_x + x1];

	const float height0 = glm::mix(h00, h10, tx);
	const float height1 = glm::mix(h01, h11, tx);

	HeightSample sample {};
	sample.Height = glm::mix(height0, height1, tz);
	sample.Gradient.x = (h10 - h00) * (1.0f - tz) + (h11 - h01) * tz;
	sample.Gradient.y = (h01 - h00) * (1.0f - tx) + (h11 - h10) * tx;
	return sample;
}

static void deposit_height(
    std::vector<float>& height_map,
    int size_x,
    int size_z,
    float x,
    float z,
    float amount) {
	if (amount <= 0.0f)
		return;

	x = std::clamp(x, 0.0f, static_cast<float>(size_x - 1));
	z = std::clamp(z, 0.0f, static_cast<float>(size_z - 1));

	const int x0 = std::min(static_cast<int>(x), size_x - 2);
	const int z0 = std::min(static_cast<int>(z), size_z - 2);
	const int x1 = x0 + 1;
	const int z1 = z0 + 1;

	const float tx = x - static_cast<float>(x0);
	const float tz = z - static_cast<float>(z0);

	height_map[z0 * size_x + x0] += amount * (1.0f - tx) * (1.0f - tz);
	height_map[z0 * size_x + x1] += amount * tx * (1.0f - tz);
	height_map[z1 * size_x + x0] += amount * (1.0f - tx) * tz;
	height_map[z1 * size_x + x1] += amount * tx * tz;
}

static float erode_height(
    std::vector<float>& height_map,
    int size_x,
    int size_z,
    float x,
    float z,
    float amount) {
	if (amount <= 0.0f)
		return 0.0f;

	x = std::clamp(x, 0.0f, static_cast<float>(size_x - 1));
	z = std::clamp(z, 0.0f, static_cast<float>(size_z - 1));

	const int x0 = std::min(static_cast<int>(x), size_x - 2);
	const int z0 = std::min(static_cast<int>(z), size_z - 2);
	const int x1 = x0 + 1;
	const int z1 = z0 + 1;

	const float tx = x - static_cast<float>(x0);
	const float tz = z - static_cast<float>(z0);

	struct WeightedCell {
		int Index;
		float Weight;
	};

	const WeightedCell cells[] = {
		{ z0 * size_x + x0, (1.0f - tx) * (1.0f - tz) },
		{ z0 * size_x + x1, tx * (1.0f - tz) },
		{ z1 * size_x + x0, (1.0f - tx) * tz },
		{ z1 * size_x + x1, tx * tz },
	};

	float removed = 0.0f;
	for (const auto& cell : cells) {
		if (cell.Weight <= 0.0f)
			continue;

		const float requested = amount * cell.Weight;
		const float delta = std::min(requested, height_map[cell.Index]);
		height_map[cell.Index] -= delta;
		removed += delta;
	}

	return removed;
}

static void apply_erosion(
    std::vector<float>& height_map,
    int size_x,
    int size_z,
    const ErosionSettings& settings) {
	if (settings.droplet_count <= 0 || size_x < 2 || size_z < 2)
		return;

	std::mt19937 rng(static_cast<uint32_t>(settings.seed));
	std::uniform_real_distribution<float> x_dist(0.0f, static_cast<float>(size_x - 1));
	std::uniform_real_distribution<float> z_dist(0.0f, static_cast<float>(size_z - 1));

	for (int droplet = 0; droplet < settings.droplet_count; ++droplet) {
		glm::vec2 position(x_dist(rng), z_dist(rng));
		glm::vec2 direction(0.0f);
		float speed = 1.0f;
		float water = 1.0f;
		float sediment = 0.0f;

		for (int lifetime = 0; lifetime < settings.max_lifetime; ++lifetime) {
			const HeightSample sample = sample_height_and_gradient(height_map, size_x, size_z, position.x, position.y);

			direction = direction * settings.inertia - sample.Gradient * (1.0f - settings.inertia);
			if (glm::dot(direction, direction) < 1e-6f) {
				std::uniform_real_distribution<float> dir_dist(-1.0f, 1.0f);
				direction = glm::normalize(glm::vec2(dir_dist(rng), dir_dist(rng)));
			} else {
				direction = glm::normalize(direction);
			}

			const glm::vec2 next_position = position + direction;
			if (next_position.x < 0.0f || next_position.x >= static_cast<float>(size_x - 1)
			    || next_position.y < 0.0f || next_position.y >= static_cast<float>(size_z - 1))
				break;

			const HeightSample next_sample = sample_height_and_gradient(height_map, size_x, size_z, next_position.x, next_position.y);
			const float delta_height = next_sample.Height - sample.Height;

			const float sediment_capacity = std::max(
			    -delta_height * speed * water * settings.sediment_capacity_factor,
			    settings.min_sediment_capacity);

			if (delta_height > 0.0f || sediment > sediment_capacity) {
				const float deposit_amount = delta_height > 0.0f
				    ? std::min(sediment, delta_height)
				    : (sediment - sediment_capacity) * settings.deposit_speed;
				deposit_height(height_map, size_x, size_z, position.x, position.y, deposit_amount);
				sediment -= deposit_amount;
			} else {
				const float erode_amount = std::min(
				    (sediment_capacity - sediment) * settings.erode_speed,
				    -delta_height);
				const float removed = erode_height(height_map, size_x, size_z, position.x, position.y, erode_amount);
				sediment += removed;
			}

			speed = std::sqrt(std::max(0.0f, speed * speed - delta_height * settings.gravity));
			water *= (1.0f - settings.evaporate_speed);
			position = next_position;

			if (water <= 0.01f)
				break;
		}
	}
}

static void normalize_height_map(std::vector<float>& height_map) {
	if (height_map.empty())
		return;

	const auto [min_it, max_it] = std::minmax_element(height_map.begin(), height_map.end());
	const float min_height = *min_it;
	const float max_height = *max_it;
	const float span = max_height - min_height;

	if (span <= 1e-6f) {
		std::fill(height_map.begin(), height_map.end(), 0.0f);
		return;
	}

	for (float& sample : height_map)
		sample = (sample - min_height) / span;
}

static void append_voxel_face(
    MeshData& mesh,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c,
    const glm::vec3& d,
    const glm::vec3& normal,
    float u_scale,
    float v_scale) {
	const unsigned int base_index = static_cast<unsigned int>(mesh.Vertices.size() / 8);

	const auto append_vertex = [&](const glm::vec3& position, float u, float v) {
		mesh.Vertices.push_back(position.x);
		mesh.Vertices.push_back(position.y);
		mesh.Vertices.push_back(position.z);
		mesh.Vertices.push_back(normal.x);
		mesh.Vertices.push_back(normal.y);
		mesh.Vertices.push_back(normal.z);
		mesh.Vertices.push_back(u);
		mesh.Vertices.push_back(v);
	};

	append_vertex(a, 0.0f, 0.0f);
	append_vertex(b, u_scale, 0.0f);
	append_vertex(c, 0.0f, v_scale);
	append_vertex(d, u_scale, v_scale);

	const glm::vec3 triangle_normal = glm::cross(b - a, c - a);
	if (glm::dot(triangle_normal, normal) >= 0.0f) {
		mesh.Indices.push_back(base_index + 0);
		mesh.Indices.push_back(base_index + 1);
		mesh.Indices.push_back(base_index + 2);
		mesh.Indices.push_back(base_index + 1);
		mesh.Indices.push_back(base_index + 3);
		mesh.Indices.push_back(base_index + 2);
	} else {
		mesh.Indices.push_back(base_index + 0);
		mesh.Indices.push_back(base_index + 2);
		mesh.Indices.push_back(base_index + 1);
		mesh.Indices.push_back(base_index + 1);
		mesh.Indices.push_back(base_index + 2);
		mesh.Indices.push_back(base_index + 3);
	}
}

static int get_voxel_height(
    const std::vector<int>& voxel_heights,
    int size_x,
    int size_z,
    int x,
    int z) {
	if (x < 0 || x >= size_x || z < 0 || z >= size_z)
		return 0;

	return voxel_heights[z * size_x + x];
}

static MeshData construct_voxel_mesh_data(
    const std::vector<float>& height_map,
    int size_x,
    int size_z,
    float cell_size) {
	MeshData mesh {};
	if (height_map.empty())
		return mesh;

	const float fine_unit = std::max(cell_size, 1e-5f);
	const float target_voxel_size = std::max(1.0f, fine_unit);
	const int voxel_footprint = std::max(1, static_cast<int>(std::round(target_voxel_size / fine_unit)));
	const float unit = fine_unit * static_cast<float>(voxel_footprint);
	const int voxel_size_x = std::max(1, (size_x + voxel_footprint - 1) / voxel_footprint);
	const int voxel_size_z = std::max(1, (size_z + voxel_footprint - 1) / voxel_footprint);

	std::vector<int> voxel_heights(voxel_size_x * voxel_size_z, 0);
	for (int vz = 0; vz < voxel_size_z; ++vz) {
		for (int vx = 0; vx < voxel_size_x; ++vx) {
			float accumulated_height = 0.0f;
			int sample_count = 0;

			for (int dz = 0; dz < voxel_footprint; ++dz) {
				const int source_z = vz * voxel_footprint + dz;
				if (source_z >= size_z)
					break;

				for (int dx = 0; dx < voxel_footprint; ++dx) {
					const int source_x = vx * voxel_footprint + dx;
					if (source_x >= size_x)
						break;

					accumulated_height += height_map[source_z * size_x + source_x];
					++sample_count;
				}
			}

			if (sample_count == 0)
				continue;

			const float averaged_height = accumulated_height / static_cast<float>(sample_count);
			voxel_heights[vz * voxel_size_x + vx] = std::max(0, static_cast<int>(std::round(averaged_height / unit)));
		}
	}

	std::vector<uint8_t> top_visited(voxel_size_x * voxel_size_z, 0);

	for (int z = 0; z < voxel_size_z; ++z) {
		for (int x = 0; x < voxel_size_x; ++x) {
			const int index = z * voxel_size_x + x;
			const int column_height = voxel_heights[index];
			if (column_height <= 0 || top_visited[index])
				continue;

			int width = 1;
			while (x + width < voxel_size_x) {
				const int next_index = z * voxel_size_x + (x + width);
				if (top_visited[next_index] || voxel_heights[next_index] != column_height)
					break;
				++width;
			}

			int depth = 1;
			bool can_extend = true;
			while (z + depth < voxel_size_z && can_extend) {
				for (int dx = 0; dx < width; ++dx) {
					const int next_index = (z + depth) * voxel_size_x + (x + dx);
					if (top_visited[next_index] || voxel_heights[next_index] != column_height) {
						can_extend = false;
						break;
					}
				}
				if (can_extend)
					++depth;
			}

			for (int dz = 0; dz < depth; ++dz) {
				for (int dx = 0; dx < width; ++dx)
					top_visited[(z + dz) * voxel_size_x + (x + dx)] = 1;
			}

			const float x0 = static_cast<float>(x) * unit;
			const float x1 = static_cast<float>(x + width) * unit;
			const float z0 = static_cast<float>(z) * unit;
			const float z1 = static_cast<float>(z + depth) * unit;
			const float y = static_cast<float>(column_height) * unit;

			append_voxel_face(
			    mesh,
			    glm::vec3(x0, y, z0),
			    glm::vec3(x1, y, z0),
			    glm::vec3(x0, y, z1),
			    glm::vec3(x1, y, z1),
			    glm::vec3(0.0f, 1.0f, 0.0f),
			    static_cast<float>(width),
			    static_cast<float>(depth));
		}
	}

	for (int x = 0; x < voxel_size_x; ++x) {
		int z = 0;
		while (z < voxel_size_z) {
			const int column_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z);
			const int left_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x - 1, z);
			if (column_height <= left_height) {
				++z;
				continue;
			}

			int depth = 1;
			while (z + depth < voxel_size_z) {
				const int run_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z + depth);
				const int run_left_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x - 1, z + depth);
				if (run_height != column_height || run_left_height != left_height)
					break;
				++depth;
			}

			const float x_plane = static_cast<float>(x) * unit;
			const float z0 = static_cast<float>(z) * unit;
			const float z1 = static_cast<float>(z + depth) * unit;
			append_voxel_face(
			    mesh,
			    glm::vec3(x_plane, static_cast<float>(left_height) * unit, z0),
			    glm::vec3(x_plane, static_cast<float>(left_height) * unit, z1),
			    glm::vec3(x_plane, static_cast<float>(column_height) * unit, z0),
			    glm::vec3(x_plane, static_cast<float>(column_height) * unit, z1),
			    glm::vec3(-1.0f, 0.0f, 0.0f),
			    static_cast<float>(depth),
			    static_cast<float>(column_height - left_height));
			z += depth;
		}
	}

	for (int x = 0; x < voxel_size_x; ++x) {
		int z = 0;
		while (z < voxel_size_z) {
			const int column_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z);
			const int right_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x + 1, z);
			if (column_height <= right_height) {
				++z;
				continue;
			}

			int depth = 1;
			while (z + depth < voxel_size_z) {
				const int run_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z + depth);
				const int run_right_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x + 1, z + depth);
				if (run_height != column_height || run_right_height != right_height)
					break;
				++depth;
			}

			const float x_plane = static_cast<float>(x + 1) * unit;
			const float z0 = static_cast<float>(z) * unit;
			const float z1 = static_cast<float>(z + depth) * unit;
			append_voxel_face(
			    mesh,
			    glm::vec3(x_plane, static_cast<float>(right_height) * unit, z0),
			    glm::vec3(x_plane, static_cast<float>(column_height) * unit, z0),
			    glm::vec3(x_plane, static_cast<float>(right_height) * unit, z1),
			    glm::vec3(x_plane, static_cast<float>(column_height) * unit, z1),
			    glm::vec3(1.0f, 0.0f, 0.0f),
			    static_cast<float>(depth),
			    static_cast<float>(column_height - right_height));
			z += depth;
		}
	}

	for (int z = 0; z < voxel_size_z; ++z) {
		int x = 0;
		while (x < voxel_size_x) {
			const int column_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z);
			const int back_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z - 1);
			if (column_height <= back_height) {
				++x;
				continue;
			}

			int width = 1;
			while (x + width < voxel_size_x) {
				const int run_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x + width, z);
				const int run_back_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x + width, z - 1);
				if (run_height != column_height || run_back_height != back_height)
					break;
				++width;
			}

			const float z_plane = static_cast<float>(z) * unit;
			const float x0 = static_cast<float>(x) * unit;
			const float x1 = static_cast<float>(x + width) * unit;
			append_voxel_face(
			    mesh,
			    glm::vec3(x0, static_cast<float>(back_height) * unit, z_plane),
			    glm::vec3(x1, static_cast<float>(back_height) * unit, z_plane),
			    glm::vec3(x0, static_cast<float>(column_height) * unit, z_plane),
			    glm::vec3(x1, static_cast<float>(column_height) * unit, z_plane),
			    glm::vec3(0.0f, 0.0f, -1.0f),
			    static_cast<float>(width),
			    static_cast<float>(column_height - back_height));
			x += width;
		}
	}

	for (int z = 0; z < voxel_size_z; ++z) {
		int x = 0;
		while (x < voxel_size_x) {
			const int column_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z);
			const int front_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x, z + 1);
			if (column_height <= front_height) {
				++x;
				continue;
			}

			int width = 1;
			while (x + width < voxel_size_x) {
				const int run_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x + width, z);
				const int run_front_height = get_voxel_height(voxel_heights, voxel_size_x, voxel_size_z, x + width, z + 1);
				if (run_height != column_height || run_front_height != front_height)
					break;
				++width;
			}

			const float z_plane = static_cast<float>(z + 1) * unit;
			const float x0 = static_cast<float>(x) * unit;
			const float x1 = static_cast<float>(x + width) * unit;
			append_voxel_face(
			    mesh,
			    glm::vec3(x0, static_cast<float>(front_height) * unit, z_plane),
			    glm::vec3(x0, static_cast<float>(column_height) * unit, z_plane),
			    glm::vec3(x1, static_cast<float>(front_height) * unit, z_plane),
			    glm::vec3(x1, static_cast<float>(column_height) * unit, z_plane),
			    glm::vec3(0.0f, 0.0f, 1.0f),
			    static_cast<float>(width),
			    static_cast<float>(column_height - front_height));
			x += width;
		}
	}

	return mesh;
}

static MeshData generate_terrain_mesh_data(const TerrainComponent& terrain_component) {
	const int size_x = terrain_component.size_x;
	const int size_z = terrain_component.size_z;

	Noise::Generator base_generator { terrain_component.noise_settings };

	Noise::Settings warp_settings = terrain_component.noise_settings;
	warp_settings.seed += 101;
	warp_settings.frequency *= 0.45f;
	warp_settings.octaves = std::max(1, warp_settings.octaves - 1);

	Noise::Settings warp_settings_z = warp_settings;
	warp_settings_z.seed += 173;

	Noise::Settings ridge_settings = terrain_component.noise_settings;
	ridge_settings.seed += 211;
	ridge_settings.frequency *= terrain_component.shape_settings.ridge_frequency_multiplier;
	ridge_settings.gain *= 0.85f;

	Noise::Settings medium_settings = terrain_component.noise_settings;
	medium_settings.seed += 149;
	medium_settings.frequency *= terrain_component.shape_settings.medium_frequency_multiplier;
	medium_settings.octaves = std::max(2, terrain_component.noise_settings.octaves - 1);
	medium_settings.gain *= 0.8f;

	Noise::Settings fine_settings = terrain_component.noise_settings;
	fine_settings.seed += 307;
	fine_settings.frequency *= terrain_component.shape_settings.fine_frequency_multiplier;
	fine_settings.octaves = std::max(1, fine_settings.octaves);
	fine_settings.gain *= 0.7f;

	Noise::Generator warp_generator_x { warp_settings };
	Noise::Generator warp_generator_z { warp_settings_z };
	Noise::Generator medium_generator { medium_settings };
	Noise::Generator ridge_generator { ridge_settings };
	Noise::Generator fine_generator { fine_settings };

	std::vector<float> height_map(size_x * size_z);

	for (int z = 0; z < size_z; ++z) {
		for (int x = 0; x < size_x; ++x) {
			const float height = build_base_height(
			    terrain_component,
			    base_generator,
			    warp_generator_x,
			    warp_generator_z,
			    medium_generator,
			    ridge_generator,
			    fine_generator,
			    x,
			    z);

			height_map[z * size_x + x] = height;
		}
	}

	apply_erosion(height_map, size_x, size_z, terrain_component.erosion_settings);
	normalize_height_map(height_map);

	for (float& sample : height_map)
		sample *= terrain_component.height;

	if (terrain_component.style == TerrainStyle::Voxel) {
		return construct_voxel_mesh_data(
		    height_map,
		    size_x,
		    size_z,
		    terrain_component.cell_size);
	}

	return TerrainComponentMeshBuilderSystem::construct_mesh_data_from_heightmap(
	    height_map,
	    size_x,
	    size_z,
	    terrain_component.cell_size);
}

static MeshData construct_water_mesh_data(const TerrainComponent& terrain_component) {
	MeshData mesh {};
	const float world_y = terrain_component.water_settings.water_level * terrain_component.height - 0.1f;
	const float max_x = static_cast<float>(terrain_component.size_x - 1) * terrain_component.cell_size;
	const float max_z = static_cast<float>(terrain_component.size_z - 1) * terrain_component.cell_size;

	mesh.Vertices = {
		0.0f, world_y, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		max_x, world_y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, world_y, max_z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		max_x, world_y, max_z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	};
	mesh.Indices = { 0, 2, 1, 1, 2, 3 };
	return mesh;
}

static float get_height(
    const std::vector<float>& height_map,
    int size_x,
    int size_z,
    int x,
    int z) {
	x = std::clamp(x, 0, size_x - 1);
	z = std::clamp(z, 0, size_z - 1);
	return height_map[z * size_x + x];
}

static glm::vec3 get_height_position(
    const std::vector<float>& height_map,
    int size_x,
    int size_z,
    int x,
    int z,
    float cell_size) {
	return glm::vec3(
	    static_cast<float>(std::clamp(x, 0, size_x - 1)) * cell_size,
	    get_height(height_map, size_x, size_z, x, z),
	    static_cast<float>(std::clamp(z, 0, size_z - 1)) * cell_size);
}

static glm::vec3 compute_smooth_heightmap_normal(
    const std::vector<float>& height_map,
    int size_x,
    int size_z,
    int x,
    int z,
    float cell_size) {
	const glm::vec3 center = get_height_position(height_map, size_x, size_z, x, z, cell_size);
	const glm::vec3 ring[] = {
		get_height_position(height_map, size_x, size_z, x, z - 1, cell_size),
		get_height_position(height_map, size_x, size_z, x + 1, z - 1, cell_size),
		get_height_position(height_map, size_x, size_z, x + 1, z, cell_size),
		get_height_position(height_map, size_x, size_z, x + 1, z + 1, cell_size),
		get_height_position(height_map, size_x, size_z, x, z + 1, cell_size),
		get_height_position(height_map, size_x, size_z, x - 1, z + 1, cell_size),
		get_height_position(height_map, size_x, size_z, x - 1, z, cell_size),
		get_height_position(height_map, size_x, size_z, x - 1, z - 1, cell_size),
	};

	glm::vec3 accumulated_normal(0.0f);
	for (size_t i = 0; i < std::size(ring); ++i) {
		const glm::vec3 edge_a = ring[i] - center;
		const glm::vec3 edge_b = ring[(i + 1) % std::size(ring)] - center;
		accumulated_normal += glm::cross(edge_b, edge_a);
	}

	if (glm::dot(accumulated_normal, accumulated_normal) <= 1e-10f) {
		const float h_l = get_height(height_map, size_x, size_z, x - 1, z);
		const float h_r = get_height(height_map, size_x, size_z, x + 1, z);
		const float h_d = get_height(height_map, size_x, size_z, x, z - 1);
		const float h_u = get_height(height_map, size_x, size_z, x, z + 1);
		const float ddx = (h_r - h_l) / std::max(cell_size * 2.0f, 1e-5f);
		const float ddz = (h_u - h_d) / std::max(cell_size * 2.0f, 1e-5f);
		accumulated_normal = glm::vec3(-ddx, 1.0f, -ddz);
	}

	return glm::normalize(accumulated_normal);
}

MeshData TerrainComponentMeshBuilderSystem::construct_mesh_data_from_heightmap(
    const std::vector<float>& height_map,
    int size_x,
    int size_z,
    float cell_size) {
	MeshData mesh {};
	const int stride = 8;

	mesh.Vertices.reserve(size_x * size_z * stride);

	for (int z = 0; z < size_z; ++z) {
		for (int x = 0; x < size_x; ++x) {
			float h = height_map[z * size_x + x];
			const glm::vec3 normal = compute_smooth_heightmap_normal(
			    height_map,
			    size_x,
			    size_z,
			    x,
			    z,
			    cell_size);

			mesh.Vertices.push_back(static_cast<float>(x) * cell_size);
			mesh.Vertices.push_back(h);
			mesh.Vertices.push_back(static_cast<float>(z) * cell_size);

			mesh.Vertices.push_back(normal.x);
			mesh.Vertices.push_back(normal.y);
			mesh.Vertices.push_back(normal.z);

			mesh.Vertices.push_back(static_cast<float>(x));
			mesh.Vertices.push_back(static_cast<float>(z));
		}
	}

	mesh.Indices.reserve((size_x - 1) * (size_z - 1) * 6);

	for (int z = 0; z < size_z - 1; ++z) {
		for (int x = 0; x < size_x - 1; ++x) {
			unsigned int i0 = z * size_x + x;
			unsigned int i1 = z * size_x + (x + 1);
			unsigned int i2 = (z + 1) * size_x + x;
			unsigned int i3 = (z + 1) * size_x + (x + 1);

			mesh.Indices.push_back(i0);
			mesh.Indices.push_back(i2);
			mesh.Indices.push_back(i1);

			mesh.Indices.push_back(i1);
			mesh.Indices.push_back(i2);
			mesh.Indices.push_back(i3);
		}
	}

	return mesh;
}

void TerrainComponentMeshBuilderSystem::update(Scene* scene, float dt) {
	scene->view<TerrainComponent>([&](RID entity, TerrainComponent& terrain_component) {
		if (!terrain_component.should_rebuild && !terrain_component.started_rebuilding)
			return;

		if (terrain_component.started_rebuilding) {
			auto task = terrain_component.active_build_task;
			if (!task || !task->Ready.load(std::memory_order_acquire))
				return;

			std::optional<MeshData> built_mesh = std::nullopt;
			{
				std::scoped_lock lock(task->ResultMutex);
				built_mesh = std::move(task->Result);
			}

			if (!built_mesh.has_value()) {
				terrain_component.started_rebuilding = false;
				terrain_component.should_rebuild = false;
				terrain_component.active_build_task.reset();
				return;
			}

			const int layout_id = VertexLayout::get_static_mesh_layout();
			const int heap_id = GeometryHeapRegistry::get_or_create_static_mesh_heap(layout_id);

			auto* heap = GeometryHeapRegistry::get_heap(heap_id);
			auto* layout = VertexLayoutRegistry::get(layout_id);
			if (!heap || !layout) {
				terrain_component.started_rebuilding = false;
				terrain_component.active_build_task.reset();
				return;
			}

			release_terrain_mesh(terrain_component.terrain_mesh);

			try {
				const auto range = heap->allocate(
				    built_mesh->Vertices.data(),
				    built_mesh->Vertices.size() * sizeof(float),
				    layout->get_stride(),
				    built_mesh->Indices.data(),
				    built_mesh->Indices.size());

				terrain_component.terrain_mesh = AssetFactory::get_instance().create_runtime_asset<Mesh>(
				    "TerrainMesh_" + std::to_string(entity.get_id()),
				    layout_id,
				    heap_id,
				    range);
				apply_mesh_bounds(*terrain_component.terrain_mesh, *built_mesh, layout->get_stride() / sizeof(float));
				release_terrain_mesh(terrain_component.water_mesh);
				if (terrain_component.water_settings.enabled) {
					const MeshData water_mesh = construct_water_mesh_data(terrain_component);
					const auto water_range = heap->allocate(
					    water_mesh.Vertices.data(),
					    water_mesh.Vertices.size() * sizeof(float),
					    layout->get_stride(),
					    water_mesh.Indices.data(),
					    water_mesh.Indices.size());

					terrain_component.water_mesh = AssetFactory::get_instance().create_runtime_asset<Mesh>(
					    "TerrainWaterMesh_" + std::to_string(entity.get_id()),
					    layout_id,
					    heap_id,
					    water_range);
					apply_mesh_bounds(*terrain_component.water_mesh, water_mesh, layout->get_stride() / sizeof(float));
				}
			} catch (const std::exception& ex) {
				ORG_CORE_ERROR(
				    "Failed to allocate terrain mesh for entity {}: {} (vertices: {}, indices: {})",
				    entity.get_id(),
				    ex.what(),
				    built_mesh->Vertices.size() / 8,
				    built_mesh->Indices.size());
				release_terrain_mesh(terrain_component.terrain_mesh);
				release_terrain_mesh(terrain_component.water_mesh);
			}

			terrain_component.should_rebuild = false;
			terrain_component.started_rebuilding = false;
			terrain_component.active_build_task.reset();
			return;
		}

		terrain_component.started_rebuilding = true;

		const int size_x = terrain_component.size_x;
		const int size_z = terrain_component.size_z;

		if (size_x < 2 || size_z < 2) {
			release_terrain_mesh(terrain_component.terrain_mesh);
			release_terrain_mesh(terrain_component.water_mesh);
			terrain_component.should_rebuild = false;
			terrain_component.started_rebuilding = false;
			return;
		}

		auto task = std::make_shared<TerrainBuildTask>();
		task->Generation = ++terrain_component.build_generation;
		terrain_component.active_build_task = task;

		const TerrainComponent snapshot = terrain_component;
		std::thread([task, snapshot]() mutable {
			MeshData mesh = generate_terrain_mesh_data(snapshot);
			{
				std::scoped_lock lock(task->ResultMutex);
				task->Result = std::move(mesh);
			}
			task->Ready.store(true, std::memory_order_release);
		}).detach();
	});
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateAlways,
    Origo::TerrainComponentMeshBuilderSystem)
