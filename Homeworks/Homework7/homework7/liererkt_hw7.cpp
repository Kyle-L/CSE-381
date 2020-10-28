/* 
 * A simple Banking-type web-server.  
 * 
 * This multithreaded web-server performs simple bank transactions on
 * accounts.  Accounts are maintained in an unordered_map.  
 * 
 * File:   liererkt_hw7.cpp
 * Author: Kyle Lierer
 *
 * Copyright (C) 2020 liererkt@miamiOH.edu
 *  
 */

// All the necessary includes are present
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

// Forward declaration for method defined further below
std::string url_decode(std::string);

/*
 * Document this class. Use it for encapsulating all of your data,
 * including a std::mutex to avoid race conditions. No, none of the
 * instance variables should be static.
 */
class Bank {
public:
    /**
     * Clears the bank map and outputs a message indicating it has been reset.
     * @param os
     */
    void resetBank(std::ostream& os) {
        std::lock_guard<std::mutex> guard(mutex);
        bank.clear();
        os << "All accounts reset";
    }
    
    /**
     * Creates an account in the bank with a default balance of $0.00
     * @param account
     * @param os
     */
    void createAcct(const std::string& account, std::ostream& os) {
        std::lock_guard<std::mutex> guard(mutex);
        if (bank.find(account) == bank.end()) {
            bank[account] = 0;
            os  << "Account " << account << " created";
        } else {
            os  << "Account " << account << " already exists";
        }
    }
    
    /**
     * Adds a balance to an account in the bank and outputs a message 
     * indicating the changes.
     * @param account
     * @param balance
     * @param os
     */
    void modifyBalance(const std::string& account, const double& balance, 
                            std::ostream& os) {
        std::lock_guard<std::mutex> guard(mutex);
        auto find = bank.find(account);
        if (find != bank.end()) {
            find->second += balance;
            os << "Account balance updated";
        } else {
            os << "Account not found";
        }
    }
    
    /**
     * Outputs a message indicating the status of an account in the bank.
     * @param account
     * @param os
     */
    void getStatus(const std::string& account, std::ostream& os) {
        auto find = bank.find(account);
        if (find != bank.end()) {
            os  << "Account " << account << ": $" << std::fixed 
                << std::setprecision(2) << find->second;
        } else {
            os << "Account not found";
        }
    }
    
private:
    std::mutex mutex;
    std::unordered_map<std::string, double> bank;    
};

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

/**
 * Processes URL parameters in order to modify a bank and return the 
 * modifications to an output stream. 
 * 
 * For example.
 * trans=create&acct=0x01
 * Would create a new account '0x01' with a starting balance of $0.00.
 * 
 * @param urlParams Parameters in URL format.
 * @param os An output stream to return the modifications to.
 * @param bank A bank that will be modified.
 */
void process(const std::string& urlParams, std::ostream& os, Bank& bank) {    
    std::string url = urlParams;
    
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
    
    // Updates the bank based on the URL parameters.
    if (paramMap.at("trans") == "reset") {
        bank.resetBank(os);
    } else if (paramMap.at("trans") == "create") {
        bank.createAcct(paramMap.at("acct"), os);
    } else if (paramMap.at("trans") == "credit") {
        bank.modifyBalance(paramMap.at("acct"), 
                std::stod(paramMap.at("amount")), os);
    } else if (paramMap.at("trans") == "debit") {
        bank.modifyBalance(paramMap.at("acct"), 
                -std::stod(paramMap.at("amount")), os);
    } else if (paramMap.at("trans") == "status") {
        bank.getStatus(paramMap.at("acct"), os);
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
void serveClient(std::istream& is, std::ostream& os, Bank& bank) {
    // Gets the relative url.
    std::string url;
    url = url_decode(extractURL(is));
    
    std::ostringstream oss;
    process(url, oss, bank);
    
    // Gets the data that will be output in the HTTP reponse.
    std::string htmlData = oss.str();
    
    // Formats the http header with the correct length.
    std::string httpHeader = boost::str(boost::format(HTTPRespHeader) %
                               htmlData.length());

    // Sends the result to the client.
    os << httpHeader << htmlData;
}

/**
 * Top-level method to run a custom HTTP server to process bank
 * transaction requests using multiple threads. Each request should
 * be processed using a separate detached thread. This method just loops 
 * for-ever.
 *
 * @param server The boost::tcp::acceptor object to be used to accept
 * connections from various clients.
 */
void runServer(tcp::acceptor& server) {
    // The bank object shared by all the threads!
    Bank myBank;
    
    // Accepts client connections and creates a new thread for each
    // new connection.
    while (true) {
        auto client = std::make_shared<tcp::iostream>();
        server.accept(*client->rdbuf());
        std::thread thr([client, &myBank]() {
            serveClient(*client, *client, myBank);
        });
        thr.detach();
    }
}

//-------------------------------------------------------------------
//  DO  NOT   MODIFY  CODE  BELOW  THIS  LINE
//-------------------------------------------------------------------

/** Convenience method to decode HTML/URL encoded strings.

    This method must be used to decode query string parameters
    supplied along with GET request.  This method converts URL encoded
    entities in the from %nn (where 'n' is a hexadecimal digit) to
    corresponding ASCII characters.

    \param[in] str The string to be decoded.  If the string does not
    have any URL encoded characters then this original string is
    returned.  So it is always safe to call this method!

    \return The decoded string.
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
void checkRunClient(const std::string& port);

/*
 * The main method that performs the basic task of accepting
 * connections from the user and processing each request using
 * multiple threads.
 */
int main(int argc, char** argv) {
    // Setup the port number for use by the server
    const int port = (argc > 1 ? std::stoi(argv[1]) : 0);
    io_service service;
    // Create end point.  If port is zero a random port will be set
    tcp::endpoint myEndpoint(tcp::v4(), port);
    tcp::acceptor server(service, myEndpoint);  // create a server socket
    // Print information where the server is operating.    
    std::cout << "Listening for commands on port "
              << server.local_endpoint().port() << std::endl;
    // Check run tester client.
#ifdef TEST_CLIENT
    checkRunClient(argv[1]);
#endif

    // Run the server on the specified acceptor
    runServer(server);
    
    // All done.
    return 0;
}

// End of source code

