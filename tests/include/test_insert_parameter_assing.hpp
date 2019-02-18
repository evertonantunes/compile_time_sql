#pragma once

#include <catch2/catch.hpp>
#include "sample_database.hpp"

using descriptions_t = database::tables::Descriptions;
using users_t = database::tables::Users;

TEST_CASE("validate parameter assign type string fail", "[validate][assign][fail][parameter][type][string]")
{
    REQUIRE_THROWS(descriptions_t::text = 39);
    REQUIRE_THROWS(descriptions_t::text = 39l);
    REQUIRE_THROWS(descriptions_t::text = false);
    REQUIRE_THROWS(descriptions_t::text = 'c');
}

TEST_CASE("validate parameter assign type string success", "[validate][assign][success][success][parameter][type][string]")
{
    REQUIRE_NOTHROW(descriptions_t::text = std::string_view{});
    REQUIRE_NOTHROW(descriptions_t::text = "abc");
}

TEST_CASE("validate parameter assign type numeric success", "[validate][assign][success][success][parameter][type][numeric]")
{
    REQUIRE_NOTHROW(users_t::id = 39l);
}

TEST_CASE("validate parameter assign type numeric fail", "[validate][assign][fail][parameter][type][numeric]")
{
    REQUIRE_THROWS(users_t::id = 39);
    REQUIRE_THROWS(users_t::id = false);
    REQUIRE_THROWS(users_t::id = "abc");
    REQUIRE_THROWS(users_t::id = 'c');
}
