#include "api.hpp"
#include "sqlite_factory.hpp"

#include <iostream>


namespace database
{
    using factory_t = factory::SQLite;

    std::string_view get_echema()
    {
        return R"(
               CREATE TABLE users
               (
                 id            PRIMARY     KEY      NOT NULL
               , first_name                TEXT     NOT NULL
               , second_name               TEXT     NOT NULL
               , age                       INT      NOT NULL
               );
               )";
    }

    namespace tables
    {
        using namespace sql;

        struct strings : public Table<decltype("strings"_s)>
        {
            static constexpr Column<strings, decltype("id"_s)  , std::ptrdiff_t  , Flags<pk, not_null>> id   = {};
            static constexpr Column<strings, decltype("text"_s), std::string_view, Flags<not_null>    > text = {};
        };

        struct users : public Table<decltype("users"_s)>
        {
            static constexpr Column<users, decltype("id"_s)         , std::ptrdiff_t  , Flags<pk, not_null>> id          = {};
            static constexpr Column<users, decltype("first_name"_s) , std::string_view, Flags<not_null>    > first_name  = {};
            static constexpr Column<users, decltype("second_name"_s), std::string_view, Flags<not_null>    > second_name = {};
            static constexpr Column<users, decltype("age"_s)        , std::ptrdiff_t  , Flags<not_null>    > age         = {};
        };

        struct users2 : public Table<decltype("users2"_s)>
        {
            static constexpr Column<users2, decltype("id"_s)         , std::ptrdiff_t, Flags<pk, not_null>> id          = {};
            static constexpr Column<users2, decltype("first_name"_s) , std::ptrdiff_t, Flags<fk, not_null>> first_name  = {};
            static constexpr Column<users2, decltype("second_name"_s), std::ptrdiff_t, Flags<fk, not_null>> second_name = {};
            static constexpr Column<users2, decltype("age"_s)        , std::ptrdiff_t, Flags<not_null>    > age         = {};
        };
    }

    template<typename ...T>
    auto select( T &&...args )
    {
        return sql::select<factory_t>(std::forward<T>(args)...);
    }

    template<typename H, typename ...T>
    void insert_into( T &&...args )
    {
        sql::insert_into<factory_t, H>(std::forward<T>(args)...);
    }

    template<typename T>
    void create_table()
    {
        factory_t::create_table<T>();
    }
}

int main()
{
    database::factory_t::execute(database::get_echema());

    database::create_table<database::tables::strings>();
    database::create_table<database::tables::users2>();

    using users_t = database::tables::users;


    database::insert_into<users_t>(   users_t::id = 1l
                                    , users_t::first_name = "Blaise"
                                    , users_t::second_name = "Pascal"
                                    , users_t::age = 39l );

    database::insert_into<users_t>(   users_t::id = 2l
                                    , users_t::first_name = "Leonhard"
                                    , users_t::second_name = "Euler"
                                    , users_t::age = 76l );

    database::insert_into<users_t>(   users_t::id = 3l
                                    , users_t::first_name = "Robert"
                                    , users_t::second_name = "Hooke"
                                    , users_t::age = 67l );


    for ( auto [id, first_name, second_name, age] : database::select(  users_t::id
                                                                     , users_t::first_name
                                                                     , users_t::second_name
                                                                     , users_t::age)
          .from<users_t>() )
    {
        std::cout << "id: " << id << " first_name: " << first_name << " second_name: " << second_name << " age: " << age << std::endl;
    }

    for ( auto [id, first_name, second_name, age] : database::select(  users_t::id
                                                                     , users_t::first_name
                                                                     , users_t::second_name
                                                                     , users_t::age)
          .from<users_t>()
          .where(users_t::age == 39l) )
    {
        std::cout << "id: " << id << " first_name: " << first_name << " second_name: " << second_name << " age: " << age << std::endl;
    }

    database::insert_into<users_t>(   users_t::id = 4l
                                    , users_t::first_name = "Éverton"
                                    , users_t::second_name = "Éverton"
                                    , users_t::age = 31l );

    for ( auto [id, first_name, second_name, age] : database::select(  users_t::id
                                                                     , users_t::first_name
                                                                     , users_t::second_name
                                                                     , users_t::age)
          .from<users_t>() )
    {
        std::cout << "id: " << id << " first_name: " << first_name << " second_name: " << second_name << " age: " << age << std::endl;
    }

    return 0;
}
