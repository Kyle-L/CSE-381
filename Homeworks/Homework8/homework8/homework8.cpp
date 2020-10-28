/* 
 * A simple online stock exchange web-server.  
 * 
 * This multithreaded web-server performs simple stock trading
 * transactions on stocks.  Stocks must be maintained in an
 * unordered_map.
 * 
 * Copyright (C) 2020 liererkt@miamiOH.edu
 */

// The commonly used headers are included.  Of course, you may add any
// additional headers as needed.
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <vector>
#include "Stock.h"

// Setup a server socket to accept connections on the socket
using namespace boost::asio;
using namespace boost::asio::ip;

/** The HTTP response header to be printed at the beginning of the
    response */
const std::string HTTPRespHeader =
    "HTTP/1.1 200 OK\r\n"
    "Server: BankServer\r\n"
    "Content-Length: %1%\r\n"
    "Connection: Close\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n";

// Shortcut to smart pointer with TcpStream
using TcpStreamPtr = std::shared_ptr<tcp::iostream>;

// Forward declaration for methods defined further below
std::string url_decode(std::string);

// The name space to hold all of the information that is shared
// between multiple threads.
namespace sm {
    // A mutex associated with this the stock market.
    std::mutex mutex;
    
    // The condition variable associated with the stock market.
    std::condition_variable condVar;

    // Keeps track of the number of stock market threads.
    std::atomic<int> threadCount;
    
    // Unordered map including stock's name as the key (std::string)
    // and the actual Stock entry as the value.
    std::unordered_map<std::string, Stock> stockMap;

    /**
     * Creates a new stock with a starting amount.
     * @param stock The name of the new stock.
     * @param amount The starting amount of the new stock.
     * @param os An output stream to indicate whether the stock 
     *        was created or not.
     */
    void createStock(const std::string& stock, const double& amount, 
                            std::ostream& os) {
        if (stockMap.find(stock) == stockMap.end()) {
            stockMap[stock].name = stock;
            stockMap[stock].balance = amount;
            os << "Stock " << stock << " created with balance = " << amount;
        } else {
            os << "Stock " << stock << " already exists";
        }
    }

    /**
     * Buys a specified amount of the stock.
     * @param stock The name of the stock.
     * @param amount The amount of stock being bought.
     * @param os An output stream to indicate whether the stock 
     *        was bought or the stock could not be found.
     */
    void buyStock(const std::string& stock, const double& amount, 
                         std::ostream& os) {
        auto it = stockMap.find(stock); 
        if (it != stockMap.end()) {
            std::unique_lock<std::mutex> lock(stockMap[stock].mutex);
            stockMap[stock].condVar.wait(lock, 
                [stock, amount]{ return stockMap[stock].balance >= amount; });
            stockMap[stock].balance -= amount;
            stockMap[stock].condVar.notify_one();
            os << "Stock " << stock << "'s balance updated";
        } else {
            os << "Stock not found";
        }
    }

    /**
     * Sells a specified amount of the stock.
     * @param stock The name of the stock.
     * @param amount The amount of stock being sold.
     * @param os An output stream to indicate whether the stock 
     *        was sold or the stock could not be found.
     */
    void sellStock(const std::string& stock, const double& amount, 
                          std::ostream& os) {
        auto it = stockMap.find(stock); 
        if (it != stockMap.end()) {
            std::unique_lock<std::mutex> lock(stockMap[stock].mutex);
            stockMap[stock].balance += amount;
            stockMap[stock].condVar.notify_one();
            os << "Stock " << stock << "'s balance updated";
        } else {
            os << "Stock not found";
        }   
    }

    /**
     * Gets the status of a stock.
     * @param stock The name of the stock.
     * @param os An output stream to indicate the stock's status to.
     */
    void getStockStatus(const std::string& stock, std::ostream& os) {
        auto it = stockMap.find(stock); 
        if (it != stockMap.end()) {
            os << "Balance for stock " << stock << " = " << it->second.balance;
        } else {
            os << "Stock not found";
        }
    }
    
}  // namespace sm

/**
 * Reads HTTP headers and extracts the URL and discards any HTTP headers
 * in the inputs. 
 *
 * For example, if the 1st line of input is "GET
 * /http://localhost:8080/~raodm HTTP/1.1" then this method returns
 * "http://localhost:8080/~raodm"
 *
 * @return This method returns the path specified in the GET
 * request.
 */
std::string extractURL(std::istream& is) {
    std::string line, url;

    // Extract the GET request line from the input
    std::getline(is, line);
    
    // Read and skip HTTP headers. Without reading & skipping HTTP
    // headers.
    for (std::string hdr; std::getline(is, hdr) &&
             !hdr.empty() && hdr != "\r";) {}

    // Extract the URL that is delimited by space from the first line of input.
    std::istringstream(line) >> url >> url;
    return url.substr(1);
}

