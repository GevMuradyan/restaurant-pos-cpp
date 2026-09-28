#include "Zone.hpp"

#include <iostream>

Zone::Zone(const std::string& name)
    : zone_name(name)
{
}

bool Zone::add_table(const Table& table)
{
    if (table.get_table_number() <= 0)
    {
        return false;
    }

    if (table.get_chairs_count() <= 0)
    {
        return false;
    }

    const int table_number = table.get_table_number();

    for (const Table& existing_table : tables)
    {
        if (existing_table.get_table_number() == table_number)
        {
            return false;
        }
    }

    tables.push_back(table);

    return true;
}

bool Zone::remove_table(int table_number)
{
    for (auto it = tables.begin(); it != tables.end(); ++it)
    {
        if (it->get_table_number() == table_number)
        {
            if (it->get_status() != TableStatus::Free)
            {
                return false;
            }

            tables.erase(it);

            return true;
        }
    }

    return false;
}

const std::vector<Table>& Zone::get_tables() const
{
    return tables;
}

Table* Zone::get_table_by_number(int table_number)
{
    for (Table& table : tables)
    {
        if (table.get_table_number() == table_number)
        {
            return &table;
        }
    }

    return nullptr;
}

const Table* Zone::get_table_by_number(int table_number) const
{
    for (const Table& table : tables)
    {
        if (table.get_table_number() == table_number)
        {
            return &table;
        }
    }

    return nullptr;
}

const std::string& Zone::get_name() const
{
    return zone_name;
}

void Zone::display() const
{
    std::cout
        << "======== "
        << zone_name
        << " ========\n\n";

    for (const Table& table : tables)
    {
        std::cout
            << "Table #"
            << table.get_table_number()
            << '\n';

        std::cout
            << "Seats: "
            << table.get_chairs_count()
            << '\n';

        std::cout
            << "Guests: "
            << table.get_clients_count()
            << '\n';

        std::cout << "Status: ";

        switch (table.get_status())
        {
            case TableStatus::Free:
                std::cout << "Free";
                break;

            case TableStatus::Occupied:
                std::cout << "Occupied";
                break;

            case TableStatus::CallingWaiter:
                std::cout << "Calling Waiter";
                break;

            case TableStatus::BillRequested:
                std::cout << "Bill Requested";
                break;
        }

        std::cout << "\n\n";
    }
}