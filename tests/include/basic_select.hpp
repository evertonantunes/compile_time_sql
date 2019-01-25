#pragma once

#include <catch2/catch.hpp>
#include <algorithm>
#include "sample_database.hpp"

using users_t = database::tables::users;

TEST_CASE("Select basic", "[select]")
{

//    for ( auto [id, name] : database::select(users_t::id, users_t::name)
//          .from<users_t>()
//          .where((users_t::x == 1l) && (users_t::y == 2l) && (users_t::name == "Everton") || (users_t::name == "Everton2")) )
//    {

//    }

    REQUIRE( true );
}
