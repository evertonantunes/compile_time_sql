#pragma once

#include <catch2/catch.hpp>
#include <algorithm>
#include "sample_database.hpp"

class Fixture
{
public:
    Fixture()
    {
        using strings_t = database::tables::strings;
        using users_t = database::tables::users;

        database::create_table<strings_t>();
        database::create_table<users_t>();

        database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Blaise")
                                        , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Pascal")
                                        , users_t::age = 39l );

        database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Leonhard")
                                        , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Euler")
                                        , users_t::age = 76l );

        database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Robert")
                                        , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Hooke")
                                        , users_t::age = 67l );
    }

    ~Fixture()
    {
        database::factory_t::instance()->reset();
    }
};

namespace
{
    template<typename T, typename H>
    constexpr bool is_same()
    {
        static_assert(std::is_same<T, H>::value, "required same types");
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

TEST_CASE_METHOD(Fixture, "Select basic", "[select]")
{
    const auto rows = database::select(database::tables::users::id).from<database::tables::users>().where(database::tables::users::age == 39l);

    auto it = std::begin(rows);

    SECTION( "has item" ) {
        REQUIRE( it != std::end(rows) );
    }

    SECTION( "check result type is expected" ) {
        REQUIRE( validate_column_type(database::tables::users::age, std::get<0>(*it)) );
    }

    SECTION( "item is expected" ) {
        REQUIRE( std::get<0>(*it) == 1l );
    }

    SECTION( "next element is end" ) {
        ++it;
        REQUIRE( it == std::end(rows) );
    }
}
