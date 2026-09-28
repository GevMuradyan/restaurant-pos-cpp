#include "RestaurantData.hpp"

void initialize_restaurant(Restaurant& restaurant)
{
    // =========================================================
    // ZONES
    // =========================================================

    restaurant.add_zone(Zone("Main Hall"));
    restaurant.add_zone(Zone("Terrace"));
    restaurant.add_zone(Zone("VIP Room"));
    restaurant.add_zone(Zone("Private Room"));
    restaurant.add_zone(Zone("Bar Area"));

    // =========================================================
    // MAIN HALL
    // Tables 1 - 12
    // =========================================================

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(1, 4)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(2, 4)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(3, 4)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(4, 4)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(5, 6)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(6, 6)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(7, 4)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(8, 4)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(9, 6)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(10, 6)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(11, 8)
    );

    restaurant.add_table_to_zone(
        "Main Hall",
        Table(12, 8)
    );

    // =========================================================
    // TERRACE
    // Tables 13 - 20
    // =========================================================

    restaurant.add_table_to_zone(
        "Terrace",
        Table(13, 4)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(14, 4)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(15, 4)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(16, 6)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(17, 6)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(18, 6)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(19, 8)
    );

    restaurant.add_table_to_zone(
        "Terrace",
        Table(20, 8)
    );

    // =========================================================
    // VIP ROOM
    // Tables 21 - 23
    // =========================================================

    restaurant.add_table_to_zone(
        "VIP Room",
        Table(21, 6)
    );

    restaurant.add_table_to_zone(
        "VIP Room",
        Table(22, 8)
    );

    restaurant.add_table_to_zone(
        "VIP Room",
        Table(23, 8)
    );

    // =========================================================
    // PRIVATE ROOM
    // Tables 24 - 25
    // =========================================================

    restaurant.add_table_to_zone(
        "Private Room",
        Table(24, 6)
    );

    restaurant.add_table_to_zone(
        "Private Room",
        Table(25, 8)
    );

    // =========================================================
    // BAR AREA
    // Tables 26 - 31
    // =========================================================

    restaurant.add_table_to_zone(
        "Bar Area",
        Table(26, 2)
    );

    restaurant.add_table_to_zone(
        "Bar Area",
        Table(27, 2)
    );

    restaurant.add_table_to_zone(
        "Bar Area",
        Table(28, 2)
    );

    restaurant.add_table_to_zone(
        "Bar Area",
        Table(29, 4)
    );

    restaurant.add_table_to_zone(
        "Bar Area",
        Table(30, 4)
    );

    restaurant.add_table_to_zone(
        "Bar Area",
        Table(31, 4)
    );

    // =========================================================
    // MENU
    // =========================================================

    Menu& menu = restaurant.get_menu();

    // Categories

    menu.add_category(
        MenuCategory("Appetizers")
    );

    menu.add_category(
        MenuCategory("Salads")
    );

    menu.add_category(
        MenuCategory("Main Courses")
    );

    menu.add_category(
        MenuCategory("Drinks")
    );

    menu.add_category(
        MenuCategory("Desserts")
    );

    // ---------------------------------------------------------
    // APPETIZERS
    // ---------------------------------------------------------

    menu.add_item_to_category(
        "Appetizers",
        MenuItem(1, "Bruschetta", 1800)
    );

    menu.add_item_to_category(
        "Appetizers",
        MenuItem(2, "Cheese Plate", 3200)
    );

    menu.add_item_to_category(
        "Appetizers",
        MenuItem(3, "Chicken Wings", 2800)
    );

    // ---------------------------------------------------------
    // SALADS
    // ---------------------------------------------------------

    menu.add_item_to_category(
        "Salads",
        MenuItem(4, "Caesar Salad", 2800)
    );

    menu.add_item_to_category(
        "Salads",
        MenuItem(5, "Greek Salad", 2400)
    );

    menu.add_item_to_category(
        "Salads",
        MenuItem(6, "Garden Salad", 1900)
    );

    // ---------------------------------------------------------
    // MAIN COURSES
    // ---------------------------------------------------------

    menu.add_item_to_category(
        "Main Courses",
        MenuItem(7, "Beef Steak", 6500)
    );

    menu.add_item_to_category(
        "Main Courses",
        MenuItem(8, "Chicken Steak", 4800)
    );

    menu.add_item_to_category(
        "Main Courses",
        MenuItem(9, "Pasta Carbonara", 3900)
    );

    menu.add_item_to_category(
        "Main Courses",
        MenuItem(10, "Burger", 3500)
    );

    // ---------------------------------------------------------
    // DRINKS
    // ---------------------------------------------------------

    menu.add_item_to_category(
        "Drinks",
        MenuItem(11, "Coca-Cola", 700)
    );

    menu.add_item_to_category(
        "Drinks",
        MenuItem(12, "Fanta", 700)
    );

    menu.add_item_to_category(
        "Drinks",
        MenuItem(13, "Sprite", 700)
    );

    menu.add_item_to_category(
        "Drinks",
        MenuItem(14, "Mineral Water", 500)
    );

    menu.add_item_to_category(
        "Drinks",
        MenuItem(15, "Coffee", 1200)
    );

    // ---------------------------------------------------------
    // DESSERTS
    // ---------------------------------------------------------

    menu.add_item_to_category(
        "Desserts",
        MenuItem(16, "Cheesecake", 2200)
    );

    menu.add_item_to_category(
        "Desserts",
        MenuItem(17, "Chocolate Cake", 2400)
    );

    menu.add_item_to_category(
        "Desserts",
        MenuItem(18, "Ice Cream", 1600)
    );
}