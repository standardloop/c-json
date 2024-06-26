#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "./json.h"
#include "./util.h"

static void nextToken(Parser *);
static JSONValue *parse(Parser *);
static JSONValue *parseList(Parser *);
static bool parseListErrorHelper(Parser *);
static JSONValue *parseObj(Parser *);
static bool parseObjErrorHelper(Parser *);
static JSONValue *parseNumber(Parser *);

static JSONValue *initQuickJSONValue(enum JSONValueType, void *);

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
        return NULL;
    }
    parser->error = false;
    parser->error_message = NULL;
    parser->lexer = lexer;
    parser->list_nested = 0;
    parser->obj_nested = 0;
    parser->current_token = NewToken(TokenEOF, 0, 0, 0, NULL_CHAR_STRING);
    parser->peek_token = NewToken(TokenEOF, 0, 0, 0, NULL_CHAR_STRING);

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
    if (parser->error == true)
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
    parser->peek_token = Lex(parser->lexer);
}

static bool parseListErrorHelper(Parser *parser)
{

    if (parser->current_token->type == TokenIllegal)
    {
        parser->error = true;
        parser->error_message = "Illegal token found";
        return true;
    }
    if (parser->peek_token->type == TokenEOF && parser->current_token->type != TokenCloseBracket)
    {
        parser->error = true;
        parser->error_message = "Closing Bracket not found";
        return true;
    }
    if (IsTokenValueType(parser->current_token, false) && (parser->peek_token->type != TokenComma && parser->peek_token->type != TokenCloseBracket))
    {
        parser->error = true;
        parser->error_message = "Invalid token";
        return true;
    }
    if (parser->current_token->type == TokenComma && !IsTokenValueType(parser->peek_token, true))
    {
        parser->error = true;
        parser->error_message = "Invalid token after comma";
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenEOF && parser->list_nested > 1)
    {
        parser->error = true;
        parser->error_message = "Invalid token, Expected EOF";
        return true;
    }
    if (parser->current_token->type == TokenCloseBracket && (parser->peek_token->type != TokenEOF && parser->peek_token->type != TokenComma && parser->peek_token->type != TokenCloseCurlyBrace))
    {
        if (parser->list_nested >= 2)
        {
            parser->error = true;
            parser->error_message = "Invalid Token after Closing Bracket";
            return true;
        }
    }
    parser->error = false;
    parser->error_message = NULL;
    return false;
}

static JSONValue *parseList(Parser *parser)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    if (parser->current_token->type != TokenOpenBracket)
    {
        FreeJSONValue(json_value, false);
        return NULL;
    }
    DynamicArray *list = DefaultDynamicArrayInit();

    while (ALWAYS)
    {
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenComma)
        {
            nextToken(parser); // skip comma
            break;
        }
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenEOF && parser->list_nested == 0)
        {
            break;
        }
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenCloseCurlyBrace)
        {
            break;
        }

        JSONValue *list_value = parse(parser);
        if (parseListErrorHelper(parser))
        {
            FreeDynamicArray(list);
            FreeJSONValue(json_value, false);
            // parser->error; // parseListErrorHelper writes this value
            // parser->error; // parseListErrorHelper writes this value
            return NULL;
        }
        if (list_value == NULL)
        {
        }
        else
        {
            DynamicArrayAddLast(list, list_value);
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
        if (deep)
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
        }
        free(json_value);
    }
}

