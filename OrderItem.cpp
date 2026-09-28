#include "OrderItem.hpp"

#include <iostream>
#include <limits>

OrderItem::OrderItem(
    int item_id,
    const std::string& name,
    int unit_price,
    int quantity
)
    : item_id(item_id),
      name(name),
      unit_price(unit_price),
      quantity(quantity)
{
}

OrderItem::OrderItem(
    const MenuItem& item,
    int quantity
)
    : item_id(item.get_id()),
      name(item.get_name()),
      unit_price(item.get_price()),
      quantity(quantity)
{
}


int OrderItem::get_id() const
{
    return item_id;
}

const std::string& OrderItem::get_name() const
{
    return name;
}

int OrderItem::get_unit_price() const
{
    return unit_price;
}

int OrderItem::get_quantity() const
{
    return quantity;
}

int OrderItem::get_subtotal() const
{
    return unit_price * quantity;
}

bool OrderItem::set_quantity(int new_quantity)
{
    if (new_quantity <= 0)
    {
        return false;
    }

    quantity = new_quantity;

    return true;
}

bool OrderItem::increase_quantity(int amount)
{
    if (amount <= 0)
    {
        return false;
    }

    if (quantity > std::numeric_limits<int>::max() - amount)
    {
        return false;
    }

    quantity += amount;

    return true;
}

void OrderItem::display() const
{
    std::cout
        << name
        << " x "
        << quantity
        << " - "
        << get_subtotal()
        << " AMD\n";
}