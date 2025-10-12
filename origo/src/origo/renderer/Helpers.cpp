#include "origo/renderer/Helpers.h"

namespace Origo {
int Glsizeof(GLuint type) {
	switch (type) {
	case GL_FLOAT:
		return sizeof(float);
	case GL_UNSIGNED_INT:
		return sizeof(unsigned int);
	default:
		return -1;
	}
}
}
