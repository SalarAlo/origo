#pragma once

#include <filesystem>

#include "origo/assets/AssetManager.h"

namespace Origo {

class ISerializer {
public:
	ISerializer(const std::filesystem::path& path)
	    : m_path(path) { }
	virtual ~ISerializer() = default;

	virtual void save_to_file() = 0;
	virtual void load_file() = 0;

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

	virtual void write(int value) = 0;
	virtual void write(unsigned int value) = 0;
	virtual void write(float value) = 0;
	virtual void write(std::string_view value) = 0;
	void write(const Vec3& value);
	void write(const OptionalAssetHandle& asset);

	virtual bool try_read(std::string_view key, int& value) = 0;
	virtual bool try_read(std::string_view key, unsigned int& value) = 0;
	virtual bool try_read(std::string_view key, float& value) = 0;
	virtual bool try_read(std::string_view key, std::string& value) = 0;
	bool try_read(std::string_view key, Vec3& value);

	virtual bool try_read_array_object(int& value) = 0;
	virtual bool try_read_array_object(unsigned int& value) = 0;
	virtual bool try_read_array_object(float& value) = 0;
	virtual bool try_read_array_object(std::string& value) = 0;
	bool try_read_array_object(Vec3& value);

	virtual bool try_begin_array_object_read() = 0;

protected:
	std::filesystem::path m_path;
};

#define SERIALIZER_FWD                                                     \
	void save_to_file() override;                                      \
	void load_file() override;                                         \
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
