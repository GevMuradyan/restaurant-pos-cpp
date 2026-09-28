#include "MenuItem.hpp"

#include <iostream>

MenuItem::MenuItem(
    int id,
    const std::string& name,
    int price
)
    : id(id),
      name(name),
      price(price)
{
}

int MenuItem::get_id() const
{
    return id;
}

const std::string& MenuItem::get_name() const
{
    return name;
}

int MenuItem::get_price() const
{
    return price;
}

bool MenuItem::set_name(const std::string& name)
{
    if (name.empty())
    {
        return false;
    }

    this->name = name;
    return true;
}

bool MenuItem::set_price(int price)
{
    if (price < 0)
    {
        return false;
    }

    this->price = price;
    return true;
}

void MenuItem::display() const
{
    std::cout
        << id
        << ". "
        << name
        << "    "
        << price
        << " AMD\n";
}