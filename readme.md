### Overview
This program attempts to trade five different stocks over the course of a year to make profit. The strategy (explained in detail below) is trained on two years' worth of old SP500 price data, then applied to the moving averages on the stocks to be traded. A dynamic programming algorithm is used to find the optimal trades for each day, while the data is read from CSV files into appropriate objects. In the end, the program outputs the profit made. 

### Motivation
I am interested in (quantitative) finance and was curious to see if I could make a rudimentary trading algorithm. The previous semester I had been informally exposed to the basics of C++, so I thought this would be a good challenge to sharpen my skills. The fact that we are not expected to do large programming projects in my course added to the appeal. I also decided to develop the program on a Unix OS (Ubuntu) and publish it on with Git to become familiar with common tools in industry, neither of which I had prior experience with. 

### Trading strategy
Due to the simplicity, I decided to devise a trading strategy based on moving averages (2, 7, 14, and 30 day). Each day, the prices are updated and the next day's price is predicted using the following formula: 

<img src="http://latex.codecogs.com/svg.latex?\text{prediction}=w_1*\text{MA}_2+w_2*\text{MA}_{7}+w_3*\text{MA}_{14}+w_4*\text{MA}_{30}" border="0"/>

The challenge is then to find weights that enable accurate predictions. The code in ```estimator_weights_finder.cpp``` iterates over all possible combinations (with some restrictions) uses attempts to predict two years of daily SP500 price data. Overlap in dates with the stocks the program trades was avoided. This is a difficult computation, but keeping the next day prediction within 10% of the current day's price optimises the process hugely.

The current weights were found by minimising total error, equal to the sum of prediction errors for each day. As it turns out, this produces the exact same weights as minimising the percentage error each day. These weights lead to a 0.21% gain in the main program. See ```estimator_weights_results.txt``` for more details.

Also attempted was optimising for total gains at the end of the two years, logic being that this is similar to what the trading program aims to do. However, the weights produced under this scheme led to only a 0.17% gain.

Initially, I considered modelling the cyclic nature of the stock market with a formula

<img src="http://latex.codecogs.com/svg.latex?p(x)=0.4*sin(\frac{2\pi*x}{365})+\frac{4}{365*100}" border="0"/>

where the coefficient 0.4 is supposed to dampen the behaviour, and the second term is supposed to represent an upward trend in the economy. Unfortunately this proved useless for predicting stock price behaviour and was not refined further.

### Knapsack abstraction
A solver for the general knapsack problem is implemented in ```knapsack_solver.cpp```. It requires a max capacity, values of items, and weights of items. To fit this in the context of stock trading, the max capacity is given as the cash available to buy stocks. The items are stocks available to buy, their prices being weights. The value of each item is the predicted gain for the next day, seeing as this is what the program is trying to maximise. 

Because the knapsack solver takes in a finite list of items, then picks a subset of them, we need to provide a list of stocks to it. But since one could buy a huge amount of each stock, the number of copies is limited by the maximum number buyable with current cash at current prices (rounded up). Then an items array can be created, with an auxiliary array to assist in finding the actual stocks picked by the solver. One also needs to be careful that the knapsack problem deals with integer values, so everything relevant has to be cast appropriately.

### Limitations
The biggest issue with the program is that the matrices created in the knapsack solver are proportional in size to the initial cash multiplied by the number of items. As a result, including too many stocks to trade or beginning with a large amount of cash can easily deplete memory. This could perhaps be mitigated by approximating the solver with one that divides each item's value (as well as the total capacity) by a constant. 

The other limitation is the strategy's poor performance. Perhaps after I learn more about finance and quantitative trading, I will be able to improve it.

### Possible extensions
* Multithreading, perhaps by splitting up the computation of each row of the knapsack DP matrix
* Reading data and processing everything day by day, to reduce maximum memory usage
* User requests a particular set of stocks, make HTTP web requests to get relevant data
* See behaviour of different strategies on different sets of stocks
* Graph portfolio value over time

### How to use
* Compiling: 

  ```cd``` to ```/tradingsim```. Run the following code: 

  
  ```g++ main.cpp core_code/stock.cpp core_code/holding.cpp core_code/portfolio.cpp core_code/market.cpp core_code/knapsack_solve.cpp core_code/read_data.cpp core_code/get_file_names.cpp helper_code/print_help.cpp -o main```

  ```print_help``` is not necessary, but might be useful. 
  To run the compiled binary, execute ```./main``` in your shell.

* Adding stocks: 

  Find the historical daily price data 31.01.2019 - 31.01.2020 from Yahoo finance and download the ```csv```. Add the file to ```market_data```.
