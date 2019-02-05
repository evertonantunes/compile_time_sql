#include <tuple>
#include <vector>
#include <algorithm>
#include <type_traits>

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
    inline void add( std::string::iterator &it, const StaticArray<String...> &str )
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
    };

    template<typename Table, typename Name>
    struct alias : public Table
    {
        using alias_t = Name;
        using this_t = alias<Table, Name>;

        template<typename T>
        static constexpr auto as( const T & )
        {
            return T::template alias<this_t>();
        }
    };

    template<typename Table, typename Name>
    constexpr bool is_alias( const alias<Table, Name> & )
    {
        return true;
    }

    template<typename Name>
    constexpr bool is_alias( const Table<Name> & )
    {
        return false;
    }

    namespace operators
    {
        template<typename T>
        struct Operator
        {
            template<typename F>
            static constexpr void accumulate( F &out )
            {
                add(out, T());
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

                auto data() const
                {
                    return std::tuple_cat(m_l.data(), m_r.data());
                }

                template<typename F>
                static constexpr void accumulate( F &out )
                {
                    L::accumulate(out);
                    OP::accumulate(out);
                    R::accumulate(out);
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

            template<typename OP, typename L, typename R>
            struct TypeLogical
            {
                TypeLogical( )
                {}

                template<typename F>
                static constexpr void accumulate( F &out )
                {
                    L::accumulate(out);
                    OP::accumulate(out);
                    R::accumulate(out);
                }

                template< typename T >
                auto operator &&( T && )
                {
                    return TypeLogical<And, Logical<OP,L,R>, T>();
                }

                template< typename T >
                auto operator ||( T && )
                {
                    return TypeLogical<Or, Logical<OP,L,R>, T>();
                }
            };
        }

        namespace comparation
        {
            using Equals = Operator<decltype(" == "_s)>;
            using NotEquals = Operator<decltype(" != "_s)>;
            using LessThan = Operator<decltype(" < "_s)>;
            using LessThanEqual = Operator<decltype(" <= "_s)>;
            using GreaterThan = Operator<decltype(" > "_s)>;
            using GreaterThanEqual = Operator<decltype(" >= "_s)>;

            template<typename OP, typename T, typename H>
            struct Comparation
            {
                static constexpr const string_t m_text = string_t("?");

                H m_data;

                Comparation( H data  )
                    : m_data(data)
                { }

                template<typename F>
                static constexpr void accumulate( F &out )
                {
                    add(out, typename T::name_t());
                    OP::accumulate(out);
                    add(out, m_text);
                }

                auto data() const
                {
                    return std::make_tuple(m_data);
                }

                template< typename OTHER >
                auto operator &&( OTHER &&other )
                {
                    return logical::Logical<logical::And, Comparation<OP,T,H>, OTHER>(std::move(*this), std::move(other));
                }

                template< typename OTHER >
                auto operator ||( OTHER &&other )
                {
                    return logical::Logical<logical::Or, Comparation<OP, T,H>, OTHER>(std::move(*this), std::move(other));
                }
            };

            template<typename OP, typename T, typename H>
            struct TypeComparation
            {
                TypeComparation()
                { }

                template<typename COLUMN, typename OUT>
                static constexpr void accumulate_column( OUT &out )
                {
                    using table_t = typename COLUMN::table_t;

                    if constexpr (is_alias(table_t()))
                    {
                        add(out, typename table_t::alias_t());
                        add(out, ".");
                        add(out, typename COLUMN::name_t());
                    }
                    else
                    {
                        add(out, typename table_t::name_t());
                        add(out, ".");
                        add(out, typename COLUMN::name_t());
                    }
                }

                template<typename F>
                static constexpr void accumulate( F &out )
                {
                    accumulate_column<T>(out);
                    OP::accumulate(out);
                    accumulate_column<H>(out);
                }

                template< typename OTHER >
                auto operator &&( OTHER && )
                {
                    return logical::TypeLogical<logical::And, TypeComparation<OP,T,H>, OTHER>();
                }

                template< typename OTHER >
                auto operator ||( OTHER && )
                {
                    return logical::TypeLogical<logical::Or, TypeComparation<OP,T,H>, OTHER>();
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

        template<typename T>
        static auto alias()
        {
            return Column<T, name_t, type_t, flags_t>();
        }

        template< typename OP, typename VALUE >
        inline auto make_operation( const VALUE &value ) const
        {
            using namespace operators::comparation;

            if constexpr (std::is_same<const char*, VALUE>::value)
            {
                static_assert(std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>, "Parameter type does not compatible");
                return Comparation<OP, this_t, std::string_view>(std::string_view(value));
            }
            else if constexpr (std::is_same_v<std::string, VALUE>)
            {
                static_assert(std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>, "Parameter type does not compatible");
                return Comparation<OP, this_t, const std::string&>(value);
            }
            else
            {
                static_assert(std::is_same<type_t, VALUE>::value, "Parameter type does not compatible");
                return Comparation<OP, this_t, VALUE>(value);
            }
        }

        template< typename OP, typename _Table, typename _NameString, typename _Type, typename _Flags>
        inline auto make_operation( const Column<_Table, _NameString, _Type, _Flags> &value ) const
        {
            static_assert(std::is_same<type_t, _Type>::value, "Parameter type does not compatible");
            using other_t = Column<_Table, _NameString, _Type, _Flags>;
            return operators::comparation::TypeComparation<OP, this_t, other_t>();
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
            return make_operation<operators::comparation::Equals>(value);
        }

        inline auto operator ==( const auto &value ) const
        {
            return make_operation<operators::comparation::Equals>(value);
        }

        inline auto operator !=( const char *value ) const
        {
            return make_operation<operators::comparation::NotEquals>(value);
        }

        inline auto operator !=( const auto &value ) const
        {
            return make_operation<operators::comparation::NotEquals>(value);
        }

        inline auto operator <( const char *value ) const
        {
            return make_operation<operators::comparation::LessThan>(value);
        }

        inline auto operator <( const auto &value ) const
        {
            return make_operation<operators::comparation::LessThan>(value);
        }

        inline auto operator <=( const char *value ) const
        {
            return make_operation<operators::comparation::LessThanEqual>(value);
        }

        inline auto operator <=( const auto &value ) const
        {
            return make_operation<operators::comparation::LessThanEqual>(value);
        }

        inline auto operator >( const char *value ) const
        {
            return make_operation<operators::comparation::GreaterThan>(value);
        }

        inline auto operator >( const auto &value ) const
        {
            return make_operation<operators::comparation::GreaterThan>(value);
        }

        inline auto operator >=( const char *value ) const
        {
            return make_operation<operators::comparation::GreaterThanEqual>(value);
        }

        inline auto operator >=( const auto &value ) const
        {
            return make_operation<operators::comparation::GreaterThanEqual>(value);
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

        template<typename T, typename WHERE>
        struct SelectFromWHERE
        {
            WHERE m_where;

            using ConnectionFactory = typename T::ConnectionFactory;

            using iterator_t = typename T::iterator_t;

            SelectFromWHERE( WHERE &&where )
                : m_where(std::move(where))
            {}

            template<typename F>
            static constexpr void accumulate( F &out )
            {
                T::accumulate(out);
                add(out, "WHERE ");
                WHERE::accumulate(out);
                add(out, ";");
            }

            static constexpr std::size_t size()
            {
                std::size_t size = 0;
                SelectFromWHERE::accumulate(size);
                return size;
            }

            auto get_sql() const
            {
                const constexpr std::size_t buffer_size = SelectFromWHERE::size();
                std::string text(buffer_size, '.');
                auto it = std::begin(text);
                SelectFromWHERE::accumulate(it);
                return text;
            }

            auto create_statement() const
            {
                return ConnectionFactory::make_context(get_sql(), m_where.data());
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

        template<typename StringType, typename Table, typename EXP>
        struct Join
        {
            template<typename OUT>
            static constexpr void accumulate( OUT &out )
            {
                add(out, StringType());
                if constexpr (is_alias(Table()))
                {
                    add(out, typename Table::name_t());
                    add(out, " as ");
                    add(out, typename Table::alias_t());
                }
                else
                {
                    add(out, typename Table::table_t());
                }
                add(out, " ON ");
                EXP::accumulate(out);
                add(out, " ");
            }
        };

        template<typename SELECT, typename FROM, typename ... JOIN>
        struct SelectFrom
        {
            using row_t = typename SELECT::row_t;
            using ConnectionFactory = typename SELECT::ConnectionFactory;
            using iterator_t = Iterator<typename ConnectionFactory::context_t, typename SELECT::row_t>;

            using this_t = SelectFrom<SELECT, FROM, JOIN...>;

            template<typename T>
            static auto where( T &&expression )
            {
                return SelectFromWHERE<this_t, T>(std::move(expression));
            }

            template<typename Table, typename Exp>
            static constexpr auto left_join( Exp && )
            {
                using join_type_t = decltype("LEFT JOIN "_s);
                return SelectFrom<SELECT, FROM, JOIN..., Join<join_type_t, Table, Exp>>();
            }

            template<typename OUT>
            static constexpr void accumulate( OUT &out )
            {
                SELECT::accumulate(out);
                FROM::accumulate(out);
                (JOIN::accumulate(out), ... );
            }

            static constexpr std::size_t size()
            {
                std::size_t size = 0;
                SelectFrom::accumulate(size);
                return size;
            }

            auto get_sql() const
            {
                const constexpr std::size_t buffer_size = SelectFrom::size() + 1;
                std::string text(buffer_size, '.');
                auto it = std::begin(text);
                SelectFrom::accumulate(it);
                *it = ';';
                return text;
            }

            auto create_statement() const
            {
                return ConnectionFactory::make_context(get_sql(), std::tuple<>{});
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
            template<typename F>
            static constexpr void accumulate( F &out )
            {
                add(out, "FROM ");
                if constexpr (is_alias(T()))
                {
                    add(out, typename T::name_t());
                    add(out, " as ");
                    add(out, typename T::alias_t());
                }
                else
                {
                    add(out, typename T::name_t());
                }
                add(out, " ");
            }
        };

        template<typename F, typename ...T>
        struct Select
        {
            using ConnectionFactory = F;
            typedef std::tuple< typename T::type_t ... > row_t;

            template<typename COLUMN, typename OUT >
            static constexpr void accumulate_columns( OUT &out, const std::size_t index )
            {
                using flags_t = typename COLUMN::flags_t;
                using table_t = typename COLUMN::table_t;

                if (index > 0)
                {
                    add(out, ", ");
                }
                if constexpr (is_alias(table_t()))
                {
                    add(out, typename table_t::alias_t());
                    add(out, ".");
                    add(out, typename COLUMN::name_t());
                }
                else
                {
                    add(out, typename table_t::name_t());
                    add(out, ".");
                    add(out, typename COLUMN::name_t());
                }
            }

            template<typename OUT>
            static constexpr void accumulate( OUT &out )
            {
                add(out, "SELECT ");
                std::size_t index = 0;
                (accumulate_columns<T>(out, index++), ... ); // columns names and separators
                add(out, " ");
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

            Insert( DATA &&data )
                : m_data(std::move(data))
            { }

            template<typename COLUMN, typename F >
            static constexpr void accumulate_columns( F &out, const std::size_t index )
            {
                using flags_t = typename COLUMN::flags_t;

                if (index > 0)
                {
                    add(out, ",");
                }

                add(out, typename COLUMN::name_t());
            }

            template<typename COLUMN, typename F >
            static constexpr void accumulate_args( F &out, const std::size_t index )
            {
                using flags_t = typename COLUMN::flags_t;

                if (index > 0)
                {
                    add(out, ",");
                }

                add(out, "?");
            }

            template<typename F>
            static constexpr void accumulate( F &out )
            {
                add(out, "INSERT INTO ");
                add(out, typename TABLE::name_t());
                add(out, "(");
                std::size_t index = 0;
                ((accumulate_columns<typename T::column_t>(out, index++)), ... ); // columns names and separators
                add(out, ") VALUES (");
                index = 0;
                ((accumulate_args<typename T::column_t>(out, index++)), ... ); // columns names and separators
                add(out, ");");
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
            template<typename T, typename F >
            static constexpr void accumulate_constraint( F &out )
            {
                using flags_t = typename T::flags_t;
                if constexpr (flags_t::template is<pk>())
                {
                    using table_t = typename T::table_t;
                    add(out, ", constraint pk_");
                    add(out, typename table_t::name_t());
                    add(out, " PRIMARY KEY");
                    add(out, " (");
                    add(out, typename T::name_t());
                    add(out, ")");
                }
            }

            template<typename T, typename F >
            static constexpr void accumulate_type( F &out )
            {
                if constexpr (std::is_same_v<std::string, T>)
                {
                    add(out, " TEXT");
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    add(out, " TEXT");
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    add(out, " INTEGER");
                }
            }

            template<typename T, typename F >
            static constexpr void accumulate_columns( F &out, const std::size_t index )
            {
                using flags_t = typename T::flags_t;

                if (index > 0)
                {
                    add(out, ",");
                }

                add(out, typename T::name_t());
                accumulate_type<typename T::type_t>(out);
                if constexpr (flags_t::template is<not_null>())
                {
                    add(out, " NOT NULL");
                }
            }

            template<typename F>
            static constexpr void accumulate( F &out )
            {
                add(out, "CREATE TABLE ");
                add(out, typename Table::name_t());
                add(out, "(");
                std::size_t index = 0;
                (accumulate_columns<COLUMNS>(out, index++), ... );
                (accumulate_constraint<COLUMNS>(out), ... );
                add(out, ")");
                add(out, ";");
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
