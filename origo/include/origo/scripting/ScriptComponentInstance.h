
#pragma once

#include "origo/scripting/Variant.h"

namespace Origo {
using ScriptComponentID = int;

struct ScriptComponentInstance {
	ScriptComponentID ID;
	std::vector<std::string> FieldNames;
	std::vector<Variant> Values;

	std::optional<Origo::Variant> FindValue(const std::string& name) {
		for (size_t i = 0; i < FieldNames.size(); ++i)
			if (FieldNames[i] == name)
				return &Values[i];

		return std::nullopt;
	}
};

}
