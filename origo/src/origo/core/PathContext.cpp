#include <cstdlib>

#include "origo/core/PathContext.h"

namespace Origo {

namespace {

	constexpr std::string_view k_editor_prefix = "_editor";

	std::filesystem::path normalize_path(const std::filesystem::path& path) {
		if (path.empty())
			return {};
		return path.lexically_normal();
	}

	bool is_relative_to(const std::filesystem::path& path, const std::filesystem::path& root) {
		if (path.empty() || root.empty())
			return false;

		const auto normalized_path = normalize_path(path);
		const auto normalized_root = normalize_path(root);

		if (!normalized_path.is_absolute() || !normalized_root.is_absolute())
			return false;

		const auto relative = normalized_path.lexically_relative(normalized_root);
		if (relative.empty())
			return false;

		for (const auto& part : relative) {
			if (part == "..")
				return false;
			break;
		}

		return true;
	}

	std::filesystem::path env_or_default(const char* env_var, const std::filesystem::path& fallback) {
		const char* override = std::getenv(env_var);
		if (!override || std::string_view(override).empty())
			return fallback;

		return normalize_path(override);
	}

	std::filesystem::path append_editor_prefix(const std::filesystem::path& relative_path) {
		std::filesystem::path prefixed { k_editor_prefix };
		return normalize_path(prefixed / relative_path);
	}

}

std::filesystem::path EditorResourcePathContext::resolve(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);
	if (normalized.is_absolute() || normalized.empty())
		return normalized;

	if (normalized.begin() != normalized.end() && normalized.begin()->string() == k_editor_prefix) {
		auto it = normalized.begin();
		++it;

		std::filesystem::path relative;
		for (; it != normalized.end(); ++it)
			relative /= *it;

		return normalize_path(Root / relative);
	}

	return normalize_path(Root / normalized);
}

std::filesystem::path EditorResourcePathContext::make_serialized_path(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);
	if (!normalized.is_absolute())
		return normalized;

	if (!owns(normalized))
		return normalized;

	return append_editor_prefix(normalized.lexically_relative(Root));
}

bool EditorResourcePathContext::owns(const std::filesystem::path& path) const {
	return is_relative_to(normalize_path(path), normalize_path(Root));
}

std::filesystem::path EditorResourcePathContext::icons_root() const {
	return normalize_path(Root / "icons");
}

std::filesystem::path EditorResourcePathContext::fonts_root() const {
	return normalize_path(Root / "fonts");
}

std::filesystem::path EditorResourcePathContext::fallback_root() const {
	return normalize_path(Root / "fallback");
}

std::filesystem::path ProjectPathContext::resolve_project_path(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);
	if (normalized.is_absolute() || normalized.empty())
		return normalized;

	return normalize_path(Root / normalized);
}

std::filesystem::path ProjectPathContext::resolve_resource_path(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);
	if (normalized.is_absolute() || normalized.empty())
		return normalized;

	if (normalized.begin() != normalized.end() && normalized.begin()->string() == "resources")
		return resolve_project_path(normalized);

	return normalize_path(resources_root() / normalized);
}

std::filesystem::path ProjectPathContext::make_project_relative(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);
	if (!normalized.is_absolute())
		return normalized;

	if (!owns(normalized))
		return normalized;

	return normalize_path(normalized.lexically_relative(Root));
}

bool ProjectPathContext::owns(const std::filesystem::path& path) const {
	return is_relative_to(normalize_path(path), normalize_path(Root));
}

std::filesystem::path ProjectPathContext::resources_root() const {
	return normalize_path(Root / "resources");
}

std::filesystem::path ProjectPathContext::scenes_root() const {
	return normalize_path(resources_root() / "scenes");
}

std::filesystem::path ProjectPathContext::generated_assets_root() const {
	return normalize_path(Root / "assets");
}

void PathContextService::configure(EditorResourcePathContext editor, ProjectPathContext project) {
	m_editor.Root = normalize_path(editor.Root);
	m_project.Root = normalize_path(project.Root);
}

std::filesystem::path PathContextService::serialize_path(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);

	if (m_project.owns(normalized))
		return m_project.make_project_relative(normalized);

	if (m_editor.owns(normalized))
		return m_editor.make_serialized_path(normalized);

	return normalized;
}

std::filesystem::path PathContextService::resolve_serialized_path(const std::filesystem::path& path) const {
	const auto normalized = normalize_path(path);

	if (normalized.is_absolute() || normalized.empty())
		return normalized;

	if (normalized.begin() != normalized.end() && normalized.begin()->string() == k_editor_prefix)
		return m_editor.resolve(normalized);

	return m_project.resolve_project_path(normalized);
}

std::filesystem::path get_repo_source_root() {
#ifdef ORIGO_SOURCE_ROOT
	return normalize_path(ORIGO_SOURCE_ROOT);
#else
	return normalize_path(std::filesystem::current_path());
#endif
}

std::filesystem::path get_default_editor_workspace_root() {
	return env_or_default("ORIGO_EDITOR_WORKSPACE_ROOT", get_repo_source_root() / "editor" / "workspace");
}

EditorResourcePathContext make_default_editor_resource_context() {
	return {
		.Root = env_or_default("ORIGO_EDITOR_RESOURCE_ROOT", get_repo_source_root() / "editor" / "resources")
	};
}

ProjectPathContext make_default_project_path_context() {
	return {
		.Root = env_or_default("ORIGO_PROJECT_ROOT", get_repo_source_root() / "samples" / "sample_project")
	};
}

}
