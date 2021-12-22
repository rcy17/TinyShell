#include <cstring>
#include <cassert>
#include <iostream>

using namespace std;

#include "terminal.h"
#include "commands.h"
#include "utils.h"

static void printHelp()
{
    strcat(gTerm.strout, "Usage: cd [--help] PATH\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
}
void doCd(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0 || argc != 2)
        printHelp();
    const char *path = argv[1];
    // Get temp result
    char target[MAXLINE];
    strcpy(target, gTerm.wdir);
    if (path[0] == '/')
        strcpy(target, path);
    else
    {
        // Append "/" if current wdir != "/"
        if (target[1])
            strcat(target, "/");
        strcat(target, path);
    }
    // Then deal with "." and ".."
    if (!pathReduce(target))
    {
        cerr << "Error: can't cd to " << argv[1] << " which is outside the root" << endl;
        return;
    }
    strcpy(gTerm.wdir, target);
}
