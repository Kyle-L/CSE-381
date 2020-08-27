# Copyright (C) 2019 raodm@miamioh.edu

import sys

def readFile(fileName):
    with open (fileName, "r") as dataFile:
        data = dataFile.read()
    return data

def toggleCase(str, low):
    up = low.upper();
    result = ""
    for char in str:
        if (char == low):
            result += up
        elif (char == up):
            result += low
        else:
            result += char
    return result

# The main part of the script
if __name__ == '__main__':
    if (len(sys.argv) < 2):
        print ("Specify <File> <toggle_chars>")
        sys.exit(1)

    # Read the file by slurping the whole file into a string.
    data = readFile(sys.argv[1])

    toggleChars = sys.argv[2]
    for c in toggleChars:
        data = toggleCase(data, c)

    # Print the modifed data on screen.
    print(data)
