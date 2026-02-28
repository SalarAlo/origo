#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

#include "origo/core/UUID.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

enum class MaterialSourceType {
	File,
	Raw,
	ID
};

struct MaterialData {
	AssetHandle AlbedoHandle;
	AssetHandle ShaderHandle;
};

class Material2DSource {
public:
	virtual ~Material2DSource() = default;
	virtual MaterialData get_material_data() const = 0;
	virtual MaterialSourceType get_source_type() const = 0;

	void serialize(ISerializer& backend) const;
	static Scope<Material2DSource> deserialize(ISerializer& backend);

protected:
	virtual void serialize_body(ISerializer& backend) const = 0;
};

class Material2DSourceRaw : public Material2DSource {
public:
	Material2DSourceRaw(const OptionalAssetHandle& albedo, const AssetHandle& shader)
	    : m_shader(shader)
	    , m_albedo(albedo ? *albedo : DefaultAssetCache::get_instance().get_texture()) {
	}

	MaterialData get_material_data() const override {
		return MaterialData { m_albedo, m_shader };
	}

	MaterialSourceType get_source_type() const override {
		return MaterialSourceType::Raw;
	}

	void serialize_body(ISerializer& backend) const override {
		backend.write("albedo", AssetManager::get_instance().get_uuid(m_albedo)->to_string());
		backend.write("shader", AssetManager::get_instance().get_uuid(m_shader)->to_string());
	}

private:
	AssetHandle m_albedo;
	AssetHandle m_shader;
};

class Material2DSourceID : public Material2DSource {
public:
	Material2DSourceID(const UUID& albedo, const UUID& shader)
	    : m_albedo(albedo)
	    , m_shader(shader) {
	}

	MaterialData get_material_data() const override;

	MaterialSourceType get_source_type() const override {
		return MaterialSourceType::ID;
	}

	void serialize_body(ISerializer& backend) const override {
		backend.write("albedo", m_albedo.to_string());
		backend.write("shader", m_shader.to_string());
	}

private:
	UUID m_albedo;
	UUID m_shader;
};

class Material2DSourceFile : public Material2DSource {
public:
	Material2DSourceFile(std::string_view path)
	    : m_path(path) { };

	MaterialData get_material_data() const override;
	std::string get_path() const { return m_path; }

	MaterialSourceType get_source_type() const override {
		return MaterialSourceType::File;
	}

	void serialize_body(ISerializer& backend) const override {
		backend.write("path", m_path);
	}

private:
	std::string m_path;
};

}
