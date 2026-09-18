# API Reference

## Classes

| Name                        | Description |
| --------------------------- | ----------- |
| [`JSON`](#json)             |             |
| [`JSONLexer`](#jsonlexer)   |             |
| [`JSONToken`](#jsontoken)   |             |
| [`JSONValue`](#jsonvalue)   |             |
| [`JSONParser`](#jsonparser) |             |

## Macros

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

## Functions

---

### JSONValueBlankInit

```cpp
JSONValue * JSONValueBlankInit()
```

---

### JSONValueInit

```cpp
JSONValue * JSONValueInit(enum JSONValueType value_type, void * value)
```

---

### JSONValueToString

```cpp
char * JSONValueToString(void * value)
```

---

### JSONValueFree

```cpp
void JSONValueFree(void * value)
```

---

### JSONValuePrint

```cpp
void JSONValuePrint(void * value)
```

---

### JSONValueDuplicate

```cpp
void * JSONValueDuplicate(void * value)
```

---

### JSONInit

```cpp
JSON * JSONInit()
```

---

### JSONFree

```cpp
void JSONFree(JSON * json)
```

---

### JSONPrint

```cpp
void JSONPrint(JSON * json)
```

---

### JSONGetRoot

```cpp
JSONValue * JSONGetRoot(JSON * json)
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
char * JSONToString(JSON *)
```

---

### TestJSON

```cpp
void TestJSON()
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

## Variables

---

### ItemValueJSONValueOperations

```cpp
ItemValueOperations ItemValueJSONValueOperations
```

## JSON

```cpp
struct JSON
```

### Public Attributes

| Return   | Name            | Description |
| -------- | --------------- | ----------- |
| `Item *` | [`root`](#root) |             |

---

#### root

```cpp
Item * root
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

| Return                                                               | Name                                                                                                     | Description |
| -------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- | ----------- |
| enum [`JSONValueType`](#jsonvaluetype)                               | [`value_type`](#value_type)                                                                              |             |
| `union JSONValue::@372020044153077151131055174206216317330213243071` | [`@101112273240373251367232114052174162075306057323`](#101112273240373251367232114052174162075306057323) |             |

---

#### value_type

```cpp
enum JSONValueType value_type
```

Type: enum [`JSONValueType`](#jsonvaluetype)

---

#### @101112273240373251367232114052174162075306057323

```cpp
union JSONValue::@372020044153077151131055174206216317330213243071 @101112273240373251367232114052174162075306057323
```

## [union].**unnamed0**

```cpp
union [union].__unnamed0__
```

### Public Attributes

| Return      | Name                        | Description |
| ----------- | --------------------------- | ----------- |
| `List *`    | [`list`](#list)             |             |
| `HashMap *` | [`obj`](#obj)               |             |
| `int64_t *` | [`num_int`](#num_int)       |             |
| `double *`  | [`num_double`](#num_double) |             |
| `char *`    | [`str`](#str)               |             |
| `bool *`    | [`boolean`](#boolean)       |             |

---

#### list

```cpp
List * list
```

---

#### obj

```cpp
HashMap * obj
```

---

#### num_int

```cpp
int64_t * num_int
```

---

#### num_double

```cpp
double * num_double
```

---

#### str

```cpp
char * str
```

---

#### boolean

```cpp
bool * boolean
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

## [union].**unnamed0**

```cpp
union [union].__unnamed0__
```

### Public Attributes

| Return      | Name                        | Description |
| ----------- | --------------------------- | ----------- |
| `List *`    | [`list`](#list)             |             |
| `HashMap *` | [`obj`](#obj)               |             |
| `int64_t *` | [`num_int`](#num_int)       |             |
| `double *`  | [`num_double`](#num_double) |             |
| `char *`    | [`str`](#str)               |             |
| `bool *`    | [`boolean`](#boolean)       |             |

---

#### list

```cpp
List * list
```

---

#### obj

```cpp
HashMap * obj
```

---

#### num_int

```cpp
int64_t * num_int
```

---

#### num_double

```cpp
double * num_double
```

---

#### str

```cpp
char * str
```

---

#### boolean

```cpp
bool * boolean
```
