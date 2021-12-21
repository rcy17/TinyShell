bool isDigit(char c);
bool isAlpha(char c);
bool isSpecial(char c);
bool isFilename(const char *filename);

void getFullPath(const char *filename, char *path);
void reportFileOpenFailure(const char * path);

int getLastLineFromString(const char * s, int lastStop, char * line = NULL);
