#include "Menu.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

bool Menu::add_category(const MenuCategory& category)
{
    if (category.get_name().empty())
    {
        return false;
    }

    if (get_category_by_name(category.get_name()) != nullptr)
    {
        return false;
    }

    categories.push_back(category);

    return true;
}

bool Menu::remove_category(const std::string& category_name)
{
    if (category_name.empty())
    {
        return false;
    }

    for (auto it = categories.begin(); it != categories.end(); ++it)
    {
        if (it->get_name() == category_name)
        {
            categories.erase(it);

            return true;
        }
    }

    return false;
}

bool Menu::rename_category(
    const std::string& old_name,
    const std::string& new_name
)
{
    if (old_name.empty() || new_name.empty())
    {
        return false;
    }

    MenuCategory* category =
        get_category_by_name(old_name);

    if (category == nullptr)
    {
        return false;
    }

    if (old_name != new_name &&
        get_category_by_name(new_name) != nullptr)
    {
        return false;
    }

    return category->set_name(new_name);
}

MenuCategory* Menu::get_category_by_name(
    const std::string& name
)
{
    for (MenuCategory& category : categories)
    {
        if (category.get_name() == name)
        {
            return &category;
        }
    }

    return nullptr;
}

const MenuCategory* Menu::get_category_by_name(
    const std::string& name
) const
{
    for (const MenuCategory& category : categories)
    {
        if (category.get_name() == name)
        {
            return &category;
        }
    }

    return nullptr;
}

bool Menu::add_item_to_category(
    const std::string& category_name,
    const MenuItem& item
)
{
    if (item.get_id() <= 0)
    {
        return false;
    }

    if (item.get_price() < 0)
    {
        return false;
    }

    if (item.get_name().empty())
    {
        return false;
    }

    // Item IDs are globally unique.
    if (get_item_by_id(item.get_id()) != nullptr)
    {
        return false;
    }

    MenuCategory* category =
        get_category_by_name(category_name);

    if (category == nullptr)
    {
        return false;
    }

    return category->add_item(item);
}

bool Menu::remove_item(int item_id)
{
    if (item_id <= 0)
    {
        return false;
    }

    for (MenuCategory& category : categories)
    {
        if (category.find_item_by_id(item_id) != nullptr)
        {
            return category.remove_item(item_id);
        }
    }

    return false;
}

bool Menu::rename_item(
    int item_id,
    const std::string& new_name
)
{
    if (item_id <= 0 || new_name.empty())
    {
        return false;
    }

    MenuItem* item = get_item_by_id(item_id);

    if (item == nullptr)
    {
        return false;
    }

    return item->set_name(new_name);
}

bool Menu::change_item_price(
    int item_id,
    int new_price
)
{
    if (item_id <= 0 || new_price < 0)
    {
        return false;
    }

    MenuItem* item = get_item_by_id(item_id);

    if (item == nullptr)
    {
        return false;
    }

    return item->set_price(new_price);
}

MenuItem* Menu::get_item_by_id(int item_id)
{
    for (MenuCategory& category : categories)
    {
        MenuItem* item =
            category.find_item_by_id(item_id);

        if (item != nullptr)
        {
            return item;
        }
    }

    return nullptr;
}

const MenuItem* Menu::get_item_by_id(int item_id) const
{
    for (const MenuCategory& category : categories)
    {
        const MenuItem* item =
            category.find_item_by_id(item_id);

        if (item != nullptr)
        {
            return item;
        }
    }

    return nullptr;
}

const std::vector<MenuCategory>& Menu::get_categories() const
{
    return categories;
}

bool Menu::load_from_file(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return false;
    }

    std::vector<MenuCategory> loaded_categories;

    std::string line;
    int line_number = 0;
    MenuCategory* current_category = nullptr;

    while (std::getline(file, line))
    {
        ++line_number;

        if (line.empty())
        {
            continue;
        }

        // Category format:
        // [CategoryName]
        if (line.front() == '[' &&
            line.back() == ']')
        {
            const std::string category_name =
                line.substr(1, line.size() - 2);

            if (category_name.empty())
            {
                return false;
            }

            bool duplicate = false;

            for (const MenuCategory& category : loaded_categories)
            {
                if (category.get_name() == category_name)
                {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate)
            {
                return false;
            }

            loaded_categories.emplace_back(category_name);

            current_category =
                &loaded_categories.back();

            continue;
        }

        if (current_category == nullptr)
        {
            return false;
        }

        std::stringstream ss(line);

        std::string id_text;
        std::string name;
        std::string price_text;

        if (!std::getline(ss, id_text, ',') ||
            !std::getline(ss, name, ',') ||
            !std::getline(ss, price_text))
        {
            return false;
        }

        try
        {
            const int id = std::stoi(id_text);
            const int price = std::stoi(price_text);

            if (id <= 0 ||
                price < 0 ||
                name.empty())
            {
                return false;
            }

            // IDs must be globally unique.
            for (const MenuCategory& category :
                 loaded_categories)
            {
                if (category.find_item_by_id(id) != nullptr)
                {
                    return false;
                }
            }

            MenuItem item(id, name, price);

            if (!current_category->add_item(item))
            {
                return false;
            }
        }
        catch (const std::invalid_argument&)
        {
            return false;
        }
        catch (const std::out_of_range&)
        {
            return false;
        }
    }

    categories = std::move(loaded_categories);

    return true;
}

bool Menu::save_to_file(const std::string& filename) const
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        return false;
    }

    for (const MenuCategory& category : categories)
    {
        file
            << '['
            << category.get_name()
            << "]\n";

        for (const MenuItem& item :
             category.get_items())
        {
            file
                << item.get_id()
                << ','
                << item.get_name()
                << ','
                << item.get_price()
                << '\n';
        }

        file << '\n';
    }

    return true;
}

void Menu::display() const
{
    std::cout
        << "======= MENU ========\n\n";

    for (const MenuCategory& category : categories)
    {
        category.display();
        std::cout << '\n';
    }
}