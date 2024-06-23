#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

int main(void)
{
    // char *example_json_string = "[\"josh\", \"test\", [1, 2, 3, 4]]";
    // char *example_json_string = "[[1, 2, 3], \"hi\"]";
    // char *example_json_string = "[{\"key\": \"value\", \"test\": true, {\"data\": null}, \"numbers\": [-3.123]}]";
    // char *example_json_string = "[null, [1, 2, 3, 5], [\"hello\", \"hello\", \"hello\", \"hello\", \"hello\"], null, null, -4.1, 493213023, false]";
    // char *example_json_string = "[{\"key\": {\"data\": [1,2,3]}}]";

    // char *example_json_string = "{\"example\": {\"example\": [1, 2, 3, 5], \"test\": true}, \"test\": true}";
    char *example_json_string = "{}";
    JSON *json = StringToJSON(example_json_string);
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    // PrintJSON(json);
    FreeJSON(json);

    json = JSONFromFile("lab/1mil.json");
    PrintJSON(json);
    FreeJSON(json);
    return EXIT_SUCCESS;
}
