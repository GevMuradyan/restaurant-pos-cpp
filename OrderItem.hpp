#ifndef ORDERITEM_HPP
#define ORDERITEM_HPP

#include "MenuItem.hpp"

#include <string>

class OrderItem
{
private:
    int item_id;
    std::string name;
    int unit_price;
    int quantity;

public:
    OrderItem(
        int item_id,
        const std::string& name,
        int unit_price,
        int quantity
    );

    OrderItem(
        const MenuItem& item,
        int quantity
    );

    int get_id() const;
    const std::string& get_name() const;

    int get_unit_price() const;
    int get_quantity() const;
    int get_subtotal() const;

    bool increase_quantity(int amount);
    bool set_quantity(int new_quantity);

    void display() const;
};

#endif