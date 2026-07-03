#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <functional>
#include <optional>
#include <span>
#include <sstream>
#include <unordered_map>

#include "origo/scripting/ScriptExecutionSystem.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"
#include "origo/components/BoxColliderComponent.h"
#include "origo/components/MeshRendererComponent.h"
#include "origo/components/NameComponent.h"
#include "origo/components/RigidbodyComponent.h"
#include "origo/components/TransformComponent.h"
#include "origo/core/Random.h"
#include "origo/core/Logger.h"
#include "origo/input/Input.h"
#include "origo/physics/PhysicsWorld.h"
#include "origo/scene/Scene.h"
#include "origo/scripting/ComponentRegistry.h"
#include "origo/scripting/ScriptRegistry.h"
#include "origo/scripting/ScriptSystem.h"

#include "Runtime.h"
#include "module/Module.h"
#include "parser/expression/NativeObject.h"

namespace Origo {

namespace {
	std::string trim(std::string_view value) {
		std::size_t start = 0;
		while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])))
			++start;

		std::size_t end = value.size();
		while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
			--end;

		return std::string { value.substr(start, end - start) };
	}

	std::vector<std::string> split_top_level_args(std::string_view value) {
		std::vector<std::string> args;
		std::string current;
		int depth = 0;
		for (char c : value) {
			if (c == '(')
				++depth;
			else if (c == ')')
				--depth;

			if (c == ',' && depth == 0) {
				args.push_back(trim(current));
				current.clear();
				continue;
			}
			current.push_back(c);
		}
		if (!current.empty())
			args.push_back(trim(current));
		return args;
	}

	std::size_t find_matching(std::string_view source, std::size_t open, char open_char, char close_char) {
		int depth = 0;
		for (std::size_t i = open; i < source.size(); ++i) {
			if (source[i] == open_char) {
				++depth;
			} else if (source[i] == close_char) {
				--depth;
				if (depth == 0)
					return i;
			}
		}
		return std::string_view::npos;
	}

	std::string next_statement(std::string_view source, std::size_t& pos) {
		while (pos < source.size() && std::isspace(static_cast<unsigned char>(source[pos])))
			++pos;

		const std::size_t start = pos;
		int brace_depth = 0;
		while (pos < source.size()) {
			char c = source[pos];
			if (c == '{') {
				++brace_depth;
			} else if (c == '}') {
				--brace_depth;
				++pos;
				if (brace_depth <= 0)
					break;
				continue;
			} else if (c == ';' && brace_depth == 0) {
				std::string statement = trim(source.substr(start, pos - start));
				++pos;
				return statement;
			}
			++pos;
		}

		return trim(source.substr(start, pos - start));
	}

	class NumericExpressionParser {
	public:
		NumericExpressionParser(std::string_view source, const ScriptInstance& instance, const Scene* scene, float dt)
		    : m_source(source)
		    , m_instance(instance)
		    , m_scene(scene)
		    , m_dt(dt) { }

		float parse() {
			float value = expression();
			skip_ws();
			if (m_pos < m_source.size() && m_error.empty())
				m_error = "Unexpected token in expression '" + std::string { m_source } + "'";
			return value;
		}

		const std::string& error() const { return m_error; }

	private:
		float expression() {
			float value = term();
			while (true) {
				skip_ws();
				if (match('+')) {
					value += term();
				} else if (match('-')) {
					value -= term();
				} else {
					return value;
				}
			}
		}

		float term() {
			float value = factor();
			while (true) {
				skip_ws();
				if (match('*')) {
					value *= factor();
				} else if (match('/')) {
					float rhs = factor();
					if (rhs != 0.0f)
						value /= rhs;
				} else {
					return value;
				}
			}
		}

		float factor() {
			skip_ws();
			if (match('(')) {
				float value = expression();
				match(')');
				return value;
			}
			if (match('-'))
				return -factor();
			if (peek_digit())
				return number();
			std::string name = identifier();
			if (name.empty()) {
				m_error = "Expected number or identifier in expression '" + std::string { m_source } + "'";
				return 0.0f;
			}
			return identifier_value(name);
		}

		float number() {
			std::size_t start = m_pos;
			while (m_pos < m_source.size() && (std::isdigit(static_cast<unsigned char>(m_source[m_pos])) || m_source[m_pos] == '.'))
				++m_pos;
			return std::stof(std::string { m_source.substr(start, m_pos - start) });
		}

		std::string identifier() {
			std::size_t start = m_pos;
			while (m_pos < m_source.size() && (std::isalnum(static_cast<unsigned char>(m_source[m_pos])) || m_source[m_pos] == '_'))
				++m_pos;
			return std::string { m_source.substr(start, m_pos - start) };
		}

		float identifier_value(const std::string& name) const {
			if (name == "dt")
				return m_dt;

			if (m_instance.Descriptor && m_instance.Descriptor->ComponentType.Type != UUID::bad()) {
				if (m_scene) {
					if (const auto* field = m_scene->find_component_data_field(
					        m_instance.Entity,
					        m_instance.Descriptor->ComponentType.Type,
					        name))
						return field_value_as_number(*field);
				}
				m_error = "Unknown component field '" + name + "' in script '" + m_instance.Descriptor->Name + "'";
				return 0.0f;
			}

			for (const auto& field : m_instance.Fields) {
				if (field.Name != name)
					continue;
				return field_value_as_number(field);
			}
			if (m_scene) {
				if (const auto* field = m_scene->find_component_data_field(m_instance.Entity, name))
					return field_value_as_number(*field);
			}
			if (m_instance.Descriptor)
				m_error = "Unknown field or variable '" + name + "' in script '" + m_instance.Descriptor->Name + "'";
			return 0.0f;
		}

		static float field_value_as_number(const ReflectedFieldValue& field) {
			int int_value {};
			if (field.Value.TryGetAsInt(int_value))
				return static_cast<float>(int_value);
			float float_value {};
			if (field.Value.TryGetAsFloat(float_value))
				return float_value;
			bool bool_value {};
			if (field.Value.TryGetAsBool(bool_value))
				return bool_value ? 1.0f : 0.0f;
			return 0.0f;
		}

		void skip_ws() {
			while (m_pos < m_source.size() && std::isspace(static_cast<unsigned char>(m_source[m_pos])))
				++m_pos;
		}

		bool match(char c) {
			skip_ws();
			if (m_pos < m_source.size() && m_source[m_pos] == c) {
				++m_pos;
				return true;
			}
			return false;
		}

		bool peek_digit() const {
			return m_pos < m_source.size() && (std::isdigit(static_cast<unsigned char>(m_source[m_pos])) || m_source[m_pos] == '.');
		}

		std::string_view m_source;
		const ScriptInstance& m_instance;
		const Scene* m_scene {};
		float m_dt {};
		std::size_t m_pos {};
		mutable std::string m_error {};
	};

	struct EvalNumberResult {
		float Value {};
		std::string Error {};
	};

	EvalNumberResult eval_number(std::string_view source, const ScriptInstance& instance, const Scene* scene, float dt) {
		try {
			NumericExpressionParser parser(source, instance, scene, dt);
			float value = parser.parse();
			return EvalNumberResult { .Value = value, .Error = parser.error() };
		} catch (const std::exception& e) {
			return EvalNumberResult { .Value = 0.0f, .Error = e.what() };
		}
	}

	std::string strip_string_literal(std::string value) {
		value = trim(value);
		if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
			return value.substr(1, value.size() - 2);
		return value;
	}

	std::string escape_noct_string(std::string_view value) {
		std::string out;
		out.reserve(value.size());
		for (char c : value) {
			if (c == '\\' || c == '"')
				out.push_back('\\');
			out.push_back(c);
		}
		return out;
	}

	class IEntityReferenceNativeObject {
	public:
		virtual ~IEntityReferenceNativeObject() = default;
		virtual RID id() const = 0;
	};

	Noct::NativeObjectRef make_entity_object(Scene& scene, RID entity, std::string script_name, std::function<void(std::string, std::string)> warn_once);

	std::string noct_value_type_name(const Noct::NoctObject& value) {
		if (std::holds_alternative<std::monostate>(value))
			return "None";
		if (std::holds_alternative<double>(value))
			return "Number";
		if (std::holds_alternative<bool>(value))
			return "Bool";
		if (std::holds_alternative<std::string>(value))
			return "String";
		if (std::holds_alternative<Noct::CallableRef>(value))
			return "Callable";
		if (const auto* native = std::get_if<Noct::NativeObjectRef>(&value); native && *native)
			return std::string { (*native)->TypeName() };
		return "Unknown";
	}

	Noct::NoctObject noct_value_from_field(
	    const ReflectedFieldValue& field,
	    Scene* scene = nullptr,
	    std::string script_name = {},
	    std::function<void(std::string, std::string)> warn_once = {}) {
		int int_value {};
		if (field.Value.TryGetAsInt(int_value))
			return static_cast<double>(int_value);
		float float_value {};
		if (field.Value.TryGetAsFloat(float_value))
			return static_cast<double>(float_value);
		bool bool_value {};
		if (field.Value.TryGetAsBool(bool_value))
			return bool_value;
		std::string string_value;
		if (field.Value.TryGetAsString(string_value))
			return string_value;
		RID entity_value {};
		if (field.Value.TryGetAsEntity(entity_value)) {
			if (!scene || entity_value.get_id() < 0 || !scene->has_entity(entity_value))
				return std::monostate {};
			return make_entity_object(*scene, entity_value, std::move(script_name), std::move(warn_once));
		}
		return std::monostate {};
	}

	std::string assign_noct_value_to_field(ReflectedFieldValue& field, const Noct::NoctObject& value) {
		switch (field.Value.get_type()) {
		case VariantType::Int:
			if (const auto* number = std::get_if<double>(&value)) {
				field.Value = Variant(static_cast<int>(*number));
				return {};
			}
			break;
		case VariantType::Float:
			if (const auto* number = std::get_if<double>(&value)) {
				field.Value = Variant(static_cast<float>(*number));
				return {};
			}
			break;
		case VariantType::Bool:
			if (const auto* boolean = std::get_if<bool>(&value)) {
				field.Value = Variant(*boolean);
				return {};
			}
			break;
		case VariantType::String:
			if (const auto* string = std::get_if<std::string>(&value)) {
				field.Value = Variant(*string);
				return {};
			}
			break;
		case VariantType::Entity:
			if (std::holds_alternative<std::monostate>(value)) {
				field.Value = Variant(RID {});
				return {};
			}
			if (const auto* native = std::get_if<Noct::NativeObjectRef>(&value); native && *native) {
				auto entity = std::dynamic_pointer_cast<IEntityReferenceNativeObject>(*native);
				if (entity) {
					field.Value = Variant(entity->id());
					return {};
				}
			}
			return "Cannot assign " + noct_value_type_name(value) + " to Entity field '" + field.Name + "'.";
		case VariantType::Enum:
			if (const auto* number = std::get_if<double>(&value)) {
				field.Value = Variant(static_cast<int>(*number), VariantType::Enum);
				return {};
			}
			break;
		case VariantType::Vec2:
		case VariantType::Vec3:
		case VariantType::Vec4:
		case VariantType::Asset:
		case VariantType::Struct:
			return "Field '" + field.Name + "' is not currently assignable from Noct scalar values.";
		}

		return "Cannot assign incompatible Noct value to field '" + field.Name + "'.";
	}

	Noct::NativeObjectRef make_vector_object(const Vec3& value);
	Noct::NativeObjectRef make_vector_object(const Vec2& value);

	Vec3 require_vec3(Noct::NativeCallContext& ctx, std::size_t offset = 0) {
		return Vec3 {
			static_cast<float>(ctx.require_number(offset)),
			static_cast<float>(ctx.require_number(offset + 1)),
			static_cast<float>(ctx.require_number(offset + 2)),
		};
	}

	std::string normalize_name(std::string value) {
		std::ranges::transform(value, value.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		return value;
	}

	KeyboardKey keyboard_key_from_name(const std::string& raw) {
		const std::string name = normalize_name(raw);
		static const std::unordered_map<std::string, KeyboardKey> keys {
			{ "a", KeyboardKey::KEY_A },
			{ "b", KeyboardKey::KEY_B },
			{ "c", KeyboardKey::KEY_C },
			{ "d", KeyboardKey::KEY_D },
			{ "e", KeyboardKey::KEY_E },
			{ "f", KeyboardKey::KEY_F },
			{ "g", KeyboardKey::KEY_G },
			{ "h", KeyboardKey::KEY_H },
			{ "i", KeyboardKey::KEY_I },
			{ "j", KeyboardKey::KEY_J },
			{ "k", KeyboardKey::KEY_K },
			{ "l", KeyboardKey::KEY_L },
			{ "m", KeyboardKey::KEY_M },
			{ "n", KeyboardKey::KEY_N },
			{ "o", KeyboardKey::KEY_O },
			{ "p", KeyboardKey::KEY_P },
			{ "q", KeyboardKey::KEY_Q },
			{ "r", KeyboardKey::KEY_R },
			{ "s", KeyboardKey::KEY_S },
			{ "t", KeyboardKey::KEY_T },
			{ "u", KeyboardKey::KEY_U },
			{ "v", KeyboardKey::KEY_V },
			{ "w", KeyboardKey::KEY_W },
			{ "x", KeyboardKey::KEY_X },
			{ "y", KeyboardKey::KEY_Y },
			{ "z", KeyboardKey::KEY_Z },
			{ "0", KeyboardKey::KEY_0 },
			{ "1", KeyboardKey::KEY_1 },
			{ "2", KeyboardKey::KEY_2 },
			{ "3", KeyboardKey::KEY_3 },
			{ "4", KeyboardKey::KEY_4 },
			{ "5", KeyboardKey::KEY_5 },
			{ "6", KeyboardKey::KEY_6 },
			{ "7", KeyboardKey::KEY_7 },
			{ "8", KeyboardKey::KEY_8 },
			{ "9", KeyboardKey::KEY_9 },
			{ "space", KeyboardKey::KEY_SPACE },
			{ "enter", KeyboardKey::KEY_ENTER },
			{ "escape", KeyboardKey::KEY_ESCAPE },
			{ "esc", KeyboardKey::KEY_ESCAPE },
			{ "tab", KeyboardKey::KEY_TAB },
			{ "backspace", KeyboardKey::KEY_BACKSPACE },
			{ "shift", KeyboardKey::KEY_LEFT_SHIFT },
			{ "leftshift", KeyboardKey::KEY_LEFT_SHIFT },
			{ "rightshift", KeyboardKey::KEY_RIGHT_SHIFT },
			{ "ctrl", KeyboardKey::KEY_LEFT_CONTROL },
			{ "control", KeyboardKey::KEY_LEFT_CONTROL },
			{ "leftctrl", KeyboardKey::KEY_LEFT_CONTROL },
			{ "rightctrl", KeyboardKey::KEY_RIGHT_CONTROL },
			{ "alt", KeyboardKey::KEY_LEFT_ALT },
			{ "leftalt", KeyboardKey::KEY_LEFT_ALT },
			{ "rightalt", KeyboardKey::KEY_RIGHT_ALT },
			{ "up", KeyboardKey::KEY_UP },
			{ "down", KeyboardKey::KEY_DOWN },
			{ "left", KeyboardKey::KEY_LEFT },
			{ "right", KeyboardKey::KEY_RIGHT },
			{ "arrowup", KeyboardKey::KEY_UP },
			{ "arrowdown", KeyboardKey::KEY_DOWN },
			{ "arrowleft", KeyboardKey::KEY_LEFT },
			{ "arrowright", KeyboardKey::KEY_RIGHT },
		};

		if (const auto it = keys.find(name); it != keys.end())
			return it->second;
		return KeyboardKey::KEY_UNKNOWN;
	}

	Input::MouseButton mouse_button_from_name(const std::string& raw) {
		const std::string name = normalize_name(raw);
		if (name == "left")
			return Input::MouseButton::Left;
		if (name == "right")
			return Input::MouseButton::Right;
		if (name == "middle")
			return Input::MouseButton::Middle;
		return Input::MouseButton::Unknown;
	}

	std::optional<AssetHandle> primitive_mesh_from_name(const std::string& raw) {
		const std::string name = normalize_name(raw);
		auto& primitives = PrimitiveShapeCache::get_instance();
		if (name == "cube" || name == "box")
			return primitives.get_cube_mesh();
		if (name == "sphere")
			return primitives.get_sphere_mesh();
		if (name == "cone")
			return primitives.get_cone_mesh();
		if (name == "wirecube" || name == "wire_cube")
			return primitives.get_wire_cube_mesh();
		if (name == "wirecone" || name == "wire_cone")
			return primitives.get_wire_cone_mesh();
		return std::nullopt;
	}

	class VectorNativeObject final : public Noct::INativeObject {
	public:
		explicit VectorNativeObject(Vec3 value)
		    : m_value(value) { }

		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "x")
				return static_cast<double>(m_value.x);
			if (name.Lexeme == "y")
				return static_cast<double>(m_value.y);
			if (name.Lexeme == "z")
				return static_cast<double>(m_value.z);
			throw Noct::RuntimeError(name, "Vector has no field '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject& value) override {
			const auto* number = std::get_if<double>(&value);
			if (!number)
				throw Noct::RuntimeError(name, "Vector fields require numeric values.");
			if (name.Lexeme == "x")
				m_value.x = static_cast<float>(*number);
			else if (name.Lexeme == "y")
				m_value.y = static_cast<float>(*number);
			else if (name.Lexeme == "z")
				m_value.z = static_cast<float>(*number);
			else
				throw Noct::RuntimeError(name, "Vector has no field '" + name.Lexeme + "'.");
		}

		std::string_view TypeName() const override { return "Vector3"; }
		std::string ToString() const override {
			return "<vec3 " + std::to_string(m_value.x) + ", " + std::to_string(m_value.y) + ", " + std::to_string(m_value.z) + ">";
		}

	private:
		Vec3 m_value {};
	};

	Noct::NativeObjectRef make_vector_object(const Vec3& value) {
		return Noct::NativeObjectRef { std::make_shared<VectorNativeObject>(value) };
	}

	Noct::NativeObjectRef make_vector_object(const Vec2& value) {
		return make_vector_object(Vec3 { value.x, value.y, 0.0f });
	}

	class ComponentTypeNativeObject final : public Noct::INativeObject {
	public:
		ComponentTypeNativeObject(ComponentTypeID id, std::string name)
		    : m_id(id)
		    , m_name(std::move(name)) { }

		ComponentTypeID id() const { return m_id; }
		std::string_view name() const { return m_name; }

		Noct::NoctObject Get(const Noct::Token& name) override {
			throw Noct::RuntimeError(name, "Component type references do not expose properties.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Component type references are read-only.");
		}

		std::string_view TypeName() const override { return "ComponentType"; }
		std::string ToString() const override { return "<component type " + m_name + ">"; }

	private:
		ComponentTypeID m_id { UUID::bad() };
		std::string m_name {};
	};

	std::shared_ptr<ComponentTypeNativeObject> require_component_type(Noct::NativeCallContext& ctx, std::string_view api_name) {
		const auto* native = std::get_if<Noct::NativeObjectRef>(&ctx.argument(0));
		if (!native || !*native)
			throw Noct::RuntimeError(std::string { api_name } + " expects a component type reference.");

		auto type = std::dynamic_pointer_cast<ComponentTypeNativeObject>(*native);
		if (!type)
			throw Noct::RuntimeError(std::string { api_name } + " expects a component type reference.");
		return type;
	}

	class ComponentInstanceNativeObject final : public Noct::INativeObject {
	public:
		ComponentInstanceNativeObject(Scene& scene, RID entity, ComponentTypeID type, std::string name)
		    : m_scene(scene)
		    , m_entity(entity)
		    , m_type(type)
		    , m_name(std::move(name)) { }

		Noct::NoctObject Get(const Noct::Token& name) override {
			auto* field = m_scene.find_component_data_field(m_entity, m_type, name.Lexeme);
			if (!field) {
				throw Noct::RuntimeError(name, "Component '" + m_name + "' has no field '" + name.Lexeme + "'.");
			}
			return noct_value_from_field(*field, &m_scene, m_name, {});
		}

		void Set(const Noct::Token& name, const Noct::NoctObject& value) override {
			auto* field = m_scene.find_component_data_field(m_entity, m_type, name.Lexeme);
			if (!field) {
				throw Noct::RuntimeError(name, "Component '" + m_name + "' has no field '" + name.Lexeme + "'.");
			}

			const std::string error = assign_noct_value_to_field(*field, value);
			if (!error.empty())
				throw Noct::RuntimeError(name, error);
		}

		std::string_view TypeName() const override { return "ComponentInstance"; }
		std::string ToString() const override { return "<component " + m_name + ">"; }

	private:
		Scene& m_scene;
		RID m_entity {};
		ComponentTypeID m_type { UUID::bad() };
		std::string m_name {};
	};

	class EntityNativeObject final : public Noct::INativeObject, public IEntityReferenceNativeObject {
	public:
		EntityNativeObject(Scene& scene, RID entity, std::string script_name, std::function<void(std::string, std::string)> warn_once)
		    : m_scene(scene)
		    , m_entity(entity)
		    , m_script_name(std::move(script_name))
		    , m_warn_once(std::move(warn_once)) { }

		RID id() const { return m_entity; }

		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "position")
				return make_callable("entity.position", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* transform = transform_or_warn("position", false);
					return transform ? Noct::NoctObject { make_vector_object(transform->get_position()) } : Noct::NoctObject { std::monostate {} };
				});
			if (name.Lexeme == "rotation")
				return make_callable("entity.rotation", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* transform = transform_or_warn("rotation", false);
					return transform ? Noct::NoctObject { make_vector_object(transform->get_rotation()) } : Noct::NoctObject { std::monostate {} };
				});
			if (name.Lexeme == "scale")
				return make_callable("entity.scale", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* transform = transform_or_warn("scale", false);
					return transform ? Noct::NoctObject { make_vector_object(transform->get_scale()) } : Noct::NoctObject { std::monostate {} };
				});
			if (name.Lexeme == "forward")
				return make_callable("entity.forward", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* transform = transform_or_warn("forward", false);
					return transform ? Noct::NoctObject { make_vector_object(transform->get_forward()) } : Noct::NoctObject { std::monostate {} };
				});
			if (name.Lexeme == "right")
				return make_callable("entity.right", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* transform = transform_or_warn("right", false);
					return transform ? Noct::NoctObject { make_vector_object(transform->get_right()) } : Noct::NoctObject { std::monostate {} };
				});
			if (name.Lexeme == "up")
				return make_callable("entity.up", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* transform = transform_or_warn("up", false);
					return transform ? Noct::NoctObject { make_vector_object(transform->get_up()) } : Noct::NoctObject { std::monostate {} };
				});
			if (name.Lexeme == "set_position")
				return make_callable("entity.set_position", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* transform = transform_or_warn("set_position", true))
						transform->set_position(require_vec3(ctx));
					return std::monostate {};
				});
			if (name.Lexeme == "set_rotation")
				return make_callable("entity.set_rotation", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* transform = transform_or_warn("set_rotation", true))
						transform->set_rotation(require_vec3(ctx));
					return std::monostate {};
				});
			if (name.Lexeme == "set_scale")
				return make_callable("entity.set_scale", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* transform = transform_or_warn("set_scale", true))
						transform->set_scale(require_vec3(ctx));
					return std::monostate {};
				});
			if (name.Lexeme == "translate")
				return make_callable("entity.translate", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* transform = transform_or_warn("translate", true))
						transform->translate(require_vec3(ctx));
					return std::monostate {};
				});
			if (name.Lexeme == "rotate")
				return make_callable("entity.rotate", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* transform = transform_or_warn("rotate", true))
						transform->rotate(require_vec3(ctx));
					return std::monostate {};
				});

			if (name.Lexeme == "get_component") {
				Noct::CallableRef callable = std::make_shared<Noct::NativeContextFunction>(
				    "entity.get_component",
				    [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					    auto type = require_component_type(ctx, "entity.get_component");

					    if (!m_scene.has_component_data(m_entity, type->id()))
						    return std::monostate {};

					    return Noct::NativeObjectRef {
						    std::make_shared<ComponentInstanceNativeObject>(
						        m_scene,
						        m_entity,
						        type->id(),
						        std::string { type->name() }) };
				    });
				return callable;
			}
			if (name.Lexeme == "has_component")
				return make_callable("entity.has_component", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					auto type = require_component_type(ctx, "entity.has_component");
					return m_scene.has_component_data(m_entity, type->id());
				});
			if (name.Lexeme == "add_component")
				return make_callable("entity.add_component", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					auto type = require_component_type(ctx, "entity.add_component");
					if (!m_scene.has_component_data(m_entity, type->id()))
						m_scene.add_component_data(m_entity, type->id());
					return Noct::NativeObjectRef {
						std::make_shared<ComponentInstanceNativeObject>(m_scene, m_entity, type->id(), std::string { type->name() })
					};
				});
			if (name.Lexeme == "remove_component")
				return make_callable("entity.remove_component", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					auto type = require_component_type(ctx, "entity.remove_component");
					return m_scene.remove_component_data_if_exists(m_entity, type->id());
				});

			if (name.Lexeme == "has_rigidbody")
				return make_callable("entity.has_rigidbody", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					return m_scene.has_native_component<RigidbodyComponent>(m_entity);
				});
			if (name.Lexeme == "has_collider")
				return make_callable("entity.has_collider", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					return m_scene.has_native_component<BoxColliderComponent>(m_entity);
				});
			if (name.Lexeme == "velocity")
				return make_callable("entity.velocity", [this](Noct::NativeCallContext&) -> Noct::NoctObject {
					auto* rb = rigidbody_or_warn("velocity", false);
					if (!rb || !rb->HasBody || !m_scene.get_physics_world().has_body(m_entity))
						return std::monostate {};
					const JPH::Vec3 velocity = m_scene.get_physics_world().get_body_interface().GetLinearVelocity(rb->RuntimeBodyID);
					return make_vector_object(Vec3 { velocity.GetX(), velocity.GetY(), velocity.GetZ() });
				});
			if (name.Lexeme == "set_velocity")
				return make_callable("entity.set_velocity", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* rb = rigidbody_or_warn("set_velocity", true); rb && rb->HasBody && m_scene.get_physics_world().has_body(m_entity)) {
						const Vec3 value = require_vec3(ctx);
						m_scene.get_physics_world().get_body_interface().SetLinearVelocity(rb->RuntimeBodyID, JPH::Vec3 { value.x, value.y, value.z });
					}
					return std::monostate {};
				});
			if (name.Lexeme == "apply_force")
				return make_callable("entity.apply_force", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* rb = rigidbody_or_warn("apply_force", true); rb && rb->HasBody && m_scene.get_physics_world().has_body(m_entity)) {
						const Vec3 value = require_vec3(ctx);
						m_scene.get_physics_world().get_body_interface().AddForce(rb->RuntimeBodyID, JPH::Vec3 { value.x, value.y, value.z });
					}
					return std::monostate {};
				});
			if (name.Lexeme == "apply_impulse")
				return make_callable("entity.apply_impulse", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* rb = rigidbody_or_warn("apply_impulse", true); rb && rb->HasBody && m_scene.get_physics_world().has_body(m_entity)) {
						const Vec3 value = require_vec3(ctx);
						m_scene.get_physics_world().get_body_interface().AddImpulse(rb->RuntimeBodyID, JPH::Vec3 { value.x, value.y, value.z });
					}
					return std::monostate {};
				});
			if (name.Lexeme == "set_gravity_enabled")
				return make_callable("entity.set_gravity_enabled", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* rb = rigidbody_or_warn("set_gravity_enabled", true)) {
						rb->UseGravity = ctx.require_bool(0);
						rb->RuntimeUseGravity = rb->UseGravity;
					}
					return std::monostate {};
				});
			if (name.Lexeme == "set_kinematic")
				return make_callable("entity.set_kinematic", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* rb = rigidbody_or_warn("set_kinematic", true))
						rb->MotionType = ctx.require_bool(0) ? RigidbodyMotionType::Kinematic : RigidbodyMotionType::Dynamic;
					return std::monostate {};
				});
			if (name.Lexeme == "set_trigger")
				return make_callable("entity.set_trigger", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					if (auto* rb = rigidbody_or_warn("set_trigger", true))
						rb->IsSensor = ctx.require_bool(0);
					return std::monostate {};
				});
			if (name.Lexeme == "play_sound")
				return make_callable("entity.play_sound", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					warn_once("audio-play-sound",
					    "[Noct] Script '" + m_script_name + "' called entity.play_sound('" + ctx.require_string(0) + "'), but script audio playback is not wired yet.");
					return std::monostate {};
				});

			throw Noct::RuntimeError(name, "Entity has no function or property '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Entity properties are read-only.");
		}

		std::string_view TypeName() const override { return "Entity"; }
		std::string ToString() const override { return "<entity " + m_entity.to_string() + ">"; }

	private:
		static Noct::CallableRef make_callable(std::string name, Noct::NativeContextFunction::Handler handler) {
			return std::make_shared<Noct::NativeContextFunction>(std::move(name), std::move(handler));
		}

		TransformComponent* transform_or_warn(std::string_view api_name, bool mutating) {
			auto* transform = m_scene.get_native_component<TransformComponent>(m_entity);
			if (!transform && mutating) {
				warn_once("missing-transform:" + std::string { api_name },
				    "[Noct] Script '" + m_script_name + "' called entity." + std::string { api_name }
				        + ", but entity " + m_entity.to_string() + " has no TransformComponent.");
			}
			return transform;
		}

		RigidbodyComponent* rigidbody_or_warn(std::string_view api_name, bool mutating) {
			auto* rigidbody = m_scene.get_native_component<RigidbodyComponent>(m_entity);
			if (!rigidbody && mutating) {
				warn_once("missing-rigidbody:" + std::string { api_name },
				    "[Noct] Script '" + m_script_name + "' called entity." + std::string { api_name }
				        + ", but entity " + m_entity.to_string() + " has no RigidbodyComponent.");
			}
			return rigidbody;
		}

		void warn_once(const std::string& key, const std::string& message) {
			if (m_warn_once)
				m_warn_once(key, message);
		}

	private:
		Scene& m_scene;
		RID m_entity {};
		std::string m_script_name {};
		std::function<void(std::string, std::string)> m_warn_once {};
	};

	Noct::NativeObjectRef make_entity_object(Scene& scene, RID entity, std::string script_name, std::function<void(std::string, std::string)> warn_once) {
		return Noct::NativeObjectRef {
			std::make_shared<EntityNativeObject>(scene, entity, std::move(script_name), std::move(warn_once))
		};
	}

	class EntityCollectionNativeObject final : public Noct::INativeObject {
	public:
		EntityCollectionNativeObject(Scene& scene, std::vector<RID> entities, std::string script_name, std::function<void(std::string, std::string)> warn_once)
		    : m_scene(scene)
		    , m_entities(std::move(entities))
		    , m_script_name(std::move(script_name))
		    , m_warn_once(std::move(warn_once)) { }

		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "count")
				return static_cast<double>(m_entities.size());
			if (name.Lexeme == "get") {
				Noct::CallableRef callable = std::make_shared<Noct::NativeContextFunction>(
				    "EntityCollection.get",
				    [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					    const auto index = static_cast<std::size_t>(ctx.require_number(0));
					    if (index >= m_entities.size())
						    return std::monostate {};
					    RID entity = m_entities[index];
					    if (!m_scene.has_entity(entity))
						    return std::monostate {};
					    return make_entity_object(m_scene, entity, m_script_name, m_warn_once);
				    });
				return callable;
			}
			throw Noct::RuntimeError(name, "Entity collection has no property '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Entity collections are read-only.");
		}

		std::string_view TypeName() const override { return "EntityCollection"; }
		std::string ToString() const override { return "<entity collection>"; }

	private:
		Scene& m_scene;
		std::vector<RID> m_entities {};
		std::string m_script_name {};
		std::function<void(std::string, std::string)> m_warn_once {};
	};

	class SceneNativeObject final : public Noct::INativeObject {
	public:
		SceneNativeObject(Scene& scene, std::string script_name, std::function<void(std::string, std::string)> warn_once)
		    : m_scene(scene)
		    , m_script_name(std::move(script_name))
		    , m_warn_once(std::move(warn_once)) { }

		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "find_entity")
				return make_callable("scene.find_entity", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					const std::string target = ctx.require_string(0);
					for (RID entity : m_scene.get_entities()) {
						if (const auto* name = m_scene.get_native_component<NameComponent>(entity); name && name->Name == target)
							return make_entity_object(m_scene, entity, m_script_name, m_warn_once);
					}
					return std::monostate {};
				});
			if (name.Lexeme == "find_entities_with")
				return make_callable("scene.find_entities_with", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					auto type = require_component_type(ctx, "scene.find_entities_with");
					std::vector<RID> matches;
					for (RID entity : m_scene.get_entities()) {
						if (m_scene.has_component_data(entity, type->id()))
							matches.push_back(entity);
					}
					return Noct::NativeObjectRef {
						std::make_shared<EntityCollectionNativeObject>(m_scene, std::move(matches), m_script_name, m_warn_once)
					};
				});
			if (name.Lexeme == "create_entity")
				return make_callable("scene.create_entity", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					RID entity = m_scene.create_entity(ctx.require_string(0));
					return make_entity_object(m_scene, entity, m_script_name, m_warn_once);
				});
			if (name.Lexeme == "create_primitive")
				return make_callable("scene.create_primitive", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					const std::string kind = ctx.require_string(0);
					const std::string name = ctx.argument_count() > 1 ? ctx.require_string(1) : kind;
					auto mesh = primitive_mesh_from_name(kind);
					if (!mesh) {
						if (m_warn_once)
							m_warn_once("unknown-primitive:" + kind, "[Noct] scene.create_primitive unknown primitive type '" + kind + "'.");
						return std::monostate {};
					}

					RID entity = m_scene.create_entity(name);
					auto* renderer = m_scene.get_native_component<MeshRendererComponent>(entity);
					if (!renderer)
						renderer = m_scene.add_native_component<MeshRendererComponent>(entity);
					renderer->MeshHandle = *mesh;
					renderer->MaterialHandle = DefaultAssetCache::get_instance().get_material();
					return make_entity_object(m_scene, entity, m_script_name, m_warn_once);
				});
			if (name.Lexeme == "destroy_entity")
				return make_callable("scene.destroy_entity", [this](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					const auto* native = std::get_if<Noct::NativeObjectRef>(&ctx.argument(0));
					if (!native || !*native)
						return false;
					auto entity = std::dynamic_pointer_cast<EntityNativeObject>(*native);
					if (!entity)
						return false;
					m_scene.schedule_remove_entity(entity->id());
					return true;
				});
			throw Noct::RuntimeError(name, "Scene has no function or property '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Scene properties are read-only.");
		}

		std::string_view TypeName() const override { return "Scene"; }
		std::string ToString() const override { return "<scene>"; }

	private:
		static Noct::CallableRef make_callable(std::string name, Noct::NativeContextFunction::Handler handler) {
			return std::make_shared<Noct::NativeContextFunction>(std::move(name), std::move(handler));
		}

		Scene& m_scene;
		std::string m_script_name {};
		std::function<void(std::string, std::string)> m_warn_once {};
	};

	class TimeNativeObject final : public Noct::INativeObject {
	public:
		TimeNativeObject(float delta, float fixed_delta, float elapsed, std::uint64_t frame)
		    : m_delta(delta)
		    , m_fixed_delta(fixed_delta)
		    , m_elapsed(elapsed)
		    , m_frame(frame) { }

		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "delta")
				return number_callable("Time.delta", m_delta);
			if (name.Lexeme == "fixed_delta")
				return number_callable("Time.fixed_delta", m_fixed_delta);
			if (name.Lexeme == "elapsed")
				return number_callable("Time.elapsed", m_elapsed);
			if (name.Lexeme == "frame")
				return number_callable("Time.frame", static_cast<double>(m_frame));
			throw Noct::RuntimeError(name, "Time has no function or property '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Time is read-only.");
		}

		std::string_view TypeName() const override { return "Time"; }
		std::string ToString() const override { return "<Time>"; }

	private:
		static Noct::CallableRef number_callable(std::string name, double value) {
			return std::make_shared<Noct::NativeContextFunction>(std::move(name), [value](Noct::NativeCallContext&) -> Noct::NoctObject {
				return value;
			});
		}

		float m_delta {};
		float m_fixed_delta {};
		float m_elapsed {};
		std::uint64_t m_frame {};
	};

	class RandomNativeObject final : public Noct::INativeObject {
	public:
		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "float")
				return callable("Random.float", [](Noct::NativeCallContext&) -> Noct::NoctObject {
					return static_cast<double>(Random::float01());
				});
			if (name.Lexeme == "range")
				return callable("Random.range", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					return static_cast<double>(Random::range(static_cast<float>(ctx.require_number(0)), static_cast<float>(ctx.require_number(1))));
				});
			if (name.Lexeme == "int")
				return callable("Random.int", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					return static_cast<double>(Random::range(static_cast<int>(ctx.require_number(0)), static_cast<int>(ctx.require_number(1))));
				});
			if (name.Lexeme == "chance")
				return callable("Random.chance", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					const double probability = std::clamp(ctx.require_number(0), 0.0, 1.0);
					return Random::float01() <= probability;
				});
			if (name.Lexeme == "seed")
				return callable("Random.seed", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					Random::seed(static_cast<int>(ctx.require_number(0)));
					return std::monostate {};
				});
			throw Noct::RuntimeError(name, "Random has no function or property '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Random is read-only.");
		}

		std::string_view TypeName() const override { return "Random"; }
		std::string ToString() const override { return "<Random>"; }

	private:
		static Noct::CallableRef callable(std::string name, Noct::NativeContextFunction::Handler handler) {
			return std::make_shared<Noct::NativeContextFunction>(std::move(name), std::move(handler));
		}
	};

	class InputNativeObject final : public Noct::INativeObject {
	public:
		Noct::NoctObject Get(const Noct::Token& name) override {
			if (name.Lexeme == "key_down" || name.Lexeme == "key_pressed" || name.Lexeme == "key_released") {
				const std::string api = "Input." + name.Lexeme;
				return callable(api, [api](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					const auto key = keyboard_key_from_name(ctx.require_string(0));
					if (key == KeyboardKey::KEY_UNKNOWN)
						return false;
					if (api == "Input.key_down")
						return Input::is_key_down(key);
					if (api == "Input.key_pressed")
						return Input::was_key_pressed(key);
					return Input::was_key_released(key);
				});
			}
			if (name.Lexeme == "mouse_down" || name.Lexeme == "mouse_pressed" || name.Lexeme == "mouse_released")
				return callable("Input." + name.Lexeme, [api = "Input." + name.Lexeme](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					const auto button = mouse_button_from_name(ctx.require_string(0));
					if (button == Input::MouseButton::Unknown)
						return false;
					if (api == "Input.mouse_down")
						return Input::is_mouse_down(button);
					if (api == "Input.mouse_pressed")
						return Input::was_mouse_pressed(button);
					return Input::was_mouse_released(button);
				});
			if (name.Lexeme == "mouse_position")
				return callable("Input.mouse_position", [](Noct::NativeCallContext&) -> Noct::NoctObject {
					return make_vector_object(Input::mouse_position());
				});
			if (name.Lexeme == "mouse_delta")
				return callable("Input.mouse_delta", [](Noct::NativeCallContext&) -> Noct::NoctObject {
					return make_vector_object(Input::mouse_delta());
				});
			if (name.Lexeme == "scroll_delta")
				return callable("Input.scroll_delta", [](Noct::NativeCallContext&) -> Noct::NoctObject {
					return make_vector_object(Input::scroll_delta());
				});
			throw Noct::RuntimeError(name, "Input has no function or property '" + name.Lexeme + "'.");
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, "Input is read-only.");
		}

		std::string_view TypeName() const override { return "Input"; }
		std::string ToString() const override { return "<Input>"; }

	private:
		static Noct::CallableRef callable(std::string name, Noct::NativeContextFunction::Handler handler) {
			return std::make_shared<Noct::NativeContextFunction>(std::move(name), std::move(handler));
		}
	};

	class NoOpServiceNativeObject final : public Noct::INativeObject {
	public:
		NoOpServiceNativeObject(std::string type_name, std::function<void(std::string, std::string)> warn_once)
		    : m_type_name(std::move(type_name))
		    , m_warn_once(std::move(warn_once)) { }

		Noct::NoctObject Get(const Noct::Token& name) override {
			if (m_type_name == "Assets" && name.Lexeme == "exists")
				return callable("Assets.exists", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					return std::filesystem::exists(ctx.require_string(0));
				});
			if (m_type_name == "Assets" && name.Lexeme == "load")
				return callable("Assets.load", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
					auto handle = AssetManager::get_instance().load(ctx.require_string(0));
					return handle.has_value();
				});

			return callable(m_type_name + "." + name.Lexeme, [this, api = m_type_name + "." + name.Lexeme](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
				if (m_warn_once)
					m_warn_once("noop:" + api, "[Noct] " + api + " is not wired to a native backend yet.");
				if (api == "Audio.play" || api == "Audio.stop")
					(void)ctx.require_string(0);
				return std::monostate {};
			});
		}

		void Set(const Noct::Token& name, const Noct::NoctObject&) override {
			throw Noct::RuntimeError(name, m_type_name + " is read-only.");
		}

		std::string_view TypeName() const override { return m_type_name; }
		std::string ToString() const override { return "<" + m_type_name + ">"; }

	private:
		static Noct::CallableRef callable(std::string name, Noct::NativeContextFunction::Handler handler) {
			return std::make_shared<Noct::NativeContextFunction>(std::move(name), std::move(handler));
		}

		std::string m_type_name {};
		std::function<void(std::string, std::string)> m_warn_once {};
	};

	std::vector<ReflectedFieldValue*> writable_fields_for(ScriptInstance& instance, Scene* scene) {
		if (instance.Descriptor && instance.Descriptor->ComponentType.Type != UUID::bad()) {
			if (scene) {
				if (auto* component = scene->get_component_data(instance.Entity, instance.Descriptor->ComponentType.Type)) {
					std::vector<ReflectedFieldValue*> fields;
					fields.reserve(component->Fields.size());
					for (auto& field : component->Fields)
						fields.push_back(&field);
					return fields;
				}
			}
			return {};
		}

		std::vector<ReflectedFieldValue*> fields;
		fields.reserve(instance.Fields.size());
		for (auto& field : instance.Fields)
			fields.push_back(&field);
		return fields;
	}

	ReflectedFieldValue* find_writable_field(std::span<ReflectedFieldValue*> fields, std::string_view name) {
		for (auto* field : fields) {
			if (field && field->Name == name)
				return field;
		}
		return nullptr;
	}

	bool has_any_lifecycle(const ScriptLifecycleBindings& lifecycle) {
		return lifecycle.OnStart != UUID::bad()
		    || lifecycle.OnUpdate != UUID::bad()
		    || lifecycle.OnFixedUpdate != UUID::bad()
		    || lifecycle.OnDestroy != UUID::bad()
		    || lifecycle.OnCollision != UUID::bad()
		    || lifecycle.OnTriggerEnter != UUID::bad()
		    || lifecycle.OnTriggerExit != UUID::bad();
	}

	bool is_assignment_statement(std::string_view statement) {
		std::size_t equal = statement.find('=');
		if (equal == std::string_view::npos)
			return false;

		const char before = equal == 0 ? '\0' : statement[equal - 1];
		const char after = equal + 1 >= statement.size() ? '\0' : statement[equal + 1];
		return before != '=' && before != '!' && before != '<' && before != '>' && after != '=';
	}

	ReflectedFieldValue* find_writable_field(ScriptInstance& instance, Scene* scene, std::string_view name) {
		if (instance.Descriptor && instance.Descriptor->ComponentType.Type != UUID::bad())
			return scene ? scene->find_component_data_field(instance.Entity, instance.Descriptor->ComponentType.Type, name) : nullptr;

		for (auto& field : instance.Fields) {
			if (field.Name == name)
				return &field;
		}
		return nullptr;
	}

	std::string assign_numeric_value(ReflectedFieldValue& field, float value) {
		switch (field.Value.get_type()) {
		case VariantType::Int:
			field.Value = Variant(static_cast<int>(value));
			return {};
		case VariantType::Float:
			field.Value = Variant(value);
			return {};
		case VariantType::Bool:
			field.Value = Variant(value != 0.0f);
			return {};
		case VariantType::String:
		case VariantType::Enum:
		case VariantType::Vec2:
		case VariantType::Vec3:
		case VariantType::Vec4:
		case VariantType::Entity:
		case VariantType::Asset:
		case VariantType::Struct:
			return "Field '" + field.Name + "' is not assignable from a numeric expression.";
		}
		return "Field '" + field.Name + "' has an unknown type.";
	}
}

