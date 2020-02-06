#include "read_market_data.h"

std::vector<std::string> get_file_names() {
    std::vector<std::string> file_names;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("./market_data/")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string curr_file_name = ent -> d_name;

            // Check they aren't equal to . or ..
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

// Only read the adjusted close data for now.
void readData(market *market) {
    std::vector<std::string> file_names = get_file_names();
    
    int stock_idx = 0;
    for (std::string file_name : file_names) {
        std::ifstream file("market_data/" + file_name);

        if (!file.is_open()) {
            std::cerr << "File " + file_name + "couldn't be opened" << std::endl;
        }

        // Prepare to read data
        stock curr;
        curr.ticker = file_name.substr(0, file_name.length() - 4); // Need to cut the .CSV off
        curr.id = stock_idx;
        std::string line;

        // Ignore the first line
        std::getline(file, line);

        // Read the rest of the lines
        // Note that adjusted volume is column 5 (0-indexed)
        float val;
        int day_idx = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            int col = 0;
            while (ss >> val) {
                if (col == 5) {
                    curr.prices[day_idx] = val;
                    day_idx++;
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
        curr.initial_price = curr.prices[0];
        curr.curr_price = curr.initial_price;
        curr.ma_2days, curr.ma_7days, curr.ma_14days, curr.ma_30days = curr.initial_price;
        market -> stocks[stock_idx] = curr;
        stock_idx++;
    }
}