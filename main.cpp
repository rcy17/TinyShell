#include <iostream>
#include <cstring>

#include "terminal.h"
#include "banner.h"
#include "shell.h"

using namespace std;

int main()
{
    welcome();
    init();
    run();
    bye();
    return 0;
}
