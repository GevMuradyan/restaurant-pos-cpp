#ifndef RESTAURANT_HPP
#define RESTAURANT_HPP

#include <string>
#include <vector>

#include "Menu.hpp"
#include "Zone.hpp"

class Restaurant
{
private:
    std::string restaurant_name;
    std::vector<Zone> zones;
    Menu menu;

public:
    explicit Restaurant(const std::string& name);

    // ===== ZONES =====

    bool add_zone(const Zone& zone);
    bool remove_zone(const std::string& zone_name);

    Zone* get_zone_by_name(const std::string& zone_name);
    const Zone* get_zone_by_name(
        const std::string& zone_name
    ) const;

    const std::vector<Zone>& get_zones() const;

    // ===== TABLES =====

    bool add_table_to_zone(
        const std::string& zone_name,
        const Table& table
    );

    bool remove_table(int table_number);

    Zone* get_zone_by_table_number(int table_number);
    const Zone* get_zone_by_table_number(
        int table_number
    ) const;

    Table* get_table_by_number(int table_number);
    const Table* get_table_by_number(
        int table_number
    ) const;

    // ===== MENU =====

    Menu& get_menu();
    const Menu& get_menu() const;

    // ===== RESTAURANT =====

    const std::string& get_name() const;

    void display() const;
};

#endif