#include <_string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"
#include <standardloop/collections.h>

int main(void)
{
    printf("starting test....\n");
    TestJSON();
    printf("done!\n");
    return EXIT_SUCCESS;
}
