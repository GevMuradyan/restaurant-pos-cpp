#include "Interface.hpp"

#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>

namespace
{
constexpr int UI_WIDTH = 82;
constexpr int INNER = UI_WIDTH - 2;

int terminal_width()
{
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        return ws.ws_col;

    return 100;
}

std::string repeat(const std::string& s, int count)
{
    std::string result;
    for (int i = 0; i < count; ++i)
        result += s;
    return result;
}

std::string margin()
{
    int left = (terminal_width() - UI_WIDTH) / 2;
    if (left < 0) left = 0;

    return std::string(static_cast<std::size_t>(left), ' ');
}

std::string fit(const std::string& text, std::size_t width)
{
    if (text.size() <= width)
        return text;

    if (width <= 3)
        return text.substr(0, width);

    return text.substr(0, width - 3) + "...";
}

std::string pad(const std::string& text, int width)
{
    std::string value = fit(text, static_cast<std::size_t>(width));

    if (static_cast<int>(value.size()) < width)
        value += std::string(width - value.size(), ' ');

    return value;
}

std::string center(const std::string& text, int width)
{
    std::string value = fit(text, static_cast<std::size_t>(width));

    int spaces = width - static_cast<int>(value.size());
    int left = spaces / 2;
    int right = spaces - left;

    return std::string(left, ' ') + value + std::string(right, ' ');
}

std::string upper_spaced(const std::string& text)
{
    std::string result;

    for (char c : text)
    {
        if (c == ' ')
        {
            result += "   ";
            continue;
        }

        result += static_cast<char>(
            std::toupper(static_cast<unsigned char>(c))
        );

        result += ' ';
    }

    if (!result.empty())
        result.pop_back();

    return result;
}

void clear_screen()
{
    std::cout << "\033[2J\033[H";
    std::cout<<"\n\n\n\n";
}

void raw(const std::string& text = "")
{
    std::cout << margin() << text << '\n';
}

void prompt(const std::string& text)
{
    std::cout << margin() << "  > " << text;
}

void app_header(const Restaurant& restaurant, const std::string& screen)
{
    raw("╔" + repeat("═", INNER) + "╗");
    raw("║" + center(upper_spaced(restaurant.get_name()), INNER) + "║");
    raw("║" + center("RESTAURANT POS", INNER) + "║");
    raw("╠" + repeat("═", INNER) + "╣");
    raw("║" + center(screen, INNER) + "║");
    raw("╚" + repeat("═", INNER) + "╝");
}

void thin_top()
{
    raw("┌" + repeat("─", INNER) + "┐");
}

void thin_bottom()
{
    raw("└" + repeat("─", INNER) + "┘");
}

void thin_separator()
{
    raw("├" + repeat("─", INNER) + "┤");
}

void box_line(const std::string& text = "")
{
    raw("│" + pad(text, INNER) + "│");
}

void centered_line(const std::string& text)
{
    raw("│" + center(text, INNER) + "│");
}

void section_top(const std::string& title)
{
    std::string value = fit(title, INNER - 4);
    int remaining = INNER - 3 - static_cast<int>(value.size());

    raw("┌─ " + value + " " + repeat("─", remaining) + "┐");
}

void wait_enter()
{
    std::cout << '\n';
    prompt("Press ENTER to continue...");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::cin.get();
}

std::string status_to_string(TableStatus status)
{
    switch (status)
    {
        case TableStatus::Free: return "Free";
        case TableStatus::Occupied: return "Occupied";
        case TableStatus::CallingWaiter: return "Calling waiter";
        case TableStatus::BillRequested: return "Bill requested";
    }

    return "Unknown";
}

bool is_coupe(const Zone& zone)
{
    return zone.get_name().find("Coupe ") == 0;
}

void message_screen(const Restaurant& restaurant, const std::string& message)
{
    clear_screen();
    app_header(restaurant, "MESSAGE");

    std::cout << '\n';

    thin_top();
    box_line();
    centered_line(message);
    box_line();
    thin_bottom();

    wait_enter();
}

void print_zone(const Zone& zone)
{
    section_top(zone.get_name());
    box_line();

    std::ostringstream head;
    head << "   "
         << std::left
         << std::setw(14) << "TABLE"
         << std::setw(14) << "CHAIRS"
         << std::setw(14) << "GUESTS"
         << std::setw(28) << "STATUS";

    box_line(head.str());

    for (const Table& table : zone.get_tables())
    {
        std::ostringstream row;

        row << "   "
            << std::left
            << std::setw(14) << table.get_table_number()
            << std::setw(14) << table.get_chairs_count()
            << std::setw(14) << table.get_clients_count()
            << std::setw(28) << status_to_string(table.get_status());

        box_line(row.str());
    }

    box_line();
    thin_bottom();
}

void print_coupes(const std::vector<Zone>& zones)
{
    bool has_coupes = false;

    for (const Zone& zone : zones)
    {
        if (is_coupe(zone))
        {
            has_coupes = true;
            break;
        }
    }

    if (!has_coupes)
        return;

    section_top("PRIVATE COUPES");
    box_line();

    std::ostringstream head;

    head << "   "
         << std::left
         << std::setw(18) << "COUPE"
         << std::setw(12) << "TABLE"
         << std::setw(12) << "CHAIRS"
         << std::setw(12) << "GUESTS"
         << std::setw(22) << "STATUS";

    box_line(head.str());

    for (const Zone& zone : zones)
    {
        if (!is_coupe(zone))
            continue;

        for (const Table& table : zone.get_tables())
        {
            std::ostringstream row;

            row << "   "
                << std::left
                << std::setw(18) << zone.get_name()
                << std::setw(12) << table.get_table_number()
                << std::setw(12) << table.get_chairs_count()
                << std::setw(12) << table.get_clients_count()
                << std::setw(22) << status_to_string(table.get_status());

            box_line(row.str());
        }
    }

    box_line();
    thin_bottom();
}

void show_tables_screen(const Restaurant& restaurant)
{
    clear_screen();
    app_header(restaurant, "FLOOR OVERVIEW");

    const std::vector<Zone>& zones = restaurant.get_zones();

    for (const Zone& zone : zones)
    {
        if (is_coupe(zone))
            continue;

        std::cout << '\n';
        print_zone(zone);
    }

    std::cout << '\n';
    print_coupes(zones);

    std::cout << '\n';
    raw("  Enter table number");
    raw("  [0] Back");
    std::cout << '\n';
}

void show_menu_screen(const Restaurant& restaurant, const Menu& menu)
{
    clear_screen();
    app_header(restaurant, "RESTAURANT MENU");

    for (const MenuCategory& category : menu.get_categories())
    {
        std::cout << '\n';

        section_top(category.get_name());
        box_line();

        std::ostringstream head;

        head << "   "
             << std::left
             << std::setw(10) << "ID"
             << std::setw(45) << "ITEM"
             << std::right
             << std::setw(16) << "PRICE";

        box_line(head.str());

        for (const MenuItem& item : category.get_items())
        {
            std::ostringstream row;

            std::string price =
                std::to_string(item.get_price()) + " AMD";

            row << "   "
                << std::left
                << std::setw(10) << item.get_id()
                << std::setw(45) << fit(item.get_name(), 42)
                << std::right
                << std::setw(16) << price;

            box_line(row.str());
        }

        box_line();
        thin_bottom();
    }

    std::cout << '\n';
    raw("  [0] Back");
}

void browse_menu(const Restaurant& restaurant, const Menu& menu)
{
    show_menu_screen(restaurant, menu);

    int command = 0;

    std::cout << '\n';
    prompt("Enter 0 to return: ");
    std::cin >> command;
}

void show_main_screen(const Restaurant& restaurant)
{
    clear_screen();
    app_header(restaurant, "MAIN MENU");

    std::cout << '\n';

    thin_top();
    box_line();
    centered_line("[1]  ADMIN PANEL");
    box_line();
    centered_line("[2]  CLIENT POS");
    box_line();
    centered_line("[0]  EXIT");
    box_line();
    thin_bottom();
}

void show_admin_screen(const Restaurant& restaurant)
{
    clear_screen();
    app_header(restaurant, "ADMIN PANEL");

    std::cout << '\n';

    section_top("RESTAURANT MANAGEMENT");
    box_line();
    box_line("   [1] Manage zones");
    box_line("   [2] Manage tables");
    box_line("   [3] Manage menu categories");
    box_line("   [4] Manage menu items");
    box_line("   [5] Save configuration");
    box_line();
    box_line("   [0] Back");
    box_line();
    thin_bottom();
}

void show_pos_screen(
    const Restaurant& restaurant,
    const Table& table,
    const Zone& zone
)
{
    clear_screen();
    app_header(restaurant, "ORDER MANAGEMENT");

    std::cout << '\n';

    section_top("TABLE INFORMATION");
    box_line();

    std::ostringstream info;

    info << "   Table: " << table.get_table_number()
         << "     Zone: " << zone.get_name()
         << "     Guests: " << table.get_clients_count();

    box_line(info.str());
    box_line();
    thin_bottom();

    std::cout << '\n';

    section_top("CURRENT ORDER");
    box_line();

    std::ostringstream head;

    head << "   "
         << std::left
         << std::setw(5) << "#"
         << std::setw(8) << "ID"
         << std::setw(35) << "ITEM"
         << std::right
         << std::setw(8) << "QTY"
         << std::setw(15) << "PRICE";

    box_line(head.str());

    const Order* order = table.get_order();

    if (order != nullptr && !order->get_items().empty())
    {
        int number = 1;

        for (const OrderItem& item : order->get_items())
        {
            std::ostringstream row;

            row << "   "
                << std::left
                << std::setw(5) << number
                << std::setw(8) << item.get_id()
                << std::setw(35) << fit(item.get_name(), 32)
                << std::right
                << std::setw(8) << item.get_quantity()
                << std::setw(15) << item.get_subtotal();

            box_line(row.str());
            ++number;
        }
    }
    else
    {
        box_line();
        centered_line("ORDER IS EMPTY");
    }

    box_line();
    thin_separator();

    if (order != nullptr)
    {
        std::string total =
            "TOTAL: " +
            std::to_string(order->get_total()) +
            " AMD";

        box_line(
            std::string(
                INNER -
                static_cast<int>(total.size()) -
                3,
                ' '
            ) + total
        );
    }

    thin_bottom();

    std::cout << '\n';

    section_top("ACTIONS");
    box_line();
    box_line("   [1] Add item");
    box_line("   [2] Remove item");
    box_line("   [3] Change quantity");
    box_line("   [4] Change table");
    box_line("   [5] View menu");
    box_line("   [6] Show bill");
    box_line("   [7] Payment");
    box_line("   [8] Close table");
    box_line();
    box_line("   [0] Exit client POS");
    box_line();
    thin_bottom();
}

void show_bill(
    const Restaurant& restaurant,
    const Table& table,
    const Zone& zone
)
{
    clear_screen();
    app_header(restaurant, "CUSTOMER BILL");

    const Order* order = table.get_order();

    std::cout << '\n';

    int left = (terminal_width() - 32) / 2;
    if (left < 0) left = 0;

    std::string m(static_cast<std::size_t>(left), ' ');

    std::cout << m << "┌──────────────────────────────┐\n";
    std::cout << m << "│         CTRL + EAT           │\n";
    std::cout << m << "│       RESTAURANT & BAR       │\n";
    std::cout << m << "│                              │\n";
    std::cout << m << "│          CUSTOMER BILL       │\n";
    std::cout << m << "│                              │\n";

    std::ostringstream info1;
    info1 << "  Table: " << table.get_table_number()
          << "        Guests: " << table.get_clients_count();

    std::cout << m << "│" << pad(info1.str(), 30) << "│\n";

    std::string zone_text = "  Zone: " + zone.get_name();
    std::cout << m << "│" << pad(zone_text, 30) << "│\n";

    std::cout << m << "│                              │\n";
    std::cout << m << "│------------------------------│\n";
    std::cout << m << "│                              │\n";

    if (order == nullptr || order->get_items().empty())
    {
        std::cout << m << "│        ORDER IS EMPTY        │\n";
    }
    else
    {
        for (const OrderItem& item : order->get_items())
        {
            std::ostringstream row;

            std::string left_part =
                std::to_string(item.get_quantity()) +
                " x " +
                fit(item.get_name(), 17);

            row << "  "
                << std::left
                << std::setw(20)
                << left_part
                << std::right
                << std::setw(8)
                << item.get_subtotal();

            std::cout << m << "│" << pad(row.str(), 30) << "│\n";
        }
    }

    std::cout << m << "│                              │\n";
    std::cout << m << "│------------------------------│\n";
    std::cout << m << "│                              │\n";

    if (order != nullptr)
    {
        std::ostringstream total_row;

        total_row << "  TOTAL"
                  << std::right
                  << std::setw(19)
                  << order->get_total();

        std::cout << m << "│" << pad(total_row.str(), 30) << "│\n";

        std::cout << m << "│                              │\n";

        std::string total =
            "TOTAL: " +
            std::to_string(order->get_total()) +
            " AMD";

        std::cout << m
                  << "│"
                  << center(total, 30)
                  << "│\n";
    }

    std::cout << m << "│                              │\n";
    std::cout << m << "│------------------------------│\n";
    std::cout << m << "│                              │\n";
    std::cout << m << "│        THANK YOU!            │\n";
    std::cout << m << "│       PLEASE VISIT AGAIN     │\n";
    std::cout << m << "│                              │\n";
    std::cout << m << "└──────────────────────────────┘\n";
}

bool process_payment(
    const Restaurant& restaurant,
    Order& order
)
{
    if (order.is_paid())
    {
        message_screen(restaurant, "ORDER IS ALREADY PAID");
        return false;
    }

    if (order.get_total() <= 0)
    {
        message_screen(restaurant, "ORDER IS EMPTY");
        return false;
    }

    clear_screen();
    app_header(restaurant, "PAYMENT");

    std::cout << '\n';

    section_top("PAYMENT METHOD");
    box_line();

    centered_line(
        "TOTAL: " +
        std::to_string(order.get_total()) +
        " AMD"
    );

    box_line();
    box_line("   [1] Cash");
    box_line("   [2] Card");
    box_line();
    box_line("   [0] Cancel");
    box_line();
    thin_bottom();

    int choice = 0;

    std::cout << '\n';
    prompt("Select payment method: ");
    std::cin >> choice;

    if (choice == 0)
        return false;

    if (choice == 1)
    {
        clear_screen();
        app_header(restaurant, "CASH PAYMENT");

        std::cout << '\n';

        const int CASH_WIDTH = 46;
        const int CASH_INNER = CASH_WIDTH - 2;
        int left = (terminal_width() - CASH_WIDTH) / 2;

        if (left < 0)
            left = 0;

        std::string m(static_cast<std::size_t>(left), ' ');

        auto cash_raw = [&](const std::string& text)
        {
            std::cout << m << text << '\n';
        };

        auto cash_line = [&](const std::string& text = "")
        {
            cash_raw("│" + pad(text, CASH_INNER) + "│");
        };

        cash_raw("┌─ CASH REGISTER " +
                 repeat("─", CASH_INNER - 15) +
                 "┐");

        cash_line();

        std::ostringstream total_row;

        total_row << "   "
                  << std::left
                  << std::setw(18)
                  << "TOTAL"
                  << std::right
                  << std::setw(18)
                  << std::to_string(order.get_total()) + " AMD";

        cash_line(total_row.str());
        cash_line();
        cash_raw("└" + repeat("─", CASH_INNER) + "┘");

        int amount = 0;

        std::cout << '\n';
        prompt("Received amount: ");
        std::cin >> amount;

        if (amount < order.get_total())
        {
            message_screen(
                restaurant,
                "MISSING " +
                std::to_string(order.get_total() - amount) +
                " AMD"
            );

            return false;
        }

        int change = amount - order.get_total();

        if (!order.pay_for_order())
            return false;

        clear_screen();
        app_header(restaurant, "CASH PAYMENT");

        std::cout << '\n';

        cash_raw("┌─ CASH REGISTER " +
                 repeat("─", CASH_INNER - 15) +
                 "┐");

        cash_line();

        std::ostringstream total_done;
        total_done << "   "
                   << std::left
                   << std::setw(18)
                   << "TOTAL"
                   << std::right
                   << std::setw(18)
                   << std::to_string(order.get_total()) + " AMD";

        cash_line(total_done.str());

        std::ostringstream received_done;
        received_done << "   "
                      << std::left
                      << std::setw(18)
                      << "RECEIVED"
                      << std::right
                      << std::setw(18)
                      << std::to_string(amount) + " AMD";

        cash_line(received_done.str());

        std::ostringstream change_done;
        change_done << "   "
                    << std::left
                    << std::setw(18)
                    << "CHANGE"
                    << std::right
                    << std::setw(18)
                    << std::to_string(change) + " AMD";

        cash_line(change_done.str());

        cash_line();
        cash_raw("├" + repeat("─", CASH_INNER) + "┤");
        cash_raw("│" + center("PAYMENT COMPLETE", CASH_INNER) + "│");
        cash_raw("└" + repeat("─", CASH_INNER) + "┘");

        wait_enter();
        return true;
    }

    if (choice == 2)
    {
        clear_screen();
        app_header(restaurant, "CARD PAYMENT");

        std::cout << '\n';

        const int TERM_WIDTH = 40;
        const int TERM_INNER = TERM_WIDTH - 2;
        int left = (terminal_width() - TERM_WIDTH) / 2;

        if (left < 0)
            left = 0;

        std::string m(static_cast<std::size_t>(left), ' ');

        auto term_raw = [&](const std::string& text)
        {
            std::cout << m << text << '\n';
        };

        auto term_line = [&](const std::string& text = "")
        {
            term_raw("│" + pad(text, TERM_INNER) + "│");
        };

        term_raw("┌" + repeat("─", TERM_INNER) + "┐");
        term_line();
        term_raw("│" + center(restaurant.get_name(), TERM_INNER) + "│");
        term_raw("│" + center("PAYMENT TERMINAL", TERM_INNER) + "│");
        term_line();
        term_raw("│" + center("AMOUNT", TERM_INNER) + "│");
        term_raw(
            "│" +
            center(
                std::to_string(order.get_total()) + " AMD",
                TERM_INNER
            ) +
            "│"
        );
        term_line();

        term_raw("│    ┌──────────────────────────┐    │");
        term_raw("│    │                          │    │");
        term_raw("│    │      TAP / INSERT        │    │");
        term_raw("│    │          CARD            │    │");
        term_raw("│    │                          │    │");
        term_raw("│    └──────────────────────────┘    │");

        term_line();

        term_raw("│          [1] [2] [3]               │");
        term_raw("│          [4] [5] [6]               │");
        term_raw("│          [7] [8] [9]               │");
        term_raw("│          [C] [0] [OK]              │");

        term_line();
        term_raw("└" + repeat("─", TERM_INNER) + "┘");

        std::string pin;

        std::cout << '\n';
        prompt("PIN: ");
        std::cin >> pin;

        if (!order.pay_for_order())
            return false;

        std::cout << '\a' << std::flush;
        std::system("paplay /usr/share/sounds/freedesktop/stereo/complete.oga >/dev/null 2>&1 &");

        clear_screen();
        app_header(restaurant, "CARD PAYMENT");

        std::cout << '\n';

        term_raw("┌" + repeat("─", TERM_INNER) + "┐");
        term_line();
        term_raw("│" + center(restaurant.get_name(), TERM_INNER) + "│");
        term_raw("│" + center("PAYMENT TERMINAL", TERM_INNER) + "│");
        term_line();
        term_raw("│" + center("PAYMENT APPROVED", TERM_INNER) + "│");
        term_line();
        term_raw(
            "│" +
            center(
                std::to_string(order.get_total()) + " AMD",
                TERM_INNER
            ) +
            "│"
        );
        term_line();
        term_raw("└" + repeat("─", TERM_INNER) + "┘");

        wait_enter();
        return true;
    }

    message_screen(restaurant, "INVALID PAYMENT METHOD");
    return false;
}

bool select_table(
    Restaurant& restaurant,
    Table*& current_table,
    Zone*& current_zone
)
{
    while (true)
    {
        show_tables_screen(restaurant);

        int table_number = 0;

        prompt("Table number: ");
        std::cin >> table_number;

        if (table_number == 0)
            return false;

        Table* table =
            restaurant.get_table_by_number(table_number);

        if (table == nullptr)
        {
            message_screen(restaurant, "TABLE NOT FOUND");
            continue;
        }

        Zone* zone =
            restaurant.get_zone_by_table_number(table_number);

        if (zone == nullptr)
        {
            message_screen(restaurant, "ZONE NOT FOUND");
            continue;
        }

        if (table->get_status() == TableStatus::Free)
        {
            clear_screen();
            app_header(restaurant, "OPEN TABLE");

            std::cout << '\n';

            section_top("TABLE INFORMATION");
            box_line();
            box_line("   Table: " + std::to_string(table->get_table_number()));
            box_line("   Zone: " + zone->get_name());
            box_line("   Chairs: " + std::to_string(table->get_chairs_count()));
            box_line();
            thin_bottom();

            int guests = 0;

            std::cout << '\n';
            prompt("Guests: ");
            std::cin >> guests;

            if (guests <= 0)
            {
                message_screen(restaurant, "INVALID NUMBER OF GUESTS");
                continue;
            }

            if (guests > table->get_chairs_count())
            {
                message_screen(restaurant, "NOT ENOUGH CHAIRS");
                continue;
            }

            table->open_table(guests);

            if (table->get_order() == nullptr)
            {
                message_screen(restaurant, "CANNOT OPEN TABLE");
                continue;
            }
        }

        current_table = table;
        current_zone = zone;
        return true;
    }
}

void add_items_screen(
    const Restaurant& restaurant,
    Menu& menu,
    Order& order
)
{
    while (true)
    {
        show_menu_screen(restaurant, menu);

        std::cout << '\n';
        raw("  Enter item ID");
        raw("  [0] Finish order");

        int item_id = 0;

        std::cout << '\n';
        prompt("Item ID: ");
        std::cin >> item_id;

        if (item_id == 0)
            return;

        MenuItem* item =
            menu.get_item_by_id(item_id);

        if (item == nullptr)
        {
            message_screen(restaurant, "ITEM NOT FOUND");
            continue;
        }

        int quantity = 0;

        prompt("Quantity: ");
        std::cin >> quantity;

        if (quantity <= 0)
        {
            message_screen(restaurant, "INVALID QUANTITY");
            continue;
        }

        order.add_item(OrderItem(*item, quantity));
    }
}

} // namespace

