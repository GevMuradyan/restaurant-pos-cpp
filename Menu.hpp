#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include <vector>

#include "MenuCategory.hpp"
#include "MenuItem.hpp"

class Menu
{
private:
    std::vector<MenuCategory> categories;

public:
    bool add_category(const MenuCategory& category);
    bool remove_category(const std::string& category_name);

    bool rename_category(
        const std::string& old_name,
        const std::string& new_name
    );

    MenuCategory* get_category_by_name(
        const std::string& name
    );

    const MenuCategory* get_category_by_name(
        const std::string& name
    ) const;

    bool add_item_to_category(
        const std::string& category_name,
        const MenuItem& item
    );

    bool remove_item(int item_id);

    bool rename_item(
        int item_id,
        const std::string& new_name
    );

    bool change_item_price(
        int item_id,
        int new_price
    );

    MenuItem* get_item_by_id(int item_id);
    const MenuItem* get_item_by_id(int item_id) const;

    const std::vector<MenuCategory>& get_categories() const;

    bool load_from_file(const std::string& filename);
    bool save_to_file(const std::string& filename) const;

    void display() const;
};

#endif