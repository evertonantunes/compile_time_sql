#include <benchmark/benchmark.h>
#include "database.hpp"
#include <assert.h>
#include <iostream>

using descriptions_t = database::tables::Descriptions;
using users_t = database::tables::users;

using namespace sql;

using f_name = alias<descriptions_t, decltype("f_name"_s)>;
using s_name = alias<descriptions_t, decltype("s_name"_s)>;

class Fixture : public benchmark::Fixture
{
public:
    void SetUp(const ::benchmark::State& state)
    {
        database::factory_t::instance()->execute("PRAGMA foreign_keys = ON;");

        database::create_table<descriptions_t>();
        database::create_table<users_t>();

        for (int x = 0; x < 1000; x++)
        {
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
    }

    void TearDown(const ::benchmark::State& state)
    {
        database::factory_t::instance()->reset();
    }
};

BENCHMARK_F(Fixture, BM_raw_select)(benchmark::State& state)
{
    const std::string_view sql = "SELECT users.id, f_name.text, s_name.text, users.age FROM users LEFT JOIN descriptions AS f_name ON f_name.id == users.first_name LEFT JOIN descriptions AS s_name ON s_name.id == users.second_name WHERE users.age == ? OR users.age > ?";
    auto connection = database::factory_t::instance()->m_database_connection;
    for (auto _ : state)
    {
        std::size_t result = 0;
        int ec = SQLITE_OK;
        sqlite3_stmt *stmt = nullptr;
        ec = sqlite3_prepare_v2(connection, sql.data(), sql.size(), &stmt, NULL);
        assert(ec == SQLITE_OK);
        ec = sqlite3_bind_int64(stmt, 1, 39l);
        assert(ec == SQLITE_OK);
        ec = sqlite3_bind_int64(stmt, 2, 70l);
        assert(ec == SQLITE_OK);

        std::ptrdiff_t id = 0;
        std::string_view first_name;
        std::string_view last_name;
        std::ptrdiff_t age = 0;

        ec = SQLITE_ROW;
        while (ec == SQLITE_ROW)
        {
            ec = sqlite3_step(stmt);

            if  (ec == SQLITE_ROW)
            {
                id = sqlite3_column_int64(stmt, 0);
                assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));
                {
                    const unsigned char *data = sqlite3_column_text(stmt, 1);
                    assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));
                    first_name = reinterpret_cast<const char*>(data);
                }
                {
                    const unsigned char *data = sqlite3_column_text(stmt, 2);
                    assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));
                    last_name = reinterpret_cast<const char*>(data);
                }
                age = sqlite3_column_int64(stmt, 3);
                assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));

                result += age;

                benchmark::DoNotOptimize(id);
                benchmark::DoNotOptimize(first_name);
                benchmark::DoNotOptimize(last_name);
                benchmark::DoNotOptimize(age);
            }
            else
            {
                break;
            }
        }        
        assert(result == 115000);
        sqlite3_finalize(stmt);
    }
}


BENCHMARK_F(Fixture, BM_raw_select_reuse_stmt)(benchmark::State& state)
{
    const std::string_view sql = "SELECT users.id, f_name.text, s_name.text, users.age FROM users LEFT JOIN descriptions AS f_name ON f_name.id == users.first_name LEFT JOIN descriptions AS s_name ON s_name.id == users.second_name WHERE users.age == ? OR users.age > ?;";
    auto connection = database::factory_t::instance()->m_database_connection;

    int ec = SQLITE_OK;
    sqlite3_stmt *stmt = nullptr;
    ec = sqlite3_prepare_v2(connection, sql.data(), sql.size(), &stmt, NULL);
    assert(ec == SQLITE_OK);

    for (auto _ : state)
    {
        std::size_t result = 0;
        ec = sqlite3_bind_int64(stmt, 1, 39l);
        assert(ec == SQLITE_OK);
        ec = sqlite3_bind_int64(stmt, 2, 70l);
        assert(ec == SQLITE_OK);

        std::ptrdiff_t id = 0;
        std::string_view first_name;
        std::string_view last_name;
        std::ptrdiff_t age = 0;

        ec = SQLITE_ROW;
        while (ec == SQLITE_ROW)
        {
            ec = sqlite3_step(stmt);

            if  (ec == SQLITE_ROW)
            {
                id = sqlite3_column_int64(stmt, 0);
                assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));
                {
                    const unsigned char *data = sqlite3_column_text(stmt, 1);
                    assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));
                    first_name = reinterpret_cast<const char*>(data);
                }
                {
                    const unsigned char *data = sqlite3_column_text(stmt, 2);
                    assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));
                    last_name = reinterpret_cast<const char*>(data);
                }
                age = sqlite3_column_int64(stmt, 3);
                assert((ec == SQLITE_OK) || (ec == SQLITE_ROW));

                result += age;

                benchmark::DoNotOptimize(id);
                benchmark::DoNotOptimize(first_name);
                benchmark::DoNotOptimize(last_name);
                benchmark::DoNotOptimize(age);
            }
            else
            {
                break;
            }
        }
        assert(result == 115000);

        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
}


using my_select_t = decltype(database::select(    users_t::id
                                                , f_name::as(descriptions_t::text)
                                                , s_name::as(descriptions_t::text)
                                                , users_t::age)
                                            .from<users_t>()
                                            .left_join<f_name>(f_name::as(descriptions_t::id) == users_t::first_name)
                                            .left_join<s_name>(s_name::as(descriptions_t::id) == users_t::second_name));

BENCHMARK_F(Fixture, BM_static_select)(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::size_t result = 0;
        for ( const auto [id, first_name, second_name, age] : my_select_t().where(users_t::age == 39l || users_t::age > 70l))
        {
            result += age;
            benchmark::DoNotOptimize(id);
            benchmark::DoNotOptimize(first_name);
            benchmark::DoNotOptimize(second_name);
            benchmark::DoNotOptimize(age);
        }
        assert(result == 115000);
    }
}

BENCHMARK_MAIN();

