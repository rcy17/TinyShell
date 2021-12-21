#include <cstring>
#include <cassert>
#include <cerrno>
#include <iostream>

using namespace std;

#include "utils.h"
#include "terminal.h"

bool isDigit(char c) { return (c >= '0' && c <= '9'); }

bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

bool isSpecial(char c) { return (c == '-' || c == '.' || c == '_'); }

bool isFilename(const char *filename)
{
    for (int i = 0; i < strlen(filename); i++)
    {
        if (!isDigit(filename[i]) && !isSpecial(filename[i]) && !isAlpha(filename[i]))
            return false;
    }
    return true;
}

void getFullPath(const char *filename, char *path)
{
    assert(filename[0]); // filename shouldn't be empty'
    strcpy(path, gTerm.root);
    if (filename[0] != '/')
    {
        strcat(path, gTerm.wdir);
    }
    strcat(path, filename);
}

void reportFileOpenFailure(const char * path) {
    cerr << "Fail to open '" << path << "': " << strerror(errno) << endl;
}


/*
* s:        the full text to be split
* lastStop: the last stop position returned from this function
* line:     save the line into the array if it's not NULL
* return:   stop of the next line
*/
int getLastLineFromString(const char * s, int lastStop, char * line) {
    // laststop should be -1 in the beginning
    int start = lastStop + 1;
    int stop = start;
    while (s[stop] && s[stop] != '\n') stop++;
    if (line) {
        strncpy(line, s + start, stop - start);
        line[stop - start] = '\0';
    }
    return stop;
}
