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

void display_status(TableStatus status);

class Table
{
private:
    int table_number;
    int chairs_count;
    int clients_count;
    TableStatus status;
    std::optional<Order>order;

public:
    Table(int my_table_number, int my_chairs_count);

    void open_table(int my_clients_count);
    void close_table();

    Order* get_order();
    const Order* get_order()const;
    int get_table_number() const;
    int get_chairs_count() const;
    int get_clients_count() const;
    TableStatus get_status() const;

    void display() const;
};

#endif