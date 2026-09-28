#include "Restaurant.hpp"
#include "Menu.hpp"
#include "Interface.hpp"

#include <iostream>

int main()
{
    Restaurant restaurant("(*_*) Ctrl + Eat (*_*)");

    restaurant.add_zone(Zone("Main Hall"));
    restaurant.add_zone(Zone("Window Zone"));
    restaurant.add_zone(Zone("Family Zone"));
    restaurant.add_zone(Zone("VIP Hall"));
    restaurant.add_zone(Zone("Terrace"));
    restaurant.add_zone(Zone("Coupe 1"));
    restaurant.add_zone(Zone("Coupe 2"));
    restaurant.add_zone(Zone("Coupe 3"));
    restaurant.add_zone(Zone("Coupe 4"));

    restaurant.add_table_to_zone("Main Hall", Table(1, 2));
    restaurant.add_table_to_zone("Main Hall", Table(2, 2));
    restaurant.add_table_to_zone("Main Hall", Table(3, 4));
    restaurant.add_table_to_zone("Main Hall", Table(4, 4));
    restaurant.add_table_to_zone("Main Hall", Table(5, 4));
    restaurant.add_table_to_zone("Main Hall", Table(6, 4));
    restaurant.add_table_to_zone("Main Hall", Table(7, 6));
    restaurant.add_table_to_zone("Main Hall", Table(8, 6));

    restaurant.add_table_to_zone("Window Zone", Table(11, 2));
    restaurant.add_table_to_zone("Window Zone", Table(12, 2));
    restaurant.add_table_to_zone("Window Zone", Table(13, 4));
    restaurant.add_table_to_zone("Window Zone", Table(14, 4));

    restaurant.add_table_to_zone("Family Zone", Table(15, 6));
    restaurant.add_table_to_zone("Family Zone", Table(16, 6));
    restaurant.add_table_to_zone("Family Zone", Table(17, 8));
    restaurant.add_table_to_zone("Family Zone", Table(18, 8));

    restaurant.add_table_to_zone("VIP Hall", Table(21, 4));
    restaurant.add_table_to_zone("VIP Hall", Table(22, 6));
    restaurant.add_table_to_zone("VIP Hall", Table(23, 8));
    restaurant.add_table_to_zone("VIP Hall", Table(24, 10));

    restaurant.add_table_to_zone("Terrace", Table(31, 2));
    restaurant.add_table_to_zone("Terrace", Table(32, 2));
    restaurant.add_table_to_zone("Terrace", Table(33, 4));
    restaurant.add_table_to_zone("Terrace", Table(34, 4));
    restaurant.add_table_to_zone("Terrace", Table(35, 6));
    restaurant.add_table_to_zone("Terrace", Table(36, 6));

    restaurant.add_table_to_zone("Coupe 1", Table(41, 4));
    restaurant.add_table_to_zone("Coupe 2", Table(42, 6));
    restaurant.add_table_to_zone("Coupe 3", Table(43, 6));
    restaurant.add_table_to_zone("Coupe 4", Table(44, 8));

    Menu menu;

    if (!menu.load_from_file("menu.txt"))
    {
        std::cout << "Cannot load menu.txt\n";
        return 1;
    }

    run_interface(restaurant);

    return 0;
}