#ifndef MARKET_H
#define MARKET_H

#include <vector>
#include "stock.h"
class Market {
    public: 
        int size;
        double value;
        std::vector<Stock> stocks;
        
        Market(std::vector<Stock>);
        void update(int);
        void predict(int);
        void cheat_predict(int);
};

#endif