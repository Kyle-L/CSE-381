// Copyright (C) 2019 raodm@miamioh.edu
// Pragma incase user forgets to add the -O3 flag
#pragma GCC optimize("-O3")


#include <fstream>
#include <iostream>
#include <string>
#include <locale>

std::string readFile(const std::string& fileName) {
    std::ifstream dataFile(fileName, std::ios::ate);
    const size_t fileSize = dataFile.tellg();  // find file size
    dataFile.seekg(0);  // to to beginning of file.
    std::string data(fileSize, 0);  // create string to hold characters in file
    dataFile.read(&data[0], fileSize);  // read all characters
    return data;  // return data in file back to caller
}

void toggleCase(std::string& str, const char low) {
    const char up = std::toupper(low);
    for (char& c : str) {
        if (c == low) {
            c = up;   // Convert lower to upper
        } else if (c == up) {
            c = low;  // Conver upper to lower
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Specify <File> <toggle_char> ...\n";
        return 1;
    }

    // Read the file by slurping the whole file into a string.
    std::string data = readFile(argv[1]);

    const std::string toggleChars = argv[2];
    for (char c : toggleChars) {
        toggleCase(data, c);
    }
    // Print the modifed data.
    std::cout << data;
}
