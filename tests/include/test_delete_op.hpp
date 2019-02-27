#pragma once

#include <catch2/catch.hpp>
#include <algorithm>
#include "sample_database.hpp"

TEST_CASE_METHOD(Fixture, "Delete where", "[delete][where]")
{
    const std::size_t count = database::delete_from<users_t>( users_t::id == 1l || users_t::age == 76l );
    REQUIRE( count == 2 );
}
