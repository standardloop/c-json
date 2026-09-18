#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

static void testJSONSimple()
{
    printf("testJSONSimple\n");

    JSONValue *json_v = JSONValueInit(JSONSTRING_t, strdup("testing"));
    assert(json_v != NULL);
    assert(json_v->value_type == JSONSTRING_t);
    assert(json_v->str != NULL);
    assert(strcmp(json_v->str, "testing") == 0);

    Item *json_item = ItemInit(json_v, &ItemValueJSONValueOperations);
    assert(json_item != NULL);

    JSON *json = JSONInit();
    assert(json != NULL);
    json->root = json_item;

    JSONFree(json);
    // ---

    JSON *json_2 = JSONInit();
    assert(json_2 != NULL);
    json_2->root = ItemInit(JSONValueInit(JSONOBJ_t, HashMapInitDefault()),
                            &ItemValueJSONValueOperations);

    JSONFree(json_2);
}

static void testJSONInit()
{
    printf("testJSONInit\n");
    JSON *test = JSONInit();
    assert(test != NULL);
    assert(test->root == NULL);
    JSONFree(test);
}

static void testStringToJSON()
{
    printf("testStringToJSON\n");
    // test 1
    char *test_string = "[]";
    JSON *test_json = NULL;

    test_json = StringToJSON("[]");
    assert(test_json != NULL);
    assert(JSONGetRoot(test_json)->value_type == JSONLIST_t);
    JSONFree(test_json);

    // test 2
    test_string = "{}";
    test_json = NULL;

    test_json = StringToJSON(test_string);
    assert(test_json != NULL);
    assert(JSONGetRoot(test_json)->value_type == JSONOBJ_t);
    JSONFree(test_json);

    // test 3 (WIP)
    // test_string = "10";
    // test_json = NULL;

    // test_json = StringToJSON(test_string);
    // assert(test_json != NULL);
    // assert(test_json->root->value_type == JSONNUMBER_INT_t);
    // FreeJSON(test_json);

    // test 4
    test_string = "{\"foo\": \"bar\", \"bar\": \"foo\"}";
    test_json = NULL;

    test_json = StringToJSON(test_string);
    assert(test_json != NULL);
    assert(JSONGetRoot(test_json)->value_type == JSONOBJ_t);

    JSONFree(test_json);
}

static void testJSONFromFile()
{
    printf("testJSONFromFile\n");

    JSON *json_simple_array =
        JSONFromFile("./testfiles/simple/simple-array.json");
    assert(json_simple_array != NULL);
    assert(JSONGetRoot(json_simple_array)->value_type == JSONLIST_t);

    JSONFree(json_simple_array);
}

extern void TestJSON()
{
    testJSONSimple();
    testJSONInit();
    testStringToJSON();
    testJSONFromFile();
}
