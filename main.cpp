#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "stock.h"
#include "market.h"
#include "holding.h"
#include "portfolio.h"
#include "get_file_names.h"
#include "read_data.h"

#include "knapsack_solve.h"
#include "print_help.h"


// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 252; // equal to the # rows in each csv doc. Currently 31.01.2019 - 31.01.2020 inclusive.
const int market_size = 5; // TODO: calculate this by counting files

int main() {
    // Initialise
    Market market(market_size, readData());
    Portfolio portfolio(200);

    // Let one day go by
    int day = 1;
    market.update(day);

    day++;

    // Loop over all days, up until the last one (needs special treatment).
    while (day < days - 2) {
        // Update price to current day's, as well as MAs and totals.
        market.update(day);

        // Analyse all stocks in the market.
        market.cheat_predict(day);

        // Sell all current holdings, storing them in past holdings.
        portfolio.sellAll();

        // TODO: could optimise this using the optimising algorithm (divide capacity by a constant)
        // To find the optimal choice of stocks, we implement a solution for the knapsack 
        // problem. But first we need to determine and initialise the weights, values, and 
        // max weight as follows... 

        // Note - this is simplified hugely by considering no transaction fees, so that 
        // current portfolio holdings can be sold each new day without loss. Otherwise
        // would need to model them as items with negative weight and adjust the algo.

        // Weight capacity:
        // The sum of all cash and current holdings. Round down.
        // Note all holdings are sold already, so we don't need to iterate over them.
        int capacity = (int) std::floor(portfolio.cash);

        // Number of items: enough copies of each stock so that max capacity is less than
        // quantity * curr_price of the stock.
        // copies: track the number of copies of each stock we store. 
        int num_items = 0;
        int copies [market_size] = {0};

        for (int i = 0; i < market_size; i++) {
            Stock *s_p = &(market.stocks.at(i));

            int s_id = s_p -> id;
            copies[s_id] = (int) (std::ceil(capacity/(s_p -> curr_price)));

            num_items += copies[s_id];
        }
        
        // Weight: the stock price. 
        // Values: the predicted gain tomorrow.
        // Both values need to be rounded up to integer values.
        // ids: the i-th element here gives the id of the stock represented in values/weights[i]
        int weights [num_items];
        int values [num_items];
        int ids [num_items];
        int counter = 0;

        for (int i = 0; i < market_size; i++) {
            Stock *s_p = &(market.stocks.at(i));
            
            int price = (int) std::ceil(s_p -> curr_price);
            int tmr_price_prediction = (int) std::floor(s_p -> tmr_price_est);
            int predicted_gain = tmr_price_prediction - price;

            int s_id = s_p -> id;
            for (int n = 0; n < copies[s_id]; n++) {
                weights[counter] = price;
                values[counter] = predicted_gain;
                ids[counter] = s_id;

                counter++;
            }
        }

        std::vector<int> item_nums = knapsack_solve(capacity, num_items, weights, values);
        
        // Buy all the recommended stocks
        for (int item_num : item_nums) {
            int s_id = ids[item_num];

            Stock *s_p = &(market.stocks.at(s_id));
            portfolio.buy(s_p);
        }

        day++;
    }

    //  After the last second last day, update everything and see how the program performed.
    day++;
    market.update(day);
    portfolio.sellAll();

    std::cout << "After final day cash is " << portfolio.cash << std::endl;
}