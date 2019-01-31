#include <tuple>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <assert.h>

#include <iostream>

namespace std
{
    template<typename T, typename H>
    inline constexpr void copy( const T &from, H &to )
    {
        to = std::copy(std::begin(from), std::end(from), to);
    }
}

namespace sql
{
    using string_t = std::string_view;

    template<char ... String>
    struct StaticArray
    {
        static const constexpr char data[] = {String ... };
        static const constexpr std::size_t size = sizeof ... (String);
    };

    template<typename CharT, CharT ...String>
    constexpr auto operator "" _s( )
    {
        return StaticArray<String...>();
    }

    template<typename T>
    struct Table
    {
        using name_t = T;

        static constexpr auto name()
        {
            return std::string_view(name_t::data, name_t::size);
        }

        static constexpr auto table_name()
        {
            return name();
        }
    };

    namespace operators
    {
        template<typename T>
        struct Operator
        {
            template<typename I>
            static constexpr void write( I &iterator )
            {
                copy(T::data, iterator);
            }

            static constexpr std::size_t size()
            {
                return T::size;
            }
        };

        namespace logical
        {
            using And = Operator<decltype(" AND "_s)>;
            using Or = Operator<decltype(" OR "_s)>;

            template<typename OP, typename L, typename R>
            struct Logical
            {
                L m_l;
                R m_r;
                Logical( L &&l, R &&r )
                    : m_l(std::move(l))
                    , m_r(std::move(r))
                {}

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    L::write(iterator);
                    OP::write(iterator);
                    R::write(iterator);
                }

                auto data() const
                {
                    return std::tuple_cat(m_l.data(), m_r.data());
                }

                static constexpr std::size_t size()
                {
                    return L::size() + OP::size() + R::size();
                }

                template< typename T >
                auto operator &&( T &&other )
                {
                    return Logical<And, Logical<OP,L,R>, T>(std::move(*this), std::move(other));
                }

                template< typename T >
                auto operator ||( T &&other )
                {
                    return Logical<Or, Logical<OP,L,R>, T>(std::move(*this), std::move(other));
                }
            };
        }

        namespace comparasion
        {
            using Equals = Operator<decltype(" == "_s)>;
            using NotEquals = Operator<decltype(" != "_s)>;
            using LessThan = Operator<decltype(" < "_s)>;
            using LessThanEqual = Operator<decltype(" <= "_s)>;
            using GreaterThan = Operator<decltype(" > "_s)>;
            using GreaterThanEqual = Operator<decltype(" >= "_s)>;

            template<typename OP, typename T, typename H>
            struct Comparasion
            {
                static constexpr const string_t m_text = string_t("?");

                H m_data;

                Comparasion( H data  )
                    : m_data(data)
                { }

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(T::name(), iterator);
                    OP::write(iterator);
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return T::name_t::size + OP::size() + m_text.size();
                }

                auto data() const
                {
                    return std::make_tuple(m_data);
                }

                template< typename OTHER >
                auto operator &&( OTHER &&other )
                {
                    return logical::Logical<logical::And, Comparasion<OP,T,H>, OTHER>(std::move(*this), std::move(other));
                }

