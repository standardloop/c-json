# API Reference

## Classes

| Name                            | Description |
| ------------------------------- | ----------- |
| [`JSON`](#json)                 |             |
| [`DynamicArray`](#dynamicarray) |             |
| [`JSONLexer`](#jsonlexer)       |             |
| [`JSONToken`](#jsontoken)       |             |
| [`JSONValue`](#jsonvalue)       |             |
| [`JSONParser`](#jsonparser)     |             |
| [`JSONHashMap`](#jsonhashmap)   |             |

## Macros

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

The [JSONValueType](#jsonvaluetype) enum that stores the types for JSON values.

| Value                 | Description              |
| --------------------- | ------------------------ |
| `JSONOBJ_t`           | Object. ex `{}`.         |
| `JSONNUMBER_INT_t`    | Integer Number. ex `1`.  |
| `JSONNUMBER_DOUBLE_t` | Double Number. ex `1.1`. |
| `JSONSTRING_t`        | String. ex `"hello"`.    |
| `JSONBOOL_t`          | Boolean. ex `true`.      |
| `JSONNULL_t`          | Null. ex `null`.         |
| `JSONLIST_t`          | List. ex `[]`.           |

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

### JSONHashMapGet

```cpp
JSONValue * JSONHashMapGet(JSONHashMap *, char *)
```

---

### JSONHashMapGetValueDirect

```cpp
void * JSONHashMapGetValueDirect(JSONHashMap *, char *)
```

---

### JSONHashMapInit

```cpp
JSONHashMap * JSONHashMapInit(u_int32_t, HashFunction *, bool)
```

---

### DefaultJSONHashMapInit

```cpp
JSONHashMap * DefaultJSONHashMapInit(void)
```

---

### JSONHashMapReplicate

```cpp
JSONHashMap * JSONHashMapReplicate(JSONHashMap *)
```

---

### FreeJSONHashMap

```cpp
void FreeJSONHashMap(JSONHashMap *)
```

---

### JSONHashMapInsert

```cpp
void JSONHashMapInsert(JSONHashMap *, JSONValue *)
```

---

### JSONHashMapRemove

```cpp
void JSONHashMapRemove(JSONHashMap *, char *)
```

---

### PrintJSONHashMap

```cpp
void PrintJSONHashMap(JSONHashMap *)
```

---

### ObjToString

```cpp
char * ObjToString(JSONHashMap *)
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

### DynamicArrayToString

```cpp
char * DynamicArrayToString(DynamicArray *)
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

## DynamicArray

```cpp
struct DynamicArray
```

### Public Attributes

| Return         | Name                    | Description |
| -------------- | ----------------------- | ----------- |
| `u_int32_t`    | [`size`](#size)         |             |
| `u_int32_t`    | [`capacity`](#capacity) |             |
| `JSONValue **` | [`list`](#list)         |             |

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

| Return                                 | Name                        | Description |
| -------------------------------------- | --------------------------- | ----------- |
| `char *`                               | [`key`](#key)               |             |
| enum [`JSONValueType`](#jsonvaluetype) | [`value_type`](#value_type) |             |
| `void *`                               | [`value`](#value)           |             |
| `struct jsonValue *`                   | [`next`](#next)             |             |

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

Type: enum [`JSONValueType`](#jsonvaluetype)

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

## JSONHashMap

```cpp
struct JSONHashMap
```

### Public Attributes

| Return           | Name                                  | Description |
| ---------------- | ------------------------------------- | ----------- |
| `u_int32_t`      | [`size`](#size-1)                     |             |
| `u_int32_t`      | [`capacity`](#capacity-1)             |             |
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
