#include "origo/renderer/Helpers.h"
#include <fstream>
#include <sstream>

namespace Origo {
int glsizeof(GLuint type) {
	switch (type) {
	case GL_FLOAT:
		return sizeof(float);
	case GL_UNSIGNED_INT:
		return sizeof(unsigned int);
	default:
		return -1;
	}
}

void to_lower_in_place(std::string& s) {

	std::ranges::transform(
	    s, s.begin(),
	    [](unsigned char c) { return std::tolower(c); });
}

std::string read_file(const std::filesystem::path& path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file)
		throw std::runtime_error("Failed to open file: " + path.string());

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

}
