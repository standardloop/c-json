#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{
    //JSON *json = JSONFromFile("lab/api.json");
    JSON *json = StringToJSON("[1,23]]");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(json);
    FreeJSON(json);
    return EXIT_SUCCESS;
}
