#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

JSON *StringToJSON(const char *str)
{
    if (str == NULL)
    {
        printf("[ERROR]: input StringToJSON is NULL\n");
        return NULL;
    }
    size_t input_str_len = strlen(str);
    // printf("[STRLEN]: %d\n", (int)input_str_len);
    if ((str[0] != CURLY_OPEN_CHAR && str[0] != BRACKET_OPEN_CHAR) ||
        (str[0] == CURLY_OPEN_CHAR && str[input_str_len - 1] != CURLY_CLOSE_CHAR) ||
        (str[0] == BRACKET_OPEN_CHAR && str[input_str_len - 1] != BRACKET_CLOSE_CHAR))
    {
        printf("[ERROR]: input StringToJSON is not valid JSON\n");
        return NULL;
    }
    for (size_t i = 0; i < input_str_len; i++)
    {
    }

    JSON *json = malloc(sizeof(JSON));
    if (json == NULL)
    {
        return NULL;
    }
    return json;
}

char *JSONToString(JSON *json)
{
    if (json == NULL)
    {
        return NULL;
    }
    FreeJSON(json);
    return NULL;
}

void FreeJSON(JSON *json)
{
    if (json == NULL)
    {
        return;
    }
    free(json);
}

void PrintJSON(JSON *json)
{
    if (json == NULL)
    {
        return;
    }
}
