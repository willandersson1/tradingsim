#ifndef MARKET_H
#define MARKET_H

#include <string>
// TODO: should really be classes here... figure out the const problem too
const int days;

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

const int market_size;

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

#endif