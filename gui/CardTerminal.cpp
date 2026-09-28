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

    std::uniform_int_distribution<int> distribution(
        1,
        100
    );

    const int result =
        distribution(generator);

    // 70% — Approved
    if (result <= 70)
        return approve();

    // 20% — Declined
    if (result <= 90)
        return decline();

    // 10% — Connection Error
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

void CardTerminal::reset()
{
    status = CardTerminalStatus::Idle;
}

CardTerminalStatus CardTerminal::get_status() const
{
    return status;
}