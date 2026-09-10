#include "Zone.hpp"
#include <iostream>




Zone::Zone(const std::string& name):zone_name(name)
{
    
}
    bool Zone::add_table(const Table& table)
    {
        int tab_num = table.get_table_number();
        for(const Table& tab : tables){
            if(tab.get_table_number() == tab_num){
                return false;
            }
        }
        
        tables.push_back(table);
        return true;
    }

    bool Zone::remove_table(int table_number)
    {
        for(auto it = tables.begin(); it != tables.end(); ++it){
            if(it->get_table_number() == table_number){
                
                tables.erase(it);
                return true;
            }
        }

        return false;

    }

    const std::vector<Table>& Zone::get_tables() const
    {
    return tables;
    }

    Table* Zone::get_table_by_number(int table_number){

        for(Table& table: tables){
            if(table.get_table_number() == table_number){
                return &table;
            }
        }
        return nullptr;
    }

    const Table* Zone::get_table_by_number(int table_number)const
    {
            for(const Table& tab: tables){
            if(tab.get_table_number() == table_number){
                return &tab;
            }
        }
        return nullptr;
    
    }

    const std::string& Zone::get_name() const
    {
        return zone_name;
    }

   
    void Zone::display()const
    {
        std::cout<<"======== "<< zone_name <<" ========"<<"\n";
        std::cout<<"\n";
        
        for (const Table& table : tables)
        {
            table.display();
            std::cout<<"\n";
        }

    }