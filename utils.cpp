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

bool pathReduce(char * target) {
    // Path should be starts with "/"
    assert(target[0] == '/');
    int length = strlen(target);
    int right = 0;
    int left = 0;
    while (++right < length) {
        while (right < length && target[right] != '/') right++;
        int doMove = 0;
        if (right - left == 1 || (right - left == 2 && target[left + 1] == '.')) 
        {
            // "Dir1//Dir2" or "Dir1/./Dir2" turns into "Dir1/Dir2"
            doMove = 1;
        }
        if (right - left == 3 && strncmp(target + left, "/..", 3) == 0) {
            // "Dir0/Dir1/../Dir2" turns into "Dir0/Dir2"
            left--;
            while (left >= 0 && target[left] != '/') left--;
            if (left < 0) {
                return false;
            }
            doMove = 1;
        }
        if (doMove) {
            memmove(target + left, target + right, length - right);
            length -= right - left;
            right = left;
        } else {
            left = right;
        }
    }
    // In case of cd ".." to "/", we need to add "/"
    if (!length) target[++length] = '/';
    // In case of cd "Dir/", we need to remove "/"
    if (length > 1 && target[length - 1] == '/') length--; 
    target[length] = '\0';
    return true;
}

bool getFullPath(const char *filename, char *path)
{
    assert(filename[0]); // filename shouldn't be empty'
    path[0] = '\0';
    if (filename[0] != '/')
    {
        strcat(path, gTerm.wdir);
        strcat(path, "/");
    }
    strcat(path, filename);
    static char temp[MAXLINE];
    temp[0] = '\0';
    strcpy(temp, path);
    if (!pathReduce(temp)) {
        cerr << "Error: can't open file '" << path << "' which is outside the root" << endl;
        return false;
    }
    strcpy(path, gTerm.root);
    strcat(path, temp);
    return true;
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
