#ifndef ORDER_HPP
#define ORDER_HPP

#include <vector>
#include "OrderItem.hpp"

class Order
{
    private:
        std::vector<OrderItem>items;
        bool paid = false;

    public:


        bool add_item(const OrderItem& item);
        bool remove_item(int item_id);

        bool is_paid()const;
        bool pay_for_order();
        const std::vector<OrderItem>& get_items() const;
        bool change_item_quantity(int item_id, int new_quantity);

        int get_total()const;

        void display()const;

};           


#endif