void ScriptDispatcher::enqueue(ScriptEvent event) {
	m_events.push_back(event);
}

void ScriptDispatcher::clear() {
	m_events.clear();
}

void ScriptDispatcher::dispatch(ScriptExecutionSystem& scripts) {
	for (const auto& event : m_events)
		scripts.dispatch_event(event);
	m_events.clear();
}

void ScriptExecutionSystem::start_scene(Scene& scene, const ScriptAttachmentManager& attachments) {
	stop_scene();
	m_scene = &scene;
	m_scene_running = true;
	m_last_delta = 0.0f;
	m_fixed_delta = 0.0f;
	m_elapsed = 0.0f;
	m_frame = 0;

	for (const auto& [entity, scripts] : attachments.get_all()) {
		for (const auto& attachment : scripts)
			create_instance(entity, attachment);
	}

	for (auto& instance : m_instances) {
		if (instance.Descriptor)
			invoke_lifecycle(instance, instance.Descriptor->Lifecycle.OnStart);
		instance.State = ScriptInstanceState::Started;
	}
}

void ScriptExecutionSystem::stop_scene() {
	for (auto& instance : m_instances)
		destroy_instance(instance, true);

	m_instances.clear();
	m_instances_by_entity.clear();
	m_dispatcher.clear();
	m_reported_warnings.clear();
	m_scene = nullptr;
	m_scene_running = false;
}

