#pragma once

#include "VertexAttribute.h"
#include <stdexcept>

class VertexLayout {
public:
	template<typename T>
	void AddAttribute(unsigned int amount, bool normalized=false) {
		throw std::runtime_error("Not implemented yet for this Type");
	}

	template<>
	void AddAttribute<float>(unsigned int amount, bool normalized) {
		m_Attributes.push_back({ 
			.Type = GL_FLOAT, 
			.Amount = amount, 
			.Normalized = normalized ? GL_TRUE : GL_FALSE
		});
	}

	template<>
	void AddAttribute<unsigned int>(unsigned int amount, bool normalized) {
		m_Attributes.push_back({ 
			.Type = GL_UNSIGNED_INT, 
			.Amount = amount, 
			.Normalized = normalized ? GL_TRUE : GL_FALSE
		});
	}

	const std::vector<VertexAttribute>& GetAttributes() const {
		return m_Attributes;
	}

private:
	std::vector<VertexAttribute> m_Attributes;
};


