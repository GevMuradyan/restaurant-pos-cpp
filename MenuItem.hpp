#ifndef MENUITEM_HPP
#define MENUITEM_HPP

#include <string>

class MenuItem
{
private:
    int id;
    std::string name;
    int price;

public:
    MenuItem(int id, const std::string& name, int price);

    int get_id() const;
    const std::string& get_name() const;
    int get_price() const;

    bool set_name(const std::string& name);
    bool set_price(int price);

    void display() const;
};

#endif