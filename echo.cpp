#include <cstring>

#include "terminal.h"
#include "commands.h"

static void printHelp()
{
    strcat(gTerm.strout, "Usage: echo [--help] [-n] [WORD [WORD ...]]\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
    strcat(gTerm.strout, "  -n do not output the trailing newline\n");
}

void doEcho(int argc, char *argv[])
{
    int newLine = 1;
    int first = 1;
    for (int i = 1; i < argc; i++)
    {
        if (i == 1 && strcmp(argv[i], "--help") == 0)
        {
            return printHelp();
        }
        else if (i == 1 && strcmp(argv[i], "-n") == 0)
        {
            newLine = 0;
        }
        else
        {
            if (!first)
            {

                strcat(gTerm.strout, " ");
            }
            first = 0;
            strcat(gTerm.strout, argv[i]);
        }
    }
    if (newLine)
    {
        strcat(gTerm.strout, "\n");
    }
}