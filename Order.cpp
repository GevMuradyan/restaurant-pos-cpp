#include "Order.hpp"
#include <iostream>
#include <iomanip>


bool Order::add_item(const OrderItem& item)
{
    for(OrderItem& m_item: items)
    {
        if(item.get_id() == m_item.get_id() )
        {
             m_item.increase_quantity(item.get_quantity());
            return true;
            
        }
    }
            items.push_back(item);
            return true;
        
}

bool Order:: is_paid()const
{
    return paid;
}

bool Order:: pay_for_order()
{
    if(paid == false)
    {
        paid = true;
        return true;
    }

    return false;
}

bool Order::remove_item(int item_id)
{
    for(auto it = items.begin(); it != items.end(); ++it)
    {
    
        if(it->get_id() == item_id){
            items.erase(it);
            return true;
        }
    }
    return false;
}

int Order::get_total()const
{
    int total = 0;
    for(const OrderItem& item:items)
    {
    total += item.get_subtotal();
    }
    return total;    
}
const std::vector<OrderItem>& Order::get_items() const
{
    return items;
}

bool Order::change_item_quantity(int item_id, int new_quantity)
{
    for (OrderItem& item : items)
    {
        if (item.get_id() == item_id)
        {
            return item.set_quantity(new_quantity);
        }
    }

    return false;
}

void Order::display()const
{
    for(const OrderItem& item : items)
    {
        item.display();
    }


}

