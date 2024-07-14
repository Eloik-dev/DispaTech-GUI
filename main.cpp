#include <iostream>
#include <filesystem>
#include "src/header/controller.h"

using namespace std;

int main(int argc, char **argv)
{
    Controller *controller = new Controller(argc, argv);

    return 1;
}
