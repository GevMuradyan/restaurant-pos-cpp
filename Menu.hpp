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

    bool add_category(const MenuCategory& m_category);
    bool remove_category(const std::string& name_category);

    MenuCategory* get_category_by_name(const std::string& name);
    const MenuCategory* get_category_by_name(const std::string& name)const;

    MenuItem* get_item_by_id(int item_id);
    const MenuItem* get_item_by_id(int item_id) const;

    const std::vector<MenuCategory>& get_categories() const;

    bool load_from_file(const std::string& filename);
    bool save_to_file(const std::string& filename) const;

    void display()const;
};

#endif