#include <iostream>
#include <math.h>
#include <vector>

#define PI 3.141592653

int main() {
    int initial_price = 250;

    // Define the modifying function
    auto f = [] (double x) {
        return 0.5 * std::sin((2 * PI * x)/365);
    };

    // Define the price function
    auto p = [initial_price, f] (double x) {
        return (initial_price + initial_price * f(x));
    };

    // Define the moving average functions
    auto m = [p] (double x, int n) {
        double result;

        for (int i = 0; i < n; i++) {
            result += p(x - i);
        }

        result = result/n;

        return result;
    };

    // Define estimator function, that predicts p(x + 1)
    auto est = [m] (double x, double w1, double w2, double w3, double w4) {
        return w1 * m(x, 2) + w2 * m(x, 7) + w3 * m(x, 14) + w4 * m(x, 30);
    };

    // Define the range of possible parameters for est
    int granularity = 3;
    double start = -0.5;
    double end = 0.5;
    std::vector<double> vals;

    for (int i = 0; i < granularity; i++) {
        vals.push_back(start + ((i + 0.0)/granularity));
    }

    vals.push_back(0.0);

    for (int i = 0; i < granularity; i++) {
        vals.push_back((i + 0.0)/granularity);
    }

    vals.push_back(end);

    double lowest_err = 1e10;
    double best_weights [4];

    unsigned long count = 0;
    for (double w1 : vals) {
        for (double w2 : vals) {
            for (double w3 : vals) {
                for (double w4 : vals) {
                    count++;
                    if (count % 1000 == 0) {
                        std::cout << count << std::endl;
                    }

                    // Find total error 
                    double curr_err = 0.0;

                    for (int i = 0; i < 365; i++) {
                        curr_err += std::abs(p(i) - est(i, w1, w2, w3, w4));
                    }

                    if (curr_err < lowest_err) {
                        // std::cout << "Found a lower error: " << curr_err << std::endl;
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

    std::cout << "\n\n\n Lowest error is " << std::endl;
    for (double w : best_weights) {
        std::cout << w << std::endl;
    }
}