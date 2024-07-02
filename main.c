#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{

    JSON *json = JSONFromFile("lab/api.json");
    // JSON *json = StringToJSON("[\"1\"]");
    // JSON *json = StringToJSON("{\"key\": \"value\", \"foo\": \"bar\"}");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(json);
    FreeJSON(json);

    // char *json_string = JSONToString(json);
    // if (json_string == NULL)
    // {
    //     return EXIT_FAILURE;
    // }

    // printf("%s", json_string);
    // free(json_string);

    return EXIT_SUCCESS;
}
