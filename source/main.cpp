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
                 id                        INTEGER  NOT NULL
               , first_name                TEXT     NOT NULL
               , second_name               TEXT     NOT NULL
               , age                       INTEGER  NOT NULL
               , constraint pk_users primary key (id)
               );
               )";

        /*
    constraint pk_system_contact_type primary key (id),
    FOREIGN KEY(traceability_id) REFERENCES traceability(id)
*/
    }

    namespace tables
    {
        using namespace sql;

        struct strings : public Table<decltype("strings"_s)>
        {
            static constexpr Column<strings, decltype("id"_s)  , std::ptrdiff_t  , Flags<pk, not_null>> id   = {};
            static constexpr Column<strings, decltype("text"_s), std::string_view, Flags<not_null>    > text = {};

            using columns = std::tuple<decltype(id), decltype(text)>; // future introspection
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

            using columns = std::tuple<decltype(id), decltype(first_name), decltype(second_name), decltype(age)>; // future introspection
        };
    }

    template<typename ...T>
    auto select( T &&...args )
    {
        return sql::select<factory_t>(std::forward<T>(args)...);
    }

    template<typename H, typename ...T>
    auto insert_into( T &&...args )
    {
        return sql::insert_into<factory_t, H>(std::forward<T>(args)...);
    }

    template<typename T>
    void create_table()
    {
        sql::create_table<factory_t, T>();
    }
}

int main()
{
//    database::factory_t::execute(database::get_echema());
    database::create_table<database::tables::strings>();
    database::create_table<database::tables::users2>();

    using strings_t = database::tables::strings;
    using users_t = database::tables::users2;

    database::insert_into<users_t>(   users_t::id = 1l
                                    , users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Blaise")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Pascal")
                                    , users_t::age = 39l );

    database::insert_into<users_t>(   users_t::id = 2l
                                    , users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Leonhard")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Euler")
                                    , users_t::age = 76l );

    database::insert_into<users_t>(   users_t::id = 3l
                                    , users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Robert")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Hooke")
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

    database::insert_into<users_t>(   users_t::first_name = "Éverton"
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
