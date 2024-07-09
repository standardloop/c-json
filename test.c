#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

static char *quickAllocatedString(char *);
static void printStringDebug(char *, size_t);

static char *quickAllocatedString(char *copy)
{
    size_t true_len = strlen(copy) + 1;
    char *allocated_string = malloc(sizeof(char) * true_len);
    strcpy(allocated_string, copy);
    return allocated_string;
}

static void printStringDebug(char *input_str, size_t input_str_len)
{
    if (input_str == NULL || input_str_len == 0)
    {
    }
    for (size_t i = 0; i < input_str_len; i++)
    {
        printf("%d\n", input_str[i]);
    }
}

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
