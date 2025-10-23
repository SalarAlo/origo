#pragma once

#include "origo/assets/MeshSource.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/renderer/IndexBuffer.h"
#include "origo/renderer/VertexArray.h"
#include "origo/renderer/VertexBuffer.h"

#include "origo/assets/Asset.h"

namespace Origo {
class Mesh : public Asset {
public:
	Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
	Mesh(std::string_view meshPath, size_t idx, const MeshData& data);
	Mesh(PrimitiveShape shape);

	void Render() const;

	AssetType GetAssetType() const override {
		return AssetType::Mesh;
	}

	Ref<MeshSource> GetSource() {
		return m_Source;
	}

	bool ShouldSerialize() const override {
		return m_Source->GetType() != MeshSourceType::External;
	}

private:
	void Init();

private:
	Ref<MeshSource> m_Source;

	VertexArray m_VertexArray;
	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
};
}
