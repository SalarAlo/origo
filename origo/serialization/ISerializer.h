#pragma once

#include <filesystem>
#include <optional>

#include "origo/assets/AssetManager.h"

namespace Origo {

class ISerializer {
public:
	ISerializer() = default;
	explicit ISerializer(std::filesystem::path path)
	    : m_path(std::move(path)) { }

	virtual ~ISerializer() = default;

	void set_path(std::filesystem::path path) { m_path = std::move(path); }
	void clear_path() { m_path.reset(); }
	bool has_path() const { return m_path.has_value(); }
	const std::optional<std::filesystem::path>& get_path() const { return m_path; }

	virtual void save_to_file() = 0;
	virtual void load_file() = 0;
	virtual void write_to(ISerializer& serializer) const = 0;

	virtual void begin_object(std::string_view key) = 0;
	virtual void end_object() = 0;

	virtual void begin_array(std::string_view key) = 0;
	virtual void end_array() = 0;

	virtual void begin_array_object() = 0;
	virtual void end_array_object() = 0;

	virtual void write(std::string_view key, int value) = 0;
	virtual void write(std::string_view key, unsigned int value) = 0;
	virtual void write(std::string_view key, float value) = 0;
	virtual void write(std::string_view key, std::string_view value) = 0;

	void write(std::string_view key, const Vec3& value);
	void write(std::string_view key, const OptionalAssetHandle& asset);
	void write(std::string_view key, const bool& value);

	virtual void write(int value) = 0;
	virtual void write(unsigned int value) = 0;
	virtual void write(float value) = 0;
	virtual void write(std::string_view value) = 0;
	void write(const Vec3& value);
	void write(const OptionalAssetHandle& asset);
	void write(const bool& value);

	virtual bool try_read(std::string_view key, int& value) = 0;
	virtual bool try_read(std::string_view key, unsigned int& value) = 0;
	virtual bool try_read(std::string_view key, float& value) = 0;
	virtual bool try_read(std::string_view key, std::string& value) = 0;
	bool try_read(std::string_view key, Vec3& value);
	bool try_read(std::string_view key, bool& value);

	virtual bool try_read_array_object(int& value) = 0;
	virtual bool try_read_array_object(unsigned int& value) = 0;
	virtual bool try_read_array_object(float& value) = 0;
	virtual bool try_read_array_object(std::string& value) = 0;
	bool try_read_array_object(Vec3& value);
	bool try_read_array_object(bool& value);

	virtual bool try_begin_array_object_read() = 0;

protected:
	std::optional<std::filesystem::path> m_path;
};

#define SERIALIZER_FWD                                                     \
	void save_to_file() override;                                      \
	void load_file() override;                                         \
	void write_to(ISerializer& serializer) const override;             \
                                                                           \
	void begin_object(std::string_view key) override;                  \
	void end_object() override;                                        \
                                                                           \
	void begin_array(std::string_view key) override;                   \
	void end_array() override;                                         \
                                                                           \
	virtual void begin_array_object() override;                        \
	virtual void end_array_object() override;                          \
                                                                           \
	void write(std::string_view key, int value) override;              \
	void write(std::string_view key, unsigned int value) override;     \
	void write(std::string_view key, float value) override;            \
	void write(std::string_view key, std::string_view value) override; \
                                                                           \
	void write(int value) override;                                    \
	void write(unsigned int value) override;                           \
	void write(float value) override;                                  \
	void write(std::string_view value) override;                       \
                                                                           \
	bool try_read(std::string_view key, int& value) override;          \
	bool try_read(std::string_view key, unsigned int& value) override; \
	bool try_read(std::string_view key, float& value) override;        \
	bool try_read(std::string_view key, std::string& value) override;  \
                                                                           \
	bool try_read_array_object(int& value) override;                   \
	bool try_read_array_object(unsigned int& value) override;          \
	bool try_read_array_object(float& value) override;                 \
	bool try_read_array_object(std::string& value) override;           \
	bool try_begin_array_object_read() override;                       \
                                                                           \
	using ISerializer::try_read;                                       \
	using ISerializer::try_read_array_object;                          \
	using ISerializer::write;

}
