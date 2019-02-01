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

    inline void add( std::string::iterator &it, const string_t &str )
    {
        it = std::copy(str.begin(), str.end(), it);
    }

    inline  void constexpr add( std::size_t &size, const string_t &str )
    {
        size += str.size();
    }

    template<char ... String>
    struct StaticArray
    {
        static const constexpr char data[] = {String ... };
        static const constexpr std::size_t size = sizeof ... (String);
    };

    template<char ... String>
    inline void constexpr add( std::string::iterator &it, const StaticArray<String...> &str )
    {
        it = std::copy(std::begin(str.data), std::end(str.data), it);
    }

    template<char ... String>
    inline void constexpr add( std::size_t &size, const StaticArray<String...> &str )
    {
        size += str.size;
    }

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

    template<typename Table, typename Name>
    struct alias : public Table
    {
        using name_t = Name;
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

            using column_t = C;

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

        template< typename OP, typename _Table, typename _NameString, typename _Type, typename _Flags>
        inline auto make_operation( const Column<_Table, _NameString, _Type, _Flags> &value ) const
        {
            static_assert(std::is_same<type_t, _Type>::value, "Parameter type does not compatible");
            using other_t = Column<_Table, _NameString, _Type, _Flags>;

            using namespace operators::comparasion;

            return Comparasion<OP, this_t, other_t>(value); // criar estrutura adequada para armazenar esta informaçao em um tipo
        }

        inline auto operator =( const char *value ) const
        {
            return assign::Value<this_t, std::string_view>(std::string_view(value));
        }

        template< typename VALUE >
        inline auto operator =( const VALUE &value ) const
        {
            static_assert(std::is_same<type_t, VALUE>::value, "Parameter type does not compatible");
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

            template<typename Table, typename Exp>
            static auto left_other_join( Exp &&expression )
            {
                return SelectFrom<SELECT, FROM>();
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
            static const constexpr string_t m_text_sep = string_t(",");
            static const constexpr string_t m_open = string_t("(");
            static const constexpr string_t m_values = string_t(") VALUES (");
            static const constexpr string_t m_quest = string_t("?");
            static const constexpr string_t m_end = string_t(");");

            Insert( DATA &&data )
                : m_data(std::move(data))
            { }

            template<typename COLUMN, typename F >
            static constexpr void accumulate_columns( F &out, const std::size_t index )
            {
                using flags_t = typename COLUMN::flags_t;

                if (index > 0)
                {
                    add(out, m_text_sep);
                }

                add(out, typename COLUMN::name_t());
            }

            template<typename COLUMN, typename F >
            static constexpr void accumulate_args( F &out, const std::size_t index )
            {
                using flags_t = typename COLUMN::flags_t;

                if (index > 0)
                {
                    add(out, m_text_sep);
                }

                add(out, m_quest);
            }

            template<typename F>
            static constexpr void accumulate( F &out )
            {
                add(out, m_text);
                add(out, typename TABLE::name_t());
                add(out, m_open);
                std::size_t index = 0;
                ((accumulate_columns<typename T::column_t>(out, index++)), ... ); // columns names and separators
                add(out, m_values);
                index = 0;
                ((accumulate_args<typename T::column_t>(out, index++)), ... ); // columns names and separators
                add(out, m_end);
            }

            static constexpr std::size_t size()
            {
                std::size_t size = 0;
                Insert::accumulate(size);
                return size;
            }

            static auto get_sql()
            {
                static const constexpr std::size_t size = Insert::size();
                std::string text(size, '.');

                auto it = std::begin(text);
                Insert::accumulate(it);

                std::cout << "sql: " << text << std::endl;
                return text;
            }

            auto run() const
            {                
                auto context = FACTORY::make_context(get_sql(), std::move(m_data));
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
            static const constexpr string_t m_pk_text = string_t(", constraint pk_");
            static const constexpr string_t m_integer_type = string_t(" INTEGER");
            static const constexpr string_t m_text_type = string_t(" TEXT");

            template<typename T, typename F >
            static constexpr void accumulate_constraint( F &out )
            {
                using flags_t = typename T::flags_t;
                if constexpr (flags_t::template is<pk>())
                {
                    using table_t = typename T::table_t;
                    add(out, m_pk_text);
                    add(out, typename table_t::name_t());
                    add(out, m_c);
                    add(out, m_b);
                    add(out, typename T::name_t());
                    add(out, m_f);
                }
            }

            template<typename T, typename F >
            static constexpr void accumulate_type( F &out )
            {
                if constexpr (std::is_same_v<std::string, T>)
                {
                    add(out, m_text_type);
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    add(out, m_text_type);
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    add(out, m_integer_type);
                }
            }

            template<typename T, typename F >
            static constexpr void accumulate_columns( F &out, const std::size_t index )
            {
                using flags_t = typename T::flags_t;

                if (index > 0)
                {
                    add(out, m_e);
                }

                add(out, typename T::name_t());
                accumulate_type<typename T::type_t>(out);
                if constexpr (flags_t::template is<not_null>())
                {
                    add(out, m_d);
                }
            }

            template<typename F>
            static constexpr void accumulate( F &out )
            {
                add(out, m_a);
                add(out, typename Table::name_t());
                add(out, m_b);
                std::size_t index = 0;
                (accumulate_columns<COLUMNS>(out, index++), ... );
                (accumulate_constraint<COLUMNS>(out), ... );
                add(out, m_f);
                add(out, m_g);
            }

            static constexpr std::size_t size()
            {
                std::size_t size = 0;
                TableCreator::accumulate(size);
                return size;
            }

            static auto get_sql()
            {
                static const constexpr std::size_t size = TableCreator::size();
                std::string text(size, '.');
                auto it = std::begin(text);
                TableCreator::accumulate(it);
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
