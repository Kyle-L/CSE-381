/** 
 * A custom web-server that can process HTTP GET requests and:
 *    1. Respond with contents of a given HTML file
 *    2. Run a specified program and return the output from the process.
 *
 * Copyright (C) 2020 raodm@miamioh.edu.
 */

#include <boost/asio.hpp>
#include <string>
#include "HTTPFile.h"
#include "ChildProcess.h"

// Convenience namespace to streamline the code below.
using namespace boost::asio;
using namespace boost::asio::ip;

// Forward declaration for url_decode method defined below.  We need
// this just to organize the starter code into sections that students
// should not modify (to minimize problems for students).
std::string url_decode(std::string url);

// Forward declaration for serveClient that is defined further below.
void serveClient(std::istream& is, std::ostream& os);

/**
 * Runs the program as a server that listens to incoming connections.
 * 
 * @param port The port number on which the server should listen.
 */
void runServer(int port) {
    // Setup a server socket to accept connections on the socket
    io_service service;
    // Create end point
    tcp::endpoint myEndpoint(tcp::v4(), port);
    // Create a socket that accepts connections
    tcp::acceptor server(service, myEndpoint);
    std::cout << "Server is listening on "
              << server.local_endpoint().port()
              << " & ready to process clients...\n";
    // Process client connections one-by-one...forever
    while (true) {
        auto client = std::make_shared<tcp::iostream>();
        server.accept(*client->rdbuf());
        std::thread thr([client]() {
            serveClient(*client, *client);
        });
        thr.detach();
    }
}

void sendCmdOutput(std::string& cmd, std::ostream& os) {
    // Remove the "/cgi-bin/exec?cmd=" prefix to help process the
    // command.
    cmd = url_decode(cmd.substr(18));
    // Split the command into words for processing using helper method
    // in ChildProcess.
    const StrVec args = ChildProcess::split(cmd);
    // Create the child process.
    ChildProcess cp;
    cp.forkNexecIO(args);
    // Get the outputs from the child process.
    std::istream& is = cp.getChildOutput();
    // Send the output from child process to the client as chunked response
    os << http::DefaultHttpHeaders << "text/plain\r\n\r\n";
    // Send each line as a spearate chunk.
    for (std::string line; std::getline(is, line);) {
        line += "\n";  // Add the newline that was not included by getline
        os << std::hex << line.size() << "\r\n" << line << "\r\n";
    }
    // Finally send the trailing end-of-stream chunk
    os << "0\r\n\r\n";
}

/**
 * Process HTTP request (from first line & headers) and
 * provide suitable HTTP response back to the client.
 * 
 * @param is The input stream to read data from client.
 *
 * @param os The output stream to send data to client.
 */
void serveClient(std::istream& is, std::ostream& os) {
    // Read headers from client and print them. This server
    // does not really process client headers
    std::string line, path;
    // Read the "GET" word and then the path.
    is >> line >> path;
    // Skip/ignore all the HTTP request & headers for now.
    while (std::getline(is, line) && (line != "\r")) {}
    // Check and dispatch the request appropriately
    if (path.find("/cgi-bin/exec?cmd=") == 0) {
        // This is a command to be processed. So use a helper method
        // to streamline the code.
        sendCmdOutput(path, os);
    } else if (!path.empty()) {
        // In this case we assume the user is asking for a file.  Have
        // the helper http class do the processing.
        path = path.substr(1);  // Remove the leading '/' sign.
        // Use the http::file helper method to send the response back
        // to the client.
        os << http::file(path);
    }
}

//------------------------------------------------------------------
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//------------------------------------------------------------------

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

/**
 * The main function that serves as a test harness based on
 * command-line arguments.
 *
 * \param[in] argc The number of command-line arguments.  This test
 * harness can work with zero or one command-line argument.
 *
 * \param[in] argv The actual command-line arguments.  If this is an
 * number it is assumed to be a port number.  Otherwise it is assumed
 * to be an file name that contains inputs for testing.
 */
int main(int argc, char *argv[]) {
    // Check and use first command-line argument if any as port or file
    std::string arg = (argc > 1 ? argv[1] : "0");
    
    // Check and use a given input data file for testing.
    if (arg.find_first_not_of("1234567890") == std::string::npos) {
        // All characters are digits. So we assume this is a port
        // number and run as a standard web-server
        runServer(std::stoi(arg));
    } else {
        // In this situation, this program processes inputs from a
        // given data file for testing.  That is, instead of a
        // web-browser we just read inputs from a data file.
        std::ifstream getReq(arg);
        if (!getReq.good()) {
            std::cerr << "Unable to open " << arg << ". Aborting.\n";
            return 2;
        }
        // Have the serveClient method process the inputs from a given
        // file for testing.
        serveClient(getReq, std::cout);
    }
    // All done.
    return 0;
}
