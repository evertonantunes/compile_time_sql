#ifndef TEST__EXCEPTIONALIZE_STATIC_ASSERT_H
#define TEST__EXCEPTIONALIZE_STATIC_ASSERT_H

/* Conditionally compilable apparatus for replacing `static_assert`
    with a runtime exception of type `exceptionalized_static_assert`
    within (portions of) a test suite.
*/
#if TEST__EXCEPTIONALIZE_STATIC_ASSERT == 1

#include <string>
#include <stdexcept>

namespace test {

struct exceptionalized_static_assert : std::logic_error
{
    exceptionalized_static_assert(char const *what)
    : std::logic_error(what){};
    virtual ~exceptionalized_static_assert() noexcept {}
};

template<bool Cond>
struct exceptionalize_static_assert;

template<>
struct exceptionalize_static_assert<true>
{
    explicit exceptionalize_static_assert(char const * reason) {
        (void)reason;
    }
};


template<>
struct exceptionalize_static_assert<false>
{
    explicit exceptionalize_static_assert(char const * reason) {
        std::string s("static_assert would fail with reason: ");
        s += reason;
        throw exceptionalized_static_assert(s.c_str());
    }
};

} // namespace test

// A macro redefinition of `static_assert`
#define static_assert(cond,gripe) \
    struct _1_test \
    : test::exceptionalize_static_assert<(cond)> \
    {   _1_test() : \
        test::exceptionalize_static_assert<(cond)>(gripe){}; \
    }; \
    _1_test _2_test

#endif // TEST__EXCEPTIONALIZE_STATIC_ASSERT == 1

#endif // EOF
