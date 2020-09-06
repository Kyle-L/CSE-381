
/* 
 * File:   ex3_1.cpp
 * Author: DJ Rao
 *
 * Copyright (C) 2020 raodm@miamiOH.edu
 */

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

/*
 * A simple main function to demonstrate the operations of fork
 * and understand how Virtual Memory behaves -- memory addresses of variables
 * will be the same. But the variables are mapped to different physical
 * (or real) addresses. Hence they can store different information or data.
 */
int main() {
    // Create a child process
    int retVal = fork();
    // Print information in both parent & child
    if (retVal == 0) {
        // In child (because return value of fork was zero)
        std::cout << "In child process (pid= " << getpid() << ")\n"
                  << "  retVal (@address: " << &retVal << ") = "
                  << retVal << std::endl;
    } else {
        // In parent (because return value of fork was non-zero)
        sleep(1);  // sleep for 1 second to let child print first
        std::cout << "In parent process (pid= " << getpid() << ")\n"
                  << "  retVal (@address: " << &retVal << ") = " 
                  << retVal << std::endl;;
        // It is important to wait for child to finish. We ignore exit code
        // of child because we pass nullptr to 2nd parameter of waitpid below.
        waitpid(retVal, nullptr, 0);
    }
    return 0;
}

