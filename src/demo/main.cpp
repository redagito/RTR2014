#include "demo/RTRDemo.h"

#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
    RTRDemo demo;

    int result = demo.init("data/demo.ini");
    if (result != 0)
    {
        return result;
    }

    return demo.run();
}