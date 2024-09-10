#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "./json.h"

static void nextToken(Parser *);
static JSONValue *parse(Parser *);
static JSONValue *parseList(Parser *);
static bool parseListErrorHelper(Parser *);
static bool parseListLoopChecker(Parser *);

static JSONValue *parseObj(Parser *);
static bool parseObjErrorHelper(Parser *);
static bool parseObjLoopChecker(Parser *);

static JSONValue *parseNumber(Parser *);
static JSONValue *initQuickJSONValue(enum JSONValueType, void *);

static bool isCharInString(const char *, char);

extern Parser *ParserInit(Lexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }

    Parser *parser = malloc(sizeof(Parser));
    if (parser == NULL)
    {
        FreeLexer(lexer);
        printf("[ERROR]: couldn't allocate memory for Parser\n");
        return NULL;
    }

    parser->input_error = false;
    parser->memory_error = false;
    parser->error_message = NULL;
    parser->lexer = lexer;
    parser->list_nested = 0;
    parser->obj_nested = 0;
    parser->current_token = NULL;
    parser->peek_token = NULL;

    nextToken(parser);

    return parser;
}

extern void FreeParser(Parser *parser)
{

    if (parser != NULL)
    {
        if (parser->lexer != NULL)
        {
            FreeLexer(parser->lexer);
        }
        if (parser->current_token != NULL)
        {
            FreeToken(parser->current_token);
        }
        if (parser->peek_token != NULL)
        {
            FreeToken(parser->peek_token);
        }
        free(parser);
    }
}

extern void PrintParserError(Parser *parser)
{
    if (parser->current_token != NULL)
    {
        PrintToken(parser->current_token, false);
    }
    if (parser->input_error == true)
    {
        if (parser->error_message != NULL)
        {
            printf("[ERROR]: %s\n", parser->error_message);
        }
    }
}

static void nextToken(Parser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    FreeToken(parser->current_token);
    parser->current_token = parser->peek_token;
    if (parser->current_token != NULL)
    {
        if (parser->current_token->type == TokenOpenBracket)
        {
            parser->list_nested++;
        }
        else if (parser->current_token->type == TokenCloseBracket)
        {
            parser->list_nested--;
        }
        if (parser->current_token->type == TokenOpenCurlyBrace)
        {
            parser->obj_nested++;
        }
        else if (parser->current_token->type == TokenCloseCurlyBrace)
        {
            parser->obj_nested--;
        }
    }
    parser->peek_token = Lex(parser->lexer);
}

static bool parseListErrorHelper(Parser *parser)
{
    // printf("%lld\n", parser->list_nested);
    if (parser->input_error == true)
    {
        return true;
    }
    if (parser->current_token->type == TokenIllegal)
    {
        parser->input_error = true;
        parser->error_message = "Illegal token found";
        return true;
    }
    if (parser->peek_token->type == TokenEOF && parser->current_token->type != TokenCloseBracket)
    {
        parser->input_error = true;
        parser->error_message = "Closing Bracket not found";
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type != TokenEOF && parser->peek_token->type != TokenCloseBracket && parser->peek_token->type != TokenCloseCurlyBrace && parser->peek_token->type != TokenComma)
    {
        parser->input_error = true;
        parser->error_message = "Unexpected token after Closing Bracket";
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenEOF && (parser->list_nested != 0 || parser->obj_nested != 0))
    {
        parser->input_error = true;
        parser->error_message = "Invalid Token after Closing Bracket";
        return true;
    }
    if (parser->current_token->type == TokenComma && !IsTokenValueType(parser->peek_token, true))
    {
        parser->input_error = true;
        parser->error_message = "Value expected after comma";
        return true;
    }

    parser->input_error = false;
    parser->error_message = NULL;
    return false;
}

static inline bool parseListLoopChecker(Parser *parser)
{
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenComma)
    {
        nextToken(parser); // skip comma
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenEOF && parser->list_nested == 0)
    {
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenCloseCurlyBrace)
    {
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenCloseBracket)
    {
        return true;
    }
    return false;
}

