#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

#include "terminal.h"
#include "commands.h"
#include "color.h"
#include "utils.h"

struct GrepOptions
{
    bool noFilename;    // -h
    bool withFilename;  // -H
    bool ignoreCase;    // -i
    bool lineNumber;    // -n
    bool count;         // -c
    bool afterContext;  // -A
    bool beforeContext; // -B
    int after;          // -A<NUM>
    int before;         // -B<NUM>
};

struct GrepMatch
{
    int start;
    int stop;
};

void printHelp()
{
    strcat(gTerm.strout, "Usage: grep [OPTION]... PATTERN [FILE]...\n");
    strcat(gTerm.strout, "OPTION:\n");
    strcat(gTerm.strout, "  --help Print this help and exit\n");
    strcat(gTerm.strout, "  -h Suppress the prefixing of file names on output.\n");
    strcat(gTerm.strout, "  -H Print the file name for each match.\n");
    strcat(gTerm.strout, "  -i Ignore case distinctions in patterns and input data\n");
    strcat(gTerm.strout, "  -n Prefix each line of output with the 1-based line number within its input file.\n");
    strcat(gTerm.strout, "  -c Suppress normal output; instead print a count of matching lines for each  input  file.\n");
    strcat(gTerm.strout, "  -A<NUM> Print NUM lines of trailing context after matching lines.\n");
    strcat(gTerm.strout, "  -B<NUM> Print NUM lines of leading context before matching lines.\n");
}

/*
*  Match line and pattern from the begging (may skip line's suffix)
*  return: max match length if there is a matching
*/
int matchLine(const char *text, const char *pattern, GrepOptions *options)
{
    int p;
    // Greedy pattern first
    if (pattern[0] == '*' && (p = matchLine(text + 1, pattern, options)) >= 0)
        return p;
    // Try consume this pattern character then
    if (pattern[0] == '*' || pattern[0] == '.' || pattern[0] == text[0])
        return matchLine(text + 1, pattern + 1, options);
}

/*
*  Search a pattern match in the text
*/
GrepMatch searchLine(const char *text, const char *pattern, GrepOptions *options)
{
    // find a match at the beginning of each character
    for (int i = 0; i < strlen(text); i++)
    {
        int p = matchLine(text + i, pattern, options);
        if (p >= 0)
            return {i, p};
        // If the pattern starts with '*', then there is no need to continue
        if (pattern[0] == '*')
            break;
    }
    return {-1, -1};
}

void safeCat(char *dest, const char *src, int n)
{
    int len = strlen(dest);
    strncat(dest, src, n);
    len += n;
    dest[len] = '\0';
}

void grepLines(const char *filename, const char *pLines[], int lines, const char *pattern, GrepOptions *options)
{
    static GrepMatch search[MAXLINES];
    for (int line = 0; line < lines; line++)
    {
        GrepMatch pos = searchLine(pLines[line], pattern, options);
        if (pos.start >= 0)
        {
            if (options->withFilename)
            {
                strcat(gTerm.strout, COLOR_MAGENTA);
                strcat(gTerm.strout, filename);
                strcat(gTerm.strout, COLOR_CYAN);
                strcat(gTerm.strout, ":");
                strcat(gTerm.strout, COLOR_NONE);
            }
            safeCat(gTerm.strout, pLines[line], pos.start);
            strcat(gTerm.strout, COLOR_RED);
            safeCat(gTerm.strout, pLines[line] + pos.start, pos.stop - pos.start);
            strcat(gTerm.strout, COLOR_NONE);
            strcat(gTerm.strout, pLines[line] + pos.stop);
            strcat(gTerm.strout, "\n");
        }
        search[line] = pos;
    }
}

void patternReduce(const char *pattern, char *newPattern)
{
    for (char *p = const_cast<char *>(pattern); *p; p++)
    {
        if (*p == '*' && p[1] == '*')
            continue;
        *newPattern = *p;
        newPattern++;
    }
    *newPattern = '\0';
}

bool grepFile(const char *filename, const char *pattern, GrepOptions *options)
{
    static char data[MAXFILE];
    static char temp[MAXFILE];
    char *pLines[MAXLINES];
    int lines;
    if (strcmp(filename, "-") == 0)
    {
        lines = splitLines(gTerm.strin, data, pLines);
    }
    else
    {
        char path[MAXFILE];
        getFullPath(filename, path);
        ifstream fin(path);
        if (!fin.is_open())
        {
            reportFileOpenFailure(path);
            return false;
        }
        fin.read(temp, MAXFILE);
        temp[fin.gcount()] = '\0';
        lines = splitLines(temp, data, pLines);
    }
    grepLines(filename, pLines, lines, pattern, options);
    return true;
}

void doGrep(int argc, char *argv[])
{
    GrepOptions options = {};
    int patternIndex = 0;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            return printHelp();
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            options.noFilename = true;
            options.withFilename = false;
        }
        else if (strcmp(argv[i], "-H") == 0)
        {
            options.withFilename = true;
            options.noFilename = false;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            options.ignoreCase = true;
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            options.lineNumber = true;
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            options.count = true;
        }
        else if (strncmp(argv[i], "-A", 2) == 0)
        {
            options.afterContext = true;
            options.after = parseNonNegativeInteger(argv[i] + 2);
            if (options.after < 0)
            {
                cerr << "Error: Expected non-negative interger after '-A', get '" << argv[i] + 2 << "' instead" << endl;
                gTerm.error = true;
                return;
            }
        }
        else if (strncmp(argv[i], "-B", 2) == 0)
        {
            options.beforeContext = true;
            options.before = parseNonNegativeInteger(argv[i] + 2);
            if (options.before < 0)
            {
                cerr << "Error: Expected non-negative interger after '-B', get '" << argv[i] + 2 << "' instead" << endl;
                gTerm.error = true;
                return;
            }
        }
        else
        {
            patternIndex = i;
            break;
        }
    }
    if (!options.noFilename && argc - patternIndex > 1)
        options.withFilename = true;
    char pattern[MAXLINE];
    patternReduce(argv[patternIndex], pattern);
    for (int i = patternIndex + 1; i < argc; i++)
    {
        if (!grepFile(argv[i], pattern, &options))
            break;
    }
}
