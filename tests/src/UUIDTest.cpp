#include <doctest/doctest.h>

#include <unordered_set>

#include "origo/core/UUID.h"

using Origo::UUID;

TEST_CASE("UUID::generate produces unique values") {
	UUID a = UUID::generate();
	UUID b = UUID::generate();

	CHECK(a != b);
}

TEST_CASE("UUID equality works") {
	UUID a = UUID::from_hash("test");
	UUID b = UUID::from_hash("test");
	UUID c = UUID::from_hash("other");

	CHECK(a == b);
	CHECK_FALSE(a == c);
}

TEST_CASE("UUID ToString / from_string roundtrip") {
	UUID original = UUID::generate();
	std::string str = original.to_string();
	UUID parsed = UUID::from_string(str);

	CHECK(original == parsed);
}

TEST_CASE("UUID::from_string rejects wrong length") {
	UUID invalid = UUID::from_string("1234");
	CHECK(invalid.get_high() == 0);
	CHECK(invalid.get_low() == 0);
}

TEST_CASE("UUID::from_string rejects non-hex characters") {
	CHECK_THROWS_AS(
	    UUID::from_string("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"),
	    std::invalid_argument);
}

TEST_CASE("UUID::from_arbitrary_string is deterministic") {
	UUID a = UUID::from_arbitrary_string("hello");
	UUID b = UUID::from_arbitrary_string("hello");

	CHECK(a == b);
}

TEST_CASE("UUID::from_arbitrary_string differs for different input") {
	UUID a = UUID::from_arbitrary_string("hello");
	UUID b = UUID::from_arbitrary_string("world");

	CHECK(a != b);
}

TEST_CASE("UUID can be used as unordered_map key") {
	std::unordered_set<UUID> set;

	UUID a = UUID::from_hash("one");
	UUID b = UUID::from_hash("two");
	UUID c = UUID::from_hash("one");

	set.insert(a);
	set.insert(b);

	CHECK(set.contains(a));
	CHECK(set.contains(b));
	CHECK(set.contains(c));
}
