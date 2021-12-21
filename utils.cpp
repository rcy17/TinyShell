#include <cstring>
#include <cassert>

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
