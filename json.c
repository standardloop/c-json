#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./json.h"
#include "./util.h"

static void printJSONStringValue(char *);
static void printJSONNumberIntValue(int64_t *value);
static void printJSONNumberDoubleValue(double *value);

static void printJSONBoolValue(bool *);
static void printJSONNULLValue(void);
static void printJSONListValue(DynamicArray *);
static void printJSONObjValue(HashMap *);

static char *listToString(DynamicArray *);
static char *objToString(HashMap *);

extern JSON *StringToJSON(char *input_str)
{
    if (input_str == NULL)
    {
        printf("[ERROR]: input StringToJSON is NULL\n");
        return NULL;
    }
    size_t input_str_len = strlen(input_str);
    if ((input_str[0] != CURLY_OPEN_CHAR && input_str[0] != BRACKET_OPEN_CHAR) ||
        (input_str[0] == CURLY_OPEN_CHAR && input_str[input_str_len - 1] != CURLY_CLOSE_CHAR) ||
        (input_str[0] == BRACKET_OPEN_CHAR && input_str[input_str_len - 1] != BRACKET_CLOSE_CHAR))
    {
        printf("[ERROR]: Cannot find starting and closing Bracket or Brace\n");
        return NULL;
    }

    // LexerDebugTest(input_str, true);
    Lexer *lexer = LexerInit(input_str);
    if (lexer == NULL)
    {
        return NULL;
    }
    Parser *parser = ParserInit(lexer);
    if (parser == NULL)
    {
        return NULL;
    }
    JSON *json = ParseJSON(parser);
    if (json == NULL)
    {
        return NULL;
    }
    return json;
}

extern JSON *JSONFromFile(char *filename)
{

    FILE *file_ptr = fopen(filename, "rb");

    if (file_ptr == NULL)
    {
        printf("[ERROR]: unable to open file \"%s\"\n", filename);
        return NULL;
    }
    char *buffer = NULL;
    fseek(file_ptr, 0, SEEK_END);
    u_int64_t length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, file_ptr);
    }
    fclose(file_ptr);

    if (buffer == NULL)
    {
        return NULL;
    }
    JSON *json_from_string = StringToJSON(buffer);
    free(buffer);

    return json_from_string;
}

extern char *JSONToString(JSON *json)
{
    if (json == NULL)
    {
        return NULL;
    }
    char *json_as_string = JSONValueToString(json->root);
    if (json_as_string == NULL)
    {
        FreeJSON(json);
        return NULL;
    }
    FreeJSON(json); // Do we want to handle free here?
    return json_as_string;
}

static char *listToString(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return NULL;
    }
    size_t list_as_string_size = 3; // "[]\0"
    char *list_as_string = malloc(sizeof(char) * list_as_string_size);
    list_as_string[0] = BRACKET_OPEN_CHAR;
    list_as_string[1] = NULL_CHAR;

    size_t chars_written = 2 - 1;

    bool needs_comma = false;
    for (u_int64_t i = 0; i < dynamic_array->size; i++)
    {
        char *list_element = JSONValueToString(dynamic_array->list[i]);
        size_t list_element_len = strlen(list_element);
        if (i < dynamic_array->size - 1)
        {
            needs_comma = true;
        }
        list_as_string_size += list_element_len;
        list_as_string_size += needs_comma;
        list_as_string = realloc(list_as_string, list_as_string_size);

        CopyStringCanary(list_as_string, list_element, chars_written);
        chars_written += list_element_len;
        if (needs_comma)
        {
            CopyStringCanary(list_as_string, ",", chars_written);
            chars_written++;
        }
        needs_comma = false;
        free(list_element);
    }

    list_as_string[list_as_string_size - 2] = BRACKET_CLOSE_CHAR;
    list_as_string[list_as_string_size - 1] = NULL_CHAR;
    return list_as_string;
}

