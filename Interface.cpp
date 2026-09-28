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

// ============================================================
// TERMINAL / UI HELPERS
// ============================================================

int terminal_width()
{
    winsize ws{};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0)
    {
        return ws.ws_col;
    }

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

    if (left < 0)
        left = 0;

    return std::string(
        static_cast<std::size_t>(left),
        ' '
    );
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
    std::string value =
        fit(text, static_cast<std::size_t>(width));

    if (static_cast<int>(value.size()) < width)
    {
        value += std::string(
            width - static_cast<int>(value.size()),
            ' '
        );
    }

    return value;
}

std::string center(const std::string& text, int width)
{
    std::string value =
        fit(text, static_cast<std::size_t>(width));

    int spaces =
        width - static_cast<int>(value.size());

    int left = spaces / 2;
    int right = spaces - left;

    return std::string(left, ' ') +
           value +
           std::string(right, ' ');
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
            std::toupper(
                static_cast<unsigned char>(c)
            )
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
    std::cout << "\n\n\n\n";
}

void raw(const std::string& text = "")
{
    std::cout << margin() << text << '\n';
}

void prompt(const std::string& text)
{
    std::cout << margin() << "  > " << text;
}

void app_header(
    const Restaurant& restaurant,
    const std::string& screen
)
{
    raw("╔" + repeat("═", INNER) + "╗");
    raw(
        "║" +
        center(
            upper_spaced(restaurant.get_name()),
            INNER
        ) +
        "║"
    );
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
    std::string value =
        fit(title, INNER - 4);

    int remaining =
        INNER -
        3 -
        static_cast<int>(value.size());

    if (remaining < 0)
        remaining = 0;

    raw(
        "┌─ " +
        value +
        " " +
        repeat("─", remaining) +
        "┐"
    );
}

void clear_input()
{
    std::cin.clear();

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );
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

bool read_int(
    const std::string& text,
    int& value
)
{
    prompt(text);

    if (!(std::cin >> value))
    {
        clear_input();
        return false;
    }

    return true;
}



std::string status_to_string(TableStatus status)
{
    switch (status)
    {
        case TableStatus::Free:
            return "Free";

        case TableStatus::Occupied:
            return "Occupied";

        case TableStatus::CallingWaiter:
            return "Calling waiter";

        case TableStatus::BillRequested:
            return "Bill requested";
    }

    return "Unknown";
}

bool is_coupe(const Zone& zone)
{
    return zone.get_name().find("Coupe ") == 0;
}

// Forward declaration.
void message_screen(
    const Restaurant& restaurant,
    const std::string& message
);

// ============================================================
// MESSAGE
// ============================================================

void message_screen(
    const Restaurant& restaurant,
    const std::string& message
)
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

// ============================================================
// TABLE DISPLAY
// ============================================================

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
            << std::setw(14)
            << table.get_table_number()
            << std::setw(14)
            << table.get_chairs_count()
            << std::setw(14)
            << table.get_clients_count()
            << std::setw(28)
            << status_to_string(table.get_status());

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
                << std::setw(18)
                << zone.get_name()
                << std::setw(12)
                << table.get_table_number()
                << std::setw(12)
                << table.get_chairs_count()
                << std::setw(12)
                << table.get_clients_count()
                << std::setw(22)
                << status_to_string(table.get_status());

            box_line(row.str());
        }
    }

    box_line();

    thin_bottom();
}

void show_tables_screen(const Restaurant& restaurant)
{
    clear_screen();

    app_header(
        restaurant,
        "FLOOR OVERVIEW"
    );

    const std::vector<Zone>& zones =
        restaurant.get_zones();

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

// ============================================================
// MENU DISPLAY
// ============================================================

void show_menu_screen(
    const Restaurant& restaurant
)
{
    const Menu& menu =
        restaurant.get_menu();

    clear_screen();

    app_header(
        restaurant,
        "RESTAURANT MENU"
    );

    for (const MenuCategory& category :
         menu.get_categories())
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

        for (const MenuItem& item :
             category.get_items())
        {
            std::ostringstream row;

            std::string price =
                std::to_string(item.get_price()) +
                " AMD";

            row << "   "
                << std::left
                << std::setw(10)
                << item.get_id()
                << std::setw(45)
                << fit(item.get_name(), 42)
                << std::right
                << std::setw(16)
                << price;

            box_line(row.str());
        }

        box_line();

        thin_bottom();
    }

    std::cout << '\n';

    raw("  [0] Back");
}

void browse_menu(const Restaurant& restaurant)
{
    show_menu_screen(restaurant);

    int command = 0;

    std::cout << '\n';

    prompt("Enter 0 to return: ");

    std::cin >> command;

    if (!std::cin)
        clear_input();
}

// ============================================================
// MAIN / ADMIN SCREENS
// ============================================================

void show_main_screen(
    const Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "MAIN MENU"
    );

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

void show_admin_screen(
    const Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "ADMIN PANEL"
    );

    std::cout << '\n';

    section_top("RESTAURANT MANAGEMENT");

    box_line();

    box_line("   [1] Manage zones");
    box_line("   [2] Manage tables");
    box_line("   [3] Manage menu categories");
    box_line("   [4] Manage menu items");
    box_line("   [5] Save menu configuration");
    box_line("   [6] Load menu configuration");

    box_line();

    box_line("   [0] Back");

    box_line();

    thin_bottom();
}

