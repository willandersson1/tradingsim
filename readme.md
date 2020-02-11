### Overview
This program attempts to trade five different stocks over the course of a year to make profit. The strategy (explained in detail below) is trained on two years' worth of old SP500 price data, then is applied to the moving averages on the stocks to be traded. A dynamic programming algorithm is used to find the optimal trades for each day, while the data is read from CSV files into appropriate objects. In the end, the program outputs the profit made. 

### Motivation
I am interested in (qunatitative) finance and was curious to see if I could make a rudimentary trading algorithm. The previous semester I had been informally exposed to the basics of C++, so I thought this would be a good challenge to sharpen my skills. The fact that we are not expected to do large programming projects in my course added to the appeal. I also decided to develop the program on a Unix OS (Ubuntu) and publish it on with Git to become familiar with common tools in industry, neither of which I had prior experience with. 

### Trading strategy
Due to the simplicity, I decided to devise a trading strategy based on moving averages (2, 7, 14, and 30-day). Each day, the prices are updated and the next day's price is predicted using the following formula: 

<img src="http://latex.codecogs.com/svg.latex?\text{prediction}=w_1*\text{MA}_2+w_2*\text{MA}_{7}+w_3*\text{MA}_{14}+w_4*\text{MA}_{30}" border="0"/>

The challenge is then to find weights that enable accurate predictions. The code in ```estimator_weights_finder.cpp``` iterates over all possible combinations (with some restrictions) uses attempts to predict two years of daily SP500 price data. Overlap in dates with the stocks the program trades was avoided. This is a difficult computation, but keeping the next day prediction within 10% of the current day's price optimises the process hugely.

The current weights were found by minimising total error, equal to the sum of prediction errors for each day. As it turns out, this produces the exact same weights as minimising the percentage error each day. These weights lead to a 0.21% gain in the main program. See ```estimator_weights_results.txt``` for more details.

Also attempted was optimising for total gains at the end of the two years, logic being that this is similar to what the trading program aims to do. However, the weights produced under this scheme led to only a 0.17% gain.

Initially, I considered modeling the cyclic nature of the stock market with a formula

<img src="http://latex.codecogs.com/svg.latex?p(x)=0.4*sin(\frac{2*\pi*x}{365})+\frac{4}{365*100}" border="0"/>

where the coefficient 0.4 is supposed to dampen the behaviour, and the second term is supposed to represent an upward trend in the economy. Unfortunately this proved useless for predicting stock price behaviour and was not refined further.

### Knapsack abstraction


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
