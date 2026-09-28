#ifndef TABLE_HPP
#define TABLE_HPP

#include <optional>

#include "Order.hpp"

enum class TableStatus
{
    Free,
    Occupied,
    CallingWaiter,
    BillRequested
};

class Table
{
private:
    int table_number;
    int chairs_count;
    int clients_count;
    TableStatus status;
    std::optional<Order> order;

public:
    Table(int table_number, int chairs_count);

    bool open_table(int clients_count);
    bool close_table();

    bool call_waiter();
    bool request_bill();

    Order* get_order();
    const Order* get_order() const;

    int get_table_number() const;
    int get_chairs_count() const;
    int get_clients_count() const;
    TableStatus get_status() const;
};

#endif
