#include "demo/RTRDemo.h"

int main(int argc, const char** argv)
{
    RTRDemo demo;

    int result = demo.init("data/demo.ini");
    if (result != 0)
    {
        return result;
    }

    return demo.run();
}