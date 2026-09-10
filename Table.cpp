#include "Table.hpp"
#include <iostream>

void display_status(TableStatus status)
{
    switch (status)
    {
        case TableStatus::Free:
            std::cout << "Free\n";
            break;

        case TableStatus::Occupied:
            std::cout << "Occupied\n";
            break;

        case TableStatus::CallingWaiter:
            std::cout << "Calling Waiter\n";
            break;

        case TableStatus::BillRequested:
            std::cout << "Bill Requested\n";
            break;

        default:
            std::cout << "Unknown status\n";
    }
}

Table::Table(int my_table_number, int my_chairs_count)
    : table_number(my_table_number),
      chairs_count(my_chairs_count),
      clients_count(0),
      status(TableStatus::Free)
{
}

void Table::open_table(int my_clients_count)
{
    if (status != TableStatus::Free)
    {
        std::cout << "Table is not free.\n";
        return;
    }

    if (my_clients_count <= 0)
    {
        std::cout << "Invalid number of guests.\n";
        return;
    }

    if (my_clients_count > chairs_count)
    {
        std::cout << "Not enough seats at this table.\n";
        return;
    }

    order.emplace();
    clients_count = my_clients_count;
    status = TableStatus::Occupied;
}

void Table::close_table()
{
    if (status == TableStatus::Free)
    {
        std::cout << "Table is already free.\n";
        return;
    }

    if (!order.has_value())
    {
        std::cout << "Table has no active order.\n";
        return;
    }

    if(!order->is_paid())
    {
        std::cout<<"The table's bill has not been settled\n";
        return ;
    }

    
        status = TableStatus::Free;
        clients_count = 0;
        order.reset();
    
}

Order* Table::get_order()
{
    if(order.has_value())
    {
    return &order.value();
    }

    return nullptr;
}

const Order* Table::get_order()const
{
    if(order.has_value())
    {
    return &order.value();
    }

    return nullptr;
}

int Table::get_table_number() const
{
    return table_number;
}

int Table::get_chairs_count() const
{
    return chairs_count;
}

int Table::get_clients_count() const
{
    return clients_count;
}

TableStatus Table::get_status() const
{
    return status;
}

void Table::display() const
{
    std::cout << "Table #" << table_number << "\n";
    std::cout << "Seats: " << chairs_count << "\n";
    std::cout << "Guests: " << clients_count << "\n";
    std::cout << "Status: ";
    display_status(status);
}