void processCmd(const std::string& cmd, std::ostream& os) {    
    std::string url = cmd;
    
    // A map to store all URL parameters.
    std::unordered_map<std::string, std::string> paramMap;
    
    // Parses the URL parameters and puts them in the map for later use.
    std::replace(url.begin(), url.end(), '&', ' ');
    std::istringstream ss(url);
    std::string param;
    while (ss >> param) {
        size_t pos = param.find('=');
        std::string key = std::string(param.begin(), param.begin() + pos);
        std::string val = std::string(param.begin() + pos + 1, param.end());
        paramMap[key] = val;
    }
    
    // Updates the stock based on the URL parameters.
    if (paramMap.at("trans") == "create") {
        auto it = paramMap.find("amount");
        sm::createStock(paramMap.at("stock"), 
            it == paramMap.end() ?  0: std::stod(paramMap.at("amount")), os);
    } else if (paramMap.at("trans") == "buy") {
       sm::buyStock(paramMap.at("stock"), 
            std::stod(paramMap.at("amount")), os);
    } else if (paramMap.at("trans") == "sell") {
        sm::sellStock(paramMap.at("stock"), 
            std::stod(paramMap.at("amount")), os); 
    } else if (paramMap.at("trans") == "status") {
        sm::getStockStatus(paramMap.at("stock"), os);
    }
}

/**
 * Process HTTP request that will modify a bank and provide suitable HTTP 
 * response back to the client. 
 * @param is The client's input stream which contains the HTTP request.
 * @param os The client's output stream which is where the HTTP response will 
 * be sent.
 * @param bank The bank that will be modified.
 */
void serveClient(std::istream& is, std::ostream& os) {    
    sm::threadCount++;

    // Gets the relative url.
    std::string url;
    url = url_decode(extractURL(is));
    
    std::ostringstream oss;
    processCmd(url, oss);
    
    // Gets the data that will be output in the HTTP response.
    std::string htmlData = oss.str();
    
    // Formats the http header with the correct length.
    std::string httpHeader = boost::str(boost::format(HTTPRespHeader) %
                               htmlData.length());

    // Sends the result to the client.
    os << httpHeader << htmlData;
    
    sm::threadCount--;
    sm::condVar.notify_one();
}

/**
 * Top-level method to run a custom HTTP server to process stock trade
 * requests using multiple threads. Each request should be processed
 * using a separate detached thread. This method just loops for-ever.
 *
 * \param[in] server The boost::tcp::acceptor object to be used to accept
 * connections from various clients.
 *
 * \param[in] maxThreads The maximum number of threads that the server
 * should use at any given time.
 */
void runServer(tcp::acceptor& server, const int maxThreads) {
    // Process client connections one-by-one...forever. This method
    // must use a separate detached thread to process each
    // request.
    while (true) {
        // Locks the thread and waits to ensure that there are less threads
        // running that the maxThreads amount.
        std::unique_lock<std::mutex> lock(sm::mutex);
        sm::condVar.wait(lock, 
                [maxThreads]{ return sm::threadCount < maxThreads; });
        auto client = std::make_shared<tcp::iostream>();
        server.accept(*client->rdbuf());
        
        // Creates the new thread.
        std::thread thr([client]() {
            serveClient(*client, *client);
        });
        thr.detach();
    }
    
    // Optional feature: Limit number of detached-threads to be <=
    // maxThreads.
}

//-------------------------------------------------------------------
//  DO  NOT   MODIFY  CODE  BELOW  THIS  LINE
//-------------------------------------------------------------------

/** Convenience method to decode HTML/URL encoded strings.
 *
 * This method must be used to decode query string parameters supplied
 * along with GET request.  This method converts URL encoded entities
 * in the from %nn (where 'n' is a hexadecimal digit) to corresponding
 * ASCII characters.
 *
 * \param[in] str The string to be decoded.  If the string does not
 * have any URL encoded characters then this original string is
 * returned.  So it is always safe to call this method!
 *
 * \return The decoded string.
*/
std::string url_decode(std::string str) {
    // Decode entities in the from "%xx"
    size_t pos = 0;
    while ((pos = str.find_first_of("%+", pos)) != std::string::npos) {
        switch (str.at(pos)) {
            case '+': str.replace(pos, 1, " ");
            break;
            case '%': {
                std::string hex = str.substr(pos + 1, 2);
                char ascii = std::stoi(hex, nullptr, 16);
                str.replace(pos, 3, 1, ascii);
            }
        }
        pos++;
    }
    return str;
}

// Helper method for testing.
void checkRunClient(const std::string& port, const bool printResp = false);

/*
 * The main method that performs the basic task of accepting
 * connections from the user and processing each request using
 * multiple threads.
 *
 * \param[in] argc This program accepts up to 2 optional command-line
 * arguments (both are optional)
 *
 * \param[in] argv The actual command-line arguments that are
 * interpreted as:
 *    1. First one is a port number (default is zero)
 *    2. The maximum number of threads to use (default is 20).
 */
int main(int argc, char** argv) {
    // Setup the port number for use by the server
    const int port   = (argc > 1 ? std::stoi(argv[1]) : 0);
    // Setup the maximum number of threads to be used.
    const int maxThr = (argc > 2 ? std::stoi(argv[2]) : 20);

    // Create end point.  If port is zero a random port will be set
    io_service service;    
    tcp::endpoint myEndpoint(tcp::v4(), port);
    tcp::acceptor server(service, myEndpoint);  // create a server socket
    // Print information where the server is operating.    
    std::cout << "Listening for commands on port "
              << server.local_endpoint().port() << std::endl;

    // Check and start tester client for automatic testing.
#ifdef TEST_CLIENT
    checkRunClient(argv[1]);
#endif

    // Run the server on the specified acceptor
    runServer(server, maxThr);
    
    // All done.
    return 0;
}

// End of source code
