#pragma once

namespace Origo {

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
Ref<T> make_ref(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
};

template <typename T>
using WeakRef = std::weak_ptr<T>;

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
Scope<T> make_scope(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
};

using Vec3 = glm::vec3;
using Vec2 = glm::vec2;

};
