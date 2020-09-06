#ifndef CHILD_PROCESS_CPP
#define CHILD_PROCESS_CPP

/**
 * This source file contains the implementation for the various
 * methods defined in the ChildProcess class.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */

// All the necessary #includes are already here
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <vector>
#include "ChildProcess.h"

/** NOTE: Unlike Java, C++ does not require class names and file names
 * should match.  Hence when defining methods pertaining to a specific
 * class, the class
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

// Implement the constructor
ChildProcess::ChildProcess() : childPid(-1) {
    // childPid is initialized and not assigned!  Hence body is empty.
}

// Implement the destructor.  The destructor is an empty method
// because this class does not have any resources to release.
ChildProcess::~ChildProcess() {
    // Maybe it is a good idea to call wait() method here to ensure
    // that the child process does not get killed? On the other hand,
    // if we call wait() here and the child process misbehaves, then
    // it will appear as if this program is buggy -- Awwww.. -- no
    // good choice.  In this situation, a good API is to leave it to
    // the user to decide what is the best course of action.
}

// Use the comments in the header to implement the forkNexec method.
// This is a relatively simple method with an if-statement to call
// myExec in the child process and just return the childPid in parent.
int
ChildProcess::forkNexec(const StrVec& strVec) {
    // Fork and save the pid of the child process
    childPid = fork();
    // Call the myExec helper method in the child
    if (childPid == 0) {
        // We are in the child process
        myExec(strVec);
    }
    // Control drops here only in the parent process!
    return childPid;
}

// Use the comments in the header to implement the wait method.  This
// is a relatively simple method which uses waitpid call to get
// exitCode as shown in Slide #6 of ForkAndExec.pdf
int
ChildProcess::wait() const {
    int exitCode = 0;  // Child process's exit code
    waitpid(childPid, &exitCode, 0);  // wait for child to finish
    return exitCode;
}

#endif
