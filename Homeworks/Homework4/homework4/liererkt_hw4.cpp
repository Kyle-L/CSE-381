/*
 * COMMENT HERE
 * 
 * File:   liererkt_hw4.cpp
 * Author: Kyle Lierer
 *
 * Copyright (C) 2020 liererkt@miamioh.edu
 * 
 */

#include "liererkt_hw4.h"

#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

void process(std::istream& is, 
        const std::string& prompt, bool parallel) {
    std::string line, cmd;
    ChildVec vec;
    while (std::cout << prompt, std::getline(is, line)) {
        istringstream ss(line);
        ss >> cmd;
        if (cmd == "exit") {
            return;
        } else if (cmd == "SERIAL") {
            serveClient(ss, false);
        } else if (cmd == "PARALLEL") {
            serveClient(ss, true);
        } else {
            ChildPair pair = execute(line);
            if (!parallel && pair.second != -1)
                std::cout << "Exit code: " << pair.first.wait() << std::endl; 
            else if (pair.second != -1)
                vec.push_back(pair.first);
        }
    }
    while (!vec.empty()) {
       std::cout << "Exit code: " << vec.front().wait() << std::endl; 
       vec.erase(vec.begin());
    }
}

ChildPair execute(std::string command, std::ostream& os) { 
    std::istringstream ss(command);
    
    // Format the command to be executed on the child process by formatting it
    // as a vector of strings.
    StrVec vec;
    for (std::string temp; ss >> std::quoted(temp);) {
        vec.push_back(temp);
    }
    
    // Creates a child process where the command where will be executed on.
    ChildProcess child;
    ChildPair pair;
    pair.first = child;
    
    
    // Exit if there is no command or it is a comment.
    if (vec.empty() || vec.at(0) == "#") {
        pair.second = -1;
        return pair;
    }
    
    // Print the current command being executed.
    os << "Running:";
    for (auto x : vec) {
        os << ' ' << x;
    }
    os << std::endl;
    
    // Execute the command on a forked child process.
    pair.second = child.forkNexec(vec);
    return pair;
}

std::string extractURL(std::istream& is) {
    std::string line, url;

    // Extract the GET request line from the input
    std::getline(is, line);
    
    // Read and skip HTTP headers.
    for (std::string hdr; std::getline(is, hdr) &&
             !hdr.empty() && hdr != "\r";) {}

    // Extract the URL that is delimited by space from the first line of input.
    std::istringstream(line) >> url >> url;
    return url.substr(1);
}

std::tuple<std::string, std::string, std::string>
breakDownURL(const std::string& url) {
    // The values to be returned.
    std::string hostName, port = "80", path = "/";
    
    // Extract the substrings from the given url into the above
    // variables.

    // First find index positions of sentinel substrings to ease
    // substring operations.
    const size_t hostStart = url.find("//") + 2;
    const size_t pathStart = url.find('/', hostStart);
    const size_t portPos   = url.find(':', hostStart);
    
    // Compute were the hostname ends. If we have a colon, it ends
    // with a ":". Otherwise the hostname ends with the last
    const size_t hostEnd   = (portPos == std::string::npos ? pathStart :
                              portPos);
    
    // Now extract the hostName and path and port substrings
    hostName = url.substr(hostStart, hostEnd - hostStart);
    path     = url.substr(pathStart);
    if (portPos != std::string::npos) {
        port = url.substr(portPos + 1, pathStart - portPos - 1);
    }
    
    // Return 3-values encapsulated into 1-tuple.
    return {hostName, port, path};
}

void serveClient(std::istream& is, bool parallel) {
    // Have helper method extract the URL for downloading data from
    // the input HTTP GET request.
    auto url = extractURL(is);

    // Next extract download URL components. That is, given a URL 
    std::string hostname, port, path;
    std::tie(hostname, port, path) = breakDownURL(url);

    // Start the download of the file (that the user wants to be
    // processed) at the specified URL.  We use a BOOST tcp::iostream.
    boost::asio::ip::tcp::iostream data(hostname, port);
    data << "GET "   << path     << " HTTP/1.1\r\n"
         << "Host: " << hostname << "\r\n"
         << "Connection: Close\r\n\r\n";

    // Skip the response header.
    for (std::string hdr; std::getline(data, hdr) && 
        !hdr.empty() && hdr != "\r";) {}
    
    process(data, "", parallel);
}

/*
 * 
 */
int main(int argc, char** argv) {    
    process();
    return 0;
}

