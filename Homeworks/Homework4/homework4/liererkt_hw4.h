/*
 * COMMENT HERE
 * 
 * File:   liererkt_hw4.h
 * Author: Kyle Lierer
 *
 * Copyright (C) 2020 liererkt@miamioh.edu
 * 
 */

#ifndef LIERERKT_HW4_H
#define LIERERKT_HW4_H

#include <iostream>
#include<string>
#include <fstream>
#include <vector>
#include <utility>
#include "ChildProcess.h"

using ChildVec = std::vector<ChildProcess>;
using ChildPair = std::pair<ChildProcess, int>;

void process(std::istream& is= std::cin, const std::string& prompt = "> ", 
        bool parallel = false);

ChildPair execute(std::string command, std::ostream& os = std::cout);

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
std::string extractURL(std::istream& is);

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
breakDownURL(const std::string& url);


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
void serveClient(std::istream& is, bool parallel);

#endif /* LIERERKT_HW4_H */

