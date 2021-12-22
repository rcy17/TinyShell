#include <iostream>
#include <cstring>
using namespace std;

#include "shell.h"
#include "terminal.h"
#include "banner.h"
#include "color.h"
#include "commands.h"
#include "utils.h"

Terminal gTerm;

void welcome()
{
    cout << COLOR_BRIGHT_CYAN << banner << COLOR_NONE << endl;
    cout << COLOR_BRIGHT_YELLOW << "Welcome to TinyShell!" << COLOR_NONE << endl;
}

void init()
{
    cout << COLOR_BRIGHT_MAGENTA << "Machine Name: " << COLOR_NONE;
    cin >> gTerm.mach;
    cout << COLOR_BRIGHT_MAGENTA << "Root Directory: " << COLOR_NONE;
    cin >> gTerm.root;
    cout << COLOR_BRIGHT_MAGENTA << "Login: " << COLOR_NONE;
    cin >> gTerm.user;
    gTerm.strin[0] = '\0';
    gTerm.strout[0] = '\0';
    strcpy(gTerm.wdir, "/");
}

void bye()
{
    cout << COLOR_BRIGHT_YELLOW << "Thanks for using TinyShell!" << COLOR_NONE << endl;
}

void printPrefix()
{
    cout << COLOR_BRIGHT_GREEN << gTerm.user << "@" << gTerm.mach << COLOR_NONE << ":" << COLOR_BRIGHT_BLUE << gTerm.wdir << COLOR_NONE << "$ ";
}

void run()
{
    static char *argv[MAXARGS];
    char line[MAXLINE] = "";
    int argc;
    // Read the '\n' after inputting user
    cin.getline(line, MAXLINE);
    while (!gTerm.exit)
    {
        printPrefix();
        cin.getline(line, MAXLINE);
        if (cin.eof())
            break;
        argc = splitArgs(line, argv);
        if (argc == 0)
            continue;
        doPipe(argc, argv);
        if (!gTerm.error)
            cout << gTerm.strout;
        gTerm.error = false;
    }
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
