#include <iostream>
#include <string>
#include <vector>
#include <math.h>

// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 252; // equal to the # rows in each csv doc. Currently 31.01.2019 - 31.01.2020 inclusive.
const int portfolio_size = 10; // TODO: put find this by counting # files
const int market_size = 5;

struct stock {
    std::string ticker;
    int id;
    float curr_price;
    float prices [days];
    float initial_price;
    float ma_2days;
    float ma_7days;
    float ma_14days;
    float ma_30days;
    float tmr_price_est;
};

struct market {
    stock stocks [market_size];
    // TODO: this doesn't actually make much sense. Not weighted by # shares. Do by avg % gain?
    double total_value;
    void update_total_value() {
        double sum = 0.0;
        for (stock s : stocks) {
            sum += s.curr_price;
        }
        total_value = sum;
    }
};

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

void readData(market *);
void update(int day, market *market, portfolio *portfolio);
void predict(int day, market *market);

#include <chrono>
int main() {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Initialise
    // Stocks, initial market value, initial empty portfolio, ...
    struct market market;
    readData(&market);
    market.update_total_value();
    
    struct portfolio portfolio;
    portfolio.cash = 10000;

    // Let one day go by
    // TODO: roll this into the update function.
    int day = 1;
    for (stock s : market.stocks) {
        s.curr_price = s.prices[1];
        s.ma_2days = (s.prices[0] + s.prices[day])/2;
        s.ma_7days, s.ma_14days, s.ma_30days = s.ma_2days;
    }
    day++;

    // Loop over all days,
    while (day < days) {
        // Update price to current day's, as well as MAs and totals.
        update(day, &market, &portfolio);

        // Analyse all stocks in the market.
        predict(day, &market);

        // TODO: could optimise this using an optimising algorithm
        // To find the optimal choice of stocks, we implement a solution for the knapsack 
        // problem. But first we need to determine and initialise the weights, values, and 
        // max weight as follows... 

        // Note - this is simplified hugely by considering no transaction fees, so that 
        // current portfolio holdings can be sold each new day without loss. Otherwise
        // would need to model them as items with negative weight and adjust the algo.

        // Weight capacity:
        // The sum of all cash and current holdings
        float capacity = portfolio.cash;
        for (holding h : portfolio.curr_holdings) {
            capacity += h.stock_ptr -> curr_price;
        }

        // Number of items: enough copies of each stock so that max capacity is less than
        // quantity * curr_price of the stock
        int num_items;
        int copies [market_size];
        for (stock s : market.stocks) {
            copies[s.id] = (int) (std::ceil(capacity/s.curr_price));

            num_items += copies[s.id];
        }

        // Weight: the stock price. 
        // Values: the predicted gain tomorrow.
        // Both values need to be rounded up to integer values.
        int weights [num_items];
        int values [num_items];
        for (stock s : market.stocks) {
            int price = (int) std::ceil(s.curr_price);
            int tmr_price_prediction = (int) std::ceil(s.tmr_price_est);
            int predicted_gain = price - tmr_price_prediction;

            for (int n = 0; n < copies[s.id]; n++) {
                weights[s.id + n] = price;
                values[s.id + n] = predicted_gain;
            }
        }

        // TODO: remember to account for the difference btwn integer value and true value when buying!
        // Add to cash, then do a second round maybe?

        

        day++;
    }





    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

void update(int day, market *market, portfolio *portfolio) {
    // Update stocks (prices, MA)
    for (stock s : market -> stocks) {
        s.curr_price = s.prices[day];
        
        // Update moving averages efficiently
        float old_price;
        float new_price = s.curr_price;
        
        old_price = s.prices[day - 2];
        s.ma_2days = ((2 * s.ma_2days) - old_price + new_price)/2;

        // In these cases have to check if we have to remove a value, or just calculate the
        // mean of all prices until now.
        if (day > 7 - 1) {
            old_price = s.prices[day - 7];
            s.ma_7days = ((7 * s.ma_7days) - old_price + new_price)/7;
        }

        else {
            s.ma_7days = ((day * s.ma_7days) + new_price)/(day + 1);
        }

        if (day > 14 - 1) {
            old_price = s.prices[day - 14];
            s.ma_14days = ((14 * s.ma_14days) - old_price + new_price)/14;
        }

        else {
            s.ma_14days = ((day * s.ma_14days) + new_price)/(day + 1);
        }

        if (day > 30 - 1) {
            old_price = s.prices[day - 30];
            s.ma_14days = ((30 * s.ma_14days) - old_price + new_price)/30;
        }

        else {
            s.ma_30days = ((day * s.ma_30days) + new_price)/(day + 1);
        }
    }

    // Update portfolio value
    float sum;
    for (holding h : portfolio -> curr_holdings) {
        sum += h.stock_ptr -> curr_price;
    }
    portfolio -> holdings_value = sum;
}

void predict(int day, market *market) {
    // We now have updated MAs and can use it to predict the value of each stock 
    // (and holding in portfolio) the next day

    // Declare the coefficients for ma_2day, ma_7day, ... respectively.
    float weights [4] = {0.769231, 0.192308, 0, -0.384615};
    for (stock s : market -> stocks) {
        s.tmr_price_est = weights[0] * s.ma_2days + weights[1] * s.ma_7days
                        + weights[2] * s.ma_14days + weights[3] * s.ma_30days;
    }
}

#include <fstream>
#include <sstream>
std::string path = "market_data/";

#include <dirent.h>
std::vector<std::string> get_file_names() {
    std::vector<std::string> file_names;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("./market_data/")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string curr_file_name = ent -> d_name;

            // Check they aren't . or ..
            if (!curr_file_name.compare(".") || !curr_file_name.compare("..")) {
                continue;
            }

            file_names.push_back(curr_file_name);
        }
        closedir(dir);
    }

    else {
        std::cerr << "couldn't open" << std::endl;
    }

    return file_names;
}

// Only read the adjusted close data for now.
void readData(market *market) {
    std::vector<std::string> file_names = get_file_names();
    
    int stock_idx = 0;
    for (std::string file_name : file_names) {
        std::ifstream file("market_data/" + file_name);

        if (!file.is_open()) {
            std::cerr << "File " + file_name + "couldn't be opened" << std::endl;
        }

        // Prepare to read data
        stock curr;
        curr.ticker = file_name.substr(0, file_name.length() - 4); // Need to cut the .CSV off
        curr.id = stock_idx;
        std::string line;

        // Ignore the first line
        std::getline(file, line);

        // Read the rest of the lines
        // Note that adjusted volume is column 5 (0-indexed)
        float val;
        int day_idx = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            int col = 0;
            while (ss >> val) {
                if (col == 5) {
                    curr.prices[day_idx] = val;
                    day_idx++;
                }

                // Ignore commas
                if (ss.peek() == ',') {
                    ss.ignore();
                }

                col++;
            }
        }

        // Finalise
        file.close();
        curr.initial_price = curr.prices[0];
        curr.curr_price = curr.initial_price;
        curr.ma_2days, curr.ma_7days, curr.ma_14days, curr.ma_30days = curr.initial_price;
        market -> stocks[stock_idx] = curr;
        stock_idx++;
    }
}