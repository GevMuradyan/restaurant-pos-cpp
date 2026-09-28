#ifndef CARDTERMINAL_HPP
#define CARDTERMINAL_HPP

enum class CardTerminalStatus
{
    Idle,
    Processing,
    Approved,
    Declined,
    ConnectionError
};

class CardTerminal
{
private:
    CardTerminalStatus status =
        CardTerminalStatus::Idle;

public:
    bool start_payment();

    bool process_payment();

    bool approve();
    bool decline();
    bool connection_error();

    CardTerminalStatus get_status() const;
};

#endif