// ============================================================
// POS SCREEN
// ============================================================

void show_pos_screen(
    const Restaurant& restaurant,
    const Table& table,
    const Zone& zone
)
{
    clear_screen();

    app_header(
        restaurant,
        "ORDER MANAGEMENT"
    );

    std::cout << '\n';

    section_top("TABLE INFORMATION");

    box_line();

    std::ostringstream info;

    info << "   Table: "
         << table.get_table_number()
         << "     Zone: "
         << zone.get_name()
         << "     Guests: "
         << table.get_clients_count();

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

    const Order* order =
        table.get_order();

    if (order != nullptr &&
        !order->get_items().empty())
    {
        int number = 1;

        for (const OrderItem& item :
             order->get_items())
        {
            std::ostringstream row;

            row << "   "
                << std::left
                << std::setw(5)
                << number
                << std::setw(8)
                << item.get_id()
                << std::setw(35)
                << fit(item.get_name(), 32)
                << std::right
                << std::setw(8)
                << item.get_quantity()
                << std::setw(15)
                << item.get_subtotal();

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

        int spaces =
            INNER -
            static_cast<int>(total.size()) -
            3;

        if (spaces < 0)
            spaces = 0;

        box_line(
            std::string(
                static_cast<std::size_t>(spaces),
                ' '
            ) +
            total
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

// ============================================================
// BILL
// ============================================================

void show_bill(
    const Restaurant& restaurant,
    const Table& table,
    const Zone& zone
)
{
    clear_screen();

    app_header(
        restaurant,
        "CUSTOMER BILL"
    );

    const Order* order =
        table.get_order();

    std::cout << '\n';

    int left =
        (terminal_width() - 32) / 2;

    if (left < 0)
        left = 0;

    std::string m(
        static_cast<std::size_t>(left),
        ' '
    );

    std::cout
        << m
        << "┌──────────────────────────────┐\n";

    std::cout
        << m
        << "│         CTRL + EAT           │\n";

    std::cout
        << m
        << "│       RESTAURANT & BAR       │\n";

    std::cout
        << m
        << "│                              │\n";

    std::cout
        << m
        << "│          CUSTOMER BILL       │\n";

    std::cout
        << m
        << "│                              │\n";

    std::ostringstream info1;

    info1 << "  Table: "
          << table.get_table_number()
          << "        Guests: "
          << table.get_clients_count();

    std::cout
        << m
        << "│"
        << pad(info1.str(), 30)
        << "│\n";

    std::string zone_text =
        "  Zone: " +
        zone.get_name();

    std::cout
        << m
        << "│"
        << pad(zone_text, 30)
        << "│\n";

    std::cout
        << m
        << "│                              │\n";

    std::cout
        << m
        << "│------------------------------│\n";

    std::cout
        << m
        << "│                              │\n";

    if (order == nullptr ||
        order->get_items().empty())
    {
        std::cout
            << m
            << "│        ORDER IS EMPTY        │\n";
    }
    else
    {
        for (const OrderItem& item :
             order->get_items())
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

            std::cout
                << m
                << "│"
                << pad(row.str(), 30)
                << "│\n";
        }
    }

    std::cout
        << m
        << "│                              │\n";

    std::cout
        << m
        << "│------------------------------│\n";

    std::cout
        << m
        << "│                              │\n";

    if (order != nullptr)
    {
        std::ostringstream total_row;

        total_row
            << "  TOTAL"
            << std::right
            << std::setw(19)
            << order->get_total();

        std::cout
            << m
            << "│"
            << pad(total_row.str(), 30)
            << "│\n";

        std::cout
            << m
            << "│                              │\n";

        std::string total =
            "TOTAL: " +
            std::to_string(order->get_total()) +
            " AMD";

        std::cout
            << m
            << "│"
            << center(total, 30)
            << "│\n";
    }

    std::cout
        << m
        << "│                              │\n";

    std::cout
        << m
        << "│------------------------------│\n";

    std::cout
        << m
        << "│                              │\n";

    std::cout
        << m
        << "│        THANK YOU!            │\n";

    std::cout
        << m
        << "│       PLEASE VISIT AGAIN     │\n";

    std::cout
        << m
        << "│                              │\n";

    std::cout
        << m
        << "└──────────────────────────────┘\n";
}

// ============================================================
// PAYMENT
// ============================================================

bool process_payment(
    const Restaurant& restaurant,
    Order& order
)
{
    if (order.is_paid())
    {
        message_screen(
            restaurant,
            "ORDER IS ALREADY PAID"
        );

        return false;
    }

    if (order.get_total() <= 0)
    {
        message_screen(
            restaurant,
            "ORDER IS EMPTY"
        );

        return false;
    }

    clear_screen();

    app_header(
        restaurant,
        "PAYMENT"
    );

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

    if (!read_int(
        "Select payment method: ",
        choice
    ))
    {
        message_screen(
            restaurant,
            "INVALID INPUT"
        );

        return false;
    }

    if (choice == 0)
        return false;

    // --------------------------------------------------------
    // CASH
    // --------------------------------------------------------

    if (choice == 1)
    {
        clear_screen();

        app_header(
            restaurant,
            "CASH PAYMENT"
        );

        std::cout << '\n';

        const int CASH_WIDTH = 46;
        const int CASH_INNER = CASH_WIDTH - 2;

        int left =
            (terminal_width() - CASH_WIDTH) / 2;

        if (left < 0)
            left = 0;

        std::string m(
            static_cast<std::size_t>(left),
            ' '
        );

        auto cash_raw =
            [&](const std::string& text)
        {
            std::cout
                << m
                << text
                << '\n';
        };

        auto cash_line =
            [&](const std::string& text = "")
        {
            cash_raw(
                "│" +
                pad(text, CASH_INNER) +
                "│"
            );
        };

        cash_raw(
            "┌─ CASH REGISTER " +
            repeat(
                "─",
                CASH_INNER - 15
            ) +
            "┐"
        );

        cash_line();

        std::ostringstream total_row;

        total_row
            << "   "
            << std::left
            << std::setw(18)
            << "TOTAL"
            << std::right
            << std::setw(18)
            << std::to_string(
                order.get_total()
            ) +
            " AMD";

        cash_line(total_row.str());

        cash_line();

        cash_raw(
            "└" +
            repeat("─", CASH_INNER) +
            "┘"
        );

        int amount = 0;

        std::cout << '\n';

        if (!read_int(
            "Received amount: ",
            amount
        ))
        {
            message_screen(
                restaurant,
                "INVALID AMOUNT"
            );

            return false;
        }

        if (amount < order.get_total())
        {
            message_screen(
                restaurant,
                "MISSING " +
                std::to_string(
                    order.get_total() -
                    amount
                ) +
                " AMD"
            );

            return false;
        }

        int change =
            amount -
            order.get_total();

        if (!order.pay_for_order(PaymentMethod::Cash))
            return false;

        clear_screen();

        app_header(
            restaurant,
            "CASH PAYMENT"
        );

        std::cout << '\n';

        cash_raw(
            "┌─ CASH REGISTER " +
            repeat(
                "─",
                CASH_INNER - 15
            ) +
            "┐"
        );

        cash_line();

        std::ostringstream total_done;

        total_done
            << "   "
            << std::left
            << std::setw(18)
            << "TOTAL"
            << std::right
            << std::setw(18)
            << std::to_string(
                order.get_total()
            ) +
            " AMD";

        cash_line(total_done.str());

        std::ostringstream received_done;

        received_done
            << "   "
            << std::left
            << std::setw(18)
            << "RECEIVED"
            << std::right
            << std::setw(18)
            << std::to_string(amount) +
            " AMD";

        cash_line(received_done.str());

        std::ostringstream change_done;

        change_done
            << "   "
            << std::left
            << std::setw(18)
            << "CHANGE"
            << std::right
            << std::setw(18)
            << std::to_string(change) +
            " AMD";

        cash_line(change_done.str());

        cash_line();

        cash_raw(
            "├" +
            repeat("─", CASH_INNER) +
            "┤"
        );

        cash_raw(
            "│" +
            center(
                "PAYMENT COMPLETE",
                CASH_INNER
            ) +
            "│"
        );

        cash_raw(
            "└" +
            repeat("─", CASH_INNER) +
            "┘"
        );

        wait_enter();

        return true;
    }

    // --------------------------------------------------------
    // CARD
    // --------------------------------------------------------

    if (choice == 2)
    {
        clear_screen();

        app_header(
            restaurant,
            "CARD PAYMENT"
        );

        std::cout << '\n';

        const int TERM_WIDTH = 40;
        const int TERM_INNER = TERM_WIDTH - 2;

        int left =
            (terminal_width() - TERM_WIDTH) / 2;

        if (left < 0)
            left = 0;

        std::string m(
            static_cast<std::size_t>(left),
            ' '
        );

        auto term_raw =
            [&](const std::string& text)
        {
            std::cout
                << m
                << text
                << '\n';
        };

        auto term_line =
            [&](const std::string& text = "")
        {
            term_raw(
                "│" +
                pad(text, TERM_INNER) +
                "│"
            );
        };

        term_raw(
            "┌" +
            repeat("─", TERM_INNER) +
            "┐"
        );

        term_line();

        term_raw(
            "│" +
            center(
                restaurant.get_name(),
                TERM_INNER
            ) +
            "│"
        );

        term_raw(
            "│" +
            center(
                "PAYMENT TERMINAL",
                TERM_INNER
            ) +
            "│"
        );

        term_line();

        term_raw(
            "│" +
            center(
                "AMOUNT",
                TERM_INNER
            ) +
            "│"
        );

        term_raw(
            "│" +
            center(
                std::to_string(
                    order.get_total()
                ) +
                " AMD",
                TERM_INNER
            ) +
            "│"
        );

        term_line();

        term_raw(
            "│    ┌──────────────────────────┐    │"
        );

        term_raw(
            "│    │                          │    │"
        );

        term_raw(
            "│    │      TAP / INSERT       │    │"
        );

        term_raw(
            "│    │          CARD            │    │"
        );

        term_raw(
            "│    │                          │    │"
        );

        term_raw(
            "│    └──────────────────────────┘    │"
        );

        term_line();

        term_raw(
            "│          [1] [2] [3]               │"
        );

        term_raw(
            "│          [4] [5] [6]               │"
        );

        term_raw(
            "│          [7] [8] [9]               │"
        );

        term_raw(
            "│          [C] [0] [OK]              │"
        );

        term_line();

        term_raw(
            "└" +
            repeat("─", TERM_INNER) +
            "┘"
        );

        std::string pin;

        std::cout << '\n';

        prompt("PIN: ");

        std::cin >> pin;

        if (!std::cin)
        {
            clear_input();

            message_screen(
                restaurant,
                "INVALID PIN"
            );

            return false;
        }

        if (!order.pay_for_order(PaymentMethod::Cash))
            return false;

        std::cout << '\a' << std::flush;

        std::system(
            "paplay "
            "/usr/share/sounds/"
            "freedesktop/stereo/"
            "complete.oga "
            ">/dev/null 2>&1 &"
        );

        clear_screen();

        app_header(
            restaurant,
            "CARD PAYMENT"
        );

        std::cout << '\n';

        term_raw(
            "┌" +
            repeat("─", TERM_INNER) +
            "┐"
        );

        term_line();

        term_raw(
            "│" +
            center(
                restaurant.get_name(),
                TERM_INNER
            ) +
            "│"
        );

        term_raw(
            "│" +
            center(
                "PAYMENT TERMINAL",
                TERM_INNER
            ) +
            "│"
        );

        term_line();

        term_raw(
            "│" +
            center(
                "PAYMENT APPROVED",
                TERM_INNER
            ) +
            "│"
        );

        term_line();

        term_raw(
            "│" +
            center(
                std::to_string(
                    order.get_total()
                ) +
                " AMD",
                TERM_INNER
            ) +
            "│"
        );

        term_line();

        term_raw(
            "└" +
            repeat("─", TERM_INNER) +
            "┘"
        );

        wait_enter();

        return true;
    }

    message_screen(
        restaurant,
        "INVALID PAYMENT METHOD"
    );

    return false;
}

// ============================================================
// SELECT TABLE
// ============================================================

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

        if (!read_int(
            "Table number: ",
            table_number
        ))
        {
            message_screen(
                restaurant,
                "INVALID TABLE NUMBER"
            );

            continue;
        }

        if (table_number == 0)
            return false;

        Table* table =
            restaurant.get_table_by_number(
                table_number
            );

        if (table == nullptr)
        {
            message_screen(
                restaurant,
                "TABLE NOT FOUND"
            );

            continue;
        }

        Zone* zone =
            restaurant.get_zone_by_table_number(
                table_number
            );

        if (zone == nullptr)
        {
            message_screen(
                restaurant,
                "ZONE NOT FOUND"
            );

            continue;
        }

        if (table->get_status() ==
            TableStatus::Free)
        {
            clear_screen();

            app_header(
                restaurant,
                "OPEN TABLE"
            );

            std::cout << '\n';

            section_top(
                "TABLE INFORMATION"
            );

            box_line();

            box_line(
                "   Table: " +
                std::to_string(
                    table->get_table_number()
                )
            );

            box_line(
                "   Zone: " +
                zone->get_name()
            );

            box_line(
                "   Chairs: " +
                std::to_string(
                    table->get_chairs_count()
                )
            );

            box_line();

            thin_bottom();

            int guests = 0;

            std::cout << '\n';

            if (!read_int(
                "Guests: ",
                guests
            ))
            {
                message_screen(
                    restaurant,
                    "INVALID NUMBER OF GUESTS"
                );

                continue;
            }

            if (guests <= 0)
            {
                message_screen(
                    restaurant,
                    "INVALID NUMBER OF GUESTS"
                );

                continue;
            }

            if (guests >
                table->get_chairs_count())
            {
                message_screen(
                    restaurant,
                    "NOT ENOUGH CHAIRS"
                );

                continue;
            }

            if (!table->open_table(guests))
            {
                message_screen(
                    restaurant,
                    "CANNOT OPEN TABLE"
                );

                continue;
            }

            if (table->get_order() == nullptr)
            {
                message_screen(
                    restaurant,
                    "CANNOT CREATE ORDER"
                );

                continue;
            }
        }

        current_table = table;
        current_zone = zone;

        return true;
    }
}

// ============================================================
// ADD ITEMS TO ORDER
// ============================================================

void add_items_screen(
    const Restaurant& restaurant,
    Order& order
)
{
    const Menu& menu =
        restaurant.get_menu();

    while (true)
    {
        show_menu_screen(restaurant);

        std::cout << '\n';

        raw("  Enter item ID");
        raw("  [0] Finish order");

        int item_id = 0;

        std::cout << '\n';

        if (!read_int(
            "Item ID: ",
            item_id
        ))
        {
            message_screen(
                restaurant,
                "INVALID ITEM ID"
            );

            continue;
        }

        if (item_id == 0)
            return;

        const MenuItem* item =
            menu.get_item_by_id(item_id);

        if (item == nullptr)
        {
            message_screen(
                restaurant,
                "ITEM NOT FOUND"
            );

            continue;
        }

        int quantity = 0;

        if (!read_int(
            "Quantity: ",
            quantity
        ))
        {
            message_screen(
                restaurant,
                "INVALID QUANTITY"
            );

            continue;
        }

        if (quantity <= 0)
        {
            message_screen(
                restaurant,
                "INVALID QUANTITY"
            );

            continue;
        }

        if (!order.add_item(
            OrderItem(*item, quantity)
        ))
        {
            message_screen(
                restaurant,
                "CANNOT ADD ITEM"
            );
        }
    }
}

// ============================================================
// ADMIN - ZONES
// ============================================================

void admin_add_zone(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "ADD ZONE"
    );

    std::cout << '\n';

    std::string name;

    prompt("Zone name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(std::cin, name);

    if (name.empty())
    {
        message_screen(
            restaurant,
            "ZONE NAME CANNOT BE EMPTY"
        );

        return;
    }

    Zone zone(name);

    if (!restaurant.add_zone(zone))
    {
        message_screen(
            restaurant,
            "CANNOT ADD ZONE"
        );

        return;
    }

    message_screen(
        restaurant,
        "ZONE ADDED SUCCESSFULLY"
    );
}

void admin_remove_zone(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "REMOVE ZONE"
    );

    std::cout << '\n';

    const auto& zones =
        restaurant.get_zones();

    if (zones.empty())
    {
        message_screen(
            restaurant,
            "NO ZONES AVAILABLE"
        );

        return;
    }

    section_top("ZONES");

    box_line();

    int index = 1;

    for (const Zone& zone : zones)
    {
        std::ostringstream row;

        row << "   ["
            << index
            << "] "
            << zone.get_name()
            << "    Tables: "
            << zone.get_tables().size();

        box_line(row.str());

        ++index;
    }

    box_line();

    thin_bottom();

    std::cout << '\n';

    std::string name;

    prompt("Zone name to remove: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(std::cin, name);

    if (!restaurant.remove_zone(name))
    {
        message_screen(
            restaurant,
            "CANNOT REMOVE ZONE"
        );

        return;
    }

    message_screen(
        restaurant,
        "ZONE REMOVED SUCCESSFULLY"
    );
}

void admin_zones_menu(
    Restaurant& restaurant
)
{
    while (true)
    {
        clear_screen();

        app_header(
            restaurant,
            "ZONE MANAGEMENT"
        );

        std::cout << '\n';

        section_top("ZONES");

        box_line();

        box_line("   [1] Add zone");
        box_line("   [2] Remove zone");

        box_line();

        box_line("   [0] Back");

        box_line();

        thin_bottom();

        std::cout << '\n';

        int choice = 0;

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            case 1:
                admin_add_zone(restaurant);
                break;

            case 2:
                admin_remove_zone(restaurant);
                break;

            case 0:
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );
                break;
        }
    }
}

