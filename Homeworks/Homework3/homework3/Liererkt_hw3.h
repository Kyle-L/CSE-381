/*
 * A program to print the full hierarchy of processes for a given PID 
 * (process ID, specified as the 2nd command-line argument). The data about 
 * processes is read from a given text file (as 1st command-line argument).
 * 
 * File:   liererkt_hw3.h
 * Author: Kyle Lierer
 *
 * Copyright (C) 2020 liererkt@miamioh.edu
 * 
 */

#ifndef LIERERKT_HW3_H
#define LIERERKT_HW3_H

#include<string>
#include <fstream>
#include <unordered_map>

class Solution {
public:
    /**
     * Constructs an instance of Solution.
     */
    Solution();
    
    /**
     * Destructs the instance of Solution when it goes out of scope.
     */
    ~Solution();
    
    /**
     * Loads a process tree into memory.
     * @param in A file stream of the processes.
     * @return 
     */
    void LoadTree(std::ifstream& in);
    
    /**
     * Recursively prints the full hierarchy of processes (in top-down order) 
     * for a given PID of the in memory process tree to an ostream.
     * @param out An output stream where the hierarchy will be printed
     * @param pid The PID where the top-down hierarchy will end.
     */
    void PrintTree(std::ostream& out, const int& pid);
    
private:
    std::unordered_map<int, int> parentMap;
    std::unordered_map<int, std::string> commandMap;
};

#endif /* LIERERKT_HW3_H */

