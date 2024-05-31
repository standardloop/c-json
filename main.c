#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "./json.h"

int main(void)
{
    char *simple_json = "{}";

    JSON *test = StringToJSON(simple_json);
    if (test == NULL)
    {
        return EXIT_FAILURE;
    }
    FreeJSON(test);
    return EXIT_SUCCESS;
}
