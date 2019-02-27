#pragma once

#include <catch2/catch.hpp>
#include <algorithm>
#include "sample_database.hpp"

namespace
{
    template<typename T, typename H>
    constexpr bool is_same()
    {
        static_assert((std::is_same<T, H>::value), "required same types");
        return true;
    }

    template<typename T, typename H>
    constexpr bool validate_column_type( T, H )
    {
        using expected_type_t = std::remove_reference_t<typename T::type_t>;
        using found_type_t = std::remove_reference_t<H>;
        return is_same<expected_type_t, found_type_t>();
    }
}

TEST_CASE_METHOD(Fixture, "Select count rows", "[select][count]")
{
    const auto rows = database::select(database::count(users_t::id)).from<users_t>();
    auto it = std::begin(rows);
    REQUIRE( it != std::end(rows) );
    REQUIRE( std::is_same_v<std::ptrdiff_t, std::remove_reference_t<decltype(std::get<0>(*it))>> );
    REQUIRE( std::get<0>(*it) == 3l );
    ++it;
    REQUIRE( it == std::end(rows) );
}

TEST_CASE_METHOD(Fixture, "Select all rows", "[select][all]")
{
    const auto rows = database::select(users_t::id).from<users_t>();
    auto it = std::begin(rows);
    REQUIRE( it != std::end(rows) );
    REQUIRE( validate_column_type(users_t::id, std::get<0>(*it)) );

    REQUIRE( std::get<0>(*it) == 1l );
    ++it;
    REQUIRE( std::get<0>(*it) == 2l );
    ++it;
    REQUIRE( std::get<0>(*it) == 3l );
    ++it;
    REQUIRE( it == std::end(rows) );
}

TEST_CASE_METHOD(Fixture, "Select specific row", "[select][where]")
{
    const auto rows = database::select(users_t::id).from<users_t>().where( users_t::id == 2l );
    auto it = std::begin(rows);
    REQUIRE( it != std::end(rows) );
    REQUIRE( validate_column_type(users_t::id, std::get<0>(*it)) );

    REQUIRE( std::get<0>(*it) == 2l );

    ++it;
    REQUIRE( it == std::end(rows) );
}

