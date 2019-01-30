#pragma once

#include "../include/api.hpp"

namespace database
{
    namespace tables
    {
        using namespace sql;

        struct users : public Table<decltype("users"_s)>
        {
            static constexpr Column<users, decltype("id"_s)         , std::ptrdiff_t  , Flags<not_null>> id          = {};
            static constexpr Column<users, decltype("first_name"_s) , std::string_view, Flags<not_null>> first_name  = {};
            static constexpr Column<users, decltype("second_name"_s), std::string_view, Flags<not_null>> second_name = {};
            static constexpr Column<users, decltype("age"_s)        , std::ptrdiff_t  , Flags<not_null>> age         = {};
        };
    }

    struct ConnectionFactory
    {
        using context_t = int;

        static context_t make_context( std::string &&sql )
        {
            return context_t{0};
        }
    };

    template<typename ...T>
    auto select( T &&...args )
    {
        return sql::select<ConnectionFactory>(std::forward<T>(args)...);
    }
}
