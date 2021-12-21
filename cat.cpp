#include <cstring>
#include <fstream>

using namespace std;

#include "terminal.h"
#include "commands.h"
#include "utils.h"

static void printHelp() {
    strcat(gTerm.strout, "Usage: cat [--help] [-b] [-E] [-n] [-s] [file [file ...]]\n");
    strcat(gTerm.strout, "  --help show this help and exit\n");
    strcat(gTerm.strout, "  -b number nonempty output lines, overrides -n\n");
    strcat(gTerm.strout, "  -E display $ at end of each line\n");
    strcat(gTerm.strout, "  -n number all output lines\n");
    strcat(gTerm.strout, "  -s suppress repeated empty output lines\n");
}

void doCat(int argc, char *argv[]) {
    int numberNonBlank = 0;
    int showEnds = 0;
    int number = 0;
    int squeezeBlank = 0;
    int params = 1;
    char line[MAXLINE];
    for(int i = 1; i < argc; i++) {
        if (i == 1 && strcmp(argv[i], "--help") == 0) {
            return printHelp();
        } else if (params && strcmp(argv[i], "-b") == 0) {
            numberNonBlank = 1;
        } else if (params && strcmp(argv[i], "-E") == 0) {
            showEnds = 1;
        } else if (params && strcmp(argv[i], "-n") == 0) {
            number = 1;
        } else if (params && strcmp(argv[i], "-s") == 0) {
            squeezeBlank = 1;
        } else if (strcmp(argv[i], "-") == 0) {
            params = 0;
            int start = 0;
            int stop = -1;
            int lines = 0;
            char numberPrefix[256];
            bool lastIsBlank = false;
            while (true) {
                int length = strlen(gTerm.strout);
                stop = getLastLineFromString(gTerm.strin, stop, line);
                bool isBlank = !line[0];
                if (isBlank && !gTerm.strin[stop]) break;
                if (squeezeBlank && isBlank && lastIsBlank) continue;
                if (numberNonBlank) {
                    if (!isBlank) {
                        lines++;
                        sprintf(numberPrefix, "%6d  ", lines);
                        strcat(gTerm.strout, numberPrefix);
                    }
                } else if (number) {
                    lines++;
                    sprintf(numberPrefix, "%6d  ", lines);
                    strcat(gTerm.strout, numberPrefix);
                }
                strcat(gTerm.strout, line);
                if (!gTerm.strin[stop]) break;
                if (showEnds) strcat(gTerm.strout, "$");
                strcat(gTerm.strout, "\n");
                lastIsBlank = isBlank;
            }
        } else {
            params = 0;
            char numberPrefix[256];
            bool lastIsBlank = false;
            int lines = 0;
            char path[1024];
            getFullPath(argv[i], path);
            ifstream fin(path);
            if(fin.fail()) {
                return reportFileOpenFailure(path);
            }
            while (true) {
                fin.getline(line, MAXLINE);
                bool isBlank = !line[0];
                if (isBlank && fin.eof()) break;
                if (squeezeBlank && isBlank && lastIsBlank) continue;
                if (numberNonBlank) {
                    if (!isBlank) {
                        lines++;
                        sprintf(numberPrefix, "%6d  ", lines);
                        strcat(gTerm.strout, numberPrefix);
                    }
                } else if (number) {
                    lines++;
                    sprintf(numberPrefix, "%6d  ", lines);
                    strcat(gTerm.strout, numberPrefix);
                }
                strcat(gTerm.strout, line);
                if(fin.eof()) break;
                if (showEnds) strcat(gTerm.strout, "$");
                strcat(gTerm.strout, "\n");
                lastIsBlank = isBlank;
            }
            fin.close();
        }
    }
}
