#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{

    JSON *json = JSONFromFile("lab/1mil.json");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(json);
    // PrintDynamicArray(HashMapGet(json->root->value, "data")->value);
    FreeJSON(json);
    return EXIT_SUCCESS;
}
