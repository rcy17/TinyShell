#include <cstring>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <fstream>

using namespace std;

#include "utils.h"
#include "terminal.h"

bool isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isSpecial(char c)
{
    return (c == '-' || c == '.' || c == '_');
}

bool isLineEnd(char c)
{
    return c == '\0' || c == '\n';
}

bool isSpace(char c)
{
    return c == ' ' || c == '\0';
}

bool isWhiteChar(char c)
{
    return c == ' ' || c == '\n';
}

bool isFilename(const char *filename)
{
    for (int i = 0; i < strlen(filename); i++)
    {
        if (!isDigit(filename[i]) && !isSpecial(filename[i]) && !isAlpha(filename[i]))
            return false;
    }
    return true;
}

char toLower(char c)
{
    return c | (1 << 5);
}

void strip(char *s)
{
    int start = 0;
    while (isWhiteChar(s[start]))
        start++;
    if (!s[start])
    {
        // White space string
        s[0] = '\0';
        return;
    }
    int stop = strlen(s) - 1;
    while (isWhiteChar(s[stop]))
        stop--;
    memmove(s, s + start, stop - start);
    s[stop - start] = '\0';
}

bool pathReduce(char *target)
{
    // Path should be starts with "/"
    assert(target[0] == '/');
    int length = strlen(target);
    int right = 0;
    int left = 0;
    while (++right < length)
    {
        while (right < length && target[right] != '/')
            right++;
        int doMove = 0;
        if (right - left == 1 || (right - left == 2 && target[left + 1] == '.'))
        {
            // "Dir1//Dir2" or "Dir1/./Dir2" turns into "Dir1/Dir2"
            doMove = 1;
        }
        if (right - left == 3 && strncmp(target + left, "/..", 3) == 0)
        {
            // "Dir0/Dir1/../Dir2" turns into "Dir0/Dir2"
            left--;
            while (left >= 0 && target[left] != '/')
                left--;
            if (left < 0)
            {
                return false;
            }
            doMove = 1;
        }
        if (doMove)
        {
            memmove(target + left, target + right, length - right);
            length -= right - left;
            right = left;
        }
        else
        {
            left = right;
        }
    }
    // In case of cd ".." to "/", we need to add "/"
    if (!length)
        target[++length] = '/';
    // In case of cd "Dir/", we need to remove "/"
    if (length > 1 && target[length - 1] == '/')
        length--;
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
    if (!pathReduce(temp))
    {
        cerr << "Error: can't open file '" << path << "' which is outside the root" << endl;
        gTerm.error = true;
        return false;
    }
    strcpy(path, gTerm.root);
    strcat(path, temp);
    return true;
}

void reportFileOpenFailure(const char *path)
{
    cerr << "Fail to open '" << path << "': " << strerror(errno) << endl;
    gTerm.error = true;
}

bool fileExists(const char *file)
{
    ifstream fin(file);
    if (!fin.is_open())
        return false;
    fin.close();
    return true;
}

int getLastLineFromString(const char *s, int lastStop, char *line)
{
    // laststop should be -1 in the beginning
    int start = lastStop + 1;
    int stop = start;
    while (s[stop] && s[stop] != '\n')
        stop++;
    if (line)
    {
        strncpy(line, s + start, stop - start);
        line[stop - start] = '\0';
    }
    return stop;
}

int splitLines(const char *text, char *buffer, char *lines[])
{
    int start = 0;
    int stop = 0;
    int offset = 0;
    int count = 0;
    int len = strlen(text);
    while (stop < len)
    {
        while (text[stop] && text[stop] != '\n')
            stop++;
        stop++;
        if (count >= MAXLINE)
        {
            cerr << "WARNING: Too many lines, truncate the first " << count << " lines" << endl;
            return count;
        }
        lines[count++] = buffer + offset;
        strncpy(buffer + offset, text + start, stop - start);
        buffer[offset += stop - start] = '\0';
        offset++;
        start = stop;
    }
    return count;
}

int parseNonNegativeInteger(const char *s)
{
    if (!*s)
        return -1;
    int number = 0;
    char *p = const_cast<char *>(s);
    while (*p && '0' <= *p && *p <= '9')
        number = number * 10 + *p - '0';
    if (!*p)
        return -1;
    return number;
}