// ============================================================
// ADMIN - TABLES
// ============================================================

void admin_add_table(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "ADD TABLE"
    );

    std::cout << '\n';

    if (restaurant.get_zones().empty())
    {
        message_screen(
            restaurant,
            "ADD A ZONE FIRST"
        );

        return;
    }

    std::cout << "Available zones:\n\n";

    for (const Zone& zone :
         restaurant.get_zones())
    {
        raw(
            "  - " +
            zone.get_name()
        );
    }

    std::cout << '\n';

    std::string zone_name;

    prompt("Zone name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        zone_name
    );

    if (zone_name.empty())
    {
        message_screen(
            restaurant,
            "ZONE NAME CANNOT BE EMPTY"
        );

        return;
    }

    int table_number = 0;
    int chairs = 0;

    if (!read_int(
        "Table number: ",
        table_number
    ))
    {
        message_screen(
            restaurant,
            "INVALID TABLE NUMBER"
        );

        return;
    }

    if (!read_int(
        "Chairs count: ",
        chairs
    ))
    {
        message_screen(
            restaurant,
            "INVALID CHAIRS COUNT"
        );

        return;
    }

    Table table(
        table_number,
        chairs
    );

    if (!restaurant.add_table_to_zone(
        zone_name,
        table
    ))
    {
        message_screen(
            restaurant,
            "CANNOT ADD TABLE"
        );

        return;
    }

    message_screen(
        restaurant,
        "TABLE ADDED SUCCESSFULLY"
    );
}

