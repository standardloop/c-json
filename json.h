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

typedef struct jsonValue
{
    char *key;
    enum JSONValueType value_type;
    void *value;
    struct jsonValue *next;
} JSONValue;

extern char *JSONValueToString(JSONValue *);

typedef struct
{
    JSONValue *root;
} JSON;

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
} JSONValueV2;

extern JSONValueV2 *JSONValueV2BlankInit();
extern JSONValueV2 *JSONValueV2Init(enum JSONValueType value_type, void *value);

extern char *JSONValueV2ToString(void *value);
extern void JSONValueV2Free(void *value);
extern void JSONValueV2Print(void *value);
extern void *JSONValueV2Duplicate(void *value);

typedef struct
{
    Item *root;
} JSONV2;

extern JSON *JSONInit();
extern JSON *StringToJSON(char *);
extern JSON *JSONFromFile(char *);
extern char *JSONToString(JSON *, bool);

extern void FreeJSON(JSON *);
extern void PrintJSON(JSON *);

extern void TestJSON();

extern void PrintJSONValue(JSONValue *);
// ————————— JSON END —————————

// ————————— HASHMAP START —————————
#define DEFAULT_MAP_SIZE 16
#define DEFAULT_MAP_RESIZE_MULTIPLE 2

typedef u_int32_t(HashFunction)(char *, u_int32_t);

typedef struct
{
    u_int32_t size;
    u_int32_t capacity;
    u_int32_t collision_count;
    JSONValue **entries;
    HashFunction *hashFunction;
    bool force_lowercase;
} JSONHashMap;

extern JSONValue *JSONHashMapGet(JSONHashMap *, char *);
extern void *JSONHashMapGetValueDirect(JSONHashMap *, char *);

extern JSONHashMap *JSONHashMapInit(u_int32_t, HashFunction *, bool);
extern JSONHashMap *DefaultJSONHashMapInit(void);
extern JSONHashMap *JSONHashMapReplicate(JSONHashMap *);
extern void FreeJSONHashMap(JSONHashMap *);
extern void JSONHashMapInsert(JSONHashMap *, JSONValue *);
extern void JSONHashMapRemove(JSONHashMap *, char *);
extern void PrintJSONHashMap(JSONHashMap *);
extern char *ObjToString(JSONHashMap *);

// ————————— HASHMAP END —————————

// ————————— DYN ARRAY START —————————
#define DEFAULT_DYN_ARR_SIZE 16
#define DEFAULT_DYN_ARR_RESIZE_MULTIPLE 2

typedef struct
{
    u_int32_t size;
    u_int32_t capacity;
    JSONValue **list;
} DynamicArray;

extern DynamicArray *DynamicArrayInit(u_int32_t);
extern DynamicArray *DefaultDynamicArrayInit(void);
extern DynamicArray *DynamicArrayInitFromStr(char *);
extern DynamicArray *DynamicArrayReplicate(DynamicArray *);

extern char *DynamicArrayToString(DynamicArray *);
extern void DynamicArrayAddFirst(DynamicArray *, JSONValue *);
extern void DynamicArrayAddLast(DynamicArray *, JSONValue *);
extern void DynamicArrayAdd(DynamicArray *, JSONValue *, u_int32_t);

extern void DynamicArrayRemove(DynamicArray *, u_int32_t);
extern void DynamicArrayRemoveFirst(DynamicArray *);
extern void DynamicArrayRemoveLast(DynamicArray *);

extern JSONValue *DynamicArrayGetAtIndex(DynamicArray *, u_int32_t);

extern void PrintDynamicArray(DynamicArray *);
extern void FreeDynamicArray(DynamicArray *);
// ————————— DYN ARRAY END —————————

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
extern JSONValue *JSONValueReplicate(JSONValue *);
extern JSONValue *JSONValueInit(enum JSONValueType, void *, char *);

// ————————— PARSER END —————————

#endif
