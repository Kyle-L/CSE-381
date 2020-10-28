/* 
 * A simple online stock exchange web-server.  
 * 
 * This multithreaded web-server performs simple stock trading
 * transactions on stocks.  Stocks must be maintained in an
 * unordered_map.
 *
 */

// The commonly used headers are included.  Of course, you may add any
// additional headers as needed.
#include <boost/asio.hpp>
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

// Shortcut to smart pointer with TcpStream
using TcpStreamPtr = std::shared_ptr<tcp::iostream>;

// Forward declaration for methods defined further below
std::string url_decode(std::string);

// The name space to hold all of the information that is shared
// between multiple threads.
namespace sm {
    // Unordered map including stock's name as the key (std::string)
    // and the actual Stock entry as the value.
    std::unordered_map<std::string, Stock> stockMap;

}  // namespace sm

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
    //
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