static JSONValue *parseList(Parser *parser)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        parser->memory_error = true;
        parser->error_message = "[ERROR]: not enough memory for creating JSONValue inside parseList";
        return NULL;
    }
    if (parser->current_token->type != TokenOpenBracket)
    {
        FreeJSONValue(json_value, false);
        return NULL;
    }
    DynamicArray *list = DefaultDynamicArrayInit();
    if (list == NULL)
    {
        parser->memory_error = true;
        parser->error_message = "[ERROR]: not enough memory for creating DynamicArray inside parseList";
        return NULL;
    }
    while (ALWAYS)
    {
        JSONValue *list_value = parse(parser);
        if (parseListErrorHelper(parser))
        {
            FreeDynamicArray(list);
            FreeJSONValue(json_value, false);
            // parser->input_error; // parseListErrorHelper writes this value
            // parser->input_error; // parseListErrorHelper writes this value
            return NULL;
        }
        if (list_value == NULL)
        {
            // PrintToken(parser->current_token, false);
        }
        else
        {
            DynamicArrayAddLast(list, list_value);
        }
        if (parseListLoopChecker(parser))
        {
            break;
        }
    }

    json_value->value_type = LIST_t;
    json_value->value = list;
    return json_value;
}

extern void FreeJSONValue(JSONValue *json_value, bool deep)
{
    if (json_value != NULL)
    {
        if (deep && json_value->value != NULL)
        {
            if (json_value->value_type == LIST_t)
            {
                FreeDynamicArray(json_value->value);
            }
            else if (json_value->value_type == OBJ_t)
            {
                FreeHashMap(json_value->value);
            }
            else if (json_value->value_type != NULL_t)
            {
                free(json_value->value);
            }
            json_value->value = NULL;
        }
        free(json_value);
    }
}

static bool parseObjErrorHelper(Parser *parser)
{
    if (parser->input_error)
    {
        return true;
    }

    if (parser->current_token->type == TokenIllegal)
    {
        parser->input_error = true;
        parser->error_message = "Illegal token found";
        return true;
    }
    if (parser->peek_token->type == TokenEOF && parser->current_token->type != TokenCloseCurlyBrace)
    {
        parser->input_error = true;
        parser->error_message = "Closing Brace not found";
        return true;
    }
    if (parser->peek_token->type == TokenEOF && parser->current_token->type == TokenCloseCurlyBrace && (parser->obj_nested != 0 || parser->list_nested != 0))
    {
        parser->input_error = true;
        parser->error_message = "Unmatch brackets or braces";
        return true;
    }
    if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type != TokenEOF && parser->peek_token->type != TokenCloseBracket && parser->peek_token->type != TokenCloseCurlyBrace && parser->peek_token->type != TokenComma)
    {
        parser->input_error = true;
        parser->error_message = "Unexpected token after Closing Brace";
        return true;
    }

    return false;
}

static bool parseObjLoopChecker(Parser *parser)
{
    if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenComma)
    {
        nextToken(parser);
        return true;
    }
    if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenCloseCurlyBrace)
    {
        nextToken(parser);
        return true;
    }
    if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenEOF)
    {
        return true;
    }
    if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenCloseBracket)
    {
        return true;
    }
    return false;
}

