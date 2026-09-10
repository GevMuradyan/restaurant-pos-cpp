#include "Restaurant.hpp"

#include <iostream>


Restaurant::Restaurant(const std::string& name):restaurant_name(name)
{

}

bool Restaurant::add_zone(const Zone& zone)
{
    
        for(const Zone& i_zone : zones)
        {
            if (zone.get_name() == i_zone.get_name())
            {
                return false;
            }
        }

        zones.push_back(zone);
        return true;
}

bool Restaurant:: add_table_to_zone(const std::string& zone_name, const Table& table)
{
    if(get_table_by_number(table.get_table_number()) != nullptr)
    {
        return false;
    }

    Zone* zone = get_zone_by_name(zone_name);
    if(zone == nullptr)
    {
        return false;
    }

    return zone->add_table(table);
    
}

const std::vector<Zone>& Restaurant::get_zones() const
{
    return zones;
}

const std::string& Restaurant::get_name() const
{
    return restaurant_name;
}

bool Restaurant::remove_zone(const std::string& zone_name)
{
    for(auto it = zones.begin(); it != zones.end(); ++it)
    {
        if(it->get_name() == zone_name){
            zones.erase(it);
            return true;
        }
    }

    return false;
}

Zone* Restaurant:: get_zone_by_name(const std::string& zone_name)
{
    for(Zone& i_zone : zones){
        if(i_zone.get_name() == zone_name){
            return &i_zone;
        }
    }

    return nullptr;

}
const Zone* Restaurant::get_zone_by_name(const std::string& zone_name)const
{
    for(const Zone& i_zone : zones){
        if(i_zone.get_name() == zone_name){
            return &i_zone;
        }
    }

    return nullptr;

}

Table* Restaurant:: get_table_by_number(int table_number)
{
    for(Zone& zone: zones)
    {
      Table* table =   zone.get_table_by_number(table_number);
        if(table!=nullptr )
        {
            return table;
        }
    }

    return nullptr;
}
const Table* Restaurant:: get_table_by_number(int table_number) const
{
        for(const Zone& zone: zones)
    {
        const Table* table = zone.get_table_by_number(table_number);
            if(table != nullptr)
            {
                return table;
            }
    }
    return nullptr;

}

Zone* Restaurant::get_zone_by_table_number(int table_number)
{
    for (Zone& zone : zones)
    {
        if (zone.get_table_by_number(table_number) != nullptr)
        {
            return &zone;
        }
    }

    return nullptr;
}

const Zone* Restaurant::get_zone_by_table_number(int table_number) const
{
    for (const Zone& zone : zones)
    {
        if (zone.get_table_by_number(table_number) != nullptr)
        {
            return &zone;
        }
    }

    return nullptr;
}

void Restaurant::display()const
{
    std::cout<<"======== "<< restaurant_name <<" ========"<<"\n\n";
    std::cout<<"\n";

        for(const Zone& i_zone: zones){
            i_zone.display();
            std::cout<<"\n";
        }
}

