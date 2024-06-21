#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{
    // char *simple_json = "[\"josh\", \"test\", [1, 2, 3, 4]]";
    // char *simple_json = "[[1, 2, 3], \"hi\"]";
    // char *simple_json = "[{\"key\": \"value\", \"test\": true, {\"data\": null}, \"numbers\": [-3.123]}]";
    char *simple_json = "[null, [1, 2, 3, 5], null, null, -4.1, false]";

    JSON *test = StringToJSON(simple_json);
    if (test == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(test);
    FreeJSON(test);
    return EXIT_SUCCESS;
}
