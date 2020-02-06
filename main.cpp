#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "knapsack.h"
#include "print_help.h"
#include "market.h"
#include "read_market_data.h"

// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 252; // equal to the # rows in each csv doc. Currently 31.01.2019 - 31.01.2020 inclusive.
const int market_size = 5; // TODO: calculate this by counting files

struct holding {
    stock *stock_ptr;
    float buy_price;
    float sell_price;
};

struct portfolio {
    float cash;
    float holdings_value;
    std::vector<holding> curr_holdings; // TODO: should be pointers to this?
    std::vector<holding> past_holdings;
};

void update(int day, market *market, portfolio *portfolio);
void predict(int day, market *market);
void cheat_predict(int day, market *market);


int main() {
    // Initialise
    // Stocks, initial market value, initial empty portfolio, ...
    struct market market;
    readData(&market);
    market.update_total_value();
    
    struct portfolio portfolio;
    portfolio.cash = 300;

    // Let one day go by
    // TODO: roll this into the update function.
    int day = 1;
    update(day, &market, &portfolio);

    day++;

    // Loop over all days, up until the last one (needs special treatment).
    while (day < days - 2) {
        // Update price to current day's, as well as MAs and totals.
        update(day, &market, &portfolio);

        // Analyse all stocks in the market.
        // predict(day, &market);
        cheat_predict(day, &market);

        // Sell all current holdings, storing them in past holdings.
        for (holding h : portfolio.curr_holdings) {
            float value = h.stock_ptr -> curr_price;
            portfolio.cash += value;
            h.sell_price = value;
            portfolio.past_holdings.push_back(h);
        }
        portfolio.curr_holdings = {};

        // TODO: could optimise this using an optimising algorithm
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
            stock *s_p = &(market.stocks[i]);
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
            stock *s_p = &(market.stocks[i]);
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
            stock *s_p = &(market.stocks[s_id]);
            float price = s_p -> curr_price;

            portfolio.cash -= price;
            holding temp;
            temp.stock_ptr = s_p;
            temp.buy_price = price;
            portfolio.curr_holdings.push_back(temp);
        }

        day++;
    }

    //  After the last second last day, update everything and see how the program performed.
    day++;
    update(day, &market, &portfolio);
    for (holding h : portfolio.curr_holdings) {
        float value = h.stock_ptr -> curr_price;
        portfolio.cash += value;
        h.sell_price = value;
        portfolio.past_holdings.push_back(h);
    }
    portfolio.curr_holdings = {};

    std::cout << "After final day cash is " << portfolio.cash << std::endl;
}

void update(int day, market *market, portfolio *portfolio) {
    // Update all stocks (prices, MA)
    for (int i = 0; i < market_size; i++) {
        stock *s_p = &(market -> stocks[i]);

        // Update price
        s_p -> curr_price = s_p -> prices[day];
        
        // Update moving averages efficiently
        float old_price;
        float new_price = s_p -> curr_price;

        // In these cases have to check if we have to remove a value, or just calculate the
        // mean of all prices until now.
        if (day > 2 - 1) {
            old_price = s_p -> prices[day - 2];
            s_p -> ma_2days = ((2 * s_p -> ma_2days) - old_price + new_price)/2;   
        }

        else {
            s_p -> ma_2days = (s_p -> prices[0] + s_p -> curr_price) / 2;
        }

        if (day > 7 - 1) {
            old_price = s_p -> prices[day - 7];
            s_p -> ma_7days = ((7 * s_p -> ma_7days) - old_price + new_price)/7;
        }

        else {
            s_p -> ma_7days = ((day * s_p -> ma_7days) + new_price)/(day + 1);
        }

        if (day > 14 - 1) {
            old_price = s_p -> prices[day - 14];
            s_p -> ma_14days = ((14 * s_p -> ma_14days) - old_price + new_price)/14;
        }

        else {
            s_p -> ma_14days = ((day * s_p -> ma_14days) + new_price)/(day + 1);
        }

        if (day > 30 - 1) {
            old_price = s_p -> prices[day - 30];
            s_p -> ma_14days = ((30 * s_p -> ma_14days) - old_price + new_price)/30;
        }

        else {
            s_p -> ma_30days = ((day * s_p -> ma_30days) + new_price)/(day + 1);
        }
    }

    // Update portfolio value
    float sum = 0;
    for (holding h : portfolio -> curr_holdings) {
        sum += h.stock_ptr -> curr_price;
    }
    portfolio -> holdings_value = sum;
}

void predict(int day, market *market) {
    // We now have updated MAs and can use it to predict the value of each stock 
    // (and holding in portfolio) the next day

    // Declare the coefficients for ma_2day, ma_7day, ... respectively.
    // TODO: fix weights... this doesn't work: no positive gains are predicted!
    // float weights [4] = {0.769231, 0.192308, 0, -0.384615};
    float weights [4] = {0.25, 0.25, 0.25, 0.25};

    for (int i = 0; i < market_size; i++) {
        stock *s_p = &(market -> stocks[i]);

        s_p -> tmr_price_est = weights[0] * s_p -> ma_2days + weights[1] * s_p -> ma_7days
                        + weights[2] * s_p -> ma_14days + weights[3] * s_p -> ma_30days;
    }
}

void cheat_predict(int day, market *market) {
    // Cheat by looking at the next day for a perfect prediction
    for (int i = 0; i < market_size; i++) {
        stock *s_p = &(market -> stocks[i]);

        s_p -> tmr_price_est = s_p -> prices[day + 1];
    }
}