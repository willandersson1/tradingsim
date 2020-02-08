#include "holding.h"

Holding::Holding(Stock *s) {
    stock_ptr = s;
    buy_price = s -> curr_price;
}

void Holding::sell() {
    sell_price = stock_ptr -> curr_price;
}