#define BOOST_TEST_MODULE SQLiteManager Tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "SQLiteManager.hpp"
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <string.h>
#include <iostream>

BOOST_AUTO_TEST_CASE(constructor_test)
{
    //Given a valid path constructor creates a new .db file
    SQLiteManager dbmanager("/home/spyroslahanas/Documents/Programming/PortfolioAnalyst/Other/new.db");
    boost::filesystem::path dbPath(("/home/spyroslahanas/Documents/Programming/PortfolioAnalyst/Other/new.db"));
    BOOST_CHECK(boost::filesystem::exists(dbPath));    
    boost::filesystem::remove("/home/spyroslahanas/Documents/Programming/PortfolioAnalyst/Other/new.db");
}

BOOST_AUTO_TEST_CASE(catch_invalid_path)
{
    //Given an invalid path at initialisation the constructor raises error.
    BOOST_CHECK_THROW(SQLiteManager dbmanager("random/Path/and/wrong.db"), SQLiteInvalidPath);
}

BOOST_AUTO_TEST_CASE(validate_schema)
{
    //Check if the dummy database exists
    boost::filesystem::path dummydb("/home/spyroslahanas/Documents/Programming/PortfolioAnalyst/Other/new.db");
    if(boost::filesystem::exists(dummydb))
    {
        //If it does delete it
        boost::filesystem::remove(dummydb);
    }
    //Create a new database by instatiating an SQLiteManager
    SQLiteManager dbmanager("/home/spyroslahanas/Documents/Programming/PortfolioAnalyst/Other/new.db");
    //Retrieve the database schema
    sqlite3 *db;
    int status = sqlite3_open("/home/spyroslahanas/Documents/Programming/PortfolioAnalyst/Other/new.db", &db);
    BOOST_REQUIRE(status == SQLITE_OK);
    sqlite3_stmt *sqlstm;
    status = sqlite3_prepare_v2(db, "SELECT sql FROM sqlite_master WHERE type = 'table';", -1, &sqlstm, NULL);
    BOOST_REQUIRE(status == SQLITE_OK);
    BOOST_REQUIRE(sqlstm != NULL);
    status = sqlite3_step(sqlstm);
    BOOST_REQUIRE(status == SQLITE_ROW);
    BOOST_REQUIRE(sqlite3_column_count(sqlstm) != 0);
    const unsigned char *statement = sqlite3_column_text(sqlstm, 0);
    std::cout << statement << std::endl;
    //Check that CREATE TABLE commands are correct
    BOOST_CHECK(strcmp(reinterpret_cast<const char*>(statement), "CREATE TABLE Prices(ID INTEGER PRIMARY KEY AUTOINCREMENT, ticker TEXT, price REAL)") == 0);
    //deallocate sqlite3 connection variable
    sqlite3_finalize(sqlstm);
    sqlite3_close(db);
}
