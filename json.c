#include <stdio.h>
#include <stdlib.h>

#include "./json.h"

JSON *StringToJSON(char *str)
{
    if (str == NULL || (str[0] != CURLY_OPEN_CHAR && str[0] != BRACKET_OPEN_CHAR))
    {
        printf("[ERROR]: invalid input to StringToJSON\n");
        return NULL;
    }
    return NULL;
}

char *JSONToString(JSON *object)
{
    if (object == NULL)
    {
        return NULL;
    }
    return NULL;
}
void FreeJSON(JSON *object)
{
    if (object == NULL)
    {
        return;
    }
}

void PrintJSON(JSON *object)
{
    if (object == NULL)
    {
        return;
    }
}
