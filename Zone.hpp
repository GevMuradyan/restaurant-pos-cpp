#ifndef ZONE_HPP
#define ZONE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Table.hpp"
#include "Zone.hpp"

class Zone
{
    private:
        std::string zone_name;
        std::vector<Table> tables;


    public:

    Zone(const std::string& name);

    bool add_table(const Table& table);
    bool remove_table(int table_number);
    
    const std::vector<Table>& get_tables() const;
    
    Table* get_table_by_number(int table_number);
    const Table* get_table_by_number(int table_number) const;
    
    const std::string& get_name() const;
    
    void display()const;
};

#endif