void ScriptExecutionSystem::on_script_added(RID entity, const ScriptAttachmentData& attachment) {
	if (!m_scene_running)
		return;

	create_instance(entity, attachment);
	auto* instance = find_instance(entity, attachment.ID);
	if (instance && instance->Descriptor) {
		invoke_lifecycle(*instance, instance->Descriptor->Lifecycle.OnStart);
		instance->State = ScriptInstanceState::Started;
	}
}

void ScriptExecutionSystem::on_script_removed(RID entity, ScriptTypeID type) {
	auto* instance = find_instance(entity, type);
	if (instance)
		destroy_instance(*instance, true);

	std::erase_if(m_instances, [&](const ScriptInstance& candidate) {
		return candidate.Entity == entity && candidate.Type == type;
	});
	std::erase_if(m_instances_by_entity[entity], [&](ScriptInstanceID id) {
		return !std::any_of(m_instances.begin(), m_instances.end(), [&](const ScriptInstance& instance) {
			return instance.ID == id;
		});
	});
}

void ScriptExecutionSystem::on_entity_removed(RID entity) {
	auto it = m_instances_by_entity.find(entity);
	if (it == m_instances_by_entity.end())
		return;

	for (ScriptInstanceID id : it->second) {
		auto instance_it = std::find_if(m_instances.begin(), m_instances.end(), [&](const ScriptInstance& instance) {
			return instance.ID == id;
		});
		if (instance_it != m_instances.end())
			destroy_instance(*instance_it, true);
	}

	std::erase_if(m_instances, [&](const ScriptInstance& instance) {
		return instance.Entity == entity;
	});
	m_instances_by_entity.erase(entity);
}

