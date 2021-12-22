#include <cstring>

#include "terminal.h"
#include "commands.h"

#include <cstring>
#include <cassert>
#include <iostream>

using namespace std;

#include "terminal.h"
#include "commands.h"

static void printHelp()
{
    strcat(gTerm.strout, "Usage: pwd [--help]\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
}
void doPwd(int argc, char *argv[])
{
    if (argc > 1)
        printHelp();
    strcpy(gTerm.strout, gTerm.wdir);
    strcat(gTerm.strout, "\n");
}
