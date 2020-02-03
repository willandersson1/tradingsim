#include <iostream>
#include <math.h>
#include <vector>

#define PI 3.141592653

// Read the values of SPY in, 31.01.2017 to 31.01.2019 inclusive.
// 503 days
// For some reason need to select col 7, whereas 5 worked for the others...
#include <fstream>
#include <sstream>
#include <string>
void readData(float prices []) {
    std::ifstream file("^SP500TR.csv");

    if (!file.is_open()) {
        std::cerr << "File couldn't be opened" << std::endl;
    }

    std::string line;

    // Ignore the first line
    std::getline(file, line);

    // Read the rest of the lines
    double val;
    int day_idx = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);

        int col = 0;
        while (ss >> val) {
            if (col == 7) {
                prices[day_idx] = val;
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
}

int main() {
    // Read in the values of SPY total return
    const int days = 503;
    float prices [days];
    readData(prices);

    // Define the moving average functions
    auto m = [prices] (int d, int n) {
        double result;
        for (int i = 0; i < std::min(n, d); i++) {
            result += prices[d - i];
        }

        result = result/n;

        return result;
    };

    // Define estimator function, that predicts p(x + 1)
    auto est = [m] (int d, double w1, double w2, double w3, double w4) {
        return w1 * m(d, 2) + w2 * m(d, 7) + w3 * m(d, 14) + w4 * m(d, 30);
    };

    // Define the range of possible parameters for est
    int granularity = 13;
    double start = -2.5;
    double mid = 0.0;
    double end = -start;
    double increment = end/granularity;
    std::vector<double> vals;

    vals.push_back(start);

    for (int i = 1; i < granularity; i++) {
        vals.push_back(vals.back() + increment);
    }

    vals.push_back(mid);

    for (int i = 0; i < granularity - 1; i++) {
        vals.push_back(vals.back() + increment);
    }

    vals.push_back(end);

    double lowest_err = 1e10; // large dummy value
    double best_weights [4];

    unsigned long count = 0;
    for (double w1 : vals) {
        for (double w2 : vals) {
            for (double w3 : vals) {
                for (double w4 : vals) {
                    count++;
                    if (count % 2500 == 0) {
                        std::cout << count << std::endl;
                    }

                    // Find total error 
                    double curr_err = 0.0;

                    for (int i = 0; i < days; i++) {
                        curr_err += std::abs(prices[i + 1] - est(i, w1, w2, w3, w4));

                        // Slight optimisation
                        if (curr_err > lowest_err) {
                            break;
                        }
                    }

                    if (curr_err < lowest_err) {
                        best_weights[0] = w1;
                        best_weights[1] = w2;
                        best_weights[2] = w3;
                        best_weights[3] = w4;
                        lowest_err = curr_err;
                    }
                }
            }
        }
    }

    std::cout << "\n\n\n" << std::endl;
    std::cout << "Range: [" << start << ", " << end << "]" << std::endl;
    std::cout << "Granularity: " << granularity << std::endl;
    std::cout << "Lowest err: " << lowest_err << std::endl;
    std::cout << "Weights: " << std::endl;
    for (double w : best_weights) {
        std::cout << w << std::endl;
    }
}