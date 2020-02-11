#include "read_data.h"

std::vector<Stock> readData(int days) {
    std::vector<Stock> stocks;
    stocks.reserve(days);

    std::vector<std::string> file_names = get_file_names();
    
    int stock_idx = 0;
    for (std::string file_name : file_names) {
        std::ifstream file("market_data/" + file_name);

        if (!file.is_open()) {
            std::cerr << "File " + file_name + "couldn't be opened" << std::endl;
        }

        // Prepare to read data
        std::string ticker = file_name.substr(0, file_name.length() - 4); // Need to cut the .CSV off
        int id = stock_idx;
        std::vector<float> prices = {};

        std::string line;

        // Ignore the first line
        std::getline(file, line);

        // Read the rest of the lines
        // Note that adjusted volume is column 5 (0-indexed)
        float val;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            int col = 0;
            while (ss >> val) {
                if (col == 5) {
                    prices.push_back(val);
                }

                // Ignore commas
                if (ss.peek() == ',') {
                    ss.ignore();
                }

                col++;
            }
        }

        // Finalise
        file.close();
        stocks.emplace_back(id, ticker, prices);
        stock_idx++;
    }

    return stocks;
}