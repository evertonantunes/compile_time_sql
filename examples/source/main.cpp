
#include "database.hpp"
#include <iostream>

int main()
{
    database::create_table<descriptions_t>();
    database::create_table<users_t>();

    database::factory_t::instance()->execute("PRAGMA foreign_keys = ON;");

    database::insert_into<users_t>(   users_t::first_name = database::insert_into<descriptions_t>(descriptions_t::text = "Blaise")
                                    , users_t::second_name = database::insert_into<descriptions_t>(descriptions_t::text = "Pascal")
                                    , users_t::age = 39l );


    database::insert_into<users_t>(   users_t::first_name = database::insert_into<descriptions_t>(descriptions_t::text = "Leonhard")
                                    , users_t::second_name = database::insert_into<descriptions_t>(descriptions_t::text = "Euler")
                                    , users_t::age = 76l );

    database::insert_into<users_t>(   users_t::first_name = database::insert_into<descriptions_t>(descriptions_t::text = "Robert")
                                    , users_t::second_name = database::insert_into<descriptions_t>(descriptions_t::text = "Hooke")
                                    , users_t::age = 67l );


    using my_select_t = decltype(database::select(   users_t::id
                                                   , f_name::as(descriptions_t::text)
                                                   , s_name::as(descriptions_t::text)
                                                   , users_t::age)
        .from<users_t>()
        .left_join<f_name>(f_name::as(descriptions_t::id) == users_t::first_name)
        .left_join<s_name>(s_name::as(descriptions_t::id) == users_t::second_name));


    //std::cout << "sql: " << my_select_t().where(users_t::age == 39l || users_t::age > 70l).to_string().view() << std::endl;

    for ( int x = 0; x < 1; x++ )
    {
        for ( const auto [id, first_name, second_name, age] : my_select_t().where(users_t::age == 39l || users_t::age > 70l))
        {
            std::cout << "id: " << id << " first_name: " << first_name << " second_name: " << second_name << " age: " << age << std::endl;
        }
    }

    database::delete_from<users_t>( users_t::id == 1l || users_t::age == 76l );

    for ( const auto [id, first_name, second_name, age] : database::select(  users_t::id
                                                                           , f_name::as(descriptions_t::text)
                                                                           , s_name::as(descriptions_t::text)
                                                                           , users_t::age)
          .from<users_t>()
          .left_join<f_name>(f_name::as(descriptions_t::id) == users_t::first_name)
          .left_join<s_name>(s_name::as(descriptions_t::id) == users_t::second_name) )
    {
        std::cout << "id: " << id << " first_name: " << first_name << " second_name: " << second_name << " age: " << age << std::endl;
    }

    return 0;
}
