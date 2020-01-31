#include <iostream>
#include <string>
#include <vector>

// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 252; // equal to the # rows in each csv doc. Currently 31.01.2019 - 31.01.2020 inclusive.
const int portfolio_size = 10;
const int market_size = 5;

struct stock {
    std::string ticker;
    float curr_price;
    float prices [days];
    float initial_price;
    float ma_2days;
    float ma_7days;
    float ma_14days;
    float ma_30days;
    float rating;
};

struct market {
    stock stocks [market_size];
    // TODO: this doesn't actually make much sense. Not weigted by # shares.
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
    unsigned int quantity;
    float buy_price;
    float sell_price;
};

struct portfolio {
    float cash;
    float holdings_value;
    std::vector<holding> curr_holdings;
    std::vector<holding> past_holdings;
};

void readData(market *);
#include <chrono>
int main() {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Initialise
    // Stocks, initial market value, initial empty portfolio, ...
    struct market market;
    readData(&market);
    market.update_total_value();
    
    for (int i = 0; i < market_size; i++) {
        float p = market.stocks[i].initial_price;
        std::cout << p << std::endl;
    }
    std::cout << market.total_value << std::endl;

    // Let one day go by

    // Loop over all days,
        // Analyse all stocks
        
        // and add to portfolio those with highest rating

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
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

void readData(market *market) {
    std::vector<std::string> file_names = get_file_names();
    
    int stock_idx = 0;
    for (std::string file_name : file_names) {
        // Only do adjusted close for now.
        std::ifstream file("market_data/" + file_name);

        if (!file.is_open()) {
            std::cerr << "File " + file_name + "couldn't be opened" << std::endl;
        }

        // Prepare to read data
        stock curr;
        curr.ticker = file_name.substr(0, file_name.length() - 4); // Need to cut the .CSV off
        std::string line;

        // Ignore the first line
        std::getline(file, line);

        // Read the rest of the lines
        // Note that adjusted volume is column 6 (so 5 when 0 indexed)
        float val;
        int i = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            int col = 0;
            while (ss >> val) {
                if (col == 5) {
                    curr.prices[i] = val;
                    i++;
                }

                // ignore commas
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
        market -> stocks[stock_idx] = curr;
        stock_idx++;
    }
}