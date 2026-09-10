#include "Menu.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>


    bool Menu::add_category(const MenuCategory& m_categorie)
    {
        for(const MenuCategory& categ : categories)
        {
            if(categ.get_name() == m_categorie.get_name()){
                return false;
            }
        }
        categories.push_back(m_categorie);
        return true;
    }
    bool Menu:: remove_category(const std::string& name_categorie)
    {
        for(auto it = categories.begin(); it!= categories.end(); ++it)
        {
            if(it->get_name() == name_categorie)
            {
                categories.erase(it);
                return true;
            }
        }
        return false;
    }

    MenuCategory* Menu:: get_category_by_name(const std::string& name)
    {
        for(MenuCategory& categ: categories )
        {
            if(categ.get_name() == name){
                return &categ;
            }
        }
        return nullptr;
    }
    const MenuCategory* Menu::get_category_by_name(const std::string& name)const
    {
        for(const MenuCategory& categ: categories)
        {
            if(categ.get_name() == name){
                return &categ;
            }
        }
        return nullptr;
    }

    MenuItem* Menu::get_item_by_id(int item_id)
    {
    for (MenuCategory& category : categories)
    {
        MenuItem* item = category.find_item_by_id(item_id);

        if (item != nullptr)
        {
            return item;
        }
    }

    return nullptr;
        
    }

    const MenuItem* Menu:: get_item_by_id(int item_id) const
    {

    for (const MenuCategory& category : categories)
    {
        const MenuItem* item = category.find_item_by_id(item_id);

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

            if (line.front() == '[' && line.back() == ']')
            {
                std::string category_name =
                    line.substr(1, line.size() - 2);

                MenuCategory category(category_name);

                add_category(category);

                current_category = get_category_by_name(category_name);
            }

            else
            {
            if(current_category == nullptr)
            {
                continue;
            }

            std::stringstream ss(line);

            std::string id_text;
            std::string name;
            std::string price_text;

            if (!std::getline(ss, id_text, ',') ||
                !std::getline(ss, name, ',') ||
                !std::getline(ss, price_text))
            {
                std::cout << "Error in line " << line_number
                        << ": invalid item format\n";
                continue;
            }

            try
            {
                int id = std::stoi(id_text);
                int price = std::stoi(price_text);

                MenuItem item(id, name, price);
                current_category->add_item(item);
            }

            catch(const std::invalid_argument&)
            {
                std::cout<<"Error in line "<<line_number<<": invalid number\n";
                continue;
            }

            catch(const std::out_of_range&)
            {
                std::cout<<"Error in line "<<line_number<<": number is too larger\n";
                continue;
            }

        }
    }

    return true;
    }

    bool Menu:: save_to_file(const std::string& filename)const
    {
        std::ofstream file(filename);
        if(!file.is_open())
        {
            return false;
        }

        for(const MenuCategory& category: categories)
        {
            file<<"[" <<category.get_name()<<"]\n";

            for(const MenuItem& item: category.get_items())
            {
                file<<item.get_id()<<","<<item.get_name()<<","<<item.get_price()<<"\n";
            }
        }

        return true;    
    }

    void Menu::display()const
    {
        std::cout<<"======= MENU ========\n\n";

            for(const MenuCategory& categ: categories)
            {
                categ.display();
            }
    
    }