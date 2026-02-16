#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <errno.h>

#include <standardloop/util.h>

#include "./json.h"

static void printJSONStringValue(char *);
static void printJSONNumberIntValue(int64_t *value);
static void printJSONNumberDoubleValue(double *value);

static void printJSONBoolValue(bool *);
static void printJSONNULLValue(void);
static void printJSONListValue(DynamicArray *);
static void printJSONObjValue(HashMap *);

static char *doubleToString(double);

extern JSON *JSONInit()
{
    JSON *json = malloc(sizeof(JSON));
    if (json == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    json->root = NULL;
    return json;
}

extern JSON *StringToJSON(char *input_str)
{
    if (input_str == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    // JSONLexerDebugTest(input_str, true);
    JSONLexer *lexer = JSONLexerInit(input_str);
    if (lexer == NULL)
    {
        return NULL;
    }

    JSONParser *parser = JSONParserInit(lexer);
    if (parser == NULL)
    {
        FreeJSONLexer(lexer);
        return NULL;
    }
    JSON *json = ParseJSON(parser);
    if (json == NULL)
    {
        // need to differentitate between parsing error because JSON is invalid or
        // if we ran out of memory trying to parse it.
        return NULL;
    }

    return json;
}

extern JSON *JSONFromFile(char *filename)
{
    FILE *file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
    {
        return NULL;
    }

    fseek(file_ptr, 0, SEEK_END);
    u_int64_t length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(file_ptr);
        errno = ENOMEM;
        return NULL;
    }

    fread(buffer, 1, length, file_ptr);
    fclose(file_ptr);
    buffer[length] = NULL_CHAR;

    JSON *json_from_string = StringToJSON(buffer);
    free(buffer);

    return json_from_string;
}

extern char *JSONToString(JSON *json, bool free_json)
{
    if (json == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    char *json_as_string = JSONValueToString(json->root);
    if (json_as_string == NULL)
    {
        FreeJSON(json);
        return NULL;
    }
    if (free_json)
    {
        FreeJSON(json);
    }
    return json_as_string;
}

#define FLOAT_CHAR_MAX 10
static char *doubleToString(double num)
{
    char *double_as_string = malloc((sizeof(char) * FLOAT_CHAR_MAX) + sizeof(char));
    if (double_as_string == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    (void)gcvt(num, FLOAT_CHAR_MAX, double_as_string);
    double_as_string[FLOAT_CHAR_MAX] = NULL_CHAR;
    return double_as_string;
}

extern char *JSONValueToString(JSONValue *json_value)
{
    if (json_value == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    char *json_value_string = NULL;
    switch (json_value->value_type)
    {
    case JSONLIST_t:
        json_value_string = ListToString((DynamicArray *)json_value->value);
        break;
    case JSONOBJ_t:
        json_value_string = ObjToString((HashMap *)json_value->value);
        break;
    case JSONNUMBER_INT_t:
        json_value_string = Int64ToString(*(int64_t *)json_value->value);
        break;
    case JSONNUMBER_DOUBLE_t:
        json_value_string = doubleToString(*(double *)json_value->value);
        break;
    case JSONSTRING_t:
        json_value_string = PutQuotesAroundString(json_value->value, false);
        break;
    case JSONBOOL_t:
        if (*(bool *)json_value->value == true)
        {
            json_value_string = malloc(sizeof(char) * 5);
            strcpy(json_value_string, JSON_BOOL_TRUE);
        }
        else if (*(bool *)json_value->value == false)
        {
            json_value_string = malloc(sizeof(char) * 6);
            strcpy(json_value_string, JSON_BOOL_FALSE);
        }
        break;
    case JSONNULL_t:
        json_value_string = malloc(sizeof(char) * 5);
        strcpy(json_value_string, JSON_NULL);
        break;
    default:
        break;
    }
    return json_value_string;
}

extern void FreeJSON(JSON *json)
{
    if (json == NULL)
    {
        errno = EINVAL;
        return;
    }
    if (json->root != NULL)
    {
        if (json->root->value != NULL)
        {
            if (json->root->value_type == JSONLIST_t)
            {
                FreeDynamicArray(json->root->value);
            }
            else if (json->root->value_type == JSONOBJ_t)
            {
                FreeHashMap(json->root->value);
            }
        }
        FreeJSONValue(json->root, false);
    }
    free(json);
}

// Pretty Print can be handled by piping into jq
extern void PrintJSON(JSON *json)
{
    if (json == NULL || json->root == NULL || json->root->value == NULL)
    {
        errno = EINVAL;
        return;
    }
    if (json->root->value_type == JSONLIST_t)
    {
        PrintDynamicArray(json->root->value);
    }
    else if (json->root->value_type == JSONOBJ_t)
    {
        PrintHashMap(json->root->value);
    }
}

extern void PrintJSONValue(JSONValue *json_value)
{
    if (json_value == NULL || json_value->value == NULL)
    {
        errno = EINVAL;
        return;
    }
    switch (json_value->value_type)
    {
    case JSONOBJ_t:
        printJSONObjValue((HashMap *)json_value->value);
        break;
    case JSONNUMBER_INT_t:
        printJSONNumberIntValue((int64_t *)json_value->value);
        break;
    case JSONNUMBER_DOUBLE_t:
        printJSONNumberDoubleValue((double *)json_value->value);
        break;
    case JSONSTRING_t:
        printJSONStringValue((char *)json_value->value);
        break;
    case JSONBOOL_t:
        printJSONBoolValue((bool *)json_value->value);
        break;
    case JSONNULL_t:
        printJSONNULLValue();
        break;
    case JSONLIST_t:
        printJSONListValue((DynamicArray *)json_value->value);
        break;
    default:
        break;
    }
}

static void printJSONStringValue(char *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    printf("\"%s\"", value);
}

static void printJSONNumberIntValue(int64_t *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    printf("%lld", *value);
}

static void printJSONNumberDoubleValue(double *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    printf("%lf", *value);
}

static void printJSONBoolValue(bool *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }

    if (*value == true)
    {
        printf("%s", JSON_BOOL_TRUE);
    }
    else if (*value == false)
    {
        printf("%s", JSON_BOOL_FALSE);
    }
}

static void printJSONNULLValue(void)
{
    printf("%s", JSON_NULL);
}

static void printJSONListValue(DynamicArray *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    PrintDynamicArray(value);
}

static void printJSONObjValue(HashMap *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    PrintHashMap(value);
}
