#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <vector>
#include "holding.h"
#include "stock.h"
class Portfolio {
    public: 
        float cash;
        std::vector<Holding> curr_holdings; // TODO: should be pointers to this?
        std::vector<Holding> past_holdings;

        Portfolio(float);
        float get_curr_holdings_value();
        void buy(Stock *);
        void sellAll();
};

#endif