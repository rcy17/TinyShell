#include <cstring>
#include <cassert>
#include <fstream>

#include "terminal.h"
#include "utils.h"

using namespace std;

static void printHelp()
{
    strcat(gTerm.strout, "Usage: tee [--help] [-a] [filename [filename ...]]\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
    strcat(gTerm.strout, "  -a append to the given FILEs, do not overwrite\n");
}

void doTee(int argc, char *argv[])
{
    int append = 0;
    char filenames[16][1024];
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
        else
        {
            char path[1024];
            getFullPath(argv[i], path);
            ofstream file(path);
            file.write(gTerm.strin, MAXFILE);
            file.close();
        }
    }
    strcpy(gTerm.strin, gTerm.strout);
}
