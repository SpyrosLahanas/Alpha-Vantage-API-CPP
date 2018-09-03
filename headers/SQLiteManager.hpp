#ifndef __H_SQLiteManager
#define __H_SQLiteManager
#include <sqlite3.h>
#include <string>
#include <boost/filesystem.hpp>
#include <exception>

class SQLiteManager
{
    sqlite3 *db;
public:
    SQLiteManager() = delete;
    SQLiteManager(std::string dbFilePath);
    ~SQLiteManager();
    bool validate_schema();
};

class SQLiteSchemaError: public std::exception
{
    virtual const char* what();
};

class SQLiteInvalidPath: public std::exception
{
    virtual const char* what();
};
#endif