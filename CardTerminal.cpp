#include "CardTerminal.hpp"

#include <random>

bool CardTerminal::start_payment()
{
    if (status != CardTerminalStatus::Idle)
        return false;

    status = CardTerminalStatus::Processing;

    return true;
}

bool CardTerminal::process_payment()
{
    if (status != CardTerminalStatus::Processing)
        return false;

    std::random_device device;
    std::mt19937 generator(device());

    std::uniform_int_distribution<int> distribution(1, 100);

    const int result = distribution(generator);

    if (result <= 70)
    {
        return approve();
    }

    if (result <= 90)
    {
        return decline();
    }

    return connection_error();
}

bool CardTerminal::approve()
{
    if (status != CardTerminalStatus::Processing)
        return false;

    status = CardTerminalStatus::Approved;

    return true;
}

bool CardTerminal::decline()
{
    if (status != CardTerminalStatus::Processing)
        return false;

    status = CardTerminalStatus::Declined;

    return true;
}

bool CardTerminal::connection_error()
{
    if (status != CardTerminalStatus::Processing)
        return false;

    status = CardTerminalStatus::ConnectionError;

    return true;
}

CardTerminalStatus CardTerminal::get_status() const
{
    return status;
}