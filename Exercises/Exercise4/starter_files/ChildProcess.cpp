#ifndef CHILD_PROCESS_CPP
#define CHILD_PROCESS_CPP

/**
 * This source file contains the implementation for the various
 * methods defined in the ChildProcess class.
 *
 */

// All the necessary #includes are already here
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <vector>
#include "ChildProcess.h"

/** NOTE: Unlike Java, C++ does not require class names and file names
 * should match.  Hence when defining methods pertaining to a specific
 * class, the class name should be prefixed, as in:
 *      ____________
 *      ChildProcess::methodName()
 */

// This method is just a copy-paste from lecture notes. This is done
// to illustrate an example.
void
ChildProcess::myExec(StrVec argList) {
    std::vector<char*> args;    // list of pointers to args
    for (auto& s : argList) {
        args.push_back(&s[0]);  // address of 1st character
    }
    // nullptr is very important
    args.push_back(nullptr);
    // Make execvp system call to run desired process
    execvp(args[0], &args[0]);
    // In case execvp ever fails, we throw a runtime execption
    throw std::runtime_error("Call to execvp failed!");
}

// Implement the constructor (body should be empty).


// Implement the destructor.  The destructor body should be empty
// because this class is very simple and does not have any resources
// to release.


// Use the comments in the header to implement the forkNexec method.
// This is a relatively simple (3-to-4 lines) method with an
// if-statement to call myExec in the child process and just return
// the childPid in parent.


// Use the comments in the header to implement the wait method.  This
// is a relatively simple method which uses waitpid call to get
// exitCode as shown in Slide #6 of ForkAndExec.pdf.  This is a short
// 3-line method.

// ENSURE YOU GET THE EXIT CODE CORRECTLY FROM THE WAITPID SYSCALL

#endif
