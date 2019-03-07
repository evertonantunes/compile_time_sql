#pragma once

#include <catch2/catch.hpp>
#include "sample_database.hpp"

TEST_CASE("validate select count SQL", "[validate][select][count]")
{
    const auto context = database::select(database::count(users_t::id)).from<users_t>();
    const auto sql = context.to_string().view();

    REQUIRE(sql == "SELECT COUNT(users.id) FROM users");
}

TEST_CASE("validate select one column SQL", "[validate][select][count]")
{
    const auto context = database::select(users_t::id).from<users_t>();
    const auto sql = context.to_string().view();

    REQUIRE(sql == "SELECT users.id FROM users");
}

TEST_CASE("validate select two column SQL", "[validate][select][count]")
{
    const auto context = database::select(users_t::id, users_t::age).from<users_t>();
    const auto sql = context.to_string().view();

    REQUIRE(sql == "SELECT users.id, users.age FROM users");
}

TEST_CASE("validate select count SQL WHERE", "[validate][select][count][where]")
{
    const auto context = database::select(database::count(users_t::id)).from<users_t>().where( users_t::age == 39l );
    const auto sql = context.to_string().view();
    const auto data = context.data();

    REQUIRE(sql == "SELECT COUNT(users.id) FROM users WHERE users.age == ?");
    REQUIRE(data == std::make_tuple(39l));
}

TEST_CASE("validate select count SQL WHERE OR", "[validate][select][count][where][or]")
{
    const auto context = database::select(database::count(users_t::id)).from<users_t>().where( users_t::age == 39l || users_t::age == 40l );
    const auto sql = context.to_string().view();
    const auto data = context.data();

    REQUIRE(sql == "SELECT COUNT(users.id) FROM users WHERE users.age == ? OR users.age == ?");
    REQUIRE(data == std::make_tuple(39l, 40l));
}

TEST_CASE("validate select count SQL WHERE AND", "[validate][select][count][where][and]")
{
    const auto context = database::select(database::count(users_t::id)).from<users_t>().where( users_t::age == 39l && users_t::age == 40l );
    const auto sql = context.to_string().view();
    const auto data = context.data();

    REQUIRE(sql == "SELECT COUNT(users.id) FROM users WHERE users.age == ? AND users.age == ?");
    REQUIRE(data == std::make_tuple(39l, 40l));
}

TEST_CASE("validate select filds SQL LEFT JOIN", "[validate][select][count][left-join]")
{
    using my_select_t = decltype(database::select(   users_t::id
                                                   , f_name::as(descriptions_t::text)
                                                   , s_name::as(descriptions_t::text)
                                                   , users_t::age)
        .from<users_t>()
        .left_join<f_name>(f_name::as(descriptions_t::id) == users_t::first_name)
        .left_join<s_name>(s_name::as(descriptions_t::id) == users_t::second_name));

    const auto context = my_select_t();
    const auto sql = context.to_string().view();

    REQUIRE(sql == "SELECT users.id, f_name.text, s_name.text, users.age FROM users LEFT JOIN descriptions AS f_name ON f_name.id == users.first_name LEFT JOIN descriptions AS s_name ON s_name.id == users.second_name");
}

TEST_CASE("validate select filds SQL LEFT JOIN WHERE", "[validate][select][count][left-join][where]")
{
    using my_select_t = decltype(database::select(   users_t::id
                                                   , f_name::as(descriptions_t::text)
                                                   , s_name::as(descriptions_t::text)
                                                   , users_t::age)
        .from<users_t>()
        .left_join<f_name>(f_name::as(descriptions_t::id) == users_t::first_name)
        .left_join<s_name>(s_name::as(descriptions_t::id) == users_t::second_name));

    const auto context = my_select_t().where(users_t::age == 39l);
    const auto sql = context.to_string().view();
    const auto data = context.data();

    REQUIRE(sql == "SELECT users.id, f_name.text, s_name.text, users.age FROM users LEFT JOIN descriptions AS f_name ON f_name.id == users.first_name LEFT JOIN descriptions AS s_name ON s_name.id == users.second_name WHERE users.age == ?");
    REQUIRE(data == std::make_tuple(39l));
}
