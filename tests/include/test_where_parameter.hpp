#pragma once

#include <catch2/catch.hpp>
#include "sample_database.hpp"

using descriptions_t = database::tables::Descriptions;
using users_t = database::tables::Users;

TEST_CASE("validate parameter equality type numeric fail", "[validate][esquality][fail][parameter][type][numeric]")
{
    REQUIRE_THROWS(users_t::id == static_cast<std::size_t>(39));
    REQUIRE_THROWS(users_t::id == true);
    REQUIRE_THROWS(users_t::id == 'c');
    REQUIRE_THROWS(users_t::id == std::string_view{});
    REQUIRE_THROWS(users_t::id == "abcd");
}

TEST_CASE("validate parameter equality type numeric success", "[validate][esquality][success][parameter][type][numeric]")
{
    REQUIRE_NOTHROW(users_t::id == 39l);
}

TEST_CASE("validate parameter equality type string_view fail", "[validate][esquality][fail][parameter][type][string]")
{
    REQUIRE_THROWS(descriptions_t::text == 39l);
    REQUIRE_THROWS(descriptions_t::text == 39);
    REQUIRE_THROWS(descriptions_t::text == 'c');
    REQUIRE_THROWS(descriptions_t::text == true);
}

TEST_CASE("validate parameter equality type string_view success", "[validate][esquality][success][parameter][type][string]")
{
    REQUIRE_NOTHROW(descriptions_t::text == std::string_view{});
    REQUIRE_NOTHROW(descriptions_t::text == "abcd");
}
