#pragma once

#include <filesystem>

#include "origo/utils/Singleton.h"

namespace Origo {

struct EditorResourcePathContext {
	std::filesystem::path Root {};

	std::filesystem::path resolve(const std::filesystem::path& path) const;
	std::filesystem::path make_serialized_path(const std::filesystem::path& path) const;
	bool owns(const std::filesystem::path& path) const;

	std::filesystem::path icons_root() const;
	std::filesystem::path fonts_root() const;
	std::filesystem::path fallback_root() const;
};

struct ProjectPathContext {
	std::filesystem::path Root {};

	std::filesystem::path resolve_project_path(const std::filesystem::path& path) const;
	std::filesystem::path resolve_resource_path(const std::filesystem::path& path) const;
	std::filesystem::path make_project_relative(const std::filesystem::path& path) const;
	bool owns(const std::filesystem::path& path) const;

	std::filesystem::path resources_root() const;
	std::filesystem::path scenes_root() const;
	std::filesystem::path generated_assets_root() const;
};

class PathContextService : public Singleton<PathContextService> {
public:
	void configure(EditorResourcePathContext editor, ProjectPathContext project);

	const EditorResourcePathContext& editor() const { return m_editor; }
	const ProjectPathContext& project() const { return m_project; }

	std::filesystem::path serialize_path(const std::filesystem::path& path) const;
	std::filesystem::path resolve_serialized_path(const std::filesystem::path& path) const;

private:
	EditorResourcePathContext m_editor {};
	ProjectPathContext m_project {};
};

std::filesystem::path get_repo_source_root();
std::filesystem::path get_default_editor_workspace_root();
EditorResourcePathContext make_default_editor_resource_context();
ProjectPathContext make_default_project_path_context();

}
