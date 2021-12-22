#include <cstring>
#include <fstream>

using namespace std;

#include "terminal.h"
#include "commands.h"
#include "utils.h"

void printHelp()
{
    strcat(gTerm.strout, "Usage: copy [--help] [-n] SOURCE DEST\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
    strcat(gTerm.strout, "  -n do not overwrite an existing file\n");
}

bool fileExists(const char *file)
{
    ifstream fin(file);
    if (fin.fail())
        return false;
    fin.close();
    return true;
}

void copyFileToStrout(const char *src)
{
    ifstream fin(src, ifstream::binary);
    if (fin.fail())
    {
        return reportFileOpenFailure(src);
    }
    fin.read(gTerm.strout, MAXFILE);
    gTerm.strout[fin.gcount()] = '\0';
    fin.close();
}

void copyStrinToFile(const char *dst)
{
    ofstream fout(dst, ofstream::binary);
    if (fout.fail())
    {
        return reportFileOpenFailure(dst);
    }
    fout.write(gTerm.strin, strlen(gTerm.strin));
    fout.close();
}

void copyFileToFile(const char *src, const char *dst)
{
    ifstream fin(src, ifstream::binary);
    if (fin.fail())
    {
        return reportFileOpenFailure(src);
    }
    ofstream fout(dst, ofstream::binary);
    if (fout.fail())
    {
        return reportFileOpenFailure(dst);
    }
    fout << fin.rdbuf();
    fin.close();
    fout.close();
}

void doCp(int argc, char *argv[])
{
    int notOverwrite = 0;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        return printHelp();
    if (argc < 3 || argc > 4)
        return printHelp();
    if (argc == 3 && strcmp(argv[1], "-n") == 0)
        return printHelp();
    if (argc == 4)
    {
        if (strcmp(argv[1], "-n") != 0)
            return printHelp();
        notOverwrite = 1;
    }
    char src[256];
    char dst[256];
    if (strcmp(argv[argc - 2], "-") == 0 && strcmp(argv[argc - 1], "-") == 0)
        return;
    if (!getFullPath(argv[argc - 2], src))
        return;
    if (!getFullPath(argv[argc - 1], dst))
        return;
    if (strcmp(argv[argc - 1], "-") == 0)
        return copyFileToStrout(src);
    if (notOverwrite && fileExists(dst))
        return;
    if (strcmp(argv[argc - 2], "-") == 0)
        return copyStrinToFile(dst);
    return copyFileToFile(src, dst);
}
