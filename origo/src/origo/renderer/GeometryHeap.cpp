#include <algorithm>
#include <stdexcept>

#include "origo/renderer/GeometryHeap.h"

namespace Origo {

size_t GeometryHeap::try_allocate_from_free_list(std::vector<FreeBlock>& freeBlocks, size_t sizeBytes) {
	for (size_t i = 0; i < freeBlocks.size(); ++i) {
		FreeBlock& block = freeBlocks[i];
		if (block.SizeBytes < sizeBytes)
			continue;

		const size_t offset = block.OffsetBytes;
		block.OffsetBytes += sizeBytes;
		block.SizeBytes -= sizeBytes;

		if (block.SizeBytes == 0)
			freeBlocks.erase(freeBlocks.begin() + static_cast<std::ptrdiff_t>(i));

		return offset;
	}

	return static_cast<size_t>(-1);
}

void GeometryHeap::free_to_list(std::vector<FreeBlock>& freeBlocks, size_t offsetBytes, size_t sizeBytes) {
	if (sizeBytes == 0)
		return;

	freeBlocks.push_back({ .OffsetBytes = offsetBytes, .SizeBytes = sizeBytes });
	std::sort(
	    freeBlocks.begin(),
	    freeBlocks.end(),
	    [](const FreeBlock& a, const FreeBlock& b) {
		    return a.OffsetBytes < b.OffsetBytes;
	    });

	std::vector<FreeBlock> merged;
	merged.reserve(freeBlocks.size());

	for (const FreeBlock& block : freeBlocks) {
		if (merged.empty()) {
			merged.push_back(block);
			continue;
		}

		FreeBlock& last = merged.back();
		const size_t last_end = last.OffsetBytes + last.SizeBytes;
		if (last_end >= block.OffsetBytes) {
			const size_t block_end = block.OffsetBytes + block.SizeBytes;
			last.SizeBytes = std::max(last_end, block_end) - last.OffsetBytes;
			continue;
		}

		merged.push_back(block);
	}

	freeBlocks = std::move(merged);
}

GeometryHeap::GeometryHeap(int vertexLayoutId, GLenum usage, size_t maxVertexBytes, size_t maxIndexBytes)
    : m_vertex_layout_id(vertexLayoutId)
    , m_usage(usage)
    , m_max_vertex_bytes(maxVertexBytes)
    , m_max_index_bytes(maxIndexBytes) {
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, maxVertexBytes, nullptr, usage);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexBytes, nullptr, usage);
}

GeometryHeap::~GeometryHeap() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

MeshRange GeometryHeap::allocate(
    const void* vertexData,
    size_t vertexBytes,
    size_t sizeOfSingleVertexInBytes,
    const unsigned int* indexData,
    size_t indexCount) {
	if (sizeOfSingleVertexInBytes == 0)
		throw std::invalid_argument("GeometryHeap::allocate requires a non-zero vertex stride");

	const size_t index_bytes = indexCount * sizeof(unsigned int);
	const size_t free_vertex_offset = try_allocate_from_free_list(m_free_vertex_blocks, vertexBytes);
	const size_t free_index_offset = try_allocate_from_free_list(m_free_index_blocks, index_bytes);
	const bool reused_vertex_block = free_vertex_offset != static_cast<size_t>(-1);
	const bool reused_index_block = free_index_offset != static_cast<size_t>(-1);

	const size_t vertex_offset = reused_vertex_block ? free_vertex_offset : m_vertex_write_head_bytes;
	const size_t index_offset = reused_index_block ? free_index_offset : m_index_write_head_bytes;

	if (vertex_offset + vertexBytes > m_max_vertex_bytes) {
		if (reused_index_block)
			free_to_list(m_free_index_blocks, index_offset, index_bytes);
		throw std::runtime_error("GeometryHeap vertex buffer capacity exceeded");
	}

	if (index_offset + index_bytes > m_max_index_bytes) {
		if (reused_vertex_block)
			free_to_list(m_free_vertex_blocks, vertex_offset, vertexBytes);
		if (reused_index_block)
			free_to_list(m_free_index_blocks, index_offset, index_bytes);
		throw std::runtime_error("GeometryHeap index buffer capacity exceeded");
	}

	MeshRange range;

	range.FirstVertex = static_cast<uint32_t>(vertex_offset / sizeOfSingleVertexInBytes);
	range.VertexCount = static_cast<uint32_t>(vertexBytes / sizeOfSingleVertexInBytes);

	range.FirstIndex = static_cast<uint32_t>(index_offset / sizeof(unsigned int));
	range.IndexCount = static_cast<uint32_t>(indexCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, vertexBytes, vertexData);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, index_bytes, indexData);

	if (!reused_vertex_block)
		m_vertex_write_head_bytes += vertexBytes;
	if (!reused_index_block)
		m_index_write_head_bytes += index_bytes;

	return range;
}

bool GeometryHeap::free_range(const MeshRange& range, size_t vertexStrideBytes) {
	if (vertexStrideBytes == 0)
		return false;

	const size_t vertex_offset = static_cast<size_t>(range.FirstVertex) * vertexStrideBytes;
	const size_t vertex_size = static_cast<size_t>(range.VertexCount) * vertexStrideBytes;
	const size_t index_offset = static_cast<size_t>(range.FirstIndex) * sizeof(unsigned int);
	const size_t index_size = static_cast<size_t>(range.IndexCount) * sizeof(unsigned int);

	if (vertex_offset + vertex_size > m_max_vertex_bytes)
		return false;
	if (index_offset + index_size > m_max_index_bytes)
		return false;

	free_to_list(m_free_vertex_blocks, vertex_offset, vertex_size);
	free_to_list(m_free_index_blocks, index_offset, index_size);
	return true;
}

}
