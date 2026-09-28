#include "Transaction.hpp"

#include <iomanip>
#include <sstream>

std::string Transaction::generate_id()
{
    static int counter = 0;

    ++counter;

    std::ostringstream stream;

    stream
        << "TX-"
        << std::setw(6)
        << std::setfill('0')
        << counter;

    return stream.str();
}