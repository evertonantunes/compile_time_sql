#pragma once

#include <sqlite3.h>
#include <stdexcept>
#include <cassert>


namespace database
{
    namespace factory
    {
        class SQLiteError : public std::exception
        {
            sqlite3 *m_connection;
            std::string_view m_sql;
            const int m_code;
        public:
            SQLiteError( sqlite3 *connection, const int code, const std::string_view sql )
                : m_connection(connection)
                , m_code(code)
                , m_sql(sql)
            {}

            virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
            {
                return sqlite3_errmsg(m_connection);
            }

            std::string message() const
            {
                std::string message = "[";
                message += sqlite3_errstr(m_code);
                message += "] in [";
                message += m_sql;
                message += "] ";
                message += sqlite3_errmsg(m_connection);
                return message;
            }
        };

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
            const char *m_sql;
            int m_error_code = SQLITE_DONE;

            Context( )
                : m_stmt(nullptr)
                , m_error_code(SQLITE_DONE)
            { }

            Context( sqlite3 *connection, const char *sql, const std::size_t size )
                : m_stmt(nullptr)
                , m_sql(sql)
            {
                m_error_code = sqlite3_prepare_v2(connection, sql, size, &m_stmt, NULL);
                if (m_error_code != SQLITE_OK)
                {
                    throw SQLiteError(connection, m_error_code, std::string_view(m_sql, size));
                }
            }

            ~Context( )
            {
                if (m_stmt)
                {
                  //  sqlite3_finalize(m_stmt);
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

            void reset()
            {
                sqlite3_close(m_database_connection);
                sqlite3_open(":memory:", &m_database_connection);
            }

            static SQLite* instance()
            {
                static SQLite s_instance;
                return &s_instance;
            }

            static void execute( const std::string_view &sql )
            {
                const auto rc = sqlite3_exec(instance()->m_database_connection, sql.data(), nullptr, nullptr, nullptr);
                if (rc != SQLITE_OK)
                {
                    throw SQLiteError(instance()->m_database_connection, rc, sql);
                }
            }

            static std::ptrdiff_t get_last_insert_id()
            {
                return sqlite3_last_insert_rowid(instance()->m_database_connection);
            }

            template<typename T>
            static context_t make_context( const char *sql, const std::size_t size , T &&data )
            {
                context_t result(instance()->m_database_connection, sql, size);
                set::bind(result, data);
                return result;
            }
        };
    }
}

