#include <tuple>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace sql
{
    template<char ... String>
    struct StaticArray
    {
        static const constexpr char m_data[] = {String ... };
        static const constexpr std::size_t m_size = sizeof ... (String);

        template<char ... String2>
        constexpr auto operator+( const StaticArray<String2...> & ) const
        {
            return StaticArray<String ..., String2...>();
        }

        std::string_view view() const
        {
            return std::string_view(&m_data[0], m_size);
        }

        std::string str() const
        {
            return std::string(&m_data[0], m_size);
        }

        const char* c_str() const
        {
            return &m_data[0];
        }

        constexpr std::size_t size() const
        {
            return m_size;
        }
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


    template<typename Type, typename Name>
    struct default_column
    {
        using type_t = Type;

        static constexpr auto to_string( )
        {
            return Name();
        }
    };

    namespace operators
    {
        template<typename T>
        struct Operator
        {
            static constexpr auto to_string( )
            {
                return T();
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

                static constexpr auto to_string( )
                {
                    return L::to_string() + OP::to_string() + R::to_string();
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

                static constexpr auto to_string( )
                {
                    return L::to_string() + OP::to_string() + R::to_string();
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
                H m_data;

                Comparation( H data  )
                    : m_data(data)
                { }

                static constexpr auto to_string( )
                {
                    return T::to_string() + OP::to_string() + "?"_s;
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

                static constexpr auto to_string( )
                {
                    return T::to_string() + OP::to_string() + H::to_string();
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

    template<typename T = void>
    struct fk
    {
        using column_t = T;
    };

    template< template<typename> class T, typename A>
    constexpr bool is_same_flag( T<A>,  fk<> )
    {
        return true;
    }

    template<typename T, typename H>
    constexpr bool is_same_flag( T, H )
    {
        return std::is_same_v<T, H>;
    }

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

        template<typename SELECT, typename F>
        static constexpr auto call_if( F &&func )
        {
            return (impl_call_if<T, SELECT>(func) + ... + ""_s);
        }

    private:
        template<typename ITEM, typename SELECT, typename F>
        static constexpr auto impl_call_if( F &func )
        {
            if constexpr (is_same_flag(ITEM(), SELECT()))
            {
                return func(ITEM());
            }
            else
            {
                return ""_s;
            }
        }
    };

    template<typename SEP, std::size_t M, std::size_t I>
    constexpr auto sep( )
    {
        if constexpr (I != 0)
        {
            return SEP();
        }
        else
        {
            return ""_s;
        }
    }

    template<typename SEP, std::size_t M, std::size_t I, char ...String>
    constexpr auto format( const StaticArray<String...> &data  )
    {
        if constexpr (sizeof ... (String) > 0)
        {
            return sep<SEP, M, I>() + data;
        }
        else
        {
            return ""_s;
        }
    }

    template<typename SEP = decltype(","_s), typename ...T, std::size_t ...I>
    constexpr auto join( const std::tuple<T...> &tup, std::index_sequence<I...> )
    {
        return (format<SEP, sizeof ... (T), I>(std::get<I>(tup)) + ... + ""_s );
    }

    template<typename C>
    static constexpr void nop()
    {}

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

        static constexpr auto to_string( )
        {
            if constexpr (is_alias(table_t()))
            {
                return typename table_t::alias_t() + "."_s + name_t();
            }
            else
            {
                return typename table_t::name_t() + "."_s + name_t();
            }
        }

        template< typename OP, typename VALUE >
        inline auto make_operation( const VALUE &value ) const
        {
            using namespace operators::comparation;

            if constexpr (std::is_same<const char*, VALUE>::value)
            {
                static_assert((std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>), "Parameter type does not compatible");
                return Comparation<OP, this_t, std::string_view>(std::string_view(value));
            }
            else if constexpr (std::is_same_v<std::string, VALUE>)
            {
                static_assert((std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>), "Parameter type does not compatible");
                return Comparation<OP, this_t, const std::string&>(value);
            }
            else
            {
                static_assert((std::is_same<type_t, VALUE>::value), "Parameter type does not compatible");
                return Comparation<OP, this_t, VALUE>(value);
            }
        }

        template< typename OP, typename _Table, typename _NameString, typename _Type, typename _Flags>
        inline auto make_operation( const Column<_Table, _NameString, _Type, _Flags> & ) const
        {
            static_assert((std::is_same<type_t, _Type>::value), "Parameter type does not compatible");
            using other_t = Column<_Table, _NameString, _Type, _Flags>;
            return operators::comparation::TypeComparation<OP, this_t, other_t>();
        }

        inline auto operator =( const char *value ) const
        {
            static_assert((std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>), "Parameter type does not compatible");
            return assign::Value<this_t, std::string_view>(std::string_view(value));
        }

        template< typename VALUE >
        inline auto operator =( const VALUE &value ) const
        {
            static_assert((std::is_same<type_t, VALUE>::value), "Parameter type does not compatible");
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

            bool operator==(const Iterator& rhs) const
            {
                return is_equal(m_context, rhs.m_context);
            }

            bool operator!=(const Iterator& rhs) const
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

            static constexpr auto to_string( )
            {
                return T::to_string() + " WHERE "_s + WHERE::to_string();
            }

            auto data() const
            {
                return m_where.data();
            }

            auto create_statement() const
            {
                static const constexpr auto text = to_string();
                auto context = ConnectionFactory::make_context(text.c_str(), text.size());
                ConnectionFactory::bind(context, m_where.data());
                return context;
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

        template<typename JoinType, typename Table, typename EXP>
        struct Join
        {
            static constexpr auto to_string( )
            {
                if constexpr (is_alias(Table()))
                {
                    return " "_s + JoinType() + typename Table::name_t() + " AS "_s + typename Table::alias_t() + " ON "_s + EXP::to_string();
                }
                else
                {
                    return " "_s + JoinType() + typename Table::name_t() + " ON "_s + EXP::to_string();
                }
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
            auto where( T &&expression )
            {
                return SelectFromWHERE<this_t, T>(std::move(expression));
            }

            template<typename Table, typename Exp>
            static constexpr auto left_join( Exp && )
            {
                using join_type_t = decltype("LEFT JOIN "_s);
                return SelectFrom<SELECT, FROM, JOIN..., Join<join_type_t, Table, Exp>>();
            }

            static constexpr auto to_string( )
            {
                return SELECT::to_string() + FROM::to_string() + (JOIN::to_string() + ... + ""_s);
            }

            auto create_statement() const
            {
                static const constexpr auto text = to_string();
                return ConnectionFactory::make_context(text.c_str(), text.size());
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
            static constexpr auto to_string( )
            {
                if constexpr (is_alias(T()))
                {
                    return "FROM "_s + typename T::name_t() + " AS "_s + typename T::alias_t();
                }
                else
                {
                    return "FROM "_s + typename T::name_t();
                }
            }
        };

        template<typename F, typename ...T>
        struct Select
        {
            using ConnectionFactory = F;
            typedef std::tuple< typename T::type_t ... > row_t;

            static constexpr auto to_string( )
            {
                return   "SELECT "_s
                       + join<decltype(", "_s)>(std::make_tuple(T::to_string() ... ), std::make_index_sequence<sizeof ... (T)>())
                       + " "_s;
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

            static constexpr auto to_string( )
            {
                return "INSERT INTO "_s
                        + typename TABLE::name_t()
                        + "("_s
                        + join(std::make_tuple((typename T::column_t::name_t()) ... ), std::make_index_sequence<sizeof ... (T)>())
                        + ") VALUES ("_s
                        + join(std::make_tuple((nop<T>(), "?"_s) ... ), std::make_index_sequence<sizeof ... (T)>())
                        + ");"_s;
            }

            auto run() const
            {                
                const constexpr auto text = to_string();
                auto context = FACTORY::make_context(text.c_str(), text.size());
                FACTORY::bind(context, m_data);
                std::tuple<> tp;
                next(context, tp);
                return FACTORY::get_last_insert_id();
            }
        };

        template<typename Factory, typename Table, typename ...COLUMNS>
        struct TableCreator
        {
            template<typename T>
            static constexpr auto get_constraint( )
            {
                using flags_t = typename T::flags_t;

                return flags_t::template call_if<pk>([]( const auto )
                {
                    using table_t = typename T::table_t;
                    return    "CONSTRAINT PK_"_s
                            + typename table_t::name_t()
                            + " PRIMARY KEY"_s
                            + " ("_s
                            + typename T::name_t()
                            + ")"_s;
                })
                +
                flags_t::template call_if<fk<>>([]( const auto fk )
                {
                    using fk_t = decltype(fk);
                    using fk_column_t = typename fk_t::column_t;
                    using fk_table_t = typename fk_column_t::table_t;

                    return   "FOREIGN KEY ("_s
                           + typename T::name_t()
                           + ") REFERENCES "_s
                           + typename fk_table_t::name_t()
                           + "("_s
                           + typename fk_column_t::name_t()
                           + ")"_s;
                });
            }

            template<typename T>
            static constexpr auto get_type( )
            {
                if constexpr (std::is_same_v<std::string, T>)
                {
                    return " TEXT"_s;
                }
                else if constexpr (std::is_same_v<std::string_view, T>)
                {
                    return " TEXT"_s;
                }
                else if constexpr (std::is_same_v<std::ptrdiff_t, T>)
                {
                    return " INTEGER"_s;
                }
            }

            template<typename T>
            static constexpr auto get_flag( )
            {
                if constexpr (T::template is<not_null>())
                {
                    return " NOT NULL"_s;
                }
            }

            template<typename T >
            static constexpr auto get_column( )
            {
                return   typename T::name_t()
                       + get_type<typename T::type_t>()
                       + get_flag<typename T::flags_t>();
            }

            static constexpr auto to_string( )
            {
                return    "CREATE TABLE "_s
                        + typename Table::name_t()
                        + "("_s
                        + join(std::make_tuple(get_column<COLUMNS>() ... ), std::make_index_sequence<sizeof ... (COLUMNS)>())
                        + ", "_s
                        + join(std::make_tuple(get_constraint<COLUMNS>() ... ), std::make_index_sequence<sizeof ... (COLUMNS)>())
                        + ");"_s;
            }

            static void run()
            {
                const constexpr auto text = to_string();
                Factory::instance()->execute(text.str());
            }
        };

        template<typename Factory, typename Table, typename ...COLUMNS>
        static void create_table( const std::tuple<COLUMNS...> & )
        {
            TableCreator<Factory, Table, COLUMNS...>::run();
        }

        template<typename FACTORY, typename QUERY, typename DATA, typename Iterator>
        struct Union_all
        {
            DATA m_data;

            Union_all( DATA &&data )
                : m_data(std::move(data))
            {}

            static constexpr auto to_string( )
            {
                return QUERY();
            }

            auto data() const
            {
                return m_data;
            }

            auto create_statement() const
            {
                static const constexpr auto text = to_string();
                auto context = FACTORY::make_context(text.c_str(), text.size());
                FACTORY::bind(context, data());
                return context;
            }

            Iterator begin() const
            {
                return Iterator(create_statement());
            }

            Iterator end() const
            {
                return Iterator();
            }
        };

        template<typename FACTORY, typename TABLE, typename WHERE>
        struct Delete_from
        {
            WHERE m_data;

            Delete_from( WHERE &&data )
                : m_data(std::move(data))
            { }

            static constexpr auto to_string( )
            {
                return "DELETE FROM "_s + typename TABLE::name_t() + " WHERE "_s + WHERE::to_string() + ";"_s;
            }

            auto run() const
            {
                const constexpr auto text = to_string();
                auto context = FACTORY::make_context(text.c_str(), text.size());
                FACTORY::bind(context, m_data.data());
                std::tuple<> tp;
                next(context, tp);
                return FACTORY::get_changes();
            }
        };
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

    template<typename FACTORY, typename First, typename ...Last>
    auto union_all( First &&first, Last &&...last )
    {
        auto data = std::tuple_cat(first.data(), last.data() ... );
        const constexpr auto query = join<decltype(" UNION ALL "_s)>(std::make_tuple(first.to_string(), last.to_string() ... ), std::make_index_sequence<1 + sizeof ... (Last)>());
        return impl::Union_all<FACTORY, decltype(query), decltype(data), typename First::iterator_t>(std::move(data));
    }

    template<typename Factory, typename Table>
    static void create_table()
    {
        impl::create_table<Factory, Table>(typename Table::columns());
    }

    template<typename T>
    constexpr auto count( )
    {
        return default_column<std::ptrdiff_t, decltype("COUNT("_s  + T::to_string() + ")"_s)>();
    }

    template<typename FACTORY, typename TABLE, typename WHERE>
    auto delete_from( WHERE &&where )
    {
        return impl::Delete_from<FACTORY, TABLE, WHERE>(std::move(where)).run();
    }
}
