/**
 * A simple web-client to:
 *   1. Download numbers from a web-server.
 *   2. Print some statistics about the nubers.
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <boost/asio.hpp>
#include <boost/format.hpp>

/** A convenience format string to generate results in HTML
    format. Note that this format string has place holders in the form
    %1%, %2% etc.  These are filled-in with actual values.  For
    example, you can generate actual values as shown below:

    \code

    int sum = 10, count = 5;
    float avg = sum * 1f / count;

    auto html = boost::str(boost::format(HTMLData) % sum %
                           count % avg);

    \endcode
*/
const std::string HTMLData = R"(<html>
  <body>
    <h2>Analysis results</h2>
    <p>Number of values: %1%</p>
    <p>Sum of numbers: %2%</p>
    <p>Average value: %3%</p>
  </body>
</html>
)";


// TODO: Add Javadoc style documentation for this method. NetBeans
// will generate a template if you start the documentation correctly.
void process(std::istream& is, std::ostream& os) {
    int count = 0;  // word or number count
    int sum   = 0;  // Sum of numbers (to compute average)

    // TODO: Document Why we need the loop below. Print hdr to see
    // what they are.
    for (std::string hdr;std::getline(is, hdr) && !hdr.empty() && hdr != "\r";) {}    

    // TODO: Document what this for-loop does.
    for (int val; (is >> val); count++, sum += val)
        {
        }
    
    const float avg = sum * 1.f / count;
    // TODO: Document what the following line does.
    auto htmlData = boost::str(boost::format(HTMLData) %
                               count % sum % avg);
    // Generate the response.
    os << htmlData;
}

/**
 * The main function that serves as a test harness based on
 * command-line arguments.
 *
 * \param[in] argc The number of command-line arguments.  Here this
 * value is ignored.
 *
 * \param[in] argv The actual command-line arguments passed to this
 * method.
 */
int main(int argc, char *argv[]) {
    // Here we generate an HTTP get request similar to how a browser
    // would for a hardcoded url: http://www.users.miamioh.edu/raodm/nums.txt
    const std::string path = "/raodm/nums.txt";
    const std::string host = "www.users.miamioh.edu";
    const std::string port = "80";

    // Here we use BOOST (is a popular C++ library) to create a TCP
    // (TCP is network protcol) connection to the web-server at port 80/
    using namespace boost::asio::ip;
    tcp::iostream data(host, port);
    
    // Start the download of the file (that the user wants to be
    // processed) at the specified URL by sending a simple GET request
    // to the web-server.
    data << "GET "   << path << " HTTP/1.1\r\n"
         << "Host: " << host << "\r\n"
         << "Connection: Close\r\n\r\n";
    // Have the helper method process the file's data and print/send
    // results (in HTTP/HTML format) to a given output stream.
    process(data, std::cout);
}
