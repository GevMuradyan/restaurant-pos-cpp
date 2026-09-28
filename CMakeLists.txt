cmake_minimum_required(VERSION 3.16)

project(restaurant_pos_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# =========================
# CONSOLE APPLICATION
# =========================

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
    RestaurantData.cpp
    Transaction.cpp
    CardTerminal.cpp
    CardTerminal.cpp
)

target_compile_options(restaurant PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# =========================
# QT GUI APPLICATION
# =========================

qt_add_executable(ctrl_eat_gui
    gui/main.cpp
    gui/MainWindow.cpp
    gui/MainWindow.hpp

    Table.cpp
    Zone.cpp
    Restaurant.cpp
    MenuItem.cpp
    MenuCategory.cpp
    Menu.cpp
    OrderItem.cpp
    Order.cpp
    Transaction.cpp
    CardTerminal.cpp
    CardTerminal.cpp

    RestaurantData.cpp
    RestaurantData.hpp
)

target_link_libraries(ctrl_eat_gui PRIVATE
    Qt6::Widgets
)

target_link_libraries(ctrl_eat_gui PRIVATE Qt6::Widgets)

qt_add_resources(ctrl_eat_gui "zone_images"
    PREFIX "/images"
    FILES
        images/main_hall.jpg
        images/terrace.jpg
        images/vip_room.jpg
        images/private_room.jpg
        images/bar_area.jpg
)