#ifndef STANDARDLOOP_JSON_H
#define STANDARDLOOP_JSON_H

#define STANDARDLOOP_JSON_H_MAJOR_VERSION 0
#define STANDARDLOOP_JSON_H_MINOR_VERSION 0
#define STANDARDLOOP_JSON_H_PATCH_VERSION 4
#define STANDARDLOOP_JSON_H_VERSION "0.0.4"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ————————— JSON START —————————
#define JSON_BOOL_TRUE "true"
#define JSON_BOOL_FALSE "false"
#define JSON_NULL "null"

enum JSONValueType
{
    JSONOBJ_t,
    JSONNUMBER_INT_t,
    JSONNUMBER_DOUBLE_t,
    JSONSTRING_t,
    JSONBOOL_t,
    JSONNULL_t,
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

extern JSON *JSONInit();
extern JSON *StringToJSON(char *);
extern JSON *JSONFromFile(char *);
extern char *JSONToString(JSON *, bool);

extern void FreeJSON(JSON *);
extern void PrintJSON(JSON *);

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
} HashMap;

extern JSONValue *HashMapGet(HashMap *, char *);
extern void *HashMapGetValueDirect(HashMap *, char *);

extern HashMap *HashMapInit(u_int32_t, HashFunction *, bool);
extern HashMap *DefaultHashMapInit(void);
extern HashMap *HashMapReplicate(HashMap *);
extern void FreeHashMap(HashMap *);
extern void HashMapInsert(HashMap *, JSONValue *);
extern void HashMapRemove(HashMap *, char *);
extern void PrintHashMap(HashMap *);
extern char *ObjToString(HashMap *);

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

extern char *ListToString(DynamicArray *);
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

extern JSONToken *NewJSONToken(enum JSONTokenType, u_int32_t, u_int32_t, u_int32_t, char *);

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

// ————————— UTIL BEGIN —————————
#include <standardloop/util.h>
// ————————— UTIL END —————————

#endif
