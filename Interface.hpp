#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "Restaurant.hpp"
#include "Menu.hpp"

void run_interface(Restaurant& restaurant, Menu& menu);

void run_admin_menu(Restaurant& restaurant, Menu& menu);

void run_client_menu(Restaurant& restaurant, Menu& menu);

#endif