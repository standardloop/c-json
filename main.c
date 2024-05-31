#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./json.h"

int main(void)
{
    char *simple_json = "{}";

    JSON *test = StringToJSON(simple_json);
    if (test == NULL)
    {
        printf("JSON is NULL :/\n");
        return EXIT_FAILURE;
    }
    FreeJSON(test);
    return EXIT_SUCCESS;
}
