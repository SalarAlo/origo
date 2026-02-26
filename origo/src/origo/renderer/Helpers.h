#pragma once

namespace Origo {

int glsizeof(GLuint type);
void to_lower_in_place(std::string& s);
std::string read_file(const std::filesystem::path& path);

}
