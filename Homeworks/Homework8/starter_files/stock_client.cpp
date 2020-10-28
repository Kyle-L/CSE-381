/*
 * Copyright (C) 2020 raodm@miamiOH.edu
 *
 * A multithreaded client to test operations of a web-server.  This
 * program generates concurrent requests to a given web-server to test
 * its operations.  The operations it performs are specified as an
 * input file.
 *
 * Earlier, this was a very simple program that had to bloat into a
 * bigger one because: This client now has many checks to help
 * troubleshoot students program.  However, the checks are not
 * fool-proof as students come-up with very creative ways to break
 * their programs.  
 */

#include <boost/asio.hpp>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <memory>
#include <fstream>
#include <iomanip>
#include <utility>
#include <vector>
#include <chrono>

// Convenience namespaces used in this source file.
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std::literals::chrono_literals;

// A list of request & expected response pairs of strings
using ReqRespList = std::vector<std::pair<std::string, std::string>>;

/** Helper method to read the actual 1-line response from the server
    and check to ensure its length match with content-length.
*/
std::string getMessageFromServer(std::istream& is, size_t contentLen,
                                 std::string resp, const bool printResp) {
    // Get the message at the end of the HTTP response.
    std::string msg, line;
    resp.append("\n");
    while (std::getline(is, line)) {
        msg += line;  // Accummulate response.
        resp.append(line).append("\n");
    }

    // Print the whole message to help students troubleshoot their
    // response.
    if (printResp) {
        std::cout << resp << "--------------------------\n";
    }

    // Do sanity checks to help students troubleshoot issues.
    if (msg.size() != contentLen) {
        std::cerr << "Invalid content length (contentLen = " << contentLen
                  << ", msg='" << msg << "') reported by server!\n"
                  << "Full response = " << resp << std::endl;
    }

    // Return message for further processing
    return msg;
}

/**
 * Helper method to extract the response message from the HTTP
 * response.
 */
std::string getResponse(std::istream& is, const bool printResp = false) {
    std::string line, resp;
    size_t contentLen = -1;  // set in loop below
    while (std::getline(is, line) && (line != "\r")) {
        if (line.back() != '\r') {
            std::cout << "Response from server did not have \\r\\n\n";
        } else {
            // Remove '\r' as we don't print it in the full response.
            line.pop_back();  
        }
        // Process content-length header to get length of response
        if (line.substr(0, 16) == "Content-Length: ") {
            contentLen = std::stoi(line.substr(16));
        }
        resp.append(line).append("\n");
    }
    if (!is.good()) {
        std::cerr << "Data from server abruptly stopped! "
                  << "Are you reading all request headers?\n";
    }

    // Get the message at the end of the HTTP response, validate it,
    // and return the message for further checks.
    return getMessageFromServer(is, contentLen, resp, printResp);
}

/** Helper method to send a request to the server and process the
    response from the server.
*/
void processRequest(std::string port, const std::string& request,
                    const std::string& expectedResult,
                    const bool printResp = false) {
    ip::tcp::iostream server("localhost", port);
    if (!server) {
        // Can't connect to server
        std::cout << "Error connecting to server on port " << port << std::endl;
        return;
    }
    // First send the request to the server.
    server << "GET /" << request << " HTTP/1.1\r\n"
           << "Host: localhost:" << port << "\r\n"
           << "Connection: close\r\n\r\n";
    // Get the first line and ensure it is HTTP 200 OK.
    std::string line;
    if (std::getline(server, line), line != "HTTP/1.1 200 OK\r") {
        std::cerr << "Invalid header line from server ('" << line << "')\n";
    } else {
        const std::string msg = getResponse(server, printResp);
        // Read rest of the message from the client.
        if (msg != expectedResult) {
            std::cerr << "Invalid msg from server. Expected: '"
                      << expectedResult << "' but got '"
                      << msg << "'\n"
                      << "The request was: " << request << std::endl;
        }
    }
}

/**
 * Run requests using 1 or more threads.
 *
 * \param[in] port The port number to be use to communicate with the server.
 *
 * \param[in] reqRespList The list of requests and corresponding
 * responses expected from the server.
 */
