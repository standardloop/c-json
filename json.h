/**
 * @file json.h
 * @headerfile json.h <standardloop/json.h>
 * @brief A C library for having a JSON variable in C.
 */

#ifndef STANDARDLOOP_JSON_H
#define STANDARDLOOP_JSON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <standardloop/collections.h>
#include <standardloop/util.h>

// ————————— JSON START —————————
/// @cond INTERNAL
#define JSON_BOOL_TRUE "true"
#define JSON_BOOL_FALSE "false"
#define JSON_NULL "null"
/// @endcond

/**
 * @brief The JSONValueType enum that stores the types for JSON values.
 */
enum JSONValueType
{
    /** Object. ex `{}`. */
    JSONOBJ_t,
    /** Integer Number. ex `1`. */
    JSONNUMBER_INT_t,
    /** Double Number. ex `1.1`. */
    JSONNUMBER_DOUBLE_t,
    /** String. ex `"hello"`. */
    JSONSTRING_t,
    /** Boolean. ex `true`. */
    JSONBOOL_t,
    /** Null. ex `null`. */
    JSONNULL_t,
    /** List. ex `[]`. */
    JSONLIST_t,
};

typedef struct
{
    enum JSONValueType value_type;
    union
    {
        List *list;
        HashMap *obj;
        int64_t *num_int;
        double *num_double;
        // void *null_json; // if null, then do need to hold it
        char *str;
        bool *boolean;
    };
} JSONValue;

extern JSONValue *JSONValueBlankInit();
extern JSONValue *JSONValueInit(enum JSONValueType value_type, void *value);

extern char *JSONValueToString(void *value);
extern void JSONValueFree(void *value);
extern void JSONValuePrint(void *value);
extern void *JSONValueDuplicate(void *value);

extern ItemValueOperations ItemValueJSONValueOperations;

typedef struct
{
    Item *root;
} JSON;

extern JSON *JSONInit();
extern void JSONFree(JSON *json);
extern void JSONPrint(JSON *json);
extern JSONValue *JSONGetRoot(JSON *json);

// ----

extern JSON *JSONInit();
extern JSON *StringToJSON(char *);
extern JSON *JSONFromFile(char *);
extern char *JSONToString(JSON *);

extern void TestJSON();
// ————————— JSON END —————————

// ————————— LEXER START —————————
#define NULL_CHAR_STRING "\0"
#define JSON_TOKEN_OPEN_CURLY_BRACE_STRING "{"
#define JSON_TOKEN_CLOSE_CURLY_BRACE_STRING "}"
#define JSON_TOKEN_OPEN_BRACKET_STRING "["
#define JSON_TOKEN_CLOSE_BRACKET_STRING "]"
#define JSON_TOKEN_COLON_STRING ":"
#define JSON_TOKEN_COMMA_STRING ","

enum JSONTokenType
{
    JSONTokenEOF,
    JSONTokenColon,
    JSONTokenOpenCurlyBrace,
    JSONTokenCloseCurlyBrace,
    JSONTokenOpenBracket,
    JSONTokenCloseBracket,
    JSONTokenComma,
    JSONTokenString,
    JSONTokenNumber,
    JSONTokenBool,
    JSONTokenNULL,
    JSONTokenIllegal,
};

typedef struct
{
    enum JSONTokenType type;
    u_int32_t start;
    u_int32_t end;
    u_int32_t line;
    char *literal;
} JSONToken;

typedef struct
{
    char *input;
    u_int32_t input_len;
    char current_char;
    u_int32_t position;
    u_int32_t read_position;
    u_int32_t line;
} JSONLexer;

extern JSONLexer *JSONLexerInit(char *);
extern void FreeJSONLexer(JSONLexer *);
extern JSONToken *JSONLex(JSONLexer *);
extern void PrintJSONToken(JSONToken *, bool);
extern void FreeJSONToken(JSONToken *);
extern bool IsJSONTokenValueType(JSONToken *, bool);

extern void JSONLexerDebugTest(char *, bool);

extern JSONToken *NewJSONToken(enum JSONTokenType, u_int32_t, u_int32_t,
                               u_int32_t, char *);

// ————————— LEXER END —————————

// ————————— PARSER START —————————
typedef struct
{
    JSONLexer *lexer;
    JSONToken *current_token;
    JSONToken *peek_token;
    bool input_error;
    bool memory_error;
    char *error_message;
    int64_t list_nested;
    int64_t obj_nested;
} JSONParser;

extern JSONParser *JSONParserInit(JSONLexer *);
extern void PrintJSONParserError(JSONParser *);
extern void FreeJSONParser(JSONParser *);
extern void PrintJSONParserErrorLine(JSONParser *);
extern void FreeJSONValue(JSONValue *, bool);
extern JSON *ParseJSON(JSONParser *);

// ————————— PARSER END —————————

#endif
