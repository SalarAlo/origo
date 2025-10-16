#pragma once
#include <iostream>
#include <cassert>

inline void GL_ClearErrors() {
	while (glGetError() != GL_NO_ERROR) { }
}

// Converts GLenum to readable text
inline const char* GL_ErrorString(GLenum error) {
	switch (error) {
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
	default:
		return "Unknown Error";
	}
}

// Prints errors with context
inline bool GL_LogCall(const char* function, const char* file, int line) {
	bool success = true;
	while (GLenum error = glGetError()) {
		std::cerr << "[OpenGL Error] (" << GL_ErrorString(error)
		          << ") in " << function
		          << " at " << file << ":" << line << std::endl;
		success = false;
	}
	return success;
}

// Macro wrapper
#define GLCall(x)                                           \
	do {                                                \
		GL_ClearErrors();                           \
		x;                                          \
		assert(GL_LogCall(#x, __FILE__, __LINE__)); \
	} while (false)
