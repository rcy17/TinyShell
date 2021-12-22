#include <cstring>
#include <fstream>
#include <cassert>
#include <iostream>

using namespace std;

#include "terminal.h"
#include "commands.h"
#include "utils.h"

struct DiffOptions
{
    bool ignoreSpaceChange;
    bool ignoreBlankLines;
    bool ignoreCase;
    bool brief;
    bool ignoreAllSpace;
    bool ignoreMatchingLines;
    char pattern[MAXLINE];
};

struct Operation
{
    // type: 0 means change, 1 means add, -1 means delete
    int type;
    int line1;
    int line2;
};

static void printHelp()
{
    strcat(gTerm.strout, "Usage: diff [OPTION]... FILE1 FILE2\n");
    strcat(gTerm.strout, "OPTION:\n");
    strcat(gTerm.strout, "  --help Print this help and exit\n");
    strcat(gTerm.strout, "  -b ignore changes in the amount of white space\n");
    strcat(gTerm.strout, "  -B ignore changes where lines are all blank\n");
    strcat(gTerm.strout, "  -i ignore case differences in file contents\n");
    strcat(gTerm.strout, "  -q report only when files differ\n");
    strcat(gTerm.strout, "  -w ignore all white space\n");
    strcat(gTerm.strout, "  -I<STRING> ignore changes where all lines match STRING\n");
}

bool linesDiff(const char *a, const char *b, DiffOptions *options)
{
    char *pa = const_cast<char *>(a);
    char *pb = const_cast<char *>(b);
    while (*pa || *pb)
    {
        if (*pa == *pb)
        {
            pa++, pb++;
            continue;
        }
        else
        {
            if (options->ignoreAllSpace)
            {
                if (*pa == ' ')
                {
                    pa++;
                    continue;
                }
                if (*pb == ' ')
                {
                    pb++;
                    continue;
                }
            }
            if (options->ignoreCase)
            {
                if (toLower(*pa) == toLower(*pb))
                {
                    pa++, pb++;
                    continue;
                }
            }
            if (options->ignoreSpaceChange)
            {
                if (isLineEnd(*pa) && isWhiteChar(*pb))
                {
                    pb++;
                    continue;
                }
                if (isLineEnd(*pb) && isWhiteChar(*pa))
                {
                    pa++;
                    continue;
                }
                if (isWhiteChar(*pa) && b < pb && isWhiteChar(pb[-1]))
                {
                    pb++;
                    continue;
                }
                if (isWhiteChar(*pb) && a < pa && isWhiteChar(pa[-1]))
                {
                    pa++;
                    continue;
                }
            }
            if (options->ignoreAllSpace)
            {
                if (isWhiteChar(*pa))
                {
                    pa++;
                    continue;
                }
                if (isWhiteChar(*pb))
                {
                    pb++;
                    continue;
                }
            }
            // Mismatch!
            return true;
        }
    }
    return false;
}

bool readFileAll(const char *filename, char *buffer)
{
    ifstream fin(filename);
    if (!fin.is_open())
    {
        reportFileOpenFailure(filename);
        return false;
    }
    // Use read to gracefully read all lines
    fin.read(buffer, MAXFILE);
    buffer[fin.gcount()] = '\0';
    fin.close();
    return true;
}

