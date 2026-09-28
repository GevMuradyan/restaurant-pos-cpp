#ifndef ORDER_HPP
#define ORDER_HPP

#include <vector>
#include <string>
#include "OrderItem.hpp"

enum class PaymentMethod
{
    None,
    Cash,
    Card
};

enum class PaymentStatus
{
    None,
    Processing,
    Approved,
    Declined,
    Cancelled
};

class Order
{
private:
    std::vector<OrderItem> items;
    std::string transaction_id;

    bool paid = false;

    PaymentMethod payment_method =
        PaymentMethod::None;

    PaymentStatus payment_status =
        PaymentStatus::None;

        int cash_received = 0;
        int cash_change = 0;

public:
   public:
   bool cancel_payment();
    bool add_item(const OrderItem& item);
    bool remove_item(int item_id);
    bool change_item_quantity(
        int item_id,
        int new_quantity
    );

    const std::string& get_transaction_id() const;

    bool is_paid() const;

    bool start_payment(PaymentMethod method);

    bool approve_payment();
    bool decline_payment();

    bool pay_for_order(PaymentMethod method);

    PaymentMethod get_payment_method() const;

    PaymentStatus get_payment_status() const;


    bool start_cash_payment(int received_amount);

    int get_cash_received() const;
    int get_cash_change() const;
    

    const std::vector<OrderItem>& get_items() const;

    int get_total() const;

    void display() const;
};

#endif