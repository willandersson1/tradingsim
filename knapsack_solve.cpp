#include "knapsack_solve.h"

std::vector<int> knapsack_solve(int capacity, int n, int weights [], int values []) {
    // Solve using DP, keep track of additional items used as well.
    // DP table is capacity + 1 columns, and n rows. 
    // DP[i][w] is the max value of the first i items, with weight limit w.
    // We find the solution in DP[n - 1][capacity].
    int DP [n][capacity + 1];

    // A matrix of the same size tracks which new item is added to the knapsack
    // at each stage. This can then be used to reconstruct the exact items
    // used.
    // item_added[i][j] = k <=> item k was added to the knapsack at DP[i][j]. 
    // Set to -1 else.
    int item_added [n][capacity + 1];
    std::vector<int> knapsack;

    // Initialise:
    // Each cell in the first row is values[0] iff i > weights[i]. 
    // ie, DP[0][i] = values[0] <=> weights[0] <= i, else 0.
    for (int w = 0; w <= capacity; w++) {
        if (weights[0] <= w && values[0] > 0) {
            DP[0][w] = values[0];
            item_added[0][w] = 0;
        }

        else {
            DP[0][w] = 0;
            item_added[0][w] = -1;
        }
    }

    // Fill out the rest of the table, row by row. 
    for (int i = 1; i < n; i++) {
        for (int w = 0; w <= capacity; w++) {
            // Either keep the same set of items
            int same_set_value = DP[i - 1][w];

            // or add the i-th item to the set, if it doesn't weigh too much.
            int with_new_item;
            if (w - weights[i] < 0) {
                with_new_item = -1;
            }

            else {
                with_new_item = values[i] + DP[i - 1][w - weights[i]];
            }

            // Choose the one that gives the greatest value.
            if (with_new_item > same_set_value) {
                DP[i][w] = with_new_item;
                item_added[i][w] = i;
            }

            else {
                DP[i][w] = same_set_value;
                item_added[i][w] = -1;
            }
        }
    }

    // Backtracking
    int w = capacity;
    for (int i = n - 1; i >= 0; i--) {
        int item_num = item_added[i][w];

        if (item_num != -1) {
            // If an item was added here, add it to the list 
            // and account for its weight to continue the search.
            knapsack.push_back(item_num);
            w -= weights[item_num];
        }
    }

    return knapsack;
}