void admin_remove_table(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "REMOVE TABLE"
    );

    std::cout << '\n';

    int table_number = 0;

    if (!read_int(
        "Table number to remove: ",
        table_number
    ))
    {
        message_screen(
            restaurant,
            "INVALID TABLE NUMBER"
        );

        return;
    }

    Table* table =
        restaurant.get_table_by_number(
            table_number
        );

    if (table == nullptr)
    {
        message_screen(
            restaurant,
            "TABLE NOT FOUND"
        );

        return;
    }

    if (table->get_status() !=
        TableStatus::Free)
    {
        message_screen(
            restaurant,
            "CANNOT REMOVE OCCUPIED TABLE"
        );

        return;
    }

    if (!restaurant.remove_table(
        table_number
    ))
    {
        message_screen(
            restaurant,
            "CANNOT REMOVE TABLE"
        );

        return;
    }

    message_screen(
        restaurant,
        "TABLE REMOVED SUCCESSFULLY"
    );
}

void admin_tables_menu(
    Restaurant& restaurant
)
{
    while (true)
    {
        clear_screen();

        app_header(
            restaurant,
            "TABLE MANAGEMENT"
        );

        std::cout << '\n';

        section_top("TABLES");

        box_line();

        box_line("   [1] Add table");
        box_line("   [2] Remove table");

        box_line();

        box_line("   [0] Back");

        box_line();

        thin_bottom();

        std::cout << '\n';

        int choice = 0;

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            case 1:
                admin_add_table(restaurant);
                break;

            case 2:
                admin_remove_table(restaurant);
                break;

            case 0:
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );
                break;
        }
    }
}

