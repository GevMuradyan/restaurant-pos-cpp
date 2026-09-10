#include "OrderItem.hpp"
#include <iostream>


OrderItem::OrderItem(int m_id, const std::string& m_name, int m_unit_price, int m_quantity):item_id(m_id),name(m_name), unit_price(m_unit_price), quantity(m_quantity) 
{

}

OrderItem::OrderItem(const MenuItem& item,int m_quantity):item_id(item.get_id()),name(item.get_name()),unit_price(item.get_price()), quantity(m_quantity)  
{

}

int OrderItem::get_id()const
{
    return item_id;
}
const std::string& OrderItem:: get_name()const
{
    return name;
}
int OrderItem:: get_unit_price()const
{
    return unit_price;
}
int OrderItem:: get_quantity()const
{
    return quantity;
}
int OrderItem::get_subtotal()const
{
    return unit_price*quantity;
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
    if(amount <=0 ){
        return false;
    }
    quantity += amount;
    return true;
}

void OrderItem::display() const
{
    std::cout << name<< " x " << quantity<< " - " << get_subtotal()<< " AMD\n";
}