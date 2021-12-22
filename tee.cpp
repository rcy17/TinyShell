#include <iostream>
#include <cstring>
#include <cassert>
#include <fstream>

#include "terminal.h"
#include "utils.h"

using namespace std;

static void printHelp()
{
    strcat(gTerm.strout, "Usage: tee [OPTION] [FILE]...\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
    strcat(gTerm.strout, "  -a append to the given FILEs, do not overwrite\n");
}

void doTee(int argc, char *argv[])
{
    int append = 0;
    char filenames[MAXARGS][MAXLINE];
    int files = 0;
    for (int i = 1; i < argc; i++)
    {
        if (i == 1 && strcmp(argv[i], "--help") == 0)
        {
            return printHelp();
        }
        else if (i == 1 && strcmp(argv[i], "-a") == 0)
        {
            append = 1;
        }
        else if (strcmp(argv[i], "-") == 0)
        {
            // It only matters when append is true, which will be checked later
            strcat(gTerm.strout, gTerm.strin);
        }
        else
        {
            char path[1024];
            if (!getFullPath(argv[i], path))
                return;
            ofstream fout;
            if (append)
                fout.open(path, ofstream::app);
            else
                fout.open(path);
            if (!fout.is_open())
            {
                return reportFileOpenFailure(path);
            }
            fout << gTerm.strin;
            fout.close();
        }
    }
    // If append is true, then output file "-" will repeat strin into strout
    if (append)
        strcat(gTerm.strout, gTerm.strin);
    else
        strcpy(gTerm.strout, gTerm.strin);
}
