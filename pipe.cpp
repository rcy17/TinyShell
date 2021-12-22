#include <iostream>
#include <cstring>

using namespace std;

#include "terminal.h"
#include "commands.h"

/*
*  return code: 
*    0 means success
*    1 means quit
*    -1 means error
*    -2 means no command
*/
int runCommand(int argc, char *argv[])
{
    if (argc == 0)
        return -2;
    if (strcmp(argv[0], "echo") == 0)
        doEcho(argc, argv);
    else if (strcmp(argv[0], "tee") == 0)
        doTee(argc, argv);
    else if (strcmp(argv[0], "cat") == 0)
        doCat(argc, argv);
    else if (strcmp(argv[0], "cp") == 0)
        doCp(argc, argv);
    else if (strcmp(argv[0], "cd") == 0)
        doCd(argc, argv);
    else if (strcmp(argv[0], "pwd") == 0)
        doPwd(argc, argv);
    else if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0)
        return 1;
    else
        return -1;
    return 0;
}

void doPipe(int argc, char *argv[])
{
    int start = 0;
    gTerm.strin[0] = '\0';
    gTerm.strout[0] = '\0';
    for (int i = 0; i <= argc; i++)
    {
        if (i == argc || strcmp(argv[i], "|") == 0)
        {
            strcpy(gTerm.strin, gTerm.strout);
            gTerm.strout[0] = '\0';
            int code = runCommand(i - start, argv + start);
            switch (code)
            {
            case 1:
                // User exit
                gTerm.exit = true;
                break;
            case -1:
                cerr << "Error: Command " << argv[start] << " not found" << endl;
                gTerm.error = true;
                break;
            case -2:
                cerr << "Error: Command expected after |" << endl;
                gTerm.error = true;
                break;
            }
            // If command parsing or command execution failed, stop the pipe
            if (code || gTerm.error)
                break;
            start = i + 1;
        }
    }
}
