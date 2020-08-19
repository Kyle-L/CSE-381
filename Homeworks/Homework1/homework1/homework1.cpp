/**
 * A custom web-server that performs some simple Natural Language
 * Processing (NLP) on data from a given URL.
 *
 * Copyright (C) 2020 liererkt@miamioh.edu
 * 
 */

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iomanip>

/** A convenience format string to generate results in HTML
    format. Note that this format string has place holders in the form
    %1%, %2% etc.  These are filled-in with actual values.  For
    example, you can generate actual values as shown below:

    \code

    int wc = 10, totChars = 20;
    float avgWordLen = 4.25;

    auto html = boost::str(boost::format(HTMLData) % wc %
                           totChars % avgWordLen);

    \endcode
*/
const std::string HTMLData = R"(<html>
  <body>
    <h2>Analysis results</h2>
    <p>Number of words: %1%</p>
    <p>Total number of characters: %2%</p>
    <p>Average word length: %3% chars/word</p>
  </body>
</html>
)";

/** The HTTP response header to be sent to the client.

    Note that this format string has a place holders in the form %1%
    for the length of the data being sent back to the client.  This
    value can be filled-in as shown below:

    \code

    std::strind data = "Some data to be sent";
    auto header = boost::str(boost::format(HTTPRespHeader) % data.length());

    \endcode
*/
const std::string HTTPRespHeader =
    "HTTP/1.1 200 OK\r\n"
    "Server: localhost\r\n"
    "Connection: Close\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %1%\r\n\r\n";

/**
 * It assumes the input stream is an HTTP GET request (hence it is
 * important to understand the input format before implementing this
 * method).  This method extracts the URL to be processed from the 1st
 * line of the input stream.
 *
 * IMPORTANT --- This method must read and discards any HTTP headers
 * in the inputs.  If you don't do this your program will not work
 * correctly with web-browsers.
 *
 * For example, if the 1st line of input is "GET
 * /http://localhost:8080/~raodm HTTP/1.1" then this method returns
 * "http://localhost:8080/~raodm"
 *
 * \note See earlier exercise(s) for hints for implementing this method.
 *
 * @return This method returns the path specified in the GET
 * request.
 */
std::string extractURL(std::istream& is) {
    // Extract the GET request line from the input
    std::string url;

    // Read and skip HTTP headers. Without reading & skipping HTTP
    // headers, your program will not work correctly with
    // web-browsers.
    is >> url >> url;
    
    // Do basic substring operation to extract the URL that is
    // delimited by space from the first line of input.
    url = url.substr(1);

    return url;
}

/**
 * Helper method to break down a URL into hostname, port and path. For
 * example, given the url: "https://localhost:8080/~raodm/one.txt"
 * this method returns <"localhost", "8080", "/~raodm/one.txt">
 *
 * Similarly, given the url: "ftp://ftp.files.miamioh.edu/index.html"
 * this method returns <"ftp.files.miamioh.edu", "80", "/index.html">
 *
 * @param url A string containing a valid URL. The port number in URL
 * is always optional.  The default port number is assumed to be 80.
 *
 * @return This method returns a std::tuple with 3 strings. The 3
 * strings are in the order: hostname, port, and path.  Here we use
 * std::tuple because a method can return only 1 value.  The
 * std::tuple is a convenient class to encapsulate multiple return
 * values into a single return value.
 */
std::tuple<std::string, std::string, std::string>
breakDownURL(const std::string& url) {
    // The values to be returned.
    std::string hostName, port = "80", path = "/";
    
    // Copies the url to make parsing easier.
    std::string new_url = url;
     
    // Extract the substrings from the given url into the above
    // First, remove the http/https from the url.
    size_t httpPos = new_url.find_first_of("://");
    if (httpPos != std::string::npos) {
        new_url = new_url.substr(httpPos + 3);
    }
    
    // Find the positions of the port and path delims in order to establish
    // where the port and path are in the url.
    size_t portPos = new_url.find_first_of(":");
    size_t pathPos = new_url.find_first_of("/");
    
    // Parse the port and hostName.
    if (portPos != std::string::npos) {
        port = new_url.substr(portPos + 1, pathPos - portPos - 1);
        hostName = new_url.substr(0, portPos);
    } else {
        hostName = new_url.substr(0, pathPos);
    }
    
    // Parse the path.
    path = new_url.substr(pathPos);
    
    // Return 3-values encapsulated into 1-tuple.
    return {hostName, port, path};
}

/**
 * Process HTTP response data obtained from one web-server and send
 * results (as HTTP response) the web-browser.
 *
 * \param[in] is The input stream from where the HTTP response is to
 * be read and the number of words are to be counted. This method
 * should also compute the total number of characters in the words. In
 * addition, it should also compute average number of characters per
 * word.
 *
 * \param[out] os The output stream to where HTTP response along with
 * HTML data is to be sent back to the client.
 */