void run_interface(Restaurant& restaurant, Menu& menu)
{
    while (true)
    {
        show_main_screen(restaurant);

        int choice = 0;

        std::cout << '\n';
        prompt("Select option: ");
        std::cin >> choice;

        switch (choice)
        {
            case 1:
                run_admin_menu(restaurant, menu);
                break;

            case 2:
                run_client_menu(restaurant, menu);
                break;

            case 0:
                clear_screen();
                return;

            default:
                message_screen(restaurant, "INVALID OPTION");
        }
    }
}

void run_admin_menu(Restaurant& restaurant, Menu&)
{
    while (true)
    {
        show_admin_screen(restaurant);

        int choice = 0;

        std::cout << '\n';
        prompt("Select option: ");
        std::cin >> choice;

        if (choice == 0)
            return;

        message_screen(
            restaurant,
            "ADMIN OPTION NOT IMPLEMENTED YET"
        );
    }
}

void run_client_menu(Restaurant& restaurant, Menu& menu)
{
    Table* current_table = nullptr;
    Zone* current_zone = nullptr;

    while (true)
    {
        if (current_table == nullptr)
        {
            if (!select_table(
                restaurant,
                current_table,
                current_zone
            ))
            {
                return;
            }
        }

        show_pos_screen(
            restaurant,
            *current_table,
            *current_zone
        );

        int choice = 0;

        std::cout << '\n';
        prompt("Select option: ");
        std::cin >> choice;

        switch (choice)
        {
            case 1:
            {
                Order* order = current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(restaurant, "NO ACTIVE ORDER");
                    break;
                }

                if (order->is_paid())
                {
                    message_screen(
                        restaurant,
                        "PAID ORDER CANNOT BE CHANGED"
                    );

                    break;
                }

                add_items_screen(
                    restaurant,
                    menu,
                    *order
                );

                break;
            }

            case 2:
            {
                Order* order = current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(restaurant, "NO ACTIVE ORDER");
                    break;
                }

                if (order->is_paid())
                {
                    message_screen(
                        restaurant,
                        "PAID ORDER CANNOT BE CHANGED"
                    );

                    break;
                }

                int id = 0;

                prompt("Item ID to remove: ");
                std::cin >> id;

                if (!order->remove_item(id))
                {
                    message_screen(
                        restaurant,
                        "ITEM NOT FOUND IN ORDER"
                    );
                }

                break;
            }

            case 3:
            {
                Order* order = current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(restaurant, "NO ACTIVE ORDER");
                    break;
                }

                if (order->is_paid())
                {
                    message_screen(
                        restaurant,
                        "PAID ORDER CANNOT BE CHANGED"
                    );

                    break;
                }

                int id = 0;
                int quantity = 0;

                prompt("Item ID: ");
                std::cin >> id;

                prompt("New quantity: ");
                std::cin >> quantity;

                if (!order->change_item_quantity(id, quantity))
                {
                    message_screen(
                        restaurant,
                        "CANNOT CHANGE QUANTITY"
                    );
                }

                break;
            }

            case 4:
            {
                Table* table = nullptr;
                Zone* zone = nullptr;

                if (select_table(
                    restaurant,
                    table,
                    zone
                ))
                {
                    current_table = table;
                    current_zone = zone;
                }

                break;
            }

            case 5:
                browse_menu(restaurant, menu);
                break;

            case 6:
                show_bill(
                    restaurant,
                    *current_table,
                    *current_zone
                );

                wait_enter();
                break;

            case 7:
            {
                Order* order = current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(restaurant, "NO ACTIVE ORDER");
                    break;
                }

                process_payment(
                    restaurant,
                    *order
                );

                break;
            }

            case 8:
            {
                Order* order = current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(restaurant, "NO ACTIVE ORDER");
                    break;
                }

                if (!order->is_paid())
                {
                    message_screen(
                        restaurant,
                        "PAY THE BILL BEFORE CLOSING TABLE"
                    );

                    break;
                }

                current_table->close_table();

                current_table = nullptr;
                current_zone = nullptr;

                break;
            }

            case 0:
                return;

            default:
                message_screen(restaurant, "INVALID OPTION");
        }
    }
}