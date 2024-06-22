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
static JSONValue *parseObj(Parser *);
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
        return NULL;
    }
    parser->lexer = lexer;
    parser->current_token = NewToken(TokenEOF, 0, 0, NULL_CHAR_STRING);
    parser->peek_token = NewToken(TokenEOF, 0, 0, NULL_CHAR_STRING);

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

static void nextToken(Parser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    // FreeToken(parser->current_token);
    parser->current_token = parser->peek_token;
    parser->peek_token = Lex(parser->lexer);
}

static JSONValue *parseList(Parser *parser)
{
    // printf("parseList\n");
    // PrintToken(parser->current_token);
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    if (parser->current_token->type == TokenOpenBracket)
    {
        // nextToken(parser);
    }

    DynamicArray *list = DefaultDynamicArrayInit();
    // while (parser->current_token->type == TokenEOF)
    while (ALWAYS)
    {
        // PrintToken(parser->current_token);
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenComma)
        {
            nextToken(parser);
            break;
        }
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenEOF)
        {
            break;
        }
        if (parser->peek_token->type == TokenCloseCurlyBrace)
        {
            break;
        }
        JSONValue *list_value = parse(parser);
        if (list_value != NULL)
        {
            DynamicArrayAddLast(list, DynamicArrayElementInit(list_value->value_type, list_value->value, list_value->value_len));
        }
        // FreeJSONValue();
    }
    json_value->value_type = LIST_t;
    json_value->value = list;
    return json_value;
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
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenEOF)
        {
            break;
        }
        if (parser->peek_token->type == TokenCloseBracket)
        {
            break;
        }
        // nextToken(parser);
        JSONValue *obj_key = parse(parser);
        if (obj_key != NULL)
        {
            if (parser->peek_token->type == TokenColon)
            {
                nextToken(parser); // skip over colon
                if (parser->current_token->type == TokenColon)
                {
                    // nextToken(parser); // skip over colon
                    // if (parser->current_token->type == TokenString)
                    // {
                    //     printf("AYAYA\n");
                    // }
                    JSONValue *obj_value = parse(parser);
                    if (obj_value != NULL)
                    {
                        HashMapInsert(map, HashMapEntryInit(obj_key->value, obj_value->value, obj_value->value_type));
                    }
                }
            }
        }

        // FreeJSONValue();
    }
    json_value->value_type = OBJ_t;
    json_value->value = map;
    return json_value;
}

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
        // free(value);
        value_len = 0;
        json_value->value = NULL;
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
        // free(value);
        value_len = 1;
        json_value->value = new_bool;
    }
    else if (value_type == NUMBER_DOUBLE_t)
    {
        double *new_double = malloc(sizeof(double));
        double temp = atof((char *)value);
        *new_double = temp;
        value_len = 1;
        json_value->value = new_double;
        // free(value);
    }
    else if (value_type == NUMBER_INT_t)
    {
        int *new_int = malloc(sizeof(int));
        int temp = atoi((char *)value);
        *new_int = temp;
        value_len = 1;
        json_value->value = new_int;
        // free(value);
    }
    else
    {
        json_value->value = NULL;
        value_len = 0;
        // free(value);
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
    if (IsCharInString(parser->current_token->literal, DOT_CHAR))
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

    nextToken(parser);
    // PrintToken(parser->current_token);
    JSONValue *return_value = NULL;

    if (parser->current_token->type == TokenOpenCurlyBrace)
    {
        return_value = parseObj(parser);
    }
    else if (parser->current_token->type == TokenOpenBracket)
    {
        // printf("Parsing List!\n");
        return_value = parseList(parser);
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
    // else if (parser->current_token->type == TokenCloseBracket)
    // {
    //     // printf("found a TokenCloseBracket\n");
    //     return NULL;
    // }
    // else
    // {
    //     printf("big error in static JSONValue *parse\n");
    //     exit(1);
    //     return NULL;
    // }
    // FreeToken(parser->current_token);
    return return_value;
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
    json->root_value = parse(parser);
    // FreeParser(parser);
    return json;
}
