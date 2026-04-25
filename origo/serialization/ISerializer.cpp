#include "origo/serialization/ISerializer.h"

namespace Origo {

static bool read_vec3_components(ISerializer& s, Vec3& v) {
	return s.try_read("x", v.x) && s.try_read("y", v.y) && s.try_read("z", v.z);
}

static void write_vec3_components(ISerializer& s, const Vec3& v) {
	s.write("x", v.x);
	s.write("y", v.y);
	s.write("z", v.z);
}

bool ISerializer::try_read(std::string_view key, Vec3& value) {
	begin_object(key);

	bool ok = read_vec3_components(*this, value);

	end_object();
	return ok;
}

bool ISerializer::try_read(std::string_view key, bool& value) {
	int num_value {};

	if (!try_read(key, num_value))
		return false;

	value = num_value == 1;
	return true;
}

bool ISerializer::try_read_array_object(Vec3& value) {
	if (!try_begin_array_object_read())
		return false;

	bool ok = read_vec3_components(*this, value);

	end_array_object();
	return ok;
}

bool ISerializer::try_read_array_object(bool& value) {
	if (!try_begin_array_object_read())
		return false;

	int num_value {};
	if (!try_read_array_object(num_value))
		return false;

	value = num_value == 1;
	return true;
}

void ISerializer::write(std::string_view key, const Vec3& value) {
	begin_object(key);
	write_vec3_components(*this, value);
	end_object();
}

void ISerializer::write(const Vec3& value) {
	begin_array_object();
	write_vec3_components(*this, value);
	end_array_object();
}

void ISerializer::write(std::string_view key, const OptionalAssetHandle& opt_handle) {
	if (!opt_handle) {
		write(key, "0");
		return;
	}

	auto uuid = AssetManager::get_instance().get_uuid(*opt_handle);

	if (uuid.has_value())
		write(key, uuid->to_string());
	else
		write(key, "0");
}

void ISerializer::write(const OptionalAssetHandle& opt_handle) {
	if (!opt_handle) {
		write("0");
		return;
	}

	auto uuid = AssetManager::get_instance().get_uuid(*opt_handle);

	if (uuid.has_value())
		write(uuid->to_string());
	else
		write("0");
}

void ISerializer::write(std::string_view key, const bool& value) {
	write(key, value ? 1 : 0);
}

void ISerializer::write(const bool& value) {
	write(value ? 1 : 0);
}

}
