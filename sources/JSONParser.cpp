#include "JSONParser.hpp"

JsonPair* construct_pair(bool& readingObj,
        std::vector<std::string>& strStack, std::string& charStack)
{
    if(readingObj != true)
                throw JsonParserInvalidSyntax();
    JsonPair* jsonProperty = nullptr;
    if(!strStack.empty())
    {
        if(!charStack.empty())
        {
            strStack.push_back(charStack);
            charStack.clear();
        }
        if (strStack.back() == "true" || strStack.back() == "false")
        {
            bool input = (strStack.back() == "true" ? true : false);
            jsonProperty = new CustomPair<bool>(strStack.front(), input);
            strStack.clear();
        }
        else if(std::regex_match(strStack.back(),
                    std::regex("^[+-]?\\d+(\\.\\d+)?")))
        {
            double input = std::stod(strStack.back());
            jsonProperty = new CustomPair<double>(strStack.front(), input);
            strStack.clear();
        }
        else
        {
            if(strStack.size() != 2)
                throw JsonParserInvalidSyntax();
            jsonProperty = new CustomPair<std::string>(strStack.front(),
                     strStack.back());
            strStack.clear();
        }
    }
        return jsonProperty;
}

JsonObject* str_parser(std::string::iterator& iter_begin,
        std::string::iterator& iter_end)
{
    /*
    Track the context/environment to which each character belongs
    The possible alternatives are:
    --In an object or not,
    --if in an object either part of key or value
    */
    bool readingObj = false;
    bool readingKey = false; //when false we are reading value
    bool readingStr = false;

    JsonObject* obj;

    std::vector<std::string> strStack;
    std::string charStack;

    for(iter_begin; iter_begin != iter_end; iter_begin++)
    {
        //Change the context variables on the relevant characters.
        if ((*iter_begin) == '{')
        {
            if(readingObj && readingStr == false)
            {
                CustomPair<JsonObject>* jsonProperty = 
                    new CustomPair<JsonObject>(strStack.front(),
                            *str_parser(iter_begin, iter_end));
                obj->push_back(jsonProperty);
                strStack.clear();
            }
            else
            {
                obj = new JsonObject();
                readingObj = true;
                readingKey = true;
            }
            continue;
        }
        else if ((*iter_begin) == '}' && readingStr == false)
        {
            JsonPair* new_pair = construct_pair(readingObj, strStack,
                    charStack);
            if(new_pair != nullptr)
                obj->push_back(new_pair);
            return obj;
        }
        else if ((*iter_begin) == ':' && readingStr == false)
        {
            readingKey = !readingKey;
            continue;
        }
        else if ((*iter_begin) == ',' && readingStr == false)
        {
            JsonPair* new_pair = construct_pair(readingObj, strStack,
                    charStack);
            if(new_pair != nullptr)
                obj->push_back(new_pair);
            readingKey = !readingKey;
            continue;
        }
        else if ((*iter_begin) == '\"')
        {
            if (readingStr == true)
            {
                strStack.push_back(charStack);
                charStack.clear();
            }
            readingStr = !readingStr;
            continue;
        }

        if (readingObj == true)
        {
            if(readingStr == true)
            {
                charStack.append(&(*iter_begin), 1);
            }
            else if (readingKey == false && (std::isalnum(*iter_begin) ||
                        *iter_begin == '+' || *iter_begin == '-' ||
                        *iter_begin == '.'))
            {
                charStack.append(&(*iter_begin),1);
            }
            continue;
        }
    }
 
    throw JsonParserInvalidSyntax();
}


JsonObject* read_from_str(std::string& JsonString)
{
    std::string::iterator iter_begin = JsonString.begin();
    std::string::iterator iter_end = JsonString.end();
    return str_parser(iter_begin, iter_end);
}

const char* JsonParserInvalidReturnType::what()
{
   return "Json Parser error: The value in the Json pair is not of the"
       " requested type";
}

const char *JsonParserInvalidSyntax::what()
{
    return "Json Parser error: The string is not a syntactically correct JSON";
}

void JsonPair::get_value(std::string& return_bf)
{
    throw JsonParserInvalidReturnType();
}

void JsonPair::get_value(int& return_bf)
{
    throw JsonParserInvalidReturnType();
}

void JsonPair::get_value(double& return_bf)
{
    throw JsonParserInvalidReturnType();
}

void JsonPair::get_value(JsonObject& return_bf)
{
    throw JsonParserInvalidReturnType();
}

void JsonPair::get_value(bool& return_bf)
{
    throw JsonParserInvalidReturnType();
}

JsonObject::JsonObject() : pair_list() {}

JsonObject::JsonObject(JsonObject& otherObj): pair_list()
{
    for(std::vector<std::unique_ptr<JsonPair>>::iterator iter =
            otherObj.begin(); iter != otherObj.end(); iter++)
    {
        pair_list.push_back(std::unique_ptr<JsonPair>(iter->release()));
    }
}

void JsonObject::push_back(JsonPair* name_value)
{
    pair_list.push_back(std::unique_ptr<JsonPair>(name_value)); 
}

JsonPair* JsonObject::front()
{
    return pair_list.front().get();
}
JsonPair* JsonObject::back()
{
    return pair_list.back().get();
}
std::vector<std::unique_ptr<JsonPair>>::iterator JsonObject::begin()
{
    return pair_list.begin();
}
std::vector<std::unique_ptr<JsonPair>>::iterator JsonObject::end()
{
    return pair_list.end();
}
bool JsonObject::empty()
{
    return pair_list.empty();
}
int JsonObject::size()
{
    return pair_list.size();
}
void JsonObject::pop_back()
{
    pair_list.pop_back();
}

void JsonObject::console_print()
{
    for(std::vector<std::unique_ptr<JsonPair>>::iterator it =
            pair_list.begin(); it != pair_list.end(); it++)
            (*it)->console_print();
}
