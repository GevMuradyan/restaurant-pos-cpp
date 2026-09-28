#ifndef MENUCATEGORY_HPP
#define MENUCATEGORY_HPP

#include <string>
#include <vector>

#include "MenuItem.hpp"

class MenuCategory
{
private:
    std::string name;
    std::vector<MenuItem> items;

public:
    explicit MenuCategory(const std::string& name);

    const std::string& get_name() const;
    const std::vector<MenuItem>& get_items() const;

    bool set_name(const std::string& name);

    bool add_item(const MenuItem& item);
    bool remove_item(int item_id);

    MenuItem* find_item_by_id(int item_id);
    const MenuItem* find_item_by_id(int item_id) const;

    void display() const;
};

#endif