// ============================================================
// ADMIN - CATEGORIES
// ============================================================

void admin_add_category(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "ADD CATEGORY"
    );

    std::cout << '\n';

    std::string name;

    prompt("Category name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        name
    );

    if (name.empty())
    {
        message_screen(
            restaurant,
            "CATEGORY NAME CANNOT BE EMPTY"
        );

        return;
    }

    Menu& menu =
        restaurant.get_menu();

    if (!menu.add_category(
        MenuCategory(name)
    ))
    {
        message_screen(
            restaurant,
            "CANNOT ADD CATEGORY"
        );

        return;
    }

    message_screen(
        restaurant,
        "CATEGORY ADDED SUCCESSFULLY"
    );
}

void admin_remove_category(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "REMOVE CATEGORY"
    );

    std::cout << '\n';

    const Menu& menu =
        restaurant.get_menu();

    if (menu.get_categories().empty())
    {
        message_screen(
            restaurant,
            "NO CATEGORIES AVAILABLE"
        );

        return;
    }

    section_top("CATEGORIES");

    box_line();

    for (const MenuCategory& category :
         menu.get_categories())
    {
        std::ostringstream row;

        row << "   "
            << category.get_name()
            << "    Items: "
            << category.get_items().size();

        box_line(row.str());
    }

    box_line();

    thin_bottom();

    std::cout << '\n';

    std::string name;

    prompt("Category name to remove: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        name
    );

    if (!menu.get_category_by_name(name))
    {
        message_screen(
            restaurant,
            "CATEGORY NOT FOUND"
        );

        return;
    }

    /*
     * Backend currently allows removing
     * a category even if it contains items.
     * We allow it here according to the current
     * Menu API.
     */

    if (!restaurant.get_menu().remove_category(name))
    {
        message_screen(
            restaurant,
            "CANNOT REMOVE CATEGORY"
        );

        return;
    }

    message_screen(
        restaurant,
        "CATEGORY REMOVED SUCCESSFULLY"
    );
}

