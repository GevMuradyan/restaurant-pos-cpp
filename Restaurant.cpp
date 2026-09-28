#include "Restaurant.hpp"

#include <iostream>

Restaurant::Restaurant(const std::string& name)
    : restaurant_name(name)
{
}

// ============================================================
// ZONES
// ============================================================

bool Restaurant::add_zone(const Zone& zone)
{
    if (zone.get_name().empty())
    {
        return false;
    }

    if (get_zone_by_name(zone.get_name()) != nullptr)
    {
        return false;
    }

    zones.push_back(zone);

    return true;
}

bool Restaurant::remove_zone(const std::string& zone_name)
{
    if (zone_name.empty())
    {
        return false;
    }

    for (auto it = zones.begin(); it != zones.end(); ++it)
    {
        if (it->get_name() == zone_name)
        {
            // A zone can only be removed when it is empty.
            if (!it->get_tables().empty())
            {
                return false;
            }

            zones.erase(it);

            return true;
        }
    }

    return false;
}

Zone* Restaurant::get_zone_by_name(
    const std::string& zone_name
)
{
    for (Zone& zone : zones)
    {
        if (zone.get_name() == zone_name)
        {
            return &zone;
        }
    }

    return nullptr;
}

const Zone* Restaurant::get_zone_by_name(
    const std::string& zone_name
) const
{
    for (const Zone& zone : zones)
    {
        if (zone.get_name() == zone_name)
        {
            return &zone;
        }
    }

    return nullptr;
}

const std::vector<Zone>& Restaurant::get_zones() const
{
    return zones;
}

// ============================================================
// TABLES
// ============================================================

bool Restaurant::add_table_to_zone(
    const std::string& zone_name,
    const Table& table
)
{
    if (zone_name.empty())
    {
        return false;
    }

    if (table.get_table_number() <= 0)
    {
        return false;
    }

    if (table.get_chairs_count() <= 0)
    {
        return false;
    }

    // Table numbers are globally unique.
    if (get_table_by_number(
            table.get_table_number()) != nullptr)
    {
        return false;
    }

    Zone* zone = get_zone_by_name(zone_name);

    if (zone == nullptr)
    {
        return false;
    }

    return zone->add_table(table);
}

bool Restaurant::remove_table(int table_number)
{
    if (table_number <= 0)
    {
        return false;
    }

    Zone* zone =
        get_zone_by_table_number(table_number);

    if (zone == nullptr)
    {
        return false;
    }

    return zone->remove_table(table_number);
}

Zone* Restaurant::get_zone_by_table_number(
    int table_number
)
{
    for (Zone& zone : zones)
    {
        if (zone.get_table_by_number(table_number) != nullptr)
        {
            return &zone;
        }
    }

    return nullptr;
}

const Zone* Restaurant::get_zone_by_table_number(
    int table_number
) const
{
    for (const Zone& zone : zones)
    {
        if (zone.get_table_by_number(table_number) != nullptr)
        {
            return &zone;
        }
    }

    return nullptr;
}

Table* Restaurant::get_table_by_number(
    int table_number
)
{
    for (Zone& zone : zones)
    {
        Table* table =
            zone.get_table_by_number(table_number);

        if (table != nullptr)
        {
            return table;
        }
    }

    return nullptr;
}

const Table* Restaurant::get_table_by_number(
    int table_number
) const
{
    for (const Zone& zone : zones)
    {
        const Table* table =
            zone.get_table_by_number(table_number);

        if (table != nullptr)
        {
            return table;
        }
    }

    return nullptr;
}

// ============================================================
// MENU
// ============================================================

Menu& Restaurant::get_menu()
{
    return menu;
}

const Menu& Restaurant::get_menu() const
{
    return menu;
}

// ============================================================
// RESTAURANT
// ============================================================

const std::string& Restaurant::get_name() const
{
    return restaurant_name;
}

void Restaurant::display() const
{
    std::cout
        << "======== "
        << restaurant_name
        << " ========\n\n";

    for (const Zone& zone : zones)
    {
        zone.display();
        std::cout << '\n';
    }

    menu.display();
}