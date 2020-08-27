/* 
 * File:   exercise2.cpp
 *
 * Copyright (C) 2020 liererkt@miamioh.edu
 * 
 */

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

// A simple type-alias to streamline the code 
using Dictionary = std::unordered_map<std::string, bool>;


/** Implement this method to print all words in the dictionary that
 *  contain the given sub-string.  For example, if sub is "thou", this
 *  method will print the words that contain "thou" in it, for e.g.,
 *  "aforethought", "although", etc.
 * 
 * @param english The unordered_map containing a subset of words in
 * the English language.
 *
 * @param sub The sub-string to search for. This method uses
 * std::string::find() method to determine if the substring is present
 * in a given world.
 */
void printMatchingWords(const Dictionary& english, const std::string& sub) {
    // Implement this method using a range-based for-loop -- i.e., 
        for (const auto& entry : english) {  
            // suitable if-statement here using the find() method
            if (entry.first.find(sub) != string::npos) {
                // print matching words to std::cout
                std::cout << entry.first << std::endl;
            }
        }
}

//-------------------------------------------------------------------------
// DO   NOT   MODIFY  CODE  BELOW  THIS  LINE  (but study code below)
//-------------------------------------------------------------------------

/** A simple method to detect if a given word is in the unordered_map 
 *
 * @param english The unordered_map in which the word is to be looked up.
 *
 * @param word The word to search for.
 *
 * @return This method returns true if the word was found in the
 * dictionary. Otherwise it returns false.
 */
bool isValid(const Dictionary& english, const std::string& word) {
    return english.find(word) != std::end(english);
}

/*
 * The main method that loads words from the English dictionary and
 * checks to ensure the words are present in the unordered map.
 *
 * @param argc The number of command-line arguments.
 *
 * @param argv The actual command-line argument.
 */
int main(int argc, char *argv[]) {
    // Open the file with the list of words.
    std::ifstream wordList("english_5000.txt");
    if (!wordList.good()) {
        std::cerr << "Error opening english_5000.txt\n";
        return 2;
    }
    
    // Load the words from the file into the unordered_map for use
    Dictionary english;
    std::string word;
    while (wordList >> word) {
        english[word] = true;
    }
    
    // Validate that the words can be found in the dictionary.
    for (auto entry : english) {
        isValid(english, entry.first);
    }
  
    // Test operations of the printWord method
    printMatchingWords(english, (argc > 1 ? argv[1] : "thou"));

    // All done!
    return 0;
}