void admin_rename_category(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "RENAME CATEGORY"
    );

    std::cout << '\n';

    std::string old_name;
    std::string new_name;

    prompt("Current category name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        old_name
    );

    prompt("New category name: ");

    std::getline(
        std::cin,
        new_name
    );

    Menu& menu =
        restaurant.get_menu();

    if (!menu.rename_category(
        old_name,
        new_name
    ))
    {
        message_screen(
            restaurant,
            "CANNOT RENAME CATEGORY"
        );

        return;
    }

    message_screen(
        restaurant,
        "CATEGORY RENAMED SUCCESSFULLY"
    );
}

void admin_categories_menu(
    Restaurant& restaurant
)
{
    while (true)
    {
        clear_screen();

        app_header(
            restaurant,
            "CATEGORY MANAGEMENT"
        );

        std::cout << '\n';

        section_top("MENU CATEGORIES");

        box_line();

        box_line("   [1] Add category");
        box_line("   [2] Remove category");
        box_line("   [3] Rename category");

        box_line();

        box_line("   [0] Back");

        box_line();

        thin_bottom();

        std::cout << '\n';

        int choice = 0;

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            case 1:
                admin_add_category(
                    restaurant
                );
                break;

            case 2:
                admin_remove_category(
                    restaurant
                );
                break;

            case 3:
                admin_rename_category(
                    restaurant
                );
                break;

            case 0:
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );
                break;
        }
    }
}

// ============================================================
// ADMIN - MENU ITEMS
// ============================================================

void admin_add_item(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "ADD MENU ITEM"
    );

    std::cout << '\n';

    Menu& menu =
        restaurant.get_menu();

    if (menu.get_categories().empty())
    {
        message_screen(
            restaurant,
            "ADD A CATEGORY FIRST"
        );

        return;
    }

    std::cout << "Available categories:\n\n";

    for (const MenuCategory& category :
         menu.get_categories())
    {
        raw(
            "  - " +
            category.get_name()
        );
    }

    std::cout << '\n';

    std::string category_name;

    prompt("Category name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        category_name
    );

    if (category_name.empty())
    {
        message_screen(
            restaurant,
            "CATEGORY NAME CANNOT BE EMPTY"
        );

        return;
    }

    int id = 0;
    int price = 0;

    if (!read_int(
        "Item ID: ",
        id
    ))
    {
        message_screen(
            restaurant,
            "INVALID ITEM ID"
        );

        return;
    }

    std::string name;

    prompt("Item name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        name
    );

    if (name.empty())
    {
        message_screen(
            restaurant,
            "ITEM NAME CANNOT BE EMPTY"
        );

        return;
    }

    if (!read_int(
        "Price: ",
        price
    ))
    {
        message_screen(
            restaurant,
            "INVALID PRICE"
        );

        return;
    }

    MenuItem item(
        id,
        name,
        price
    );

    if (!menu.add_item_to_category(
        category_name,
        item
    ))
    {
        message_screen(
            restaurant,
            "CANNOT ADD MENU ITEM"
        );

        return;
    }

    message_screen(
        restaurant,
        "MENU ITEM ADDED SUCCESSFULLY"
    );
}

