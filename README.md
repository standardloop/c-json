# c-json

[https://github.com/standardloop/c-json](https://github.com/standardloop/c-json)

[![GitHub Release](https://img.shields.io/github/v/release/standardloop/c-json?sort=semver)](https://github.com/standardloop/c-json/releases) ![Platform: macOS](https://img.shields.io/badge/platform-macOS-000000?style=flat&logo=apple&logoColor=white) ![C Version](https://img.shields.io/badge/C_Standard-C17-00599C?logo=c&logoColor=white)

## Background

c-json is a json library written in C.

This was built to learn about lexing, parsing, and data structures such as dynamic arrays and hashmaps.

## Dependencies

- https://github.com/standardloop/c-util
- https://github.com/standardloop/c-collections

## Example Programs

### `StringToJSON`

```C
#include <stdio.h>
#include <stdlib.h>
#include <standardloop/json.h>

int main(void)
{
    JSON *json = StringToJSON("[1, 2, {\"key\": \"value\"}, \"hello\"]");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    JSONPrint(json);
    JSONFree(json);
    return EXIT_SUCCESS;
}
```

### `JSONFromFile`

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <standardloop/json.h>

int main(void)
{
    JSON *json = JSONFromFile("example.json");

    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    JSONPrint(json);
    JSONFree(json);
    return EXIT_SUCCESS;
}
```

### `JSONToString`

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <standardloop/json.h>

int main(void)
{
    JSON *json = StringToJSON("[1, 2, {\"key\": \"value\"}, \"hello\"]");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    char *json_string = JSONToString(json);
    if (json_string == NULL)
    {
        return EXIT_FAILURE;
    }
    printf("%s\n", json_string);
    free(json_string);
    return EXIT_SUCCESS;
}
```

## Building

### To see all avaiable

```sh
$ task --list-all
```

### To build a test program

```sh
$ task test
```

## Checking for Leaks

- The `taskfile` has a task to compile the code with address sanitizers.
- See `LEAKS.md`

## Docs

Docs are generated a first generated with `doxygen` as `xml` and then `moxygen` reads the xml to output markdown in the `docs/` folder.

This repo uses reusable tasks from https://github.com/standardloop/taskfiles/

## Precommit

This repo uses [prek](https://github.com/j178/prek).

Please run `prek install`.
