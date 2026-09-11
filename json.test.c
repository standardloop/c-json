#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

static void testJSONInit()
{
    JSON *test = JSONInit();
    assert(test != NULL);
    assert(test->root == NULL);
    FreeJSON(test);
}

static void testStringToJSON()
{
    // test 1
    char *test_string = "[]";
    JSON *test_json = NULL;

    test_json = StringToJSON("[]");
    assert(test_json != NULL);
    assert(test_json->root->value_type == JSONLIST_t);
    FreeJSON(test_json);

    // test 2
    test_string = "{}";
    test_json = NULL;

    test_json = StringToJSON(test_string);
    assert(test_json != NULL);
    assert(test_json->root->value_type == JSONOBJ_t);
    FreeJSON(test_json);

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
    assert(test_json->root->value_type == JSONOBJ_t);
    FreeJSON(test_json);
}

static void testJSONFromFile() {}

extern void TestJSON()
{
    testJSONInit();
    testStringToJSON();
    testJSONFromFile();
}
