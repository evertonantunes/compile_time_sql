#pragma once

#include "../include/api.hpp"
#include "../include/sqlite_factory.hpp"


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
