#include "stock.h"

Stock::Stock(int x, std::string t, std::vector<float> p) {
    id = x;
    ticker = t;
    prices = p;

    curr_price = initial_price = prices[0];
    tmr_price_est = 0;

    ma_2days = ma_7days = ma_14days = ma_30days = curr_price;
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