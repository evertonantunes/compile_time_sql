#include "api.hpp"
#include "sqlite_factory.hpp"

#include <iostream>


namespace database
{
    using factory_t = factory::SQLite;

    namespace tables
    {
        using namespace sql;

        struct strings : public Table<decltype("strings"_s)>
        {
            static constexpr Column<strings, decltype("id"_s)  , std::ptrdiff_t  , Flags<pk, not_null>> id   = {};
            static constexpr Column<strings, decltype("text"_s), std::string_view, Flags<not_null>    > text = {};

            using columns = std::tuple<decltype(id), decltype(text)>; // future introspection
        };

//        struct users : public Table<decltype("users"_s)>
//        {
//            static constexpr Column<users, decltype("id"_s)         , std::ptrdiff_t  , Flags<pk, not_null>> id          = {};
//            static constexpr Column<users, decltype("first_name"_s) , std::string_view, Flags<not_null>    > first_name  = {};
//            static constexpr Column<users, decltype("second_name"_s), std::string_view, Flags<not_null>    > second_name = {};
//            static constexpr Column<users, decltype("age"_s)        , std::ptrdiff_t  , Flags<not_null>    > age         = {};
//        };

        struct users : public Table<decltype("users"_s)>
        {
            static constexpr Column<users, decltype("id"_s)         , std::ptrdiff_t, Flags<pk, not_null>> id          = {};
            static constexpr Column<users, decltype("first_name"_s) , std::ptrdiff_t, Flags<not_null>> first_name  = {};
            static constexpr Column<users, decltype("second_name"_s), std::ptrdiff_t, Flags<not_null>> second_name = {};
            static constexpr Column<users, decltype("age"_s)        , std::ptrdiff_t, Flags<not_null>    > age         = {};

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
    database::create_table<database::tables::strings>();
    database::create_table<database::tables::users>();

    using strings_t = database::tables::strings;
    using users_t = database::tables::users;

    database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Blaise")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Pascal")
                                    , users_t::age = 39l );

    database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Leonhard")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Euler")
                                    , users_t::age = 76l );

    database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Robert")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Hooke")
                                    , users_t::age = 67l );

    /*

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
*/
    return 0;
}
