#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "headers/stock.h"
#include "headers/market.h"
#include "headers/holding.h"
#include "headers/portfolio.h"
#include "headers/get_file_names.h"
#include "headers/read_data.h"
#include "headers/knapsack_solve.h"

#include "../helper_code/headers/print_help.h" // useful for debugging


int main() {
    // Set initial parameters
    const int days = 252; // equal to the # rows in each csv doc. Currently 31.01.2019 - 31.01.2020 inclusive.
    const int initial_cash = 2000; // Much more than this and the program will crash

    // Initialise
    Market market(readData(days));
    Portfolio portfolio(initial_cash);

    // Let one day go by
    int day = 1;
    market.update(day);

    day++;

    // Loop over all days, up until the last one (needs special treatment).
    while (day < days - 2) {
        // Update price to current day's, as well as MAs and totals.
        market.update(day);

        // Analyse all stocks in the market.
        market.predict(day);

        // Sell all current holdings, storing them in past holdings.
        portfolio.sellAll();

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
        int copies [market.size] = {0};

        for (int i = 0; i < market.size; i++) {
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

        for (int i = 0; i < market.size; i++) {
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

    std::cout << "After the final day cash is $" << portfolio.cash << ", " 
              << (portfolio.cash - initial_cash)/initial_cash << "% gain." << std::endl;
}