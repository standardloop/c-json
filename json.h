#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ————————— GLOBAL START —————————
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
// ————————— GLOBAL END —————————

// ————————— HASHMAP START —————————
#define DEFAULT_MAP_SIZE 10
#define DEFAULT_MAP_RESIZE_MULTIPLE 2

typedef u_int32_t(HashFunction)(char *, u_int32_t);

typedef struct hashMapEntry
{
    char *key;
    void *value;
    enum JSONValueType value_type;
    struct hashMapEntry *next;
} HashMapEntry;

typedef struct
{
    u_int32_t size;
    u_int32_t capacity;
    HashMapEntry **entries;
    HashFunction *hashFunction;
} HashMap;

extern HashMapEntry *HashMapEntryInit(char *, void *, enum JSONValueType);

extern HashMap *HashMapInit(u_int32_t, HashFunction *);
extern HashMap *DefaultHashMapInit(void);

extern HashMap *HashMapInitFromStr(char *);

extern void FreeHashMap(HashMap *);

extern void HashMapInsert(HashMap *, HashMapEntry *);
extern HashMapEntry *HashMapGet(HashMap *, char *);
extern void HashMapRemove(HashMap *, char *);

extern void PrintHashMap(HashMap *);
// ————————— HASHMAP END —————————

// ————————— DYN ARRAY START —————————
#define DEFAULT_DYN_ARR_SIZE 20
#define DEFAULT_DYN_ARR_RESIZE_MULTIPLE 2

typedef struct
{
    enum JSONValueType value_type;
    void *value;
    u_int32_t len;
} DynamicArrayElement;

typedef struct
{
    u_int32_t size;
    u_int32_t capacity;
    DynamicArrayElement **list;
} DynamicArray;

extern DynamicArray *DynamicArrayInit(u_int32_t);
extern DynamicArray *DefaultDynamicArrayInit(void);
extern DynamicArray *DynamicArrayInitFromStr(char *);
extern DynamicArray *DynamicArrayReplicate(DynamicArray *);

extern DynamicArrayElement *DynamicArrayElementInit(enum JSONValueType, void *, u_int32_t);

extern void DynamicArrayAddFirst(DynamicArray *, DynamicArrayElement *);
extern void DynamicArrayAddLast(DynamicArray *, DynamicArrayElement *);
extern void DynamicArrayAdd(DynamicArray *, DynamicArrayElement *, u_int32_t);

extern void DynamicArrayRemove(DynamicArray *, u_int32_t);
extern void DynamicArrayRemoveFirst(DynamicArray *);
extern void DynamicArrayRemoveLast(DynamicArray *);

extern void PrintDynamicArray(DynamicArray *);
extern void FreeDynamicArray(DynamicArray *);

extern char *DynamicArrayToString(DynamicArray *);

// ————————— DYN ARRAY END —————————

// ————————— JSON START —————————
#define JSON_BOOL_TRUE "true"
#define JSON_BOOL_FALSE "false"
#define JSON_NULL "null"

// union JSONValues
// {
//     void *null;
//     int *number_int;
//     double *number_double;
//     DynamicArray *list;
//     char *boolean;
//     char *string;
//     HashMap *obj;
// };

typedef struct
{
    enum JSONValueType value_type;
    // union JSONValues *value;
    void *value;
    u_int32_t value_len;
} JSONValue;

typedef struct
{
    JSONValue *root_value;
} JSON;

extern JSON *StringToJSON(char *);
extern char *JSONToString(JSON *);

extern void FreeJSON(JSON *);
extern void PrintJSON(JSON *);

extern void PrintJSONValue(enum JSONValueType, void *);
// ————————— JSON END —————————

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
    char *literal;
} Token;

typedef struct
{
    char *input;
    u_int32_t input_len;
    char current_char;
    u_int32_t position;
    u_int32_t read_position;
} Lexer;

extern Lexer *LexerInit(char *);
extern void FreeLexer(Lexer *);
extern Token *Lex(Lexer *);
extern void PrintToken(Token *);
extern void FreeToken(Token *);

extern void LexerRunTest(char *);

extern Token *NewToken(enum TokenType, u_int32_t, u_int32_t, char *);

// ————————— LEXER END —————————

// ————————— PARSER START —————————
typedef struct
{
    Lexer *lexer;
    Token *current_token;
    Token *peek_token;
} Parser;

extern Parser *ParserInit(Lexer *);
extern void FreeParser(Parser *);

extern JSON *ParseJSON(Parser *);
// ————————— PARSER END —————————

#endif
