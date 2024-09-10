#ifndef STANDARDLOOP_JSON_H
#define STANDARDLOOP_JSON_H

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
    OBJ_t,
    NUMBER_INT_t,
    NUMBER_DOUBLE_t,
    STRING_t,
    BOOL_t,
    NULL_t,
    LIST_t,
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

extern JSON *StringToJSON(char *);
extern JSON *JSONFromFile(char *);
extern char *JSONToString(JSON *);

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
#define TOKEN_OPEN_CURLY_BRACE_STRING "{"
#define TOKEN_CLOSE_CURLY_BRACE_STRING "}"
#define TOKEN_OPEN_BRACKET_STRING "["
#define TOKEN_CLOSE_BRACKET_STRING "]"
#define TOKEN_COLON_STRING ":"
#define TOKEN_COMMA_STRING ","

enum TokenType
{
    TokenEOF,
    TokenColon,
    TokenOpenCurlyBrace,
    TokenCloseCurlyBrace,
    TokenOpenBracket,
    TokenCloseBracket,
    TokenComma,
    TokenString,
    TokenNumber,
    TokenBool,
    TokenNULL,
    TokenIllegal,
};

typedef struct
{
    enum TokenType type;
    u_int32_t start;
    u_int32_t end;
    u_int32_t line;
    char *literal;
} Token;

typedef struct
{
    char *input;
    u_int32_t input_len;
    char current_char;
    u_int32_t position;
    u_int32_t read_position;
    u_int32_t line;
} Lexer;

extern Lexer *LexerInit(char *);
extern void FreeLexer(Lexer *);
extern Token *Lex(Lexer *);
extern void PrintToken(Token *, bool);
extern void FreeToken(Token *);
extern bool IsTokenValueType(Token *, bool);

extern void LexerDebugTest(char *, bool);

extern Token *NewToken(enum TokenType, u_int32_t, u_int32_t, u_int32_t, char *);

// ————————— LEXER END —————————

// ————————— PARSER START —————————
typedef struct
{
    Lexer *lexer;
    Token *current_token;
    Token *peek_token;
    bool input_error;
    bool memory_error;
    char *error_message;
    int64_t list_nested;
    int64_t obj_nested;
} Parser;

extern Parser *ParserInit(Lexer *);
extern void PrintParserError(Parser *);
extern void FreeParser(Parser *);
extern void PrintErrorLine(Parser *);
extern void FreeJSONValue(JSONValue *, bool);
extern JSON *ParseJSON(Parser *);
extern JSONValue *JSONValueReplicate(JSONValue *);
extern JSONValue *JSONValueInit(enum JSONValueType, void *, char *);

// ————————— PARSER END —————————

// ————————— UTIL BEGIN —————————
#include <standardloop/util.h>
// ————————— UTIL END —————————

#endif
