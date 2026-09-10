#include "MenuItem.hpp"

#include <string>
#include <iostream>

MenuItem::MenuItem(int m_id, const std::string& m_name, int m_price):id(m_id), name(m_name), price(m_price)
{

}

int MenuItem::get_id()const
{
    return id;
}

const std::string& MenuItem::get_name()const
{
    return name;
}

int MenuItem::get_price()const
{
    return price;
}

void MenuItem::display() const
{
    std::cout << id << ". " << name << "    " << price << " AMD\n";
}
