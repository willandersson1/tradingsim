#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "knapsack.h"
#include "print_help.h"


// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 252; // equal to the # rows in each csv doc. Currently 31.01.2019 - 31.01.2020 inclusive.
const int market_size = 5; // TODO: calculate this by counting files

// Stock declaration
class Stock {
    public:
        int id;
        std::string ticker;
        std::vector<float> prices;
        float curr_price, initial_price, tmr_price_est;
        float ma_2days, ma_7days, ma_14days, ma_30days;

        Stock(int, std::string, std::vector<float>);
        void update(int);
        std::string toString();
};

// Stock definition
Stock::Stock(int x, std::string t, std::vector<float> p) {
    id = x;
    ticker = t;
    prices = p;

    curr_price = prices[0];
    initial_price = prices[0];
    tmr_price_est = 0;

    ma_2days = curr_price;
    ma_7days = curr_price;
    ma_14days = curr_price;
    ma_30days = curr_price;
}

void Stock::update(int d) {
    // Update price
    curr_price = prices[d];

    // Update MAs efficiently
    float old_price;
    float new_price = curr_price;

    // In these cases have to check if we have to remove a value, or just calculate the
    // mean of all prices until now.
    if (d > 2 - 1) {
        old_price = prices[d - 2];
        ma_2days = ((2 * ma_2days) - old_price + new_price)/2;   
    }

    else {
        ma_2days = (prices[0] + curr_price) / 2;
    }

    if (d > 7 - 1) {
        old_price = prices[d - 7];
        ma_7days = ((7 * ma_7days) - old_price + new_price)/7;
    }

    else {
        ma_7days = ((d * ma_7days) + new_price)/(d + 1);
    }

    if (d > 14 - 1) {
        old_price = prices[d - 14];
        ma_14days = ((14 * ma_14days) - old_price + new_price)/14;
    }

    else {
        ma_14days = ((d * ma_14days) + new_price)/(d + 1);
    }

    if (d > 30 - 1) {
        old_price = prices[d - 30];
        ma_14days = ((30 * ma_14days) - old_price + new_price)/30;
    }

    else {
        ma_30days = ((d * ma_30days) + new_price)/(d + 1);
    }
}

// Market declaration
class Market {
    public: 
        int size;
        double value; // TODO: this doesn't actually make much sense. Not weighted by # shares. Do by avg % gain?
        std::vector<Stock> stocks;
        
        Market(int, std::vector<Stock>);
        void update(int);
        void cheat_predict(int);
};

// Market definition
Market::Market(int sz, std::vector<Stock> s) {
    size = sz;
    stocks = s;
    update(0);
}

void Market::update(int d) {
    double sum = 0.0;
    for (Stock s : stocks) {
        s.update(d);
        sum += s.curr_price;
    }
    value = sum;
}

void Market::cheat_predict(int d) {
    for (Stock s : stocks) {
        s.tmr_price_est = s.prices.at(d + 1);
    }
}

// Holding declaration
class Holding {
    public: 
        Stock *stock_ptr;
        float buy_price;
        float sell_price;

        Holding(Stock *);
        void sell();
};

// Holding definition
Holding::Holding(Stock *s) {
    stock_ptr = s;
    buy_price = s -> curr_price;
}

void Holding::sell() {
    sell_price = stock_ptr -> curr_price;
}

// Portfolio declaration
class Portfolio {
    public: 
        float cash;
        std::vector<Holding> curr_holdings; // TODO: should be pointers to this?
        std::vector<Holding> past_holdings;

        Portfolio(float);
        float get_curr_holdings_value();
        void buy(Stock *);
        void sellAll();
};

// Portfolio definition
Portfolio::Portfolio(float initial_cash) {
    cash = initial_cash;
    curr_holdings = {};
    past_holdings = {};
}

float Portfolio::get_curr_holdings_value() {
    float sum = 0.0;
    for (Holding h : curr_holdings) {
        sum += h.stock_ptr -> curr_price;
    }
    return sum;
}

void Portfolio::buy(Stock *s) {
    cash -= s -> curr_price;
    curr_holdings.emplace_back(s);
}

void Portfolio::sellAll() {
    for (Holding h : curr_holdings) {
        h.sell();
        cash += h.sell_price;
        past_holdings.push_back(h);
    }

    curr_holdings = {};
}





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

void update(int day, market *market, portfolio *portfolio);
void predict(int day, market *market);
void cheat_predict(int day, market *market);
void readData(market *market);

std::vector<Stock> class_readData();


int main() {
    // Initialise
    Market market(market_size, class_readData());

    struct market market;
    readData(&market);
    market.update_total_value();
    
    struct portfolio portfolio;
    portfolio.cash = 200;

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

// TODO: implement in class version
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

#include <fstream>
#include <sstream>
#include <dirent.h>
#include <vector>
#include <iostream>
std::string path = "market_data/";

std::vector<std::string> get_file_names() {
    std::vector<std::string> file_names;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("./market_data/")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string curr_file_name = ent -> d_name;

            // Check they aren't equal to . or ..
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

// Class version
std::vector<Stock> class_readData() {
    std::vector<Stock> stocks = {};

    std::vector<std::string> file_names = get_file_names();
    
    int stock_idx = 0;
    for (std::string file_name : file_names) {
        std::ifstream file("market_data/" + file_name);

        if (!file.is_open()) {
            std::cerr << "File " + file_name + "couldn't be opened" << std::endl;
        }

        // Prepare to read data
        std::string ticker = file_name.substr(0, file_name.length() - 4); // Need to cut the .CSV off
        int id = stock_idx;
        std::vector<float> prices = {};

        std::string line;

        // Ignore the first line
        std::getline(file, line);

        // Read the rest of the lines
        // Note that adjusted volume is column 5 (0-indexed)
        float val;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            int col = 0;
            while (ss >> val) {
                if (col == 5) {
                    prices.push_back(val);
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
        stocks.emplace_back(id, ticker, prices);
        stock_idx++;
    }

    return stocks;
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