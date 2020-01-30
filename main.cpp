#include <iostream>
#include <string>
#include <vector>

// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 40;
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
    double total_value;
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

int main() {
    // Initialise
    // Stocks, initial market value, initial empty portfolio, ...
    double initial_market_value = 0.0;

    // Let one day go by

    // Loop over all days,
        // Analyse all stocks
        
        // and add to portfolio those with highest rating
}