#pragma once

#include "VertexAttribute.h"

class VertexLayout {
public:
	template<typename>
	void AddAttribute(unsigned int amount, bool normalized=false);
	const std::vector<VertexAttribute>& GetAttributes() const;

private:
	std::vector<VertexAttribute> m_Attributes;
};
