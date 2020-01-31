#include <iostream>
#include <string>
#include <vector>

// TODO: These should be marked as global or w/e, and then as extern when
// the structs are move to diff files.
const int days = 252; // equal to the # rows in each csv doc
const int portfolio_size = 10;
const int market_size = 5;

struct stock {
    std::string ticker;
    float curr_price;
    float prices [days];
    float initial_price;
    float ma_2days;
    float ma_7days;
    float ma_14days;
    float ma_30days;
    float rating;
};

struct market {
    stock *stocks [market_size];
    double total_value;
    double update_total_vaule() {
        double sum = 0.0;
        for (stock *s : stocks) {
            sum += s -> curr_price;
        }
        return sum;
    }
};

struct holding {
    stock *stock_ptr;
    unsigned int quantity;
    float buy_price;
    float sell_price;
};

struct portfolio {
    float cash;
    float holdings_value;
    std::vector<holding> curr_holdings;
    std::vector<holding> past_holdings;
};

void readData(market *);

int main() {
    // Initialise
    // Stocks, initial market value, initial empty portfolio, ...
    struct market market;
    readData(&market);

    // Let one day go by

    // Loop over all days,
        // Analyse all stocks
        
        // and add to portfolio those with highest rating
}

#include <fstream>
#include <sstream>
std::string path = "market_data/";

#include <dirent.h>
std::vector<std::string> get_file_names() {
    std::vector<std::string> file_names;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("./market_data/")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string curr_file_name = ent -> d_name;

            // Check they aren't . or ..
            if (!curr_file_name.compare(".") || !curr_file_name.compare("..")) {
                continue;
            }

            file_names.push_back(curr_file_name);
        }
        closedir(dir);
    }

    else {
        std::cerr << "couldn't open" << std::endl;
    }

    return file_names;
}

void readData(market *market) {
    std::vector<std::string> file_names = get_file_names();
    
    for (std::string file_name : file_names) {
        // Only do adjusted close for now.
        std::ifstream file("market_data/AMD.csv");
        std::string line;

        if (!file.is_open()) {
            std::cerr << "File couldn't be opened" << std::endl;
        }

        // Prepare to read data
        stock curr;
        curr.ticker = "AMD";

        // Ignore the first line
        std::getline(file, line);

        // Read the rest of the lines
        // Note that adjusted volume is column 6 (so 5 when 0 indexed)
        float val;
        int i = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            int col = 0;
            while (ss >> val) {
                if (col == 5) {
                    curr.prices[i] = val;
                    i++;
                }
                

                //result.at(col).second.push_back(val);
                // ignore commas
                if (ss.peek() == ',') {
                    ss.ignore();
                }

                col++;
            }

        }

        file.close();

        // Finalise
        market -> stocks[0] = &curr;
    }
}