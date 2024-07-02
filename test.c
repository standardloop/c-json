#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please supply a filename\n");
        return EXIT_FAILURE;
    }
    JSON *json = JSONFromFile(argv[1]);
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(json);
    FreeJSON(json);

    return EXIT_SUCCESS;
}
