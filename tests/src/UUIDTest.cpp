#include <doctest/doctest.h>
#include "origo/core/UUID.h"

#include <unordered_set>

using Origo::UUID;

TEST_CASE("UUID::Generate produces unique values") {
	UUID a = UUID::Generate();
	UUID b = UUID::Generate();

	CHECK(a != b);
}

TEST_CASE("UUID equality works") {
	UUID a = UUID::FromHash("test");
	UUID b = UUID::FromHash("test");
	UUID c = UUID::FromHash("other");

	CHECK(a == b);
	CHECK_FALSE(a == c);
}

TEST_CASE("UUID ToString / FromString roundtrip") {
	UUID original = UUID::Generate();
	std::string str = original.ToString();
	UUID parsed = UUID::FromString(str);

	CHECK(original == parsed);
}

TEST_CASE("UUID::FromString rejects wrong length") {
	UUID invalid = UUID::FromString("1234");
	CHECK(invalid.GetHigh() == 0);
	CHECK(invalid.GetLow() == 0);
}

TEST_CASE("UUID::FromString rejects non-hex characters") {
	CHECK_THROWS_AS(
	    UUID::FromString("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"),
	    std::invalid_argument);
}

TEST_CASE("UUID::FromArbitraryString is deterministic") {
	UUID a = UUID::FromArbitraryString("hello");
	UUID b = UUID::FromArbitraryString("hello");

	CHECK(a == b);
}

TEST_CASE("UUID::FromArbitraryString differs for different input") {
	UUID a = UUID::FromArbitraryString("hello");
	UUID b = UUID::FromArbitraryString("world");

	CHECK(a != b);
}

TEST_CASE("UUID can be used as unordered_map key") {
	std::unordered_set<UUID> set;

	UUID a = UUID::FromHash("one");
	UUID b = UUID::FromHash("two");
	UUID c = UUID::FromHash("one");

	set.insert(a);
	set.insert(b);

	CHECK(set.contains(a));
	CHECK(set.contains(b));
	CHECK(set.contains(c));
}