void runRequests(const std::string& port, const ReqRespList& reqRespList,
                 const int numThreads, const bool printResp = false,
                 const bool nowait = false) {
    // Submit requests to the server in batches of size 'numThreads'
    for (size_t stReq = 0; (stReq < reqRespList.size()); stReq += numThreads) {
        // Sumit a batch of requests to the server.
        std::vector<std::thread> thrList;
        for (int thr = 0; (thr < numThreads) &&
                 (stReq + thr < reqRespList.size()); thr++) {
            const int currReq = stReq + thr;  // Find index for this thread
            const std::string req  = reqRespList[currReq].first;
            const std::string resp = reqRespList[currReq].second;
            // Create a thread to submit request & validate response
            thrList.push_back(std::thread(processRequest, port, req,
                                          resp, printResp));
        }

        // If we should not wait for requests to finish then we simply
        // place the threads in background.
        for (auto& t : thrList) {
            nowait ? t.detach() : t.join();
        }
    }
}

/**
 * Check the number of running threads in the program.
 *
 * \param[in] pid The process id for a given process/thread.
 *
 * \param[in] numThr The expected number of threads.
 */
void checkThreads(const int pid, const int numThr) {
    // Give the system 1/2 second to update stats
    std::this_thread::sleep_for(500ms);
    const std::string statFile = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream stat(statFile);
    // To get the number of threads, skip over first 19-words
    std::string dummy;
    for (int i = 0; (i < 19) && (stat >> dummy); i++) {}
    // Now read and print the number of threads
    int thrCount = -1;
    stat >> thrCount;
    if (thrCount == numThr) {
        std::cout << "Expected thread count matched (" << numThr << ")\n";
    } else {
        std::cout << "Thread count mismatch (expected: " << numThr
                  << ", running: " << thrCount << ")\n";
    }
}

/**
 * Helper method to read line-by-line of transaction and expected
 * response and send it to server for testing.
 */
void processInputCmds(std::istream& input, const std::string& port,
                      const bool printResp = false) {
    // Read line-by-line of request-response pairs until a "run"
    // command is countered.
    std::string req, resp;   // request, response.
    ReqRespList testData;    // List of operations for testing.
    int block = 0;           // Just block number for progress reporting
    // Read line-by-line of input and process it.
    while (input >> std::quoted(req)) {
        if (req == "run" || req == "nowait") {
            // Load #threads and #repetitions from input
            int thrs, reps;
            input >> thrs >> reps;
            for (int rep = 0; (rep < reps); rep++) {
                runRequests(port, testData, thrs, printResp,
                            (rep == reps - 1) && (req == "nowait"));
            }
            testData.clear();  // Clear out this batch of tests.
            std::cout << "Finished block #" << block++ << " testing phase.\n";
        } else if (req == "chkThr") {
            // Check the number of running threads
            int numThr = -1;
            input >> numThr;
            checkThreads(getpid(), numThr);
        } else {
            // Read response and add to pending list of tests.
            input >> std::quoted(resp);
            testData.push_back({req, resp});
        }
    }    
    std::cout << "Testing completed.\n";
}

#ifndef TEST_CLIENT

void checkRunClient(const std::string& port)  {}

/**
 * The main method just checks to ensure necessary command-lien
 * arguments are specified and then 
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Specify InputFile and ServerPort\n";
        return 1;
    }
    // Open the input file to be used for testing.
    std::ifstream input(argv[1]);
    if (!input.good()) {
        std::cerr << "Unable to open input file: '" << argv[1] << ".\n";
        return 2;
    }

    // Check and process optional parameter to print response
    const bool printResp = (argc > 3);
    // Have helper method process input commands from file
    processInputCmds(input, argv[2], printResp);
    // All done.
    return 0;
}

#else 

void runClientThread(const std::string port, const bool printResp = false) {
    std::string inputFile = getenv("TEST_FILE");
    std::ifstream input(inputFile);
    if (!input.good()) {
        std::cerr << "Unable to open input file: '" << inputFile << ".\n";
        exit(0);
    }
    // Wait for a few seconds to let the main thread spin-up
    std::this_thread::sleep_for(100ms);
    processInputCmds(input, port, printResp);
    std::this_thread::sleep_for(100ms);
    if (getenv("CLIENT_ABORT")) {
        abort();
    }
    exit(0); 
}

void checkRunClient(const std::string& port, const bool printResp = false)  {
    std::thread client(runClientThread, port, printResp);
    client.detach();
}

#endif
