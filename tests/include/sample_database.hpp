#pragma once

#include "../include/api.hpp"

namespace database
{
    namespace tables
    {
        struct users
        {
            static sql::string_t table_name()
            {
                return {"users"};
            }

            static std::array<std::string_view, 4> columns_name()
            {
                return {"id", "name", "x", "y"};
            }

            static constexpr sql::column<std::ptrdiff_t, users, 0> id = {};
            static constexpr sql::column<std::string_view, users, 1> name = {};
            static constexpr sql::column<std::ptrdiff_t, users, 2> x = {};
            static constexpr sql::column<std::ptrdiff_t, users, 3> y = {};
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