static JSONValue *parseObj(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        parser->memory_error = true;
        parser->error_message = "[ERROR]: not enough memory for creating JSONValue inside parseObj";
        return NULL;
    }

    HashMap *map = DefaultHashMapInit();
    if (map == NULL)
    {
        parser->memory_error = true;
        parser->error_message = "[ERROR]: not enough memory for creating HashMap inside parseObj";
        return NULL;
    }
    while (ALWAYS)
    {
        if (parseObjErrorHelper(parser))
        {
            FreeHashMap(map);
            FreeJSONValue(json_value, true);
            // parser->input_error; // parseObjErrorHelper writes this value
            // parser->error_message; // parseObjErrorHelper writes this value
            return NULL;
        }

        if (parseObjLoopChecker(parser))
        {
            break;
        }

        JSONValue *obj_key = parse(parser);
        if (obj_key != NULL && (obj_key->value == NULL || obj_key->value_type != STRING_t))
        {
            FreeHashMap(map);
            FreeJSONValue(obj_key, true);
            FreeJSONValue(json_value, false);
            parser->input_error = true;
            parser->error_message = "Object key must be a string";
            return NULL;
        }
        // FIXME, maybe just else?
        if (obj_key != NULL && obj_key->value != NULL)
        {
            if (parser->peek_token->type == TokenColon)
            {
                // printf("[JOSH]: %s\n", (char *)obj_key->value);
                nextToken(parser); // skip over colon
                if (!IsTokenValueType(parser->peek_token, true))
                {
                    FreeHashMap(map);
                    FreeJSONValue(obj_key, true);
                    FreeJSONValue(json_value, false);
                    parser->input_error = true;
                    parser->error_message = "Invalid Token after colon, expecting value";
                    return NULL;
                }
                JSONValue *obj_value = parse(parser);
                if (obj_value == NULL)
                {
                    // PrintToken(parser->current_token, false);
                    // PrintToken(parser->peek_token, false);
                    printf("FIXME\n");
                    printf("this should never be NULL\n");
                }
                else
                {
                    obj_value->key = obj_key->value;
                    HashMapInsert(map, obj_value);
                }
            }
            else
            {
                FreeHashMap(map);
                FreeJSONValue(obj_key, true);
                FreeJSONValue(json_value, false);
                parser->input_error = true;
                parser->error_message = "Colon not found after key";
                return NULL;
            }
            FreeJSONValue(obj_key, false);
        }
    }
    json_value->value_type = OBJ_t;
    json_value->value = map;
    return json_value;
}

// Can this be combined with JSONValueInit?
static JSONValue *initQuickJSONValue(enum JSONValueType value_type, void *value)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    if (value_type == STRING_t)
    {
        json_value->value = value;
    }
    else if (value_type == NULL_t)
    {
        json_value->value = NULL;
        free(value);
    }
    else if (value_type == BOOL_t)
    {
        bool *new_bool = malloc(sizeof(bool));
        if (((char *)value)[0] == 't')
        {
            *new_bool = true;
        }
        else
        {
            *new_bool = false;
        }
        json_value->value = new_bool;
        free(value);
    }
    else if (value_type == NUMBER_DOUBLE_t)
    {
        double *new_double = malloc(sizeof(double));
        *new_double = atof((char *)value);
        json_value->value = new_double;
        free(value);
    }
    else if (value_type == NUMBER_INT_t)
    {
        int64_t *new_int = malloc(sizeof(int64_t));
        *new_int = (int64_t)atof((char *)value);
        json_value->value = new_int;
        free(value);
    }
    else
    {
        json_value->value = NULL;
        // free(value); // FIXME
    }
    json_value->value_type = value_type;
    return json_value;
}

static bool isCharInString(const char *input_str, char checker)
{
    while (*input_str != NULL_CHAR)
    {
        if (*input_str == checker)
        {
            return true;
        }
        input_str++;
    }
    return false;
}

static JSONValue *parseNumber(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    // FIXME:
    // there is an issue here where number such as 1e-2 are actually decimals
    // For now all scientific notation with be made into a floating point
    if (isCharInString(parser->current_token->literal, DOT_CHAR) || isCharInString(parser->current_token->literal, 'e') || isCharInString(parser->current_token->literal, 'E'))
    {
        return initQuickJSONValue(NUMBER_DOUBLE_t, parser->current_token->literal);
    }
    else
    {
        return initQuickJSONValue(NUMBER_INT_t, parser->current_token->literal);
    }
}

static JSONValue *parse(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    // printf("[JOSH]: %u\n", (unsigned int)parser->list_nested);

    nextToken(parser);
    // PrintToken(parser->current_token, false);
    JSONValue *return_value = NULL;

    if (parser->current_token->type == TokenOpenCurlyBrace)
    {
        // printf("Parsing Object!\n");
        // parser->obj_nested++;
        return_value = parseObj(parser);
    }
    else if (parser->current_token->type == TokenCloseCurlyBrace)
    {
        // printf("found a TokenCloseBracket\n");
        // parser->obj_nested--;
    }
    else if (parser->current_token->type == TokenOpenBracket)
    {
        // printf("Parsing List!\n");
        // parser->list_nested++;
        // PrintToken(parser->peek_token, false);
        return_value = parseList(parser);
    }
    else if (parser->current_token->type == TokenCloseBracket)
    {
        // printf("found a TokenCloseBracket\n");
        // parser->list_nested--;
    }
    else if (parser->current_token->type == TokenString)
    {
        // printf("TokenString\n");
        return_value = initQuickJSONValue(STRING_t, parser->current_token->literal);
    }
    else if (parser->current_token->type == TokenNumber)
    {
        return_value = parseNumber(parser);
    }
    else if (parser->current_token->type == TokenBool)
    {
        return_value = initQuickJSONValue(BOOL_t, parser->current_token->literal);
    }
    else if (parser->current_token->type == TokenNULL)
    {
        // printf("test\n");
        return_value = initQuickJSONValue(NULL_t, parser->current_token->literal);
    }
    else if (parser->current_token->type == TokenIllegal)
    {
        // exit(100);
    }

    return return_value;
}

