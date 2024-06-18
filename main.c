#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{
    char *simple_json = "[\"josh\", \"test\"]";

    JSON *test = StringToJSON(simple_json);
    if (test == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(test);
    FreeJSON(test);
    return EXIT_SUCCESS;
}
