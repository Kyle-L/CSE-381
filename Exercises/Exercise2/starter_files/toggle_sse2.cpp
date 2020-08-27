// Copyright (C) 2019 raodm@miamioh.edu

#include <immintrin.h>
#include <fstream>
#include <iostream>
#include <string>
#include <locale>

/** Toggle the case of characters using SSE2 instruction set. These
    SSE2 instructions enable us to process 16-characters at a time
    without having any conditionals making the loop go faster.

    Note: Modern CPUs have AVX512 instructions that can make this method
    even faster by processing 64-characters at a time!
*/
void toggleCase_sse2(std::string& str, const char low) {
    using VecType = __m128i;
    const char up = std::toupper(low);
    // Create a vector of upper-case and lower-case characters for use
    // in loop below.
    const VecType upStr = _mm_set1_epi8(up), loStr = _mm_set1_epi8(low);
    // We need to map characters into SSE instrinsic data types. This
    // assumes alignment, which may not happen on some
    // compliers. However, this example does not deal with aligment
    // issues to keep the solution simple.
    VecType *sPtr = reinterpret_cast<VecType*>(&str[0]);
    // Convenience destination pointer increment below.
    char    *dest = &str[0];

    for (size_t i = 0; (i < str.size()); i += 16) {
        // Use SSE instructions to compare 'n' charachters and get a
        // boolean vector (with 1s where characters match). For
        // example, if sPtr == "ATATtAaTCgGgAttT", and loStr ==
        // "tttttttttttttttt", then lowChars == "0000100000000110"
        const auto loChars = _mm_cmpeq_epi8(*sPtr, loStr);
        const auto hiChars = _mm_cmpeq_epi8(*sPtr, upStr);

        // Now we know where we have upper-case and lower case
        // characters. Use SSE instruction to conditionally replace
        // characters at positions where loChars[i] == 1.
        _mm_maskmoveu_si128(upStr, loChars, dest);
        // Repeat instructionis to replace characters at positions
        // where hiChars[i] == 1.
        _mm_maskmoveu_si128(loStr, hiChars, dest);
        // On to the next set of characters
        sPtr++;
        dest += 16;
    }
}

std::string readFile(const std::string& fileName) {
    std::ifstream dataFile(fileName, std::ios::ate);
    const size_t fileSize = dataFile.tellg();  // find file size
    dataFile.seekg(0);  // to to beginning of file.
    std::string data(fileSize, 0);  // create string to hold characters in file
    dataFile.read(&data[0], fileSize);  // read all characters
    return data;  // return data in file back to caller
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
        toggleCase_sse2(data, c);
    }
    // Print the modifed data.
    std::cout << data;
}
