#include <benchmark/benchmark.h>
#include "database.hpp"

using descriptions_t = database::tables::Descriptions;
using users_t = database::tables::users;

using namespace sql;

using f_name = alias<descriptions_t, decltype("f_name"_s)>;
using s_name = alias<descriptions_t, decltype("s_name"_s)>;

class Fixture
{
public:
    Fixture()
    {
        database::factory_t::instance()->execute("PRAGMA foreign_keys = ON;");

        database::create_table<descriptions_t>();
        database::create_table<users_t>();

        database::insert_into<users_t>(   users_t::first_name = database::insert_into<descriptions_t>(descriptions_t::text = "Blaise")
                                        , users_t::second_name = database::insert_into<descriptions_t>(descriptions_t::text = "Pascal")
                                        , users_t::age = 39l );

        database::insert_into<users_t>(   users_t::first_name = database::insert_into<descriptions_t>(descriptions_t::text = "Leonhard")
                                        , users_t::second_name = database::insert_into<descriptions_t>(descriptions_t::text = "Euler")
                                        , users_t::age = 76l );

        database::insert_into<users_t>(   users_t::first_name = database::insert_into<descriptions_t>(descriptions_t::text = "Robert")
                                        , users_t::second_name = database::insert_into<descriptions_t>(descriptions_t::text = "Hooke")
                                        , users_t::age = 67l );
    }

    ~Fixture()
    {
        database::factory_t::instance()->reset();
    }
};

using my_select_t = decltype(database::select(   users_t::id
                                               , f_name::as(descriptions_t::text)
                                               , s_name::as(descriptions_t::text)
                                               , users_t::age)
    .from<users_t>()
    .left_join<f_name>(f_name::as(descriptions_t::id) == users_t::first_name)
    .left_join<s_name>(s_name::as(descriptions_t::id) == users_t::second_name));

static void BM_raw_select(benchmark::State& state)
{
    Fixture f;
    for (auto _ : state)
    {
        std::string empty_string;
    }
}
BENCHMARK(BM_raw_select);

static void BM_static_select(benchmark::State& state)
{
    Fixture f;
    std::string x = "hello";
    for (auto _ : state)
    {
        for ( const auto [id, first_name, second_name, age] : my_select_t().where(users_t::age == 39l || users_t::age > 70l))
        {
            benchmark::DoNotOptimize(age);
        }
    }
}
BENCHMARK(BM_static_select);

BENCHMARK_MAIN();