                template< typename OTHER >
                auto operator ||( OTHER &&other )
                {
                    return logical::Logical<logical::Or, Comparasion<OP, T,H>, OTHER>(std::move(*this), std::move(other));
                }
            };
        }
    }

    namespace assign
    {
        template<typename C, typename T>
        struct Value
        {
            T m_data;

            Value( T data  )
                : m_data(data)
            { }

            static constexpr auto name()
            {
                return C::name();
            }

            static constexpr auto size()
            {
                return C::name_t::size;
            }

            auto data() const
            {
                return std::make_tuple(m_data);
            }
        };
    }

    struct not_null {};
    struct pk {};
    struct fk {};

    template<typename ...T>
    struct Flags
    {
        using type = Flags<T...>;

        template<typename H>
        static constexpr bool is()
        {
            const constexpr bool result = ((std::is_same_v<T, H>) || ... || false);
            return result;
        }
    };

    template<typename Table, typename NameString, typename Type, typename Flags>
    struct Column
    {
        using type_t = Type;
        using table_t = Table;
        using name_t = NameString;
        using flags_t = Flags;

        using this_t = Column<table_t, name_t, type_t, flags_t>;

        static constexpr auto name()
        {
            return std::string_view(name_t::data, name_t::size);
        }

        template< typename OP, typename VALUE >
        inline auto make_operation( const VALUE &value ) const
        {
            using namespace operators::comparasion;

            if constexpr (std::is_same<const char*, VALUE>::value)
            {
                static_assert(std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>, "Parameter type does not compatible");
                return Comparasion<OP, this_t, std::string_view>(std::string_view(value));
            }
            else if constexpr (std::is_same_v<std::string, VALUE>)
            {
                static_assert(std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>, "Parameter type does not compatible");
                return Comparasion<OP, this_t, const std::string&>(value);
            }
            else
            {
                static_assert(std::is_same<type_t, VALUE>::value, "Parameter type does not compatible");
                return Comparasion<OP, this_t, VALUE>(value);
            }
        }

        inline auto operator =( const char *value ) const
        {
            return assign::Value<this_t, std::string_view>(std::string_view(value));
        }

        template< typename VALUE >
        inline auto operator =( const VALUE &value ) const
        {
            return assign::Value<this_t, VALUE>(value);
        }

        inline auto operator ==( const char *value ) const
        {
            return make_operation<operators::comparasion::Equals>(value);
        }

        inline auto operator ==( const auto &value ) const
        {
            return make_operation<operators::comparasion::Equals>(value);
        }

        inline auto operator !=( const char *value ) const
        {
            return make_operation<operators::comparasion::NotEquals>(value);
        }

        inline auto operator !=( const auto &value ) const
        {
            return make_operation<operators::comparasion::NotEquals>(value);
        }

        inline auto operator <( const char *value ) const
        {
            return make_operation<operators::comparasion::LessThan>(value);
        }

        inline auto operator <( const auto &value ) const
        {
            return make_operation<operators::comparasion::LessThan>(value);
        }

        inline auto operator <=( const char *value ) const
        {
            return make_operation<operators::comparasion::LessThanEqual>(value);
        }

        inline auto operator <=( const auto &value ) const
        {
            return make_operation<operators::comparasion::LessThanEqual>(value);
        }

        inline auto operator >( const char *value ) const
        {
            return make_operation<operators::comparasion::GreaterThan>(value);
        }

        inline auto operator >( const auto &value ) const
        {
            return make_operation<operators::comparasion::GreaterThan>(value);
        }

        inline auto operator >=( const char *value ) const
        {
            return make_operation<operators::comparasion::GreaterThanEqual>(value);
        }

        inline auto operator >=( const auto &value ) const
        {
            return make_operation<operators::comparasion::GreaterThanEqual>(value);
        }
    };

    namespace impl
    {
        template<typename context_t, typename T>
        class Iterator : public std::iterator<std::forward_iterator_tag, T*>
        {
            context_t m_context;
            T m_data;

        public:
            Iterator( )
                : m_context()
            { }

            Iterator( context_t &&context )
                : m_context(std::move(context))
            {
                next(m_context, m_data);
            }

            Iterator ( const Iterator &other )
                : m_context(other.m_context)
            { }

            Iterator& operator++()
            {
                next(m_context, m_data);
                return *this;
            }

            bool operator==(const Iterator& rhs)
            {
                return is_equal(m_context, rhs.m_context);
            }

            bool operator!=(const Iterator& rhs)
            {
                return !is_equal(m_context, rhs.m_context);
            }

            T& operator*()
            {
                return m_data;
            }
        };

        template<typename SELECT, typename FROM, typename WHARE>
        struct SelectFromWhare
        {
            static const constexpr string_t m_text = string_t(" WHERE ");
            static const constexpr string_t m_text_end = string_t(";");

            WHARE m_whare;

            using ConnectionFactory = typename SELECT::ConnectionFactory;

            using iterator_t = Iterator<typename ConnectionFactory::context_t, typename SELECT::row_t>;

            SelectFromWhare( WHARE &&whare )
                : m_whare(std::move(whare))
            {}

            static constexpr std::size_t size()
            {
                return  SELECT::size() + FROM::size() + m_text.size() + WHARE::size() + m_text_end.size();
            }

            template<typename IT>
            static constexpr void write( IT &iterator )
            {
                copy(m_text, iterator);
                WHARE::write(iterator);
                copy(m_text_end, iterator);
            }

            auto get_query() const
            {
                const constexpr std::size_t buffer_size = SelectFromWhare::size();

                std::string text(buffer_size, '.');
                auto it = std::begin(text);

                SELECT::write(it);
                FROM::write(it);
                SelectFromWhare::write(it);

                assert(it == std::end(text));

                return text;
            }

            auto create_statement() const
            {
                return ConnectionFactory::make_context(get_query(), m_whare.data());
            }

            iterator_t begin() const
            {
                return iterator_t(create_statement());
            }

            iterator_t end() const
            {
                return iterator_t();
            }
        };

        template<typename SELECT, typename FROM>
        struct SelectFrom
        {
            static const constexpr string_t m_text_end = string_t(";");
            using ConnectionFactory = typename SELECT::ConnectionFactory;
            using iterator_t = Iterator<typename ConnectionFactory::context_t, typename SELECT::row_t>;

            template<typename T>
            static auto where( T &&expression )
            {
                return SelectFromWhare<SELECT, FROM, T>(std::move(expression));
            }

            static constexpr std::size_t size()
            {
                return  SELECT::size() + FROM::size() + m_text_end.size();
            }

            auto get_query() const
            {
                const constexpr std::size_t buffer_size = SelectFrom::size();

                std::string text(buffer_size, '.');
                auto it = std::begin(text);

                SELECT::write(it);
                FROM::write(it);
                copy(m_text_end, it);

                assert(it == std::end(text));

                return text;
            }

            auto create_statement() const
            {
                return ConnectionFactory::make_context(get_query(), std::tuple<>{});
            }

            iterator_t begin() const
            {
                return iterator_t(create_statement());
            }

            iterator_t end() const
            {
                return iterator_t();
            }
        };

        template<typename T>
        struct From
        {
            static const constexpr string_t m_data = string_t("FROM ");

            template<typename IT>
            static constexpr void write( IT &iterator )
            {
                copy(m_data, iterator);
                copy(T::table_name(), iterator);
            }

            static constexpr std::size_t size()
            {
                return  m_data.size() + T::table_name().size();
            }
        };

        template<typename F, typename ...T>
        struct Select
        {
            using ConnectionFactory = F;
            typedef std::tuple< typename T::type_t ... > row_t;

            static const constexpr string_t m_text = string_t("SELECT ");
            static const constexpr string_t m_text_sep = string_t(", ");
            static const constexpr string_t m_text_end = string_t(" ");

            template<typename IT>
            static constexpr void write( IT &iterator )
            {
                copy(m_text, iterator);
                const auto names = {T::name()...};
                std::for_each(begin(names), std::prev(end(names)), [&]( const auto &item )
                {
                    copy(item, iterator);
                    copy(m_text_sep, iterator);
                });
                copy(*std::prev(end(names)), iterator);
                copy(m_text_end, iterator);
            }

            static constexpr std::size_t size()
            {
                return    m_text.size()
                        + ((T::name_t::size) + ... + 0) // columns names sizes
                        + ((sizeof ... (T) - 1) * m_text_sep.size()) // columns names separators
                        + m_text_end.size(); // last separator
            }

            template<typename H>
            static auto from()
            {
                using select_t = Select<F, T...>;
                return SelectFrom<select_t, From<H>>();
            }
        };

        template<typename FACTORY, typename TABLE, typename DATA, typename ...T>
        struct Insert
        {
            DATA m_data;

            static const constexpr string_t m_text = string_t("INSERT INTO ");
            static const constexpr string_t m_text_sep = string_t(", ");
            static const constexpr string_t m_space = string_t(" ");
            static const constexpr string_t m_open = string_t("(");
            static const constexpr string_t m_values = string_t(") VALUES (");
            static const constexpr string_t m_quest = string_t("?");
            static const constexpr string_t m_end = string_t(");");

            Insert( DATA &&data )
                : m_data(std::move(data))
            { }

            template<typename IT>
            void write( IT &iterator ) const
            {
                copy(m_text, iterator);
                copy(TABLE::table_name(), iterator);
                copy(m_space, iterator);
                copy(m_open, iterator);

                const auto names = {T::name()...};

                std::for_each(begin(names), std::prev(end(names)), [&]( const auto &item )
                {
                    copy(item, iterator);
                    copy(m_text_sep, iterator);
                });
                copy(*std::prev(end(names)), iterator);

                copy(m_values, iterator);

                std::for_each(begin(names), std::prev(end(names)), [&]( const auto & )
                {
                    copy(m_quest, iterator);
                    copy(m_text_sep, iterator);
                });
                copy(m_quest, iterator);
                copy(m_end, iterator);
            }

            static constexpr std::size_t size()
            {
                return    m_text.size()
                        + TABLE::table_name().size()
                        + m_space.size()
                        + m_open.size()
                        + ((T::size()) + ... + 0) // columns names sizes
                        + ((sizeof ... (T) - 1) * m_text_sep.size()) // columns names separators
                        + m_values.size()
                        + (sizeof ... (T) * m_quest.size()) // columns names separators
                        + ((sizeof ... (T) - 1) * m_text_sep.size()) // columns names separators
                        + m_end.size(); // last separator
                return 0;
            }

            std::string get_query() const
            {
                const constexpr std::size_t buffer_size = Insert::size();
                std::string text(buffer_size, '.');
                auto it = std::begin(text);
                this->write(it);
                assert(it == end(text));
                return text;
            }

            auto run() const
            {                
                auto context = FACTORY::make_context(get_query(), std::move(m_data));
                std::tuple<> tp;
                next(context, tp);
                return FACTORY::get_last_insert_id();
            }
        };

        template<typename Factory, typename Table, typename ...COLUMNS>
        struct TableCreator
        {
            static const constexpr string_t m_a = string_t("CREATE TABLE ");
            static const constexpr string_t m_b = string_t(" (");
            static const constexpr string_t m_c = string_t(" PRIMARY KEY");
            static const constexpr string_t m_d = string_t(" NOT NULL");
            static const constexpr string_t m_e = string_t(",");
            static const constexpr string_t m_f = string_t(")");
            static const constexpr string_t m_g = string_t(";");

            static const constexpr string_t m_pk_text = string_t(" , constraint pk_");

            static const constexpr string_t m_integer_type = string_t(" INTEGER ");
            static const constexpr string_t m_text_type = string_t(" TEXT ");

            template<typename T>
            static constexpr std::size_t get_type_size()
            {
                if constexpr (std::is_same_v<std::string, T>)
                {
                    return m_integer_type.size();
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    return m_text_type.size();
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    return m_text_type.size();
                }
                return 0l;
            }

            template<typename T>
            static constexpr auto get_column_size()
            {
                using flags_t = typename T::flags_t;
                std::size_t result = T::name_t::size;

                result += get_type_size<typename T::type_t>();

                if constexpr (flags_t::template is<not_null>())
                {
                    result += m_d.size();
                }

                if constexpr (flags_t::template is<pk>())
                {
                    using table_t = typename T::table_t;
                    result += m_pk_text.size();
                    result += table_t::name_t::size;
                    result += m_c.size();
                    result += m_b.size();
                    result += T::name_t::size;
                    result += m_f.size();
                }

                return result;
            }

            static constexpr auto size()
            {
                return   m_a.size()
                       + Table::name_t::size
                       + m_b.size()
                       + ((get_column_size<COLUMNS>() + m_e.size()) + ... + -1 )
                       + m_f.size()
                       + m_f.size()
                       + m_g.size();
            }

            template<typename T, typename I>
            static constexpr void write_constraint( I &it )
            {
                using flags_t = typename T::flags_t;
                if constexpr (flags_t::template is<pk>())
                {
                    using table_t = typename T::table_t;
                    copy(m_pk_text, it);
                    copy(table_t::name_t::data, it);
                    copy(m_c, it);
                    copy(m_b, it);
                    copy(T::name_t::data, it);
                    copy(m_f, it);
                }
            }

            template<typename T, typename I>
            static constexpr void write_type( I &it )
            {
                if constexpr (std::is_same_v<std::string, T>)
                {
                    copy(m_text_type, it);
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    copy(m_text_type, it);
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    copy(m_integer_type, it);
                }
            }

            template<typename T, typename I>
            static constexpr void write_column( I &it )
            {
                using flags_t = typename T::flags_t;
                copy(T::name_t::data, it);
                write_type<typename T::type_t>(it);
                if constexpr (flags_t::template is<not_null>())
                {
                    copy(m_d, it);
                }
            }

            template<typename I>
            static auto write( I &it )
            {
                copy(m_a, it);
                copy(Table::name_t::data, it);
                copy(m_b, it);
                ((write_column<COLUMNS>(it), copy(m_e, it)), ..., it-- );
                ((write_constraint<COLUMNS>(it)), ... );
                copy(m_f, it);
                copy(m_g, it);
            }

            static auto get_sql()
            {
                static const constexpr std::size_t size = TableCreator::size();
                std::string text(size, '.');
                auto it = std::begin(text);
                write(it);
                std::cout << "sql: " << text << std::endl;
                assert(it == std::end(text));
                return text;

            }

            static void run()
            {
                Factory::instance()->execute(get_sql());
            }
        };

        template<typename Factory, typename Table, typename ...COLUMNS>
        static void create_table( const std::tuple<COLUMNS...> & )
        {
            TableCreator<Factory, Table, COLUMNS...>::run();
        }
    }

    template<typename F, typename ...T>
    auto select( T ... )
    {
        return impl::Select<F, T...>();
    }

    template<typename FACTORY, typename TABLE, typename ...COLUMNS>
    auto insert_into( COLUMNS &&...args )
    {
        auto _tup = std::tuple_cat(args.data()...);
        return impl::Insert<FACTORY, TABLE, decltype(_tup), COLUMNS...>(std::move(_tup)).run();
    }

    template<typename Factory, typename Table>
    static void create_table()
    {
        impl::create_table<Factory, Table>(typename Table::columns());
    }
}