static bool parseObjErrorHelper(Parser *parser)
{
    if (parser->current_token->type == TokenIllegal)
    {
        parser->error = true;
        parser->error_message = "Illegal token found";
        return true;
    }
    if (parser->peek_token->type == TokenEOF && parser->current_token->type != TokenCloseCurlyBrace)
    {
        parser->error = true;
        parser->error_message = "Closing Brace not found";
        return true;
    }
    if (parser->current_token->type == TokenComma && parser->peek_token->type != TokenString)
    {
        parser->error = true;
        parser->error_message = "Invalid token after comma, expected key string";
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
        return NULL;
    }

    HashMap *map = DefaultHashMapInit();
    while (ALWAYS)
    {
        // PrintToken(parser->current_token);
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenComma)
        {
            nextToken(parser);
            break;
        }
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenCloseCurlyBrace && parser->obj_nested >= 1)
        {
            break;
        }
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->obj_nested == 0 && parser->peek_token->type == TokenEOF)
        {
            break;
        }
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenCloseBracket)
        {
            break;
        }

        if (parseObjErrorHelper(parser))
        {
            FreeHashMap(map);
            FreeJSONValue(json_value, true);
            // parser->error; // parseObjErrorHelper writes this value
            // parser->error_message; // parseObjErrorHelper writes this value
            return NULL;
        }
        JSONValue *obj_key = parse(parser);
        if (obj_key != NULL && (obj_key->value_type != STRING_t || obj_key->value == NULL))
        {
            FreeHashMap(map);
            FreeJSONValue(obj_key, true);
            FreeJSONValue(json_value, false);
            parser->error = true;
            parser->error_message = "Object key must be a string";
            return NULL;
        }
        if (obj_key != NULL && obj_key->value != NULL)
        {
            // printf("key: %s\n", (char *)obj_key->value);

            // FIXME: maybe add a flag to enforce object key uniqueness?
            // According to spec is not forbidden to have a duplicate key, but it is discouraged.
            // if (HashMapGet(map, obj_key->value) != NULL)
            // {
            //     FreeHashMap(map);
            //     FreeJSONValue(obj_key, true);
            //     FreeJSONValue(json_value, false);
            //     parser->error = true;
            //     parser->error_message = "Duplicate object key found";
            //     return NULL;
            // }

            if (parser->peek_token->type == TokenColon)
            {
                // printf("[JOSH]: %s\n", (char *)obj_key->value);
                nextToken(parser); // skip over colon
                if (!IsTokenValueType(parser->peek_token, true))
                {
                    FreeHashMap(map);
                    FreeJSONValue(obj_key, true);
                    FreeJSONValue(json_value, false);
                    parser->error = true;
                    parser->error_message = "Invalid Token after colon, expecting value";
                    return NULL;
                }
                JSONValue *obj_value = parse(parser);
                if (obj_value == NULL)
                {
                    printf("FIXME\n");
                }
                else
                {
                    obj_value->key = obj_key->value;
                    if (obj_value != NULL)
                    {
                        HashMapInsert(map, obj_value);
                    }
                }
            }

            else
            {
                FreeHashMap(map);
                FreeJSONValue(obj_key, true);
                FreeJSONValue(json_value, false);
                parser->error = true;
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
    u_int32_t value_len;
    if (value_type == STRING_t)
    {
        value_len = strlen((char *)value) + 1;
        json_value->value = value;
    }
    else if (value_type == NULL_t)
    {
        value_len = 0;
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
        value_len = 1;
        json_value->value = new_bool;
        free(value);
    }
    else if (value_type == NUMBER_DOUBLE_t)
    {
        double *new_double = malloc(sizeof(double));
        *new_double = atof((char *)value);
        value_len = 1;
        json_value->value = new_double;
        free(value);
    }
    else if (value_type == NUMBER_INT_t)
    {
        int64_t *new_int = malloc(sizeof(int64_t));
        *new_int = (int64_t)atof((char *)value);
        value_len = 1;
        json_value->value = new_int;
        free(value);
    }
    else
    {
        json_value->value = NULL;
        value_len = 0;
        // free(value); // FIXME
    }
    json_value->value_type = value_type;
    json_value->value_len = value_len;
    return json_value;
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
    if (IsCharInString(parser->current_token->literal, DOT_CHAR) || IsCharInString(parser->current_token->literal, 'e') || IsCharInString(parser->current_token->literal, 'E'))
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
    // PrintToken(parser->current_token, true);
    JSONValue *return_value = NULL;

    if (parser->current_token->type == TokenOpenCurlyBrace)
    {
        // printf("Parsing Object!\n");
        parser->obj_nested++;
        return_value = parseObj(parser);
    }
    else if (parser->current_token->type == TokenCloseCurlyBrace)
    {
        // printf("found a TokenCloseBracket\n");
        parser->obj_nested--;
    }
    else if (parser->current_token->type == TokenOpenBracket)
    {
        // printf("Parsing List!\n");
        parser->list_nested++;
        return_value = parseList(parser);
    }
    else if (parser->current_token->type == TokenCloseBracket)
    {
        // printf("found a TokenCloseBracket\n");
        parser->list_nested--;
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

    // else
    // {
    //     return NULL;
    // }
    // else if (parser->current_token->type == TokenEOF)
    // {
    //     // FIXME
    //     // printf("found token EOF!\n");
    //     return NULL;
    // }
    // else if (parser->current_token->type == TokenComma)
    // {
    //     // printf("found a comma!!\n");
    //     return NULL;
    // }

    // else
    // {
    //     printf("big error in JSONValue *parse\n");
    //     exit(1);
    //     return NULL;
    // }
    // FreeToken(parser->current_token);

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
        FreeParser(parser);
        return NULL;
    }
    json->root = parse(parser);
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

extern JSONValue *JSONValueInit(enum JSONValueType type, void *value, char *key, u_int32_t len)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    json_value->key = key;
    json_value->value_type = type;
    json_value->value = value;
    json_value->value_len = len;
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
    switch (json_value->value_type)
    {
    case NUMBER_DOUBLE_t:
        value = (double *)malloc(sizeof(double) * json_value->value_len);
        memcpy(value, json_value->value, sizeof(double) * json_value->value_len);
        break;
    case NUMBER_INT_t:
        value = (int *)malloc(sizeof(int) * json_value->value_len);
        memcpy(value, json_value->value, sizeof(int) * json_value->value_len);
        break;
    case STRING_t:
        value = (char *)malloc(sizeof(char) * json_value->value_len);
        memcpy(value, json_value->value, sizeof(char) * json_value->value_len);
        break;
    case LIST_t:
        value = (DynamicArray *)DynamicArrayReplicate(json_value->value);
    default:
        break;
    }
    return JSONValueInit(json_value->value_type, value, NULL, json_value->value_len);
}
