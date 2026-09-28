#include "Order.hpp"
#include "Transaction.hpp"

#include <iostream>
#include <limits>

bool Order::add_item(const OrderItem& item)
{
    if (paid)
    {
        return false;
    }

    if (item.get_quantity() <= 0)
    {
        return false;
    }

    for (OrderItem& existing_item : items)
    {
        if (existing_item.get_id() == item.get_id())
        {
            return existing_item.increase_quantity(
                item.get_quantity()
            );
        }
    }

    items.push_back(item);
    return true;
}

bool Order::remove_item(int item_id)
{
    if (paid)
    {
        return false;
    }

    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it->get_id() == item_id)
        {
            items.erase(it);
            return true;
        }
    }

    return false;
}

bool Order::change_item_quantity(
    int item_id,
    int new_quantity
)
{
    if (paid)
    {
        return false;
    }

    for (OrderItem& item : items)
    {
        if (item.get_id() == item_id)
        {
            return item.set_quantity(new_quantity);
        }
    }

    return false;
}

bool Order::is_paid() const
{
    return paid;
}

bool Order::start_payment(
    PaymentMethod method
)
{
    if (paid)
        return false;

    if (items.empty())
        return false;

    if (method == PaymentMethod::None)
        return false;

    if (payment_status != PaymentStatus::None)
        return false;

    payment_method = method;
    payment_status = PaymentStatus::Processing;

    return true;
}

bool Order::approve_payment()
{
    if (paid)
        return false;

    if (payment_status != PaymentStatus::Processing)
        return false;

    if (payment_method == PaymentMethod::Card)
    {
        transaction_id =
            Transaction::generate_id();
    }

    payment_status =
        PaymentStatus::Approved;

    paid = true;

    return true;
}

bool Order::decline_payment()
{
    if (paid)
        return false;

    if (payment_status != PaymentStatus::Processing)
        return false;

    payment_status =
        PaymentStatus::Declined;

    return true;
}

bool Order::cancel_payment()
{
    if (paid)
        return false;

    if (payment_status != PaymentStatus::Processing)
        return false;

    payment_method = PaymentMethod::None;
    payment_status = PaymentStatus::Cancelled;

    return true;
}

bool Order::start_cash_payment(
    int received_amount)
{
    if (paid)
        return false;

    if (items.empty())
        return false;

    if (payment_status != PaymentStatus::None)
        return false;

    if (received_amount <= 0)
    return false;

    if (received_amount < get_total())
        return false;

    cash_received = received_amount;
    cash_change =
        received_amount - get_total();

    payment_method =
        PaymentMethod::Cash;

    payment_status =
        PaymentStatus::Processing;

    return true;
}
PaymentMethod Order::get_payment_method() const
{
    return payment_method;
}

PaymentStatus Order::get_payment_status() const
{
    return payment_status;
}

int Order::get_cash_received() const
{
    return cash_received;
}

int Order::get_cash_change() const
{
    return cash_change;
}

const std::string& Order::get_transaction_id() const
{
    return transaction_id;
}


bool Order::pay_for_order(
    PaymentMethod method
)
{
    if (!start_payment(method))
        return false;

    return approve_payment();
}


const std::vector<OrderItem>& Order::get_items() const
{
    return items;
}

int Order::get_total() const
{
    int total = 0;

    for (const OrderItem& item : items)
    {
        const int subtotal = item.get_subtotal();

        if (subtotal > std::numeric_limits<int>::max() - total)
        {
            return std::numeric_limits<int>::max();
        }

        total += subtotal;
    }

    return total;
}

void Order::display() const
{
    std::cout << "========== ORDER ==========\n";

    if (items.empty())
    {
        std::cout << "Order is empty.\n";
    }
    else
    {
        for (const OrderItem& item : items)
        {
            item.display();
        }

        std::cout << "---------------------------\n";
        std::cout << "Total: " << get_total() << " AMD\n";
    }

    std::cout
        << "Status: "
        << (paid ? "PAID" : "OPEN")
        << '\n';
}