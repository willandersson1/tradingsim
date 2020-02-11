### Overview
This program attempts to trade five different stocks over the course of a year to make profit. The strategy (explained in detail below) is trained on two years' worth of old SP500 price data, then is applied to the moving averages on the stocks to be traded. A dynamic programming algorithm is used to find the optimal trades for each day, while the data is read from CSV files into appropriate objects. In the end, the program outputs the profit made. 

### Motivation
I am interested in (qunatitative) finance and was curious to see if I could make a rudimentary trading algorithm. The previous semester I had been informally exposed to the basics of C++, so I thought this would be a good challenge to sharpen my skills. I also decided to develop the program on a Unix OS (Ubuntu) and publish it on with Git to become familiar with common tools in industry.

### Trading strategy


### Limitations


### Possible extensions


### How to use


To compile:
- Compile each file with -c individually
- run g++ main.o stock.o holding.o portfolio.o market.o knapsack_solve.o read_data.o get_file_names.o print_help.o


Explanation of strategy
Initially, attempted to simulate stock market with sin function: f(x) = 0.4 * sin(2 * pi * x/365) + (4/365*100) * x, p(x) = initial_price * f(x) + initial price with the idea that the stock market is cyclical (yearly) with max drawdown ~40%, but with inherent 4% annual growth. Decided before testing that this was both inaccurate and interesting to predict with MAs.

Then downloaded 2 years of SP500 TR data and trying to find parameters for MAs that lead to least total error (error defined as ...) ... Since data intensive I found the following results in estimator_weights_results.txt.

There are flaws: I have no funademental economic or finance arguments to support my final weight choices. Furthermore, I am unsure to what extent the initial price of the stock affects the accuracy. But for now I will continue with these to finish my program and perhaps revisit the strategy later.
