[![Build Status](https://travis-ci.org/evertonantunes/syslog_parser.svg?branch=master)](https://travis-ci.org/evertonantunes/syslog_parser)

    conan install . --build=missing
    cmake CMakeLists.txt
    cmake --build . --targe all

```cpp

    database::create_table<database::tables::strings>();
    database::create_table<database::tables::users>();

    database::insert_into<users_t>(   users_t::first_name = database::insert_into<strings_t>(strings_t::text = "Blaise")
                                    , users_t::second_name = database::insert_into<strings_t>(strings_t::text = "Pascal")
                                    , users_t::age = 39l );

    
    using f_name = alias<strings_t, decltype("f_name"_s)>;
    using s_name = alias<strings_t, decltype("s_name"_s)>;

    using my_select_t = decltype(database::select(   users_t::id
                                                   , f_name::as(strings_t::text)
                                                   , s_name::as(strings_t::text)
                                                   , users_t::age)
        .from<users_t>()
        .left_join<f_name>(f_name::as(strings_t::id) == users_t::first_name)
        .left_join<s_name>(s_name::as(strings_t::id) == users_t::second_name));

    const auto text = my_select_t().where(users_t::age == 39l || users_t::age > 70l).to_string(); // return StaticArray<'' ... >

    std::cout << "text: " << text.view() << std::endl; // text: SELECT users.id, f_name.text, s_name.text, users.age FROM users LEFT JOIN strings as f_name ON f_name.id == users.first_name LEFT JOIN strings as s_name ON s_name.id == users.second_name WHERE users.age == ? OR users.age > ?;

    for (const auto [id, first_name, second_name, age] : my_select_t().where(users_t::age == 39l || users_t::age > 70l))
    {
	...
    }
```
