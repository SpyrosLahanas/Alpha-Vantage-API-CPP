#include "SQLiteManager.hpp"

SQLiteManager::SQLiteManager(std::string dbFilePath){
    boost::filesystem::path dbPath(dbFilePath);
    //Assess the validity of path and take appropriate actions
    if(boost::filesystem::exists(dbPath)) //Check if the database already exists
    {
        //Since the file exists open db and validate data schema
        int rc = sqlite3_open(dbFilePath.c_str(), &db);
        //If the data schema is wrong throw exception
        if(!validate_schema()) throw SQLiteSchemaError();
    } else {
        //Check if the parent folder exists
        if(boost::filesystem::is_directory(dbPath.parent_path()))
        {
            //Create a new database
            int rc = sqlite3_open(dbFilePath.c_str(), &db);
            sqlite3_stmt *create_commands;
            rc = sqlite3_prepare_v2(db,"CREATE TABLE Prices(ID INTEGER PRIMARY KEY AUTOINCREMENT, ticker TEXT, price REAL);", -1, &create_commands, NULL);
            rc = sqlite3_step(create_commands);
            sqlite3_finalize(create_commands);
        } else {
            //Since the parent direcotry doesn't exist throw invalid path exception
            throw SQLiteInvalidPath();
        }
    } 
}

bool SQLiteManager::validate_schema()
{
    return true;
}

SQLiteManager::~SQLiteManager()
{
    sqlite3_close(db);
}

const char* SQLiteSchemaError::what()
{
    return "SQLite DB schema error";
}

const char* SQLiteInvalidPath::what()
{
    return "SQLite Invalid file path";
}
