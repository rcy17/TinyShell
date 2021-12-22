#pragma once
bool isDigit(char c);
bool isAlpha(char c);
bool isSpecial(char c);
bool isLineEnd(char c);
bool isSpace(char c);
bool isWhiteChar(char c);
bool isFilename(const char *filename);

char toLower(char c);

bool pathReduce(char *target);
bool getFullPath(const char *filename, char *path);
void reportFileOpenFailure(const char *path);
bool fileExists(const char *file);

/*
* s:        the full text to be split
* lastStop: the last stop position returned from this function
* line:     save the line into the array if it's not NULL
* return:   stop of the next line
*/
int getLastLineFromString(const char *s, int lastStop, char *line = NULL);

int splitLines(const char *text, char *buffer, char *lines[]);

/*
*  Parses a string into a non-negative interger, return -1 when meeting any error
*/
int parseNonNegativeInteger(const char *s);
