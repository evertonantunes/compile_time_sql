#pragma once

#include <sqlite3.h>
#include <stdexcept>
#include <cassert>


namespace database
{
    namespace factory
    {
        namespace set
        {
            template<typename CONTEXT, typename T>
            void bind( CONTEXT &context, const std::size_t index, const T value )
            {
                auto rc = SQLITE_OK;
                if constexpr (std::is_same_v<std::string, T>)
                {
                    rc = sqlite3_bind_text(context.m_stmt, index, value.data(), value.size(), nullptr);
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    rc = sqlite3_bind_text(context.m_stmt, index, value.data(), value.size(), nullptr);
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    rc = sqlite3_bind_int64(context.m_stmt, index, value);
                }
                assert(rc == SQLITE_OK);
            }

            template<typename CONTEXT, class TupType, size_t... I>
            void bind( CONTEXT &context, const TupType& _tup, std::index_sequence<I...>)
            {
                (..., bind(context, I + 1, std::get<I>(_tup)));
            }

            template<typename CONTEXT, class... T>
            void bind( CONTEXT &context, const std::tuple<T...>& _tup)
            {
                bind(context, _tup, std::make_index_sequence<sizeof...(T)>());
            }
        }

        namespace get
        {
            template<typename CONTEXT, typename T>
            void bind( CONTEXT &context, const std::size_t index, T &value )
            {
                auto rc = SQLITE_OK;
                if constexpr (std::is_same_v<std::string, T>)
                {
                    const unsigned char *data = sqlite3_column_text(context.m_stmt, index);
                    value.assign(reinterpret_cast<const char*>(data));
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    const unsigned char *data = sqlite3_column_text(context.m_stmt, index);
                    value = reinterpret_cast<const char*>(data);
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    value = sqlite3_column_int64(context.m_stmt, index);
                }
                assert(rc == SQLITE_OK);
            }

            template<typename CONTEXT, class TupType, size_t... I>
            void bind( CONTEXT &context, TupType& _tup, std::index_sequence<I...>)
            {
                (..., bind(context, I, std::get<I>(_tup)));
            }

            template<typename CONTEXT, class... T>
            void bind( CONTEXT &context, std::tuple<T...>& _tup)
            {
                bind(context, _tup, std::make_index_sequence<sizeof...(T)>());
            }
        }

        struct Context
        {
            sqlite3_stmt *m_stmt;
            std::string m_sql;
            int m_error_code = SQLITE_DONE;

            Context( )
                : m_stmt(nullptr)
                , m_error_code(SQLITE_DONE)
            { }

            Context( sqlite3 *connection, std::string &&sql )
                : m_sql(std::move(sql))
            {
                m_error_code = sqlite3_prepare(connection, m_sql.c_str(), m_sql.size(), &m_stmt, NULL);
                assert(m_error_code == SQLITE_OK);
            }

            ~Context( )
            {
                if (m_stmt)
                {
            //        sqlite3_finalize(m_stmt);
                }
            }
        };

        template<class ... T>
        void next( Context &context, std::tuple<T...> &data )
        {
            context.m_error_code = sqlite3_step(context.m_stmt);
            switch (context.m_error_code)
            {
                case SQLITE_ROW:
                {
                    if constexpr (std::tuple_size<std::tuple<T...>>::value != 0)
                    {
                        get::bind(context, data);
                    }
                }
                break;
                case SQLITE_DONE:
                    return;
                default:
                    throw std::runtime_error(sqlite3_errstr(context.m_error_code));
                break;
            }
        }

        bool is_equal( const Context &lhs, const Context &rhs )
        {
            return lhs.m_error_code == rhs.m_error_code;
        }

        struct SQLite
        {
            using context_t = Context;

            sqlite3 *m_database_connection;

            SQLite()
            {
                sqlite3_open(":memory:", &m_database_connection);
            }

            ~SQLite()
            {
                sqlite3_close(m_database_connection);
            }

            static SQLite* instance()
            {
                static SQLite s_instance;
                return &s_instance;
            }

            static void execute( const std::string_view sql )
            {
                const auto rc = sqlite3_exec(instance()->m_database_connection, sql.data(), nullptr, nullptr, nullptr);
                if (rc != SQLITE_OK)
                {
                    throw std::runtime_error(sqlite3_errstr(rc));
                }
            }

            template<typename T>
            static context_t make_context( std::string &&sql, T &&data )
            {
                context_t result(instance()->m_database_connection, std::move(sql));
                set::bind(result, data);
                return result;
            }
        };
    }
}

