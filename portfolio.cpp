#include "portfolio.h"

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