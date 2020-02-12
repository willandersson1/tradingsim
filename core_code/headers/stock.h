#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <vector>

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

#endif