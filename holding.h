#ifndef HOLDING_H
#define HOLDING_H

#include "stock.h"
class Holding {
    public: 
        Stock *stock_ptr;
        float buy_price;
        float sell_price;

        Holding(Stock *);
        void sell();
};

#endif