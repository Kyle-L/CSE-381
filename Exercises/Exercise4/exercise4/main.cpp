/**
 * A simple main file with methods to test the operation of the
 * ChildProcess class.  This program also tests the operations of the
 * runProcesses() method implemented in the serial_vs_parallel.cpp
 * source file.
 *
 * Copyright (C) 2020 raodm@miamiOH.edu
 */

#include <iostream>
#include "ChildProcess.h"

// Prototype declaration for the runProcess() method
void runProcesses();

//-------------------------------------------------------------------
//   DO  NOT  MODIFY  THIS FILE
//-------------------------------------------------------------------

/**
 * The main method creates an object and tests the operation of the
 * ChildProcess class.
 * 
 */
int main() {
    // Create an object.
    ChildProcess cp;
    // Call the forkNexec method to run a program.
    cp.forkNexec({"cat", "data.txt"});
    // Wait for child process to finish
    std::cout << "Exit code: " << cp.wait() << std::endl;

    // Run an invalid command to ensure student's implementation
    // correctly returns exit code.
    cp.forkNexec({"cat", "/blah"});
    std::cout << "Exit code: " << cp.wait() << std::endl;

    // Once you have tested the above functionality, copy-paste the
    // runProcesses() method and modify it to run programs in
    // parallel.
    std::cout << "Calling runProcesses()...\n";
    runProcesses();
    std::cout << "runProcesses() completed.\n";

    // All done!
    return 0;
}
