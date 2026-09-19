#include <standardloop/testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

static void testJSONSimple()
{

    JSONValue *json_v = JSONValueInit(JSONSTRING_t, strdup("testing"));
    TestCaseVerify(true, "Ensure JSONValueInit returns a non null value",
                   json_v != NULL);
    TestCaseVerify(true, "Ensure JSONValue is has a json value type of string",
                   json_v->value_type == JSONSTRING_t);
    TestCaseVerify(true, "Ensure JSONValue str is not NULL",
                   json_v->str != NULL);
    TestCaseVerify(true, "Ensure JSONValue str is equal to the testing string",
                   strcmp(json_v->str, "testing") == 0);

    Item *json_item = ItemInit(json_v, &ItemValueJSONValueOperations);
    TestCaseVerify(true, "Ensure Item *json_item is not NULL",
                   json_item != NULL);

    JSON *json = JSONInit();
    TestCaseVerify(true, "Ensure JSONInit returns a non NULL pointer",
                   json != NULL);

    json->root = json_item;

    JSONFree(json);
    // ---

    JSON *json_2 = JSONInit();
    TestCaseVerify(true, "Ensure JSONInit returns a non NULL pointer",
                   json_2 != NULL);
    json_2->root = ItemInit(JSONValueInit(JSONOBJ_t, HashMapInitDefault()),
                            &ItemValueJSONValueOperations);

    TestCaseVerify(true, "Ensure json_2->root is not NULL",
                   json_2->root != NULL);
    JSONFree(json_2);
}

static void testJSONInit()
{
    // printf("testJSONInit\n");
    JSON *test = JSONInit();
    TestCaseVerify(true, "Ensure JSONInit returns a non NULL pointer",
                   test != NULL);
    TestCaseVerify(true,
                   "Ensure JSON * test->root is NULL because it shouldn't "
                   "have be initialized",
                   test->root == NULL);
    JSONFree(test);
}

static void testStringToJSON()
{
    // printf("testStringToJSON\n");
    // test 1
    char *test_string = "[]";
    JSON *test_json = NULL;

    test_json = StringToJSON("[]");
    TestCaseVerify(true,
                   "ensure StringToJSON(\"[]\") returns a non NULL pointer",
                   test_json != NULL);
    TestCaseVerify(
        true,
        "ensure the root value of the pointer has a value type of JSONLIST_t",
        JSONGetRoot(test_json)->value_type == JSONLIST_t);
    JSONFree(test_json);

    // test 2
    test_string = "{}";
    test_json = NULL;

    test_json = StringToJSON(test_string);
    TestCaseVerify(true, "ensure StringToJSON(\"{}\") returns a non NULL ptr",
                   test_json != NULL);
    TestCaseVerify(
        true,
        "ensure the root value of the pointer has a value type of JSONOBJ_t",
        JSONGetRoot(test_json)->value_type == JSONOBJ_t);
    JSONFree(test_json);

    // -
    test_string = "test";
    test_json = NULL;

    test_json = StringToJSON(test_string);
    TestCaseVerify(true,
                   "ensure StringToJSON(\"test\") returns a non NULL ptr ",
                   test_json != NULL);
    TestCaseVerify(
        true,
        "ensure the root value of the pointer has a value type of JSONSTRING_t",
        test_json != NULL &&
            JSONGetRoot(test_json)->value_type == JSONSTRING_t);
    JSONFree(test_json);

    // -
    test_string = "{\"foo\": \"bar\", \"bar\": \"foo\"}";
    test_json = NULL;

    test_json = StringToJSON(test_string);
    TestCaseVerify(true,
                   "ensure StringToJSON(\"{\"foo\": \"bar\", \"bar\": "
                   "\"foo\"}\") returns a non NULL ptr",
                   test_json != NULL);
    TestCaseVerify(
        true,
        "ensure the root value of the pointer has a value type of JSONOBJ_t",
        JSONGetRoot(test_json)->value_type == JSONOBJ_t);
    JSONFree(test_json);
}

static void testJSONFromFile()
{
    // printf("testJSONFromFile\n");
    JSON *test = NULL;

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-array.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "", JSONGetRoot(test)->value_type == JSONLIST_t);

    JSONFree(test);
    test = NULL;

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-obj.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "", JSONGetRoot(test)->value_type == JSONOBJ_t);

    JSONFree(test);
    test = NULL;

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-bool.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "", JSONGetRoot(test)->value_type == JSONBOOL_t);

    JSONFree(test);
    test = NULL;

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-double.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "",
                   JSONGetRoot(test)->value_type == JSONNUMBER_DOUBLE_t);

    JSONFree(test);

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-int.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "", JSONGetRoot(test)->value_type == JSONNUMBER_INT_t);

    JSONFree(test);
    test = NULL;

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-int.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "", JSONGetRoot(test)->value_type == JSONNUMBER_INT_t);

    JSONFree(test);
    test = NULL;

    // ------------------------------------------------------------
    test = JSONFromFile("./testfiles/simple/simple-int.json");
    TestCaseVerify(true, "", test != NULL);
    TestCaseVerify(true, "", JSONGetRoot(test)->value_type == JSONNUMBER_INT_t);

    JSONFree(test);
    test = NULL;
}

extern void TestJSON()
{
    testJSONSimple();
    testJSONInit();
    testStringToJSON();
    testJSONFromFile();
}
