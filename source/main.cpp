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

            INSERT INTO users VALUES (1, 'Blaise', 'Pascal', 39);
            INSERT INTO users VALUES (2, 'Leonhard', 'Euler', 76);
            INSERT INTO users VALUES (3, 'Robert', 'Hooke', 67);
        )";
    }

    namespace tables
    {
        struct users
        {
            static constexpr sql::string_t table_name()
            {
                return {"users"};
            }

            static constexpr std::array<sql::string_t, 4> columns_name()
            {
                return {sql::string_t("id"), sql::string_t("first_name"), sql::string_t("second_name"), sql::string_t("age")};
            }

            static constexpr sql::column<std::ptrdiff_t, users, 0> id = {};
            static constexpr sql::column<std::string_view, users, 1> first_name = {};
            static constexpr sql::column<std::string_view, users, 2> second_name = {};
            static constexpr sql::column<std::ptrdiff_t, users, 3> age = {};
        };
    }

    template<typename ...T>
    auto select( T &&...args )
    {
        return sql::select<factory_t>(std::forward<T>(args)...);
    }
}

int main()
{
    database::factory_t::execute(database::get_echema());

    using users_t = database::tables::users;

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
    return 0;
}
