#include <stdlib.h>

#include "./json.h"
#include <standardloop/testing.h>

int main(void)
{
    TestingInit();
    TestJSON();
    TestingTearDown();
    return EXIT_SUCCESS;
}
