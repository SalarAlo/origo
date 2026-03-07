#include <doctest/doctest.h>

#include "origo/core/RID.h"

using Origo::RID;

TEST_CASE("RID::from_string restores the numeric id") {
	RID rid = RID::from_string("42");

	CHECK(rid.get_id() == 42);
}

TEST_CASE("RID restoration advances the generator to avoid collisions") {
	RID restored = RID::from_id(1000);
	RID generated = RID::new_rid();

	CHECK(restored.get_id() == 1000);
	CHECK(generated.get_id() > restored.get_id());
}

TEST_CASE("RID ToString / from_string roundtrip") {
	RID original = RID::new_rid();
	std::string serialized = original.to_string();
	RID parsed = RID::from_string(serialized);

	CHECK(original == parsed);
}
