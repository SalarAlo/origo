#pragma once
#include <string>

namespace Origo {

class ISerializer {
public:
	ISerializer(std::string_view path)
	    : m_Path(path) { }
	virtual ~ISerializer() = default;

	virtual void WriteFile() = 0;
	virtual void LoadFile() = 0;

	virtual void BeginObject(std::string_view key) = 0;
	virtual void EndObject() = 0;

	virtual void BeginArray(std::string_view key) = 0;
	virtual void EndArray() = 0;

	virtual void BeginArrayElement() = 0;
	virtual void EndArrayElement() = 0;

	virtual void Write(std::string_view key, int value) = 0;
	virtual void Write(std::string_view key, float value) = 0;
	virtual void Write(std::string_view key, std::string_view value) = 0;

	virtual void Write(int value) = 0;
	virtual void Write(float value) = 0;
	virtual void Write(std::string_view value) = 0;

	virtual bool TryRead(std::string_view key, int& value) = 0;
	virtual bool TryRead(std::string_view key, float& value) = 0;
	virtual bool TryRead(std::string_view key, std::string& value) = 0;

protected:
	std::string m_Path;
};

#define SERIALIZER_FWD                                                     \
	void WriteFile() override;                                         \
	void LoadFile() override;                                          \
                                                                           \
	void BeginObject(std::string_view key) override;                   \
	void EndObject() override;                                         \
                                                                           \
	void BeginArray(std::string_view key) override;                    \
	void EndArray() override;                                          \
                                                                           \
	virtual void BeginArrayElement() override;                         \
	virtual void EndArrayElement() override;                           \
                                                                           \
	void Write(std::string_view key, int value) override;              \
	void Write(std::string_view key, float value) override;            \
	void Write(std::string_view key, std::string_view value) override; \
                                                                           \
	void Write(int value) override;                                    \
	void Write(float value) override;                                  \
	void Write(std::string_view value) override;                       \
                                                                           \
	bool TryRead(std::string_view key, int& value) override;           \
	bool TryRead(std::string_view key, float& value) override;         \
	bool TryRead(std::string_view key, std::string& value) override;

}
