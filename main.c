#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{
    JSON *json = JSONFromFile("lab/invalid/string.json");
    // JSON *json = StringToJSON("[\"\"]");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(json);
    FreeJSON(json);
    return EXIT_SUCCESS;
}
