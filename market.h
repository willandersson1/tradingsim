#ifndef MARKET_H
#define MARKET_H

#include <vector>
#include "stock.h"
class Market {
    public: 
        int size;
        double value; // TODO: this doesn't actually make much sense. Not weighted by # shares. Do by avg % gain?
        std::vector<Stock> stocks;
        
        Market(std::vector<Stock>);
        void update(int);
        void predict(int);
        void cheat_predict(int);
};

#endif