#define PRINT_ERROR_LINE_PREFIX_OFFSET 3
#define PRINT_ERROR_LINE_SUFFIX_OFFSET 0

extern void PrintErrorLine(Parser *parser)
{
    if (parser == NULL || parser->lexer == NULL || parser->lexer->input == NULL)
    {
        return;
    }

    char *json_str_iterator = parser->lexer->input + parser->current_token->start - PRINT_ERROR_LINE_PREFIX_OFFSET;
    for (u_int32_t i = parser->current_token->start - PRINT_ERROR_LINE_PREFIX_OFFSET; i < parser->current_token->end + PRINT_ERROR_LINE_SUFFIX_OFFSET; i++)
    {
        if (json_str_iterator == NULL || *json_str_iterator == NULL_CHAR)
        {
            break;
        }
        printf("%c", *json_str_iterator);
        json_str_iterator++;
    }
    printf("\n");

    for (u_int32_t i = parser->current_token->start - PRINT_ERROR_LINE_PREFIX_OFFSET; i < parser->current_token->end + PRINT_ERROR_LINE_SUFFIX_OFFSET; i++)
    {
        if (i >= parser->current_token->start)
        {
            printf("^");
        }
        else
        {
            printf(" ");
        }
    }
    printf("\n");
}

extern JSON *ParseJSON(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    JSON *json = malloc(sizeof(JSON));
    if (json == NULL)
    {
        printf("[ERROR]: Not enough memory for JSON\n");
        FreeParser(parser);
        return NULL;
    }
    json->root = parse(parser);
    // probably want the error to be on JSON obj so it can be read before being freed
    // right now it just prints to stdout, but for cerver, we would want access to that error message
    if (json->root == NULL)
    {
        PrintErrorLine(parser);
        PrintParserError(parser);
        FreeParser(parser);
        FreeJSON(json);
        return NULL;
    }
    FreeParser(parser);
    return json;
}

extern JSONValue *JSONValueInit(enum JSONValueType type, void *value, char *key)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    json_value->key = key;
    json_value->value_type = type;
    json_value->value = value;
    json_value->next = NULL;
    return json_value;
}

// FIXME, object support
extern JSONValue *JSONValueReplicate(JSONValue *json_value)
{
    if (json_value == NULL)
    {
        return NULL;
    }
    void *value = NULL;
    if (json_value->value_type == NUMBER_DOUBLE_t)
    {
        value = (double *)malloc(sizeof(double) * 1);
        memcpy(value, json_value->value, sizeof(double) * 1);
    }
    else if (json_value->value_type == NUMBER_INT_t)
    {
        value = (int *)malloc(sizeof(int) * 1);
        memcpy(value, json_value->value, sizeof(int) * 1);
    }
    else if (json_value->value_type == STRING_t)
    {
        size_t value_len = strlen((char *)json_value->value) + 1;
        value = (char *)malloc(sizeof(char) * value_len);
        memcpy(value, json_value->value, sizeof(char) * value_len);
    }
    else if (json_value->value_type == LIST_t)
    {
        value = (DynamicArray *)DynamicArrayReplicate((DynamicArray *)json_value->value);
    }
    else if (json_value->value_type == OBJ_t)
    {
        value = (HashMap *)HashMapReplicate((HashMap *)json_value->value); // WIP
    }
    return JSONValueInit(json_value->value_type, value, NULL);
}
