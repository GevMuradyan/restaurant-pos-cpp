#include "Table.hpp"

Table::Table(int table_number, int chairs_count)
    : table_number(table_number),
      chairs_count(chairs_count),
      clients_count(0),
      status(TableStatus::Free)
{
}

bool Table::open_table(int clients_count)
{
    if (status != TableStatus::Free)
    {
        return false;
    }

    if (clients_count <= 0)
    {
        return false;
    }

    if (clients_count > chairs_count)
    {
        return false;
    }

    order.emplace();

    this->clients_count = clients_count;
    status = TableStatus::Occupied;

    return true;
}

bool Table::call_waiter()
{
    if (status != TableStatus::Occupied)
    {
        return false;
    }

    status = TableStatus::CallingWaiter;

    return true;
}

bool Table::request_bill()
{
    if (status != TableStatus::Occupied &&
        status != TableStatus::CallingWaiter)
    {
        return false;
    }

    if (!order.has_value())
    {
        return false;
    }

    if (order->get_items().empty())
    {
        return false;
    }

    status = TableStatus::BillRequested;

    return true;
}

bool Table::close_table()
{
    if (status == TableStatus::Free)
    {
        return false;
    }

    if (!order.has_value())
    {
        return false;
    }

    if (!order->is_paid())
    {
        return false;
    }

    status = TableStatus::Free;
    clients_count = 0;
    order.reset();

    return true;
}

Order* Table::get_order()
{
    if (!order.has_value())
    {
        return nullptr;
    }

    return &order.value();
}

const Order* Table::get_order() const
{
    if (!order.has_value())
    {
        return nullptr;
    }

    return &order.value();
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