void ScriptExecutionSystem::update(float dt) {
	Input::update_frame_state();
	m_last_delta = dt;
	m_elapsed += dt;
	++m_frame;
	for (auto& instance : m_instances) {
		if (instance.Descriptor)
			invoke_lifecycle(instance, instance.Descriptor->Lifecycle.OnUpdate, dt);
	}
}

void ScriptExecutionSystem::fixed_update(float dt) {
	m_fixed_delta = dt;
	for (auto& instance : m_instances) {
		if (instance.Descriptor)
			invoke_lifecycle(instance, instance.Descriptor->Lifecycle.OnFixedUpdate, dt);
	}
	m_dispatcher.dispatch(*this);
}

void ScriptExecutionSystem::dispatch_event(const ScriptEvent& event) {
	auto it = m_instances_by_entity.find(event.Receiver);
	if (it == m_instances_by_entity.end())
		return;

	for (ScriptInstanceID id : it->second) {
		auto instance_it = std::find_if(m_instances.begin(), m_instances.end(), [&](const ScriptInstance& instance) {
			return instance.ID == id;
		});
		if (instance_it == m_instances.end() || !instance_it->Descriptor)
			continue;

		switch (event.EventKind) {
		case ScriptEvent::Kind::Collision:
			invoke_lifecycle(*instance_it, instance_it->Descriptor->Lifecycle.OnCollision, 0.0f, event.Other);
			break;
		case ScriptEvent::Kind::TriggerEnter:
			invoke_lifecycle(*instance_it, instance_it->Descriptor->Lifecycle.OnTriggerEnter, 0.0f, event.Other);
			break;
		case ScriptEvent::Kind::TriggerExit:
			invoke_lifecycle(*instance_it, instance_it->Descriptor->Lifecycle.OnTriggerExit, 0.0f, event.Other);
			break;
		}
	}
}

ScriptInstance* ScriptExecutionSystem::find_instance(RID entity, ScriptTypeID type) {
	for (auto& instance : m_instances) {
		if (instance.Entity == entity && instance.Type == type)
			return &instance;
	}
	return nullptr;
}

void ScriptExecutionSystem::create_instance(RID entity, const ScriptAttachmentData& attachment) {
	if (find_instance(entity, attachment.ID))
		return;

	if (!ScriptRegistry::exists(attachment.ID)) {
		ORG_CORE_WARN("[Noct] Cannot create script instance for entity {}: script type {} is not registered.",
		    entity.to_string(),
		    attachment.ID.to_string());
		return;
	}

	const auto& descriptor = ScriptRegistry::get(attachment.ID);
	if (descriptor.ComponentType.Type != UUID::bad()) {
		if (!ComponentRegistry::exists(descriptor.ComponentType.Type)) {
			ORG_CORE_WARN("[Noct] Script '{}' is associated with missing component '{}' ({}). It will not run.",
			    descriptor.Name,
			    descriptor.ComponentType.Name,
			    descriptor.ComponentType.Type.to_string());
			return;
		}
		if (!m_scene || !m_scene->has_component_data(entity, descriptor.ComponentType.Type)) {
			ORG_CORE_WARN("[Noct] Script '{}' is attached to entity {} but required component '{}' is missing. It will not run.",
			    descriptor.Name,
			    entity.to_string(),
			    descriptor.ComponentType.Name);
			return;
		}
	}

	if (!has_any_lifecycle(descriptor.Lifecycle)) {
		ORG_CORE_WARN("[Noct] Script '{}' is attached to entity {} but has no recognized lifecycle functions. Expected names like on_start or on_update.",
		    descriptor.Name,
		    entity.to_string());
	}

	ScriptInstance instance {};
	instance.Entity = entity;
	instance.Type = attachment.ID;
	instance.Descriptor = &descriptor;
	instance.Object = UUID::generate();
	if (descriptor.ComponentType.Type == UUID::bad())
		instance.Fields = attachment.Fields;

	m_instances_by_entity[entity].push_back(instance.ID);
	m_instances.push_back(std::move(instance));
}

void ScriptExecutionSystem::destroy_instance(ScriptInstance& instance, bool invoke_destroy) {
	if (instance.State == ScriptInstanceState::Destroyed)
		return;

	instance.State = ScriptInstanceState::DestroyPending;
	if (invoke_destroy && instance.Descriptor)
		invoke_lifecycle(instance, instance.Descriptor->Lifecycle.OnDestroy);
	instance.Object = UUID::bad();
	instance.State = ScriptInstanceState::Destroyed;
}

void ScriptExecutionSystem::invoke_lifecycle(ScriptInstance& instance, RuntimeFunctionHandle handle, float dt, RID other) {
	if (handle == UUID::bad())
		return;

	if (!instance.Descriptor)
		return;

	const auto& lifecycle = instance.Descriptor->Lifecycle;
	if (handle == lifecycle.OnStart)
		execute_lifecycle_body(instance, lifecycle.OnStartBody, dt, other);
	else if (handle == lifecycle.OnUpdate)
		execute_lifecycle_body(instance, lifecycle.OnUpdateBody, dt, other);
	else if (handle == lifecycle.OnFixedUpdate)
		execute_lifecycle_body(instance, lifecycle.OnFixedUpdateBody, dt, other);
	else if (handle == lifecycle.OnDestroy)
		execute_lifecycle_body(instance, lifecycle.OnDestroyBody, dt, other);
	else if (handle == lifecycle.OnCollision)
		execute_lifecycle_body(instance, lifecycle.OnCollisionBody, dt, other);
	else if (handle == lifecycle.OnTriggerEnter)
		execute_lifecycle_body(instance, lifecycle.OnTriggerEnterBody, dt, other);
	else if (handle == lifecycle.OnTriggerExit)
		execute_lifecycle_body(instance, lifecycle.OnTriggerExitBody, dt, other);
}

void ScriptExecutionSystem::execute_lifecycle_body(ScriptInstance& instance, std::string_view body, float dt, RID other) {
	if (!m_scene || body.empty())
		return;

	auto* runtime = ScriptSystem::runtime();
	if (!runtime)
		return;

	auto fields = writable_fields_for(instance, m_scene);
	const std::string script_name = instance.Descriptor ? instance.Descriptor->Name : std::string {};
	auto warn_callback = [this, id = instance.ID](std::string key, std::string message) {
		warn_once(id.to_string() + ":" + key, message);
	};

	auto& origo = runtime->register_native_module("origo");
	origo.context_function("entity", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		return make_entity_object(*m_scene, instance.Entity, script_name, warn_callback);
	});
	origo.context_function("other", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		if (other.get_id() < 0 || !m_scene->has_entity(other))
			return std::monostate {};
		return make_entity_object(*m_scene, other, script_name, warn_callback);
	});
	origo.context_function("scene", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef {
			std::make_shared<SceneNativeObject>(*m_scene, script_name, warn_callback)
		};
	});
	origo.context_function("time", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef {
			std::make_shared<TimeNativeObject>(dt, m_fixed_delta, m_elapsed, m_frame)
		};
	});
	origo.context_function("random", [](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef { std::make_shared<RandomNativeObject>() };
	});
	origo.context_function("input", [](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef { std::make_shared<InputNativeObject>() };
	});
	origo.context_function("debug", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef {
			std::make_shared<NoOpServiceNativeObject>("Debug", warn_callback)
		};
	});
	origo.context_function("audio", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef {
			std::make_shared<NoOpServiceNativeObject>("Audio", warn_callback)
		};
	});
	origo.context_function("assets", [&](Noct::NativeCallContext&) -> Noct::NoctObject {
		return Noct::NativeObjectRef {
			std::make_shared<NoOpServiceNativeObject>("Assets", warn_callback)
		};
	});
	origo.context_function("component_type", [](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
		const std::string name = ctx.require_string(0);
		auto id = ComponentRegistry::try_find_by_name(name);
		if (!id)
			return std::monostate {};

		return Noct::NativeObjectRef {
			std::make_shared<ComponentTypeNativeObject>(*id, name) };
	});
	origo.context_function("get", [&](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
		const std::string name = ctx.require_string(0);
		if (name == "__dt")
			return static_cast<double>(dt);
		if (auto* field = find_writable_field(fields, name))
			return noct_value_from_field(*field, m_scene, script_name, warn_callback);
		return std::monostate {};
	});
	origo.context_function("set", [&](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
		const std::string name = ctx.require_string(0);
		auto* field = find_writable_field(fields, name);
		if (!field) {
			warn_once(instance.ID.to_string() + ":unknown-set:" + name,
			    "[Noct] Script '" + instance.Descriptor->Name + "' cannot assign unknown field '" + name + "'.");
			return std::monostate {};
		}

		const std::string error = assign_noct_value_to_field(*field, ctx.argument(1));
		if (!error.empty())
			warn_once(instance.ID.to_string() + ":set-type:" + name, "[Noct] " + error);
		return std::monostate {};
	});
	origo.context_function("info", [&](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
		ORG_CORE_INFO("[Noct] {}", ctx.require_string(0));
		return std::monostate {};
	});
	origo.context_function("warn", [&](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
		ORG_CORE_WARN("[Noct] {}", ctx.require_string(0));
		return std::monostate {};
	});
	origo.context_function("error", [&](Noct::NativeCallContext& ctx) -> Noct::NoctObject {
		ORG_CORE_ERROR("[Noct] {}", ctx.require_string(0));
		return std::monostate {};
	});

	std::vector<Noct::EmbeddedBodyBinding> bindings {
		{ .Name = "entity", .Initializer = "origo.entity()" },
		{ .Name = "other", .Initializer = "origo.other()" },
		{ .Name = "scene", .Initializer = "origo.scene()" },
		{ .Name = "Time", .Initializer = "origo.time()" },
		{ .Name = "Random", .Initializer = "origo.random()" },
		{ .Name = "Input", .Initializer = "origo.input()" },
		{ .Name = "Debug", .Initializer = "origo.debug()" },
		{ .Name = "Audio", .Initializer = "origo.audio()" },
		{ .Name = "Assets", .Initializer = "origo.assets()" },
		{ .Name = "Log", .Initializer = "origo" },
		{ .Name = "dt", .Initializer = "origo.get(\"__dt\")" },
	};
	for (const auto& [_, component] : ComponentRegistry::get_all()) {
		if (component.Name.empty())
			continue;
		bindings.push_back({
		    .Name = component.Name,
		    .Initializer = "origo.component_type(\"" + escape_noct_string(component.Name) + "\")",
		});
	}
	for (const auto* field : fields) {
		if (!field)
			continue;
		const std::string escaped_name = escape_noct_string(field->Name);
		bindings.push_back({
		    .Name = field->Name,
		    .Initializer = "origo.get(\"" + escaped_name + "\")",
		    .WriteBack = "origo.set(\"" + escaped_name + "\", " + field->Name + ");",
		});
	}

	Noct::Context ctx {};
	ctx.LoggingEnabled = false;
	if (instance.Descriptor)
		ctx.SourcePath = instance.Descriptor->Name + ".lifecycle.noct";

	const auto result = runtime->RunEmbeddedBody(ctx, body, { "origo", "Math" }, bindings);
	for (const auto& diagnostic : ctx.Diagnostics) {
		const std::string message = diagnostic.Message.empty() ? "Noct runtime diagnostic" : diagnostic.Message;
		if (diagnostic.Severity == Noct::NoctSeverity::Error) {
			warn_once(instance.ID.to_string() + ":runtime-error:" + message,
			    "[Noct] Script '" + instance.Descriptor->Name + "' runtime error: " + message);
		} else if (diagnostic.Severity == Noct::NoctSeverity::Warning) {
			warn_once(instance.ID.to_string() + ":runtime-warning:" + message,
			    "[Noct] Script '" + instance.Descriptor->Name + "' runtime warning: " + message);
		}
	}

	if (result.HadParseError || result.HadSemanticError || result.HadRuntimeError) {
		warn_once(instance.ID.to_string() + ":runtime-failed",
		    "[Noct] Script '" + instance.Descriptor->Name + "' lifecycle execution failed.");
	}
}

void ScriptExecutionSystem::warn_once(const std::string& key, const std::string& message) {
	if (!m_reported_warnings.insert(key).second)
		return;
	ORG_CORE_WARN("{}", message);
}

}
