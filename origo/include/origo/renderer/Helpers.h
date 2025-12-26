#pragma once

namespace Origo {

int Glsizeof(GLuint type);
void ToLowerInPlace(std::string& s);
std::string ReadFile(const std::filesystem::path& path);

}
