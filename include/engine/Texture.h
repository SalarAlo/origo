#pragma once

class Texture {
public:
	Texture(std::string_view pathName);
	~Texture();

private:
	GLuint m_TextureId {};
	int m_ImageWidth {};
	int m_ImageHeight {};
	int m_NumberOfColorChannels {};
};
