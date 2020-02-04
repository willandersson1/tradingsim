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
std::vector<int> knapsack_solve(int capacity, int n, int weights [], int values []);
void printArr(int n, int A []);
void printMat(int m, int n, int *A);

#include <chrono>
int main() {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Initialise
    // Stocks, initial market value, initial empty portfolio, ...
    struct market market;
    readData(&market);
    market.update_total_value();
    
    struct portfolio portfolio;
    portfolio.cash = 1000;

    // Let one day go by
    // TODO: roll this into the update function.
    int day = 1;
    for (int i = 0; i < market_size; i++) {
        stock *s_p = &market.stocks[i];
        s_p -> curr_price = s_p -> prices[day];
        s_p -> ma_2days   = (s_p -> prices[0] + s_p -> curr_price) / 2;
        s_p -> ma_7days   = s_p -> ma_2days;
        s_p -> ma_14days  = s_p -> ma_2days;
        s_p -> ma_30days  = s_p -> ma_2days;
    }

    day++;

    // Loop over all days,
    while (day < days) {
        // Update price to current day's, as well as MAs and totals.
        update(day, &market, &portfolio);

        // Analyse all stocks in the market.
        predict(day, &market);

        // TODO: add all curr holdings to past holdings

        // TODO: could optimise this using an optimising algorithm
        // To find the optimal choice of stocks, we implement a solution for the knapsack 
        // problem. But first we need to determine and initialise the weights, values, and 
        // max weight as follows... 

        // Note - this is simplified hugely by considering no transaction fees, so that 
        // current portfolio holdings can be sold each new day without loss. Otherwise
        // would need to model them as items with negative weight and adjust the algo.

        // Weight capacity:
        // The sum of all cash and current holdings. Round down.
        int capacity = 0;
        float temp = portfolio.cash;
        for (holding h : portfolio.curr_holdings) {
            temp += h.stock_ptr -> curr_price;
        }
        capacity = (int) std::floor(temp);

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
            int tmr_price_prediction = (int) std::ceil(s_p -> tmr_price_est);
            int predicted_gain = tmr_price_prediction - price;

            int s_id = s_p -> id;
            for (int n = 0; n < copies[s_id]; n++) {
                weights[counter] = price;
                values[counter] = predicted_gain;
                ids[counter] = s_id;

                counter++;
            }
        }

        std::vector<stock *> recommended_stocks;
        std::vector<int> item_nums = knapsack_solve(capacity, num_items, weights, values);


        // TODO: remember to account for the difference btwn integer value and true value when buying!
        // Add to cash, then do a second round maybe?

        day++;
    }




    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

void printArr(int n, int A []) {
    for (int i = 0; i < n - 1; i++) {
        std::cout << A[i] << "  ";
    }

    std::cout << A[n - 1] << std::endl;
}

void printMat(int m, int n, int *A) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n - 1; j++) {
            std::cout << A[j + i * n] << "  ";
        }

        std::cout << A[n - 1 + i * n] << std::endl;
    }
}

std::vector<int> knapsack_solve(int capacity, int n, int weights [], int values []) {
    // Solve using DP, keep track of additional items used as well.
    // DP table is capacity + 1 columns, and n rows. 
    // DP[i][w] is the max value of the first i items, with weight limit w.
    // We find the solution in DP[n - 1][capacity].
    int DP [n][capacity + 1];

    // A matrix of the same size tracks which new item is added to the knapsack
    // at each stage. This can then be used to reconstruct the exact items
    // used.
    // item_added[i][j] = k <=> item k was added to the knapsack at DP[i][j]. 
    // Set to -1 else.
    int item_added [n][capacity + 1];
    std::vector<int> knapsack;

    // Initialise:
    // Each cell in the first row is values[0] iff i > weights[i]. 
    // ie, DP[0][i] = values[0] <=> weights[0] <= i, else 0.
    for (int w = 0; w <= capacity; w++) {
        if (weights[0] <= w && values[0] > 0) {
            DP[0][w] = values[0];
            item_added[0][w] = 0;
        }

        else {
            DP[0][w] = 0;
            item_added[0][w] = -1;
        }
    }

    // Fill out the rest of the table, row by row. 
    for (int i = 1; i < n; i++) {
        for (int w = 0; w <= capacity; w++) {
            // Either keep the same set of items
            int same_set_value = DP[i - 1][w];

            // or add the i-th item to the set, if it doesn't weigh too much.
            int with_new_item;
            if (w - weights[i] < 0) {
                with_new_item = -1;
            }

            else {
                with_new_item = values[i] + DP[i - 1][w - weights[i]];
            }

            // Choose the one that gives the greatest value.
            if (with_new_item > same_set_value) {
                DP[i][w] = with_new_item;
                item_added[i][w] = i;
            }

            else {
                DP[i][w] = same_set_value;
                item_added[i][w] = -1;
            }
        }
    }

    // Backtracking
    int w = capacity;
    for (int i = n - 1; i >= 0; i--) {
        int item_num = item_added[i][w];

        if (item_num != -1) {
            // If an item was added here, add it to the list 
            // and account for its weight to continue the search.
            knapsack.push_back(item_num);
            w -= weights[item_num];
        }
    }

    return knapsack;
}

void update(int day, market *market, portfolio *portfolio) {
    // Update stocks (prices, MA)
    for (int i = 0; i < market_size; i++) {
        stock *s_p = &(market -> stocks[i]);
        s_p -> curr_price = s_p -> prices[day];
        
        // Update moving averages efficiently
        float old_price;
        float new_price = s_p -> curr_price;
        
        old_price = s_p -> prices[day - 2];
        s_p -> ma_2days = ((2 * s_p -> ma_2days) - old_price + new_price)/2;

        // In these cases have to check if we have to remove a value, or just calculate the
        // mean of all prices until now.
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