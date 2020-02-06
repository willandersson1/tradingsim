#ifndef READ_MARKET_DATA_H
#define READ_MARKET_DATA_H

#include <fstream>
#include <sstream>
#include <dirent.h>
#include <vector>
#include <iostream>
#include "market.h"
std::string path = "market_data/";
void readData(market *market);

#endif