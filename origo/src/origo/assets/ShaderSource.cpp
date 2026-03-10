#include "origo/assets/ShaderSource.h"

#include "origo/core/PathContext.h"

#include "origo/core/Logger.h"

#pragma region IO_RELATED
static std::string read_file(std::string_view path) {
	constexpr std::size_t read_size { 4096 };
	auto stream = std::ifstream(path.data());

	if (!stream) {
		ORG_CORE_ERROR("[Shader] Trying to read non existent file! ({})", path);
		throw std::ios_base::failure("file does not exist");
	}

	std::string out {};
	std::string buf(read_size, '\0');
	while (stream.read(&buf[0], read_size)) {
		out.append(buf, 0, stream.gcount());
	}
	out.append(buf, 0, stream.gcount());
	return out;
}

static Origo::ShaderData get_data(std::string_view path) {
	Origo::ShaderData data {};
	std::string file_content { read_file(path) };

	auto vert_pos { file_content.find("#VERTEX") };
	auto frag_pos { file_content.find("#FRAGMENT") };

	if (vert_pos == std::string::npos || frag_pos == std::string::npos) {
		ORG_CORE_ERROR("[Shader] file missing #VERTEX or #FRAGMENT section");
		throw std::runtime_error("Shader file missing #VERTEX or #FRAGMENT section");
	}

	std::string vertex_src { file_content.substr(vert_pos + 7, frag_pos - (vert_pos + 7)) };
	std::string fragment_src { file_content.substr(frag_pos + 9) };

	static std::string vertex_storage, fragment_storage;
	vertex_storage = std::move(vertex_src);
	fragment_storage = std::move(fragment_src);

	data.VertexShader = vertex_storage.c_str();
	data.FragmentShader = fragment_storage.c_str();

	return data;
}
#pragma endregion

namespace Origo {

ShaderData ShaderSourceFile::get_shader_data() const {
	return get_data(m_path.string());
}

void ShaderSourceFile::serialize_body(ISerializer& backend) const {
	backend.write("shader_path", PathContextService::get_instance().serialize_path(m_path).string());
}

void ShaderSource::serialize(ISerializer& backend) const {
	backend.begin_object("source");
	backend.write("type", magic_enum::enum_name(get_source_type()));
	serialize_body(backend);
	backend.end_object();
}

Scope<ShaderSource> ShaderSource::deserialize(ISerializer& backend) {
	backend.begin_object("source");

	std::string type_str {};
	backend.try_read("type", type_str);
	auto type_optional { magic_enum::enum_cast<ShaderSourceType>(type_str) };

	if (!type_optional.has_value()) {
		ORG_ERROR("ShaderSource::Deserialize: Unidentifiable type {}", type_str);
		return nullptr;
	}
	auto type { type_optional.value() };

	switch (type) {
	case ShaderSourceType::File: {
		std::string path {};
		backend.try_read("shader_path", path);
		backend.end_object();

		return make_scope<ShaderSourceFile>(PathContextService::get_instance().resolve_serialized_path(path));
	}

	case ShaderSourceType::Raw: {
		std::string vert {};
		std::string frag {};
		backend.try_read("vertex_shader", vert);
		backend.try_read("fragment_shader", frag);
		backend.end_object();
		return make_scope<ShaderSourceRaw>(vert, frag);
	}
	default: {
		backend.end_object();
		ORG_ERROR("ShaderSource::Deserialize: Unsupported type {}", type_str);
		return nullptr;
	}
	}
}

}
