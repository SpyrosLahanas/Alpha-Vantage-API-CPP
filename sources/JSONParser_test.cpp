#define BOOST_TEST_MODULE JSONParser Tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "JSONParser.hpp"

BOOST_AUTO_TEST_CASE(TestCustomPairTemplate)
{
    /*CHECK that the custom pair template gets instantiated appropriately given
    those parameters whick will be used for the JSON parser*/

    CustomPair<std::string> newpair1;
    CustomPair<double> newpair2;
    CustomPair<JsonObject> newpair3;
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(JsonPairInteface)
{
    //CHECK that the interface works with and str, str pair
    std::string name("Random_Name");
    std::string value("Random_Value");
    CustomPair<std::string> custom_pair(name, value);
    JsonPair& abstract_class = custom_pair;
    std::cout << "Testing the print function" << std::endl;
    BOOST_CHECK_NO_THROW(abstract_class.console_print());

    //CHECK that abstract class returns the right name
    BOOST_CHECK_EQUAL(abstract_class.get_name(), name);

    //CHECK that abstract class returns the right value
    std::string return_bf;
    abstract_class.get_value(return_bf);
    BOOST_CHECK_EQUAL(return_bf, value);

    //CHECK that the inteface works with a str, int pair
    int value1 = 5;
    CustomPair<int> custom_pair2(name, value1);
    JsonPair& abstract_class2 = custom_pair2;

    //CHECK that the iterface return the right name
    BOOST_CHECK_EQUAL(abstract_class2.get_name(), name);

    //CHECK that the inteface returns the right value
    int return_bf2;
    abstract_class2.get_value(return_bf2);
    BOOST_CHECK_EQUAL(return_bf2, 5);

    //CHECK that the inteface works with a str, bool pair
    bool value2 = true;
    CustomPair<bool> custom_pair3(name, value2);
    JsonPair& abstract_class3 = custom_pair3;

    //CHECK that the inteface returns tha right name
    BOOST_CHECK_EQUAL(abstract_class3.get_name(), name);
    
    //CHECK that the inteface returns the right value
    bool return_bf3;
    abstract_class3.get_value(return_bf3);
    BOOST_CHECK_EQUAL(return_bf3, true);

    //CHECK that the inteface works with a str, double pair
    double value3 = 20.2;
    CustomPair<double> custom_pair4(name, value3);
    JsonPair& abstract_class4 = custom_pair4;

    //CHECK that the interface returns the right name
    BOOST_CHECK_EQUAL(abstract_class4.get_name(), name);

    //CHECK that the interface returns the right value
    double return_bf4;
    abstract_class4.get_value(return_bf4);
    BOOST_CHECK_EQUAL(return_bf4, 20.2);

    //CHECK that the interface works with a str, JsonObject pair
    JsonObject value4 = JsonObject();
    value4.push_back(&abstract_class);
    value4.push_back(&abstract_class2);
    value4.push_back(&abstract_class3);
    value4.push_back(&abstract_class4);
    CustomPair<JsonObject> custom_pair5(name, value4);
    JsonPair& abstract_class5 = custom_pair5;

    //CHECK that the interface returns the right name
    BOOST_CHECK_EQUAL(abstract_class5.get_name(), name);
    //CHECK that the interface returns a Json Object
    JsonObject return_bf5 = JsonObject();
    BOOST_CHECK_NO_THROW(abstract_class5.get_value(return_bf5));
    for(std::vector<std::unique_ptr<JsonPair>>::iterator it = value4.begin();
            it != value4.end(); it++) it->release();
    for(std::vector<std::unique_ptr<JsonPair>>::iterator it = return_bf5.begin();
            it != return_bf5.end(); it++) it->release(); 
}

BOOST_AUTO_TEST_CASE(TestJsonObject)
{
    //CHECK that JsonObject can hold pairs
    //CHECK that JsonObject can iterate through the pairs it holds

    //Instatiate variable for pair construction
    std::string name("Property Name");
    std::string str_value("Description");
    int int_value = 5;
    double double_value = 10.0;
    bool bool_value = false;

    //Costurct pairs
    CustomPair<std::string>* custompair1 = new CustomPair<std::string>(name,
            str_value);
    CustomPair<int>* custompair2 = new CustomPair<int>(name, int_value);
    CustomPair<double>* custompair3 = new CustomPair<double>(name,
            double_value);
    CustomPair<bool>* custompair4 = new CustomPair<bool>(name, bool_value);

    JsonObject obj;

    obj.push_back(custompair1);
    obj.push_back(custompair2);
    obj.push_back(custompair3);
    obj.push_back(custompair4);
    
    CustomPair<JsonObject>* custompair5 = new CustomPair<JsonObject>(name,
            obj);

    JsonObject obj2;

    obj2.push_back(custompair5);
}

BOOST_AUTO_TEST_CASE(TestJsonParser)
{
    /*CHECK that the JsonParser returns a JsonObject when provided with a valid
    JSON object in string format*/

    //CHECK that the parser works with the simple key/value pair of strings
    std::string JsonString("{\"RandomName\" : \"RandomValue\"}");
    JsonObject* NewJson = read_from_str(JsonString);
    std::cout << "Print a simple JSON: one pair of strings" << std::endl;
    BOOST_CHECK_NO_THROW(NewJson->console_print());
    delete NewJson;
    JsonString.clear();

    //CHECK that the parser works with multiple key/value pairs of strings
    JsonString.append("{\"Key#1\" : \"Value#1\", "
            "\"Key#2\" : \"Value#2\"}");
    NewJson = read_from_str(JsonString);
    std::cout << "Printing a JSON with mutliptle pairs" << std::endl;
    BOOST_CHECK_NO_THROW(NewJson->console_print());
    delete NewJson;
    JsonString.clear();

    //CHECK that the parser works with one nested object
    JsonString.append("{\"Root Key#1\" :" 
            "{\"Nested Key#2\" : \"Value string\"}"
            "}");
    NewJson = read_from_str(JsonString);
    std::cout << "Printing a JSON with a nested JSON Objecst" << std::endl;
    BOOST_CHECK_NO_THROW(NewJson->console_print());
    delete NewJson;
    JsonString.clear();

    //CHECK that the parser works with multiple nested objects
    JsonString.append("{\"Key#1\" :"
            "{\"SubKey#1\" : \"SubValue#1\","
            "\"Subkey#2\" : \"SubValue#2\","
            "\"Subkey#3\" : {\"SubSubKey#1\" : \"SubSbuValue#1\"}},"
            "\"Key#2\" : \"Value#2\","
            "\"Key#3\" : \"Value#3\","
            "\"Key#4\" : {\"SubKey#3\" : \"SubValue#3\", "
            " \"Subkey#4\" : \"Subvalue#4\", "
            "\"Subkey#5\" : \"Subvalue#5\"}}");
    NewJson = read_from_str(JsonString);
    std::cout << "Printing a JSON with multiple pairs and nested objects"
                 " in multiple levels" << std::endl;
    BOOST_CHECK_NO_THROW(NewJson->console_print());
    delete NewJson;
    JsonString.clear();

    //CHECK that the parser works recongises a number and a boolean
    JsonString.append("{\"Key\" : 12345.6, "
                      "\"Key#1\" : true}");
    NewJson = read_from_str(JsonString);
    std::cout << "Printing a JSON with a pair of a string and a double"
        << std::endl;
    BOOST_CHECK_NO_THROW(NewJson->console_print());
    delete NewJson;
    JsonString.clear();
}
