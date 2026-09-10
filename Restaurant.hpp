#ifndef RESTAURANT_HPP
#define RESTAURANT_HPP

#include <string>
#include <vector>
#include "Zone.hpp"

class Restaurant
{
    private:
        std::string restaurant_name;
        std::vector<Zone> zones;

    public:

        Restaurant(const std::string& name);

        bool add_zone(const Zone& zone);
        bool remove_zone(const std::string& zone_name);
        bool add_table_to_zone(const std::string& zone_name, const Table& table);

        Zone* get_zone_by_table_number(int table_number);
        const Zone* get_zone_by_table_number(int table_number) const;

        Zone* get_zone_by_name(const std::string& zone_name);
        const Zone* get_zone_by_name(const std::string& zone_name)const;

        const std::vector<Zone>& get_zones() const;
        const std::string& get_name() const;


        Table* get_table_by_number(int table_number);
        const Table* get_table_by_number(int table_number) const;

        void display()const;
        


};

#endif