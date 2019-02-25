#pragma once

#include <sqlite3.h>
#include <stdexcept>
#include <thread>

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

                if (rc != SQLITE_OK)
                {
                    throw SQLiteError(context.m_connection, rc, context.m_sql);
                }
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

//                if (sqlite3_errcode(context.m_connection) != SQLITE_OK)
//                {
//                    throw SQLiteError(context.m_connection, sqlite3_errcode(context.m_connection), context.m_sql);
//                }
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
            sqlite3 *m_connection = nullptr;
            sqlite3_stmt *m_stmt = nullptr;
            const char *m_sql = nullptr;
            int m_error_code = SQLITE_DONE;

            Context( )
            { }

            Context( Context &&other )
                : m_connection(other.m_connection)
                , m_stmt(other.m_stmt)
                , m_sql(other.m_sql)
                , m_error_code(other.m_error_code)
            {
                other.m_connection = nullptr;
                other.m_stmt = nullptr;
                other.m_sql = nullptr;
                other.m_error_code = SQLITE_DONE;
            }

            Context( sqlite3 *connection, const char *sql, const std::size_t size )
                : m_connection(connection)
                , m_sql(sql)
            {
                m_error_code = sqlite3_prepare_v2(m_connection, sql, size, &m_stmt, NULL);
                if (m_error_code != SQLITE_OK)
                {
                    throw SQLiteError(m_connection, m_error_code, std::string_view(m_sql, size));
                }
            }

            ~Context( )
            {
                if (m_stmt)
                {
                    sqlite3_finalize(m_stmt);
                }
            }
        };

        template<class ... T>
        void next( Context &context, std::tuple<T...> &data )
        {
            while (true)
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
                        return;
                    }
                    break;
                    case SQLITE_LOCKED:
                    case SQLITE_BUSY:
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        continue;
                    break;
                    case SQLITE_DONE:
                        return;
                    default:
                        throw std::runtime_error(sqlite3_errstr(context.m_error_code));
                    break;
                }
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
                    throw std::runtime_error(sqlite3_errstr(rc));
                }
            }

            static std::ptrdiff_t get_last_insert_id()
            {
                return sqlite3_last_insert_rowid(instance()->m_database_connection);
            }

            static context_t make_context( const char *sql, const std::size_t size )
            {
                context_t result(instance()->m_database_connection, sql, size);
                return result;
            }

            template<typename T>
            static void bind( context_t &context, const T &data )
            {
                set::bind(context, data);
            }
        };
    }
}