static char *objToString(HashMap *map)
{
    if (map == NULL)
    {
        return NULL;
    }
    size_t obj_as_string_size = 3; // "{}\0"
    char *obj_as_string = malloc(sizeof(char) * obj_as_string_size);
    obj_as_string[0] = CURLY_OPEN_CHAR;
    obj_as_string[1] = NULL_CHAR;

    size_t chars_written = 2 - 1;

    u_int64_t entry_count = 0;
    for (u_int64_t i = 0; i < map->capacity; i++)
    {
        JSONValue *map_entry = map->entries[i];
        bool needs_comma = false;

        while (map_entry != NULL)
        {
            char *entry_key = map_entry->key;
            size_t duplicated_key_size = strlen(entry_key);
            char *duplicated_key = PutQuotesAroundString(entry_key, false);
            duplicated_key_size += 2;

            char *entry_value = JSONValueToString(map_entry);
            size_t entry_value_len = strlen(entry_value);
            if ((map_entry->next == NULL && entry_count < map->size - 1) || map_entry->next != NULL)
            {
                needs_comma = true;
            }

            obj_as_string_size += duplicated_key_size;
            obj_as_string_size++; // ':'
            obj_as_string_size += entry_value_len;
            obj_as_string_size += needs_comma;

            obj_as_string = realloc(obj_as_string, obj_as_string_size);

            CopyStringCanary(obj_as_string, duplicated_key, chars_written);
            chars_written += duplicated_key_size;
            CopyStringCanary(obj_as_string, ":", chars_written);
            chars_written++;
            CopyStringCanary(obj_as_string, entry_value, chars_written);
            chars_written += entry_value_len;
            if (needs_comma)
            {
                CopyStringCanary(obj_as_string, ",", chars_written);
                chars_written++;
            }
            free(duplicated_key);
            free(entry_value);
            if (map_entry->next == NULL)
            {
                entry_count++;
            }
            needs_comma = false;
            map_entry = map_entry->next;
        }
    }

    obj_as_string[obj_as_string_size - 2] = CURLY_CLOSE_CHAR;
    obj_as_string[obj_as_string_size - 1] = NULL_CHAR;
    return obj_as_string;
}

extern char *JSONValueToString(JSONValue *json_value)
{
    char *json_value_string = NULL;
    switch (json_value->value_type)
    {
    case LIST_t:
        json_value_string = listToString((DynamicArray *)json_value->value);
        break;
    case OBJ_t:
        json_value_string = objToString((HashMap *)json_value->value);
        break;
    case NUMBER_INT_t:
        json_value_string = Int64ToString(*(int64_t *)json_value->value);
        break;
    case NUMBER_DOUBLE_t:
        json_value_string = DoubleToString(*(double *)json_value->value);
        break;
    case STRING_t:
        json_value_string = PutQuotesAroundString(json_value->value, false);
        break;
    case BOOL_t:
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
    case NULL_t:
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
        return;
    }
    if (json->root != NULL)
    {
        if (json->root->value_type == LIST_t)
        {
            FreeDynamicArray(json->root->value);
        }
        else if (json->root->value_type == OBJ_t)
        {
            FreeHashMap(json->root->value);
        }
        FreeJSONValue(json->root, false);
    }
    free(json);
}

// Pretty Print can be handled by piping into jq
extern void PrintJSON(JSON *json)
{
    if (json == NULL)
    {
        return;
    }
    if (json->root->value_type == LIST_t)
    {
        PrintDynamicArray(json->root->value);
    }
    else if (json->root->value_type == OBJ_t)
    {
        PrintHashMap(json->root->value);
    }
}

extern void PrintJSONValue(JSONValue *json_value)
{
    if (json_value == NULL)
    {
        return;
    }
    switch (json_value->value_type)
    {
    case OBJ_t:
        printJSONObjValue(json_value->value);
        break;
    case NUMBER_INT_t:
        printJSONNumberIntValue(json_value->value);
        break;
    case NUMBER_DOUBLE_t:
        printJSONNumberDoubleValue(json_value->value);
        break;
    case STRING_t:
        printJSONStringValue(json_value->value);
        break;
    case BOOL_t:
        printJSONBoolValue(json_value->value);
        break;
    case NULL_t:
        printJSONNULLValue();
        break;
    case LIST_t:
        printJSONListValue(json_value->value);
        break;
    default:
        break;
    }
}

static void printJSONStringValue(char *value)
{
    printf("\"%s\"", value);
}

static void printJSONNumberIntValue(int64_t *value)
{
    printf("%lld", *value);
}

static void printJSONNumberDoubleValue(double *value)
{
    printf("%lf", *value);
}

static void printJSONBoolValue(bool *value)
{
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
    PrintDynamicArray(value);
}

static void printJSONObjValue(HashMap *value)
{
    PrintHashMap(value);
}
