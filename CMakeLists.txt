cmake_minimum_required(VERSION 3.16)

project(restaurant_pos_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(restaurant
    main.cpp
    Table.cpp
    Zone.cpp
    Restaurant.cpp
    MenuItem.cpp
    MenuCategory.cpp
    Menu.cpp
    OrderItem.cpp
    Order.cpp
    Interface.cpp
)

target_compile_options(restaurant PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