void admin_remove_item(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "REMOVE MENU ITEM"
    );

    std::cout << '\n';

    int id = 0;

    if (!read_int(
        "Item ID to remove: ",
        id
    ))
    {
        message_screen(
            restaurant,
            "INVALID ITEM ID"
        );

        return;
    }

    Menu& menu =
        restaurant.get_menu();

    if (menu.get_item_by_id(id) == nullptr)
    {
        message_screen(
            restaurant,
            "ITEM NOT FOUND"
        );

        return;
    }

    if (!menu.remove_item(id))
    {
        message_screen(
            restaurant,
            "CANNOT REMOVE ITEM"
        );

        return;
    }

    message_screen(
        restaurant,
        "ITEM REMOVED SUCCESSFULLY"
    );
}

void admin_rename_item(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "RENAME MENU ITEM"
    );

    std::cout << '\n';

    int id = 0;

    if (!read_int(
        "Item ID: ",
        id
    ))
    {
        message_screen(
            restaurant,
            "INVALID ITEM ID"
        );

        return;
    }

    Menu& menu =
        restaurant.get_menu();

    if (menu.get_item_by_id(id) == nullptr)
    {
        message_screen(
            restaurant,
            "ITEM NOT FOUND"
        );

        return;
    }

    std::string new_name;

    prompt("New item name: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        new_name
    );

    if (!menu.rename_item(
        id,
        new_name
    ))
    {
        message_screen(
            restaurant,
            "CANNOT RENAME ITEM"
        );

        return;
    }

    message_screen(
        restaurant,
        "ITEM RENAMED SUCCESSFULLY"
    );
}

void admin_change_item_price(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "CHANGE ITEM PRICE"
    );

    std::cout << '\n';

    int id = 0;
    int price = 0;

    if (!read_int(
        "Item ID: ",
        id
    ))
    {
        message_screen(
            restaurant,
            "INVALID ITEM ID"
        );

        return;
    }

    Menu& menu =
        restaurant.get_menu();

    MenuItem* item =
        menu.get_item_by_id(id);

    if (item == nullptr)
    {
        message_screen(
            restaurant,
            "ITEM NOT FOUND"
        );

        return;
    }

    std::cout
        << '\n';

    raw(
        "Current item: " +
        item->get_name()
    );

    raw(
        "Current price: " +
        std::to_string(
            item->get_price()
        ) +
        " AMD"
    );

    std::cout << '\n';

    if (!read_int(
        "New price: ",
        price
    ))
    {
        message_screen(
            restaurant,
            "INVALID PRICE"
        );

        return;
    }

    if (!menu.change_item_price(
        id,
        price
    ))
    {
        message_screen(
            restaurant,
            "CANNOT CHANGE PRICE"
        );

        return;
    }

    message_screen(
        restaurant,
        "PRICE CHANGED SUCCESSFULLY"
    );
}

void admin_items_menu(
    Restaurant& restaurant
)
{
    while (true)
    {
        clear_screen();

        app_header(
            restaurant,
            "MENU ITEM MANAGEMENT"
        );

        std::cout << '\n';

        section_top("MENU ITEMS");

        box_line();

        box_line("   [1] Add item");
        box_line("   [2] Remove item");
        box_line("   [3] Rename item");
        box_line("   [4] Change price");

        box_line();

        box_line("   [0] Back");

        box_line();

        thin_bottom();

        std::cout << '\n';

        int choice = 0;

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            case 1:
                admin_add_item(
                    restaurant
                );
                break;

            case 2:
                admin_remove_item(
                    restaurant
                );
                break;

            case 3:
                admin_rename_item(
                    restaurant
                );
                break;

            case 4:
                admin_change_item_price(
                    restaurant
                );
                break;

            case 0:
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );
                break;
        }
    }
}

// ============================================================
// ADMIN - SAVE / LOAD
// ============================================================

void admin_save_configuration(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "SAVE CONFIGURATION"
    );

    std::cout << '\n';

    std::string filename;

    prompt("Filename [menu.txt]: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        filename
    );

    if (filename.empty())
        filename = "menu.txt";

    const Menu& menu =
        restaurant.get_menu();

    if (!menu.save_to_file(filename))
    {
        message_screen(
            restaurant,
            "FAILED TO SAVE CONFIGURATION"
        );

        return;
    }

    message_screen(
        restaurant,
        "CONFIGURATION SAVED"
    );
}

