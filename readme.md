Explanation of strategy
Initially, attempted to simulate stock market with sin function: f(x) = 0.4 * sin(2 * pi * x/365) + (4/365*100) * x, p(x) = initial_price * f(x) + initial price with the idea that the stock market is cyclical (yearly) with max drawdown ~40%, but with inherent 4% annual growth. Decided before testing that this was both inaccurate and interesting to predict with MAs.

Then downloaded 2 years of SP500 TR data and trying to find parameters for MAs that lead to least total error (error defined as ...) ... Since data intensive I found the following results in estimator_weights_results.txt.
