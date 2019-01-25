#include <tuple>
#include <vector>
#include <algorithm>
#include <type_traits>

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

    namespace operators
    {
        namespace logical
        {
            struct And
            {
                static const constexpr string_t m_text = string_t(" AND ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

            struct Or
            {
                static constexpr const string_t m_text = string_t(" OR ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

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
            struct Equals
            {
                static constexpr string_t m_text = string_t(" == ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

            struct NotEquals
            {
                static constexpr string_t m_text = string_t(" != ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

            struct LessThan
            {
                static constexpr string_t m_text = string_t(" < ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

            struct LessThanEqual
            {
                static constexpr string_t m_text = string_t(" <= ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

            struct GreaterThan
            {
                static constexpr string_t m_text = string_t(" > ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

            struct GreaterThanEqual
            {
                static constexpr string_t m_text = string_t(" >= ");

                template<typename IT>
                static constexpr void write( IT &iterator )
                {
                    copy(m_text, iterator);
                }

                static constexpr std::size_t size()
                {
                    return m_text.size();
                }
            };

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
                    return T::name().size() + OP::size() + m_text.size();
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

    template<typename T, typename H, unsigned short INDEX>
    struct column
    {
        using type_t = T;
        using table_t = H;
        using this_t = column<type_t, table_t, INDEX>;
        static constexpr unsigned short index = INDEX;

        static constexpr auto name()
        {
            return table_t::columns_name()[index];
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
                        + ((T::name().size()) + ... + 0) // columns names sizes
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
    }

    template<typename F, typename ...T>
    auto select( T ... )
    {
        return impl::Select<F, T...>();
    }
}
