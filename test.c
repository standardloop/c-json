#include <_string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"
#include <standardloop/collections.h>
#include <standardloop/testing.h>

int main(void)
{
    // printf("starting test....\n");
    TestingInit();
    TestJSON();
    TestingTearDown();
    // printf("done!\n");
    return EXIT_SUCCESS;
}
