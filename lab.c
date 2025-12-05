#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <standardloop/json.h>
#include "./json.h"

int main(void)
{
    // JSON *json = StringToJSON("[{\"guid\": \"\", \"about\": \"\"}]");
    // JSON *json = StringToJSON("{\"foo\": \"\"}");
    // JSON *json = StringToJSON("{\"guid\": \"foo\", \"about\": \"bar\"}");

    JSON *json = JSONFromFile("./private/lab/1mil.json");

    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    // PrintJSON(json);
    // FreeJSON(json);

    char *json_string = JSONToString(json);
    if (json_string == NULL)
    {
        return EXIT_FAILURE;
    }
    // printf("%s", json_string);
    free(json_string);
    return EXIT_SUCCESS;
}
