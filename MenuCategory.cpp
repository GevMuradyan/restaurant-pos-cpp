#include "MenuCategory.hpp"
#include <string>
#include <iostream>

MenuCategory::MenuCategory (const std::string& m_name):name(m_name)
{

}

const std::vector<MenuItem>& MenuCategory:: get_items() const
{
    return items;
}


const std::string& MenuCategory::get_name()const
{
    return name;
}

bool MenuCategory::add_item(const MenuItem& item)
{
    for(const MenuItem& m_item : items ){
        if(m_item.get_id() == item.get_id()){
            return false;
           }
        }
        
        items.push_back(item);
        return true;


}
bool MenuCategory::remove_item(int item_id)
{
    for(auto it = items.begin(); it != items.end(); ++it){
        if(it->get_id() == item_id )
        {
            items.erase(it);
            return true;
        }
    }
    return false;
}
    
MenuItem* MenuCategory::find_item_by_id(int item_id)
{

    for(MenuItem& item : items)
    {
        if(item.get_id() == item_id)
        {
            return &item;
        }
    }
    return nullptr;

}
const MenuItem* MenuCategory::find_item_by_id(int item_id) const
{
    for(const MenuItem& item : items)
    {
        if(item.get_id() == item_id)
        {
            return &item;
        }
    }
    return nullptr;

}
        
void MenuCategory::display()const
{
    std::cout<<"====== "<<name<<" ======"<<"\n\n";

    for(const MenuItem& item: items)
    {
        item.display();
    }
    
}

