#include <iostream>
#include <cstring>
using namespace std;

#include "shell.h"
#include "terminal.h"
#include "banner.h"
#include "color.h"
#include "commands.h"

Terminal gTerm;

void welcome()
{
    cout << COLOR_CYAN << banner << COLOR_NONE << endl;
    cout << COLOR_YELLOW << "Welcome to TinyShell!" << COLOR_NONE << endl;
}

void init()
{
    cout << COLOR_MAGENTA << "Machine Name: " << COLOR_NONE;
    cin >> gTerm.mach;
    cout << COLOR_MAGENTA << "Root Directory: " << COLOR_NONE;
    cin >> gTerm.root;
    cout << COLOR_MAGENTA << "Login: " << COLOR_NONE;
    cin >> gTerm.user;
    gTerm.strin[0] = '\0';
    gTerm.strout[0] = '\0';
    strcpy(gTerm.wdir, "/");
}

void bye()
{
    cout << COLOR_YELLOW << "Thanks for using TinyShell!" << COLOR_NONE << endl;
}

void printPrefix()
{
    cout << COLOR_GREEN << gTerm.user << "@" << gTerm.mach << COLOR_NONE << ":" << COLOR_BLUE << gTerm.wdir << COLOR_NONE << "$ ";
}

void run()
{
    static char *argv[MAXARGV];
    char line[MAXLINE] = "";
    int argc;
    // Read the '\n' after inputting user
    cin.getline(line, MAXLINE);
    while (true)
    {
        printPrefix();
        gTerm.strout[0] = '\0';
        gTerm.strin[0] = '\0';
        cin.getline(line, MAXLINE);
        if (cin.eof())
            break;
        argc = splitArgs(line, argv);
        if (argc == 0)
            continue;
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
            break;
        else
            cerr << "Error: Command " << argv[0] << " not found" << endl;
        cout << gTerm.strout;
    }
}

bool isSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\0';
}

int splitArgs(char *line, char *argv[])
{
    int len = strlen(line);
    int start = 0;
    int argc = 0;
    while (start < len)
    {
        while (start < len)
        {
            if (!isSpace(line[start]))
                break;
            start++;
        }
        if (start == len)
        {
            break;
        }
        int stop;
        for (stop = start; stop < len; stop++)
        {
            if (isSpace(line[stop]))
                break;
        }
        argv[argc++] = line + start;
        line[stop] = '\0';
        start = stop;
    }
    return argc;
}

bool parseNextToken(const char *sentence, char *word)
{
    int len = strlen(sentence);
    int start = 0;
    while (start < len)
    {
        if (!isSpace(sentence[start]))
            break;
        start++;
    }
    if (start == len)
    {
        return false;
    }
    int stop;
    for (stop = start; stop < len; stop++)
    {
        if (isSpace(word[stop]))
            break;
    }
    strncpy(word, sentence + start, stop - start);
    word[stop - start] = '\0';
    return true;
}
