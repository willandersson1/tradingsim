#include "market.h"

Market::Market(int sz, std::vector<Stock> s) {
    size = sz;
    stocks = s;
    update(0);
}

void Market::update(int d) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        stocks.at(i).update(d);
        sum += stocks.at(i).curr_price;
    }
    value = sum;
}

void Market::predict(int d) {
    // We now have updated MAs and can use it to predict the value of each stock 
    // (and holding in portfolio) the next day

    // Declare the coefficients for ma_2day, ma_7day, ... respectively.
    // TODO: fix weights... this doesn't work: no positive gains are predicted!
    float weights [4] = {0.769231, 0.192308, 0, -0.384615};

    for (int i = 0; i < size; i++) {
        stocks.at(i).tmr_price_est = weights[0] * stocks.at(i).ma_2days + weights[1] * stocks.at(i).ma_7days
                             + weights[2] * stocks.at(i).ma_14days + weights[3] * stocks.at(i).ma_30days;
    }
}

void Market::cheat_predict(int d) {
    for (int i = 0; i < size; i++) {
        stocks.at(i).tmr_price_est = stocks.at(i).prices.at(d + 1);
    }
}