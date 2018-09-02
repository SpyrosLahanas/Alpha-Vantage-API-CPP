#ifndef __JSON_PARSER
#define __JSON_PARSER
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <cctype>
#include <regex>

class JsonPair;

class JsonObject
{
    //Should hold an array of pairs    
    std::vector<std::unique_ptr<JsonPair>> pair_list;
public:
    JsonObject();
    JsonObject(JsonObject& otherObj);
    JsonObject& operator=(const JsonObject& otherObj) = delete;
    void push_back(JsonPair* name_value);
    JsonPair* front();
    JsonPair* back();
    std::vector<std::unique_ptr<JsonPair>>::iterator begin();
    std::vector<std::unique_ptr<JsonPair>>::iterator end();
    bool empty();
    int size();
    void pop_back();
    void console_print();
};

class JsonParserInvalidReturnType: public std::exception
{
    const char *what();
};

class JsonParserInvalidSyntax : public std::exception
{
    const char *what();
};

class JsonPair
{
public:
    virtual void console_print() = 0;    
    //Get/set name fuction should always return string
    virtual std::string get_name()  = 0;    
    virtual void set_name(std::string InName) = 0;
    /*DESIGN: The base class (i.e., JsonPair, the base class,
      provided implementation of all getter functions. These function throws
      errors. Since this is an abstract class and all function calls are
      redirected to child classes, these implementations are utilised only when
      the child class doesn't provide its own. If the objects in the pair are
      not of the appropriate type requested the user gets an error.
    */
    virtual void get_value(std::string& return_bf);
    virtual void get_value(int& return_bf);
    virtual void get_value(double& return_bf);
    virtual void get_value(JsonObject& return_bf);
    virtual void get_value(bool& return_bf);
};

template<class Value>
class CustomPair : public JsonPair
{
    std::string name;
    Value  value;
public:
    CustomPair(const std::string& InName, Value& InValue) : name(InName),
                value(InValue) {}

    CustomPair() : name(), value() {}
 
    //Getter and setter for the name member
    std::string get_name() override 
    {
        return name; 
    }

    void set_name(std::string InName) override
    {
        name = InName;
    }

    //Getter and setter for the value member
    void get_value(Value& return_bf) override
    {
        return_bf = value;
    }

    void setValue(Value InValue)
    {
        value = InValue;
    }
    
private:
    void print()
    {
        std::cout << name << " : " << value << std::endl;
    }

public:
    void console_print() override
    {
        print();
    }
};

//Template explicit specialization of constructor for JsonObject
template<>
inline CustomPair<JsonObject>::CustomPair(const std::string& InName,
        JsonObject& InValue) : name(InName), value()
{
        for(std::vector<std::unique_ptr<JsonPair>>::iterator it =
                InValue.begin(); it != InValue.end(); it++)
            value.push_back(it->get());
}

//Template explisit specialization of get_value function for JsonObject
template<>
inline void CustomPair<JsonObject>::get_value(JsonObject& return_bf)
{
    for(std::vector<std::unique_ptr<JsonPair>>::iterator it = value.begin();
            it != value.end(); it++)
        return_bf.push_back(it->release());
}

//Template explicit specialization of set_value function for JsonObjet
template<>
inline void CustomPair<JsonObject>::setValue(JsonObject InValue)
{
    for(std::vector<std::unique_ptr<JsonPair>>::iterator it = InValue.begin();
            it != InValue.end(); it++)
        value.push_back(it->get());
}

//Template specialization of print function for JsonObject
template<>
inline void CustomPair<JsonObject>::print()
{
    std::cout << name << std::endl;
    value.console_print();
}

template<>
inline void CustomPair<bool>::print()
{
    std::cout << name << " : " << std::boolalpha <<
        value << std::noboolalpha << std::endl;
}

template<>
inline void CustomPair<double>::print()
{
    std::cout.precision(2);
    std::cout << name << std::fixed << " : "
        << value << std::defaultfloat << std::endl;
    std::cout.precision(4);
}

JsonObject* read_from_str(std::string& JsonString);
#endif
