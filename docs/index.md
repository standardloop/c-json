# API Reference

## Classes

| Name                            | Description |
| ------------------------------- | ----------- |
| [`JSON`](#json)                 |             |
| [`HashMap`](#hashmap)           |             |
| [`DynamicArray`](#dynamicarray) |             |
| [`JSONLexer`](#jsonlexer)       |             |
| [`JSONToken`](#jsontoken)       |             |
| [`JSONValue`](#jsonvalue)       |             |
| [`JSONParser`](#jsonparser)     |             |

## Macros

---

### STANDARDLOOP_JSON_H_MAJOR_VERSION

```cpp
#define STANDARDLOOP_JSON_H_MAJOR_VERSION 0
```

---

### STANDARDLOOP_JSON_H_MINOR_VERSION

```cpp
#define STANDARDLOOP_JSON_H_MINOR_VERSION 0
```

---

### STANDARDLOOP_JSON_H_PATCH_VERSION

```cpp
#define STANDARDLOOP_JSON_H_PATCH_VERSION 6
```

---

### STANDARDLOOP_JSON_H_VERSION

```cpp
#define STANDARDLOOP_JSON_H_VERSION "0.0.6"
```

---

### JSON_BOOL_TRUE

```cpp
#define JSON_BOOL_TRUE "true"
```

---

### JSON_BOOL_FALSE

```cpp
#define JSON_BOOL_FALSE "false"
```

---

### JSON_NULL

```cpp
#define JSON_NULL "null"
```

---

### DEFAULT_MAP_SIZE

```cpp
#define DEFAULT_MAP_SIZE 16
```

---

### DEFAULT_MAP_RESIZE_MULTIPLE

```cpp
#define DEFAULT_MAP_RESIZE_MULTIPLE 2
```

---

### DEFAULT_DYN_ARR_SIZE

```cpp
#define DEFAULT_DYN_ARR_SIZE 16
```

---

### DEFAULT_DYN_ARR_RESIZE_MULTIPLE

```cpp
#define DEFAULT_DYN_ARR_RESIZE_MULTIPLE 2
```

---

### NULL_CHAR_STRING

```cpp
#define NULL_CHAR_STRING "\0"
```

---

### JSON_TOKEN_OPEN_CURLY_BRACE_STRING

```cpp
#define JSON_TOKEN_OPEN_CURLY_BRACE_STRING "{"
```

---

### JSON_TOKEN_CLOSE_CURLY_BRACE_STRING

```cpp
#define JSON_TOKEN_CLOSE_CURLY_BRACE_STRING "}"
```

---

### JSON_TOKEN_OPEN_BRACKET_STRING

```cpp
#define JSON_TOKEN_OPEN_BRACKET_STRING "["
```

---

### JSON_TOKEN_CLOSE_BRACKET_STRING

```cpp
#define JSON_TOKEN_CLOSE_BRACKET_STRING "]"
```

---

### JSON_TOKEN_COLON_STRING

```cpp
#define JSON_TOKEN_COLON_STRING ":"
```

---

### JSON_TOKEN_COMMA_STRING

```cpp
#define JSON_TOKEN_COMMA_STRING ","
```

## Enumerations

---

### JSONValueType

```cpp
enum JSONValueType
```

| Value                 | Description |
| --------------------- | ----------- |
| `JSONOBJ_t`           |             |
| `JSONNUMBER_INT_t`    |             |
| `JSONNUMBER_DOUBLE_t` |             |
| `JSONSTRING_t`        |             |
| `JSONBOOL_t`          |             |
| `JSONNULL_t`          |             |
| `JSONLIST_t`          |             |

---

### JSONTokenType

```cpp
enum JSONTokenType
```

| Value                      | Description |
| -------------------------- | ----------- |
| `JSONTokenEOF`             |             |
| `JSONTokenColon`           |             |
| `JSONTokenOpenCurlyBrace`  |             |
| `JSONTokenCloseCurlyBrace` |             |
| `JSONTokenOpenBracket`     |             |
| `JSONTokenCloseBracket`    |             |
| `JSONTokenComma`           |             |
| `JSONTokenString`          |             |
| `JSONTokenNumber`          |             |
| `JSONTokenBool`            |             |
| `JSONTokenNULL`            |             |
| `JSONTokenIllegal`         |             |

## Typedefs

---

### HashFunction

```cpp
using HashFunction = u_int32_t
```

## Functions

---

### JSONValueToString

```cpp
char * JSONValueToString(JSONValue *)
```

---

### JSONInit

```cpp
JSON * JSONInit()
```

---

### StringToJSON

```cpp
JSON * StringToJSON(char *)
```

---

### JSONFromFile

```cpp
JSON * JSONFromFile(char *)
```

---

### JSONToString

```cpp
char * JSONToString(JSON *, bool)
```

---

### FreeJSON

```cpp
void FreeJSON(JSON *)
```

---

### PrintJSON

```cpp
void PrintJSON(JSON *)
```

---

### TestJSON

```cpp
void TestJSON()
```

---

### PrintJSONValue

```cpp
void PrintJSONValue(JSONValue *)
```

---

### HashMapGet

```cpp
JSONValue * HashMapGet(HashMap *, char *)
```

---

### HashMapGetValueDirect

```cpp
void * HashMapGetValueDirect(HashMap *, char *)
```

---

### HashMapInit

```cpp
HashMap * HashMapInit(u_int32_t, HashFunction *, bool)
```

---

### DefaultHashMapInit

```cpp
HashMap * DefaultHashMapInit(void)
```

---

### HashMapReplicate

```cpp
HashMap * HashMapReplicate(HashMap *)
```

---

### FreeHashMap

```cpp
void FreeHashMap(HashMap *)
```

---

### HashMapInsert

```cpp
void HashMapInsert(HashMap *, JSONValue *)
```

---

### HashMapRemove

```cpp
void HashMapRemove(HashMap *, char *)
```

---

### PrintHashMap

```cpp
void PrintHashMap(HashMap *)
```

---

### ObjToString

```cpp
char * ObjToString(HashMap *)
```

---

### DynamicArrayInit

```cpp
DynamicArray * DynamicArrayInit(u_int32_t)
```

---

### DefaultDynamicArrayInit

```cpp
DynamicArray * DefaultDynamicArrayInit(void)
```

---

### DynamicArrayInitFromStr

```cpp
DynamicArray * DynamicArrayInitFromStr(char *)
```

---

### DynamicArrayReplicate

```cpp
DynamicArray * DynamicArrayReplicate(DynamicArray *)
```

---

### ListToString

```cpp
char * ListToString(DynamicArray *)
```

---

### DynamicArrayAddFirst

```cpp
void DynamicArrayAddFirst(DynamicArray *, JSONValue *)
```

---

### DynamicArrayAddLast

```cpp
void DynamicArrayAddLast(DynamicArray *, JSONValue *)
```

---

### DynamicArrayAdd

```cpp
void DynamicArrayAdd(DynamicArray *, JSONValue *, u_int32_t)
```

---

### DynamicArrayRemove

```cpp
void DynamicArrayRemove(DynamicArray *, u_int32_t)
```

---

### DynamicArrayRemoveFirst

```cpp
void DynamicArrayRemoveFirst(DynamicArray *)
```

---

### DynamicArrayRemoveLast

```cpp
void DynamicArrayRemoveLast(DynamicArray *)
```

---

### DynamicArrayGetAtIndex

```cpp
JSONValue * DynamicArrayGetAtIndex(DynamicArray *, u_int32_t)
```

---

### PrintDynamicArray

```cpp
void PrintDynamicArray(DynamicArray *)
```

---

### FreeDynamicArray

```cpp
void FreeDynamicArray(DynamicArray *)
```

---

### JSONLexerInit

```cpp
JSONLexer * JSONLexerInit(char *)
```

---

### FreeJSONLexer

```cpp
void FreeJSONLexer(JSONLexer *)
```

---

### JSONLex

```cpp
JSONToken * JSONLex(JSONLexer *)
```

---

### PrintJSONToken

```cpp
void PrintJSONToken(JSONToken *, bool)
```

---

### FreeJSONToken

```cpp
void FreeJSONToken(JSONToken *)
```

---

### IsJSONTokenValueType

```cpp
bool IsJSONTokenValueType(JSONToken *, bool)
```

---

### JSONLexerDebugTest

```cpp
void JSONLexerDebugTest(char *, bool)
```

---

### NewJSONToken

```cpp
JSONToken * NewJSONToken(enum JSONTokenType, u_int32_t, u_int32_t, u_int32_t, char *)
```

---

### JSONParserInit

```cpp
JSONParser * JSONParserInit(JSONLexer *)
```

---

### PrintJSONParserError

```cpp
void PrintJSONParserError(JSONParser *)
```

---

### FreeJSONParser

```cpp
void FreeJSONParser(JSONParser *)
```

---

### PrintJSONParserErrorLine

```cpp
void PrintJSONParserErrorLine(JSONParser *)
```

---

### FreeJSONValue

```cpp
void FreeJSONValue(JSONValue *, bool)
```

---

### ParseJSON

```cpp
JSON * ParseJSON(JSONParser *)
```

---

### JSONValueReplicate

```cpp
JSONValue * JSONValueReplicate(JSONValue *)
```

---

### JSONValueInit

```cpp
JSONValue * JSONValueInit(enum JSONValueType, void *, char *)
```

## JSON

```cpp
struct JSON
```

### Public Attributes

| Return        | Name            | Description |
| ------------- | --------------- | ----------- |
| `JSONValue *` | [`root`](#root) |             |

---

#### root

```cpp
JSONValue * root
```

## HashMap

```cpp
struct HashMap
```

### Public Attributes

| Return           | Name                                  | Description |
| ---------------- | ------------------------------------- | ----------- |
| `u_int32_t`      | [`size`](#size)                       |             |
| `u_int32_t`      | [`capacity`](#capacity)               |             |
| `u_int32_t`      | [`collision_count`](#collision_count) |             |
| `JSONValue **`   | [`entries`](#entries)                 |             |
| `HashFunction *` | [`hashFunction`](#hashfunction-1)     |             |
| `bool`           | [`force_lowercase`](#force_lowercase) |             |

---

#### size

```cpp
u_int32_t size
```

---

#### capacity

```cpp
u_int32_t capacity
```

---

#### collision_count

```cpp
u_int32_t collision_count
```

---

#### entries

```cpp
JSONValue ** entries
```

---

#### hashFunction

```cpp
HashFunction * hashFunction
```

---

#### force_lowercase

```cpp
bool force_lowercase
```

## DynamicArray

```cpp
struct DynamicArray
```

### Public Attributes

| Return         | Name                      | Description |
| -------------- | ------------------------- | ----------- |
| `u_int32_t`    | [`size`](#size-1)         |             |
| `u_int32_t`    | [`capacity`](#capacity-1) |             |
| `JSONValue **` | [`list`](#list)           |             |

---

#### size

```cpp
u_int32_t size
```

---

#### capacity

```cpp
u_int32_t capacity
```

---

#### list

```cpp
JSONValue ** list
```

## JSONLexer

```cpp
struct JSONLexer
```

### Public Attributes

| Return      | Name                              | Description |
| ----------- | --------------------------------- | ----------- |
| `char *`    | [`input`](#input)                 |             |
| `u_int32_t` | [`input_len`](#input_len)         |             |
| `char`      | [`current_char`](#current_char)   |             |
| `u_int32_t` | [`position`](#position)           |             |
| `u_int32_t` | [`read_position`](#read_position) |             |
| `u_int32_t` | [`line`](#line)                   |             |

---

#### input

```cpp
char * input
```

---

#### input_len

```cpp
u_int32_t input_len
```

---

#### current_char

```cpp
char current_char
```

---

#### position

```cpp
u_int32_t position
```

---

#### read_position

```cpp
u_int32_t read_position
```

---

#### line

```cpp
u_int32_t line
```

## JSONToken

```cpp
struct JSONToken
```

### Public Attributes

| Return               | Name                  | Description |
| -------------------- | --------------------- | ----------- |
| `enum JSONTokenType` | [`type`](#type)       |             |
| `u_int32_t`          | [`start`](#start)     |             |
| `u_int32_t`          | [`end`](#end)         |             |
| `u_int32_t`          | [`line`](#line-1)     |             |
| `char *`             | [`literal`](#literal) |             |

---

#### type

```cpp
enum JSONTokenType type
```

---

#### start

```cpp
u_int32_t start
```

---

#### end

```cpp
u_int32_t end
```

---

#### line

```cpp
u_int32_t line
```

---

#### literal

```cpp
char * literal
```

## JSONValue

```cpp
struct JSONValue
```

### Public Attributes

| Return               | Name                        | Description |
| -------------------- | --------------------------- | ----------- |
| `char *`             | [`key`](#key)               |             |
| `enum JSONValueType` | [`value_type`](#value_type) |             |
| `void *`             | [`value`](#value)           |             |
| `struct jsonValue *` | [`next`](#next)             |             |

---

#### key

```cpp
char * key
```

---

#### value_type

```cpp
enum JSONValueType value_type
```

---

#### value

```cpp
void * value
```

---

#### next

```cpp
struct jsonValue * next
```

## JSONParser

```cpp
struct JSONParser
```

### Public Attributes

| Return        | Name                              | Description |
| ------------- | --------------------------------- | ----------- |
| `JSONLexer *` | [`lexer`](#lexer)                 |             |
| `JSONToken *` | [`current_token`](#current_token) |             |
| `JSONToken *` | [`peek_token`](#peek_token)       |             |
| `bool`        | [`input_error`](#input_error)     |             |
| `bool`        | [`memory_error`](#memory_error)   |             |
| `char *`      | [`error_message`](#error_message) |             |
| `int64_t`     | [`list_nested`](#list_nested)     |             |
| `int64_t`     | [`obj_nested`](#obj_nested)       |             |

---

#### lexer

```cpp
JSONLexer * lexer
```

---

#### current_token

```cpp
JSONToken * current_token
```

---

#### peek_token

```cpp
JSONToken * peek_token
```

---

#### input_error

```cpp
bool input_error
```

---

#### memory_error

```cpp
bool memory_error
```

---

#### error_message

```cpp
char * error_message
```

---

#### list_nested

```cpp
int64_t list_nested
```

---

#### obj_nested

```cpp
int64_t obj_nested
```

Generated by [Moxygen](https://0state.com/moxygen)
