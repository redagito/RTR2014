#include "demo/RTRDemo.h"

int main(void)
{
    RTRDemo demo;
    
    int result = demo.init();
    if (result != 0) {
        return result;
    }
    
    return demo.run();
}