bool compareLines(char *pLines1[], int lines1, char *pLines2[], int lines2, DiffOptions *options)
{
    static int dp[MAXLINES + 1][MAXLINES + 1];
    for (int i = 0; i <= lines1; i++)
        dp[i][0] = i;
    for (int j = 1; j <= lines2; j++)
        dp[0][j] = j;
    for (int i = 1; i <= lines1; i++)
    {
        for (int j = 1; j <= lines2; j++)
        {
            // Notice that pLines1(2) counts from 0
            if (linesDiff(pLines1[i - 1], pLines2[j - 1], options))
            {
                dp[i][j] = min(dp[i][j - 1] + 1, dp[i - 1][j] + 1);
                // Try use only add and delete
                // dp[i][j] = min(dp[i][j], dp[i - 1][j - 1] + 1);
            }
            else
            {
                dp[i][j] = (i && j) ? dp[i - 1][j - 1] : 0;
            }
        }
    }
    if (!dp[lines1][lines2])
        return false;
    int p = 0;
    int i = lines1, j = lines2;
    Operation ops[MAXLINES * 2];
    int count = 0;
    while (i || j)
    {
        if (j && dp[i][j - 1] + 1 == dp[i][j])
        {
            ops[count++] = {1, i, j};
            j--;
        }
        else if (i && dp[i - 1][j] + 1 == dp[i][j])
        {
            ops[count++] = {-1, i, j};
            i--;
        }
        // Try use only add and delete
        /*else if (i && j && dp[i - 1][j - 1] + 1 == dp[i][j])
        {
            i--, j--;
            ops[count++] = {0, i, j};
        }*/
        // Take this judge in the end to make more continoues operations
        else if (i && j && dp[i - 1][j - 1] == dp[i][j])
        {
            i--, j--;
        }
        else
        {
            assert(0);
        }
    }
    int last = count - 1;
    for (int i = last - 1; i >= -1; i--)
    {
        // Use -1 to make a grace end
        // If operations run out or types are different or operations are not continous, then the block ends
        if (i < 0 || ops[i].type != ops[last].type ||
            (ops[i].line1 != ops[last].line1 && ops[i].line2 != ops[last].line2))
        {
            // block (i, last]
            bool ignore = false;
            bool leftOperation = ops[last].type == -1;
            if (options->ignoreBlankLines)
            {
                bool allMatch = true;
                for (int j = last; j > i; j--)
                    allMatch = allMatch && (leftOperation ? isLineEnd(pLines1[ops[j].line1 - 1][0]) : isLineEnd(pLines2[ops[j].line2 - 1][0]));
                ignore = allMatch;
            }
            if (options->ignoreMatchingLines)
            {
                bool allMatch = true;
                for (int j = last; j > i; j--)
                    allMatch = allMatch && strstr(leftOperation ? pLines1[ops[j].line1 - 1] : pLines2[ops[j].line2 - 1], options->pattern) != NULL;
                ignore = ignore || allMatch;
            }
            if (!ignore)
            {
                char buffer[MAXLINE];
                if (last - i > 1)
                {
                    if (leftOperation)
                        sprintf(buffer, "%d,%dd%d\n", ops[last].line1, ops[i + 1].line1, ops[last].line2);
                    else
                        sprintf(buffer, "%da%d,%d\n", ops[last].line1, ops[last].line2, ops[i + 1].line2);
                }
                else
                {
                    sprintf(buffer, "%d%c%d\n", ops[last].line1, leftOperation ? 'd' : 'a', ops[last].line2);
                }
                strcat(gTerm.strout, buffer);
                for (int j = last; j > i; j--)
                {
                    char *line = leftOperation ? pLines1[ops[j].line1 - 1] : pLines2[ops[j].line2 - 1];
                    strcat(gTerm.strout, leftOperation ? "< " : "> ");
                    strcat(gTerm.strout, line);
                    if (!*line || line[strlen(line) - 1] != '\n')
                        strcat(gTerm.strout, "\n\\ No newline at end of file\n");
                }
            }
            last = i;
        }
        if (i < 0)
            break;
    }
    return true;
}

bool compareFiles(const char *a, const char *b, DiffOptions *options)
{
    static char data1[MAXFILE];
    static char data2[MAXFILE];
    static char temp[MAXFILE];
    char *pLines1[MAXLINES];
    char *pLines2[MAXLINES];
    int lines1, lines2;
    if (strcmp(a, "-") == 0 && strcmp(b, "-") == 0)
    {
        return 0;
    }
    else if (strcmp(a, "-") == 0)
    {
        lines1 = splitLines(gTerm.strin, data1, pLines1);
        readFileAll(b, temp);
        lines2 = splitLines(temp, data2, pLines2);
    }
    else if (strcmp(b, "-") == 0)
    {
        readFileAll(a, temp);
        lines1 = splitLines(temp, data1, pLines1);
        lines2 = splitLines(gTerm.strin, data2, pLines2);
    }
    else
    {
        readFileAll(a, temp);
        lines1 = splitLines(temp, data1, pLines1);
        readFileAll(b, temp);
        lines2 = splitLines(temp, data2, pLines2);
    }
    compareLines(pLines1, lines1, pLines2, lines2, options);
    if (options->brief)
    {
        // Trick: ff we got anything in strout, then there should be some differences
        if (gTerm.strout[0])
        {
            sprintf(gTerm.strout, "Files %s and %s differ\n", a, b);
        }
        else
        {
            gTerm.strout[0] = '\0';
        }
    }
    return true;
}

void doDiff(int argc, char *argv[])
{
    DiffOptions options = {};
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            return printHelp();
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            options.ignoreSpaceChange = true;
        }
        else if (strcmp(argv[i], "-B") == 0)
        {
            options.ignoreBlankLines = true;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            options.ignoreCase = true;
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            options.brief = true;
        }
        else if (strcmp(argv[i], "-w") == 0)
        {
            options.ignoreAllSpace = true;
        }
        else if (strncmp(argv[i], "-I", 2) == 0)
        {
            options.ignoreMatchingLines = true;
            strcpy(options.pattern, argv[i] + 2);
        }
        else
        {
            if (i != argc - 2)
            {
                return printHelp();
            }
            break;
        }
    }
    compareFiles(argv[argc - 2], argv[argc - 1], &options);
}
