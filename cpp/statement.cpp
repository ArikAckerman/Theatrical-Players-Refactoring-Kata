#include "statement.h"

#include <sstream>
#include <iomanip>

std::string statement(const nlohmann::json& invoice, const nlohmann::json& plays)
{
    float total_amount = 0;
    int volume_credits = 0;

    std::stringstream result;
    result << "Statement for " << invoice["customer"].get<std::string>() << '\n';

    for (const auto& perf : invoice["performances"])
    {
        float this_amount = 0;
        const auto& play = plays[perf["playID"].get<std::string>()];
        if (play["type"] == "tragedy")
        {
            this_amount = 40000;
            if (perf["audience"].get<int>() > 30)
            {
                this_amount += 1000 * (perf["audience"].get<int>() - 30);
            }
        }
        else if (play["type"] == "comedy")
        {
            this_amount = 30000;
            if (perf["audience"].get<int>() > 20)
            {
                this_amount += 10000 + 500 * (perf["audience"].get<int>() - 20);
            }
            this_amount += 300 * perf["audience"].get<int>();
        }
        else
        {
            throw std::domain_error("Unknown play type: " + play["type"].get<std::string>());
        }

        // Calculate volume credits
        volume_credits += std::max(perf["audience"].get<int>() - 30, 0);
        if (play["type"] == "comedy")
        {
            volume_credits += perf["audience"].get<int>() / 5;
        }

        // Print line for this order
        result << " " << play["name"].get<std::string>() << ": $" << std::fixed << std::setprecision(2)
               << (this_amount / 100) << " (" << perf["audience"].get<int>() << " seats)\n";

        total_amount += this_amount;
    }

    result << "Amount owed is $" << std::fixed << std::setprecision(2) << (total_amount / 100.0f) << "\n";
    result << "You earned " << volume_credits << " credits";

    return result.str();
}
