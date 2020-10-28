#ifndef STOCK_H
#define STOCK_H

/**
 * A simple C++ class to encapsulate information associated with a
 * stock.  The stock market consists of a collection of these objects.
 *
 * Copyright (c) 2020 raodm@miamioh.edu
 */

#include <string>
#include <mutex>
#include <condition_variable>

class Stock {
public:
    // The name of the stock, e.g. "msft"
    std::string name;
    // Number of available stocks.  This value can never go below
    // zero.
    unsigned int balance;
    // A mutex associated with this this stock.
    std::mutex mutex;
    // The condition variable associated with this stock.
    std::condition_variable condVar;
};

#endif