void process(std::istream& is, std::ostream& os) {
    // The following loops skips over the HTTP response header.
    // Note: This makes the assumption that the HTTP response was 200 OK.
    for (std::string hdr; std::getline(is, hdr) && 
        !hdr.empty() && hdr != "\r";) {}
    

    // Goes through the payload an calculates the word count, character count, 
    // and average.
    int wordCount = 0, charCount = 0;
    for (std::string line; is >> line; wordCount++) {
        charCount += line.length();
    }
    const float avg = charCount * 1.f / wordCount;

    // First use the predefined constant HTMLData, to generate the
    // HTML with wordCount, charCount, and average so that the length can be 
    // determined before constructing the HTTP header.
    std::string htmlData = boost::str(boost::format(HTMLData) %
                               wordCount % charCount % avg);
    
    // Formats the http header with the correct length using the HTML 
    // data length.
    std::string httpHeader = boost::str(boost::format(HTTPRespHeader) %
                               htmlData.length());
    
    std::string result = httpHeader + htmlData;
    
    // Generate the response.
    os << result;
}

//-------------------------------------------------------------------------
//  STUDY THE CODE BELOW.
//  BUT DO  NOT  MODIFY  CODE  BELOW  THIS  LINE.
//-------------------------------------------------------------------------

/**
 * Helper method to process HTTP request from a web-browser and send
 * response back.  This method operates in the following manner:
 *
 *  1. It uses the extractURL method to get the URL of the data file
 *     to be processed from the GET request.
 *
 *  2. It then uses he breakDownURL method to obtain the host, port,
 *     and path of the data file to be processed. the GET request from
 *     the URL
 *
 *  3. It the 
 */
void serveClient(std::istream& is = std::cin, std::ostream& os = std::cout) {
    // Have helper method extract the URL for downloading data from
    // the input HTTP GET request.
    auto url = extractURL(is);
    std::cout << "URL to be processed is: " << url << std::endl;

    // Next extract download URL components. That is, given a URL
    // "http://www.users.miamioh.edu/raodm/ones.txt", 
    std::string hostname, port, path;
    std::tie(hostname, port, path) = breakDownURL(url);
    std::cout << "Processing file " << std::quoted(path) << " from "
              << std::quoted(hostname) << ":" << std::quoted(port) << " ...\n";

    // Start the download of the file (that the user wants to be
    // processed) at the specified URL.  We use a BOOST tcp::iostream.
    boost::asio::ip::tcp::iostream data(hostname, port);
    data << "GET "   << path     << " HTTP/1.1\r\n"
         << "Host: " << hostname << "\r\n"
         << "Connection: Close\r\n\r\n";
    // Have the helper method process the file's data and print/send
    // results (in HTTP/HTML format) to a given output stream.
    process(data, os);
}

/**
 * The main function that serves as a test harness based on
 * command-line arguments.  This approach is used for web-based
 * solutions because testing via a web-browser will not work if there
 * are even 1-character differences in output.
 *
 * \param[in] argc The number of command-line arguments.  This test
 * harness can work with zero or one command-line argument.
 *
 * \param[in] argv The actual command-line arguments.
 */
int main(int argc, char *argv[]) {
    // Check and use a given input data file for testing.
    if (argc == 2) {
        // In this situation, this program processes inputs from a
        // given data file for testing.  That is, instead of a
        // web-browser we just read inputs from a data file.
        std::ifstream getReq(argv[1]);
        if (!getReq.good()) {
            std::cerr << "Unable to open " << argv[1] << ". Aborting.\n";
            return 2;
        }
        // Have the serveClient method process inputs from a given
        // file for testing.
        serveClient(getReq);
        return 0;
    }

    // If a command-line argument has not been specified, we operate
    // as a web-server that can accept and process 1 request from an
    // actual web-browser.
    using namespace boost::asio;
    using namespace boost::asio::ip;

    // Create a server socket to accept connections over the Internet
    // from a web-browser.
    io_service service;
    tcp::acceptor server(service, tcp::endpoint(tcp::v4(), 0));
    server.listen();
    // Print port number so that the user knows which port the
    // operating system has assigned to this program.
    std::cout << "Server is listening on port "
              << server.local_endpoint().port() << std::endl;

    // Accept connection from a web-browser
    tcp::iostream browser;
    server.accept(*browser.rdbuf());
    // Have the serveClient method do the processing by reading inputs
    // from the client and sending results back to the client.
    serveClient(browser, browser);
    return 0;  // Successful run.
}