void admin_load_configuration(
    Restaurant& restaurant
)
{
    clear_screen();

    app_header(
        restaurant,
        "LOAD CONFIGURATION"
    );

    std::cout << '\n';

    std::string filename;

    prompt("Filename [menu.txt]: ");

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::getline(
        std::cin,
        filename
    );

    if (filename.empty())
        filename = "menu.txt";

    Menu& menu =
        restaurant.get_menu();

    if (!menu.load_from_file(filename))
    {
        message_screen(
            restaurant,
            "FAILED TO LOAD CONFIGURATION"
        );

        return;
    }

    message_screen(
        restaurant,
        "CONFIGURATION LOADED"
    );
}

// ============================================================
// ADMIN MENU
// ============================================================

} // namespace

void run_interface(Restaurant& restaurant)
{
    while (true)
    {
        show_main_screen(restaurant);

        int choice = 0;

        std::cout << '\n';

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            case 1:
                run_admin_menu(restaurant);
                break;

            case 2:
                run_client_menu(restaurant);
                break;

            case 0:
                clear_screen();
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );
                break;
        }
    }
}

void run_admin_menu(Restaurant& restaurant)
{
    while (true)
    {
        show_admin_screen(restaurant);

        int choice = 0;

        std::cout << '\n';

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            case 1:
                admin_zones_menu(
                    restaurant
                );
                break;

            case 2:
                admin_tables_menu(
                    restaurant
                );
                break;

            case 3:
                admin_categories_menu(
                    restaurant
                );
                break;

            case 4:
                admin_items_menu(
                    restaurant
                );
                break;

            case 5:
                admin_save_configuration(
                    restaurant
                );
                break;

            case 6:
                admin_load_configuration(
                    restaurant
                );
                break;

            case 0:
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );
                break;
        }
    }
}

// ============================================================
// CLIENT POS
// ============================================================

void run_client_menu(Restaurant& restaurant)
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

        if (!read_int(
            "Select option: ",
            choice
        ))
        {
            message_screen(
                restaurant,
                "INVALID OPTION"
            );

            continue;
        }

        switch (choice)
        {
            // ------------------------------------------------
            // ADD ITEM
            // ------------------------------------------------

            case 1:
            {
                Order* order =
                    current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(
                        restaurant,
                        "NO ACTIVE ORDER"
                    );

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
                    *order
                );

                break;
            }

            // ------------------------------------------------
            // REMOVE ITEM
            // ------------------------------------------------

            case 2:
            {
                Order* order =
                    current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(
                        restaurant,
                        "NO ACTIVE ORDER"
                    );

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

                if (!read_int(
                    "Item ID to remove: ",
                    id
                ))
                {
                    message_screen(
                        restaurant,
                        "INVALID ITEM ID"
                    );

                    break;
                }

                if (!order->remove_item(id))
                {
                    message_screen(
                        restaurant,
                        "ITEM NOT FOUND IN ORDER"
                    );
                }

                break;
            }

            // ------------------------------------------------
            // CHANGE QUANTITY
            // ------------------------------------------------

            case 3:
            {
                Order* order =
                    current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(
                        restaurant,
                        "NO ACTIVE ORDER"
                    );

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

                if (!read_int(
                    "Item ID: ",
                    id
                ))
                {
                    message_screen(
                        restaurant,
                        "INVALID ITEM ID"
                    );

                    break;
                }

                if (!read_int(
                    "New quantity: ",
                    quantity
                ))
                {
                    message_screen(
                        restaurant,
                        "INVALID QUANTITY"
                    );

                    break;
                }

                if (!order->change_item_quantity(
                    id,
                    quantity
                ))
                {
                    message_screen(
                        restaurant,
                        "CANNOT CHANGE QUANTITY"
                    );
                }

                break;
            }

            // ------------------------------------------------
            // CHANGE TABLE
            // ------------------------------------------------

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

            // ------------------------------------------------
            // VIEW MENU
            // ------------------------------------------------

            case 5:
            {
                browse_menu(restaurant);
                break;
            }

            // ------------------------------------------------
            // SHOW BILL
            // ------------------------------------------------

            case 6:
            {
                show_bill(
                    restaurant,
                    *current_table,
                    *current_zone
                );

                wait_enter();

                break;
            }

            // ------------------------------------------------
            // PAYMENT
            // ------------------------------------------------

            case 7:
            {
                Order* order =
                    current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(
                        restaurant,
                        "NO ACTIVE ORDER"
                    );

                    break;
                }

                process_payment(
                    restaurant,
                    *order
                );

                break;
            }

            // ------------------------------------------------
            // CLOSE TABLE
            // ------------------------------------------------

            case 8:
            {
                Order* order =
                    current_table->get_order();

                if (order == nullptr)
                {
                    message_screen(
                        restaurant,
                        "NO ACTIVE ORDER"
                    );

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

                if (!current_table->close_table())
                {
                    message_screen(
                        restaurant,
                        "CANNOT CLOSE TABLE"
                    );

                    break;
                }

                current_table = nullptr;
                current_zone = nullptr;

                break;
            }

            // ------------------------------------------------
            // EXIT CLIENT
            // ------------------------------------------------

            case 0:
                return;

            default:
                message_screen(
                    restaurant,
                    "INVALID OPTION"
                );

                break;
        }
    }
}