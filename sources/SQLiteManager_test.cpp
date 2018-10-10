#define BOOST_TEST_MODULE SQLiteManager Tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "SQLiteManager.hpp"
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <string.h>
#include <iostream>
#include "Instrument.hpp"

BOOST_AUTO_TEST_CASE(constructor_test)
{
    //Given a valid path constructor creates a new .db file
    SQLiteManager dbmanager("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    boost::filesystem::path dbPath(("/home/spyroslahanas/Documents/"
                "Programming/PortfolioAnalyst/Other/new.db"));
    BOOST_CHECK(boost::filesystem::exists(dbPath));    
    boost::filesystem::remove("/home/spyroslahanas/Documents/Programming"
            "/PortfolioAnalyst/Other/new.db");
}

BOOST_AUTO_TEST_CASE(catch_invalid_path)
{
    //Given an invalid path at initialisation the constructor raises error.
    BOOST_CHECK_THROW(SQLiteManager dbmanager("random/Path/and/wrong.db"),
            SQLiteInvalidPath);
}

BOOST_AUTO_TEST_CASE(Test_Create_New_DB)
{
    //Check if the dummy database exists
    boost::filesystem::path dummydb("/home/spyroslahanas/Documents/"
            "Programming/PortfolioAnalyst/Other/new.db");
    if(boost::filesystem::exists(dummydb))
    {
        //If it does delete it
        boost::filesystem::remove(dummydb);
    }
    //Create a new database by instatiating an SQLiteManager
    SQLiteManager dbmanager("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    //Retrieve the database schema
    sqlite3 *db;
    int status = sqlite3_open("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db", &db);
    BOOST_REQUIRE(status == SQLITE_OK);
    sqlite3_stmt *sqlstm;
    status = sqlite3_prepare_v2(db, "SELECT sql FROM sqlite_master "
            "WHERE type = 'table' AND tbl_name = ?1;", -1, &sqlstm, NULL);
    BOOST_REQUIRE(status == SQLITE_OK);
    BOOST_REQUIRE(sqlstm != NULL);
    status = sqlite3_bind_text(sqlstm, 1, "tickers", -1, SQLITE_TRANSIENT);
    status = sqlite3_step(sqlstm);
    BOOST_REQUIRE(status == SQLITE_ROW);
    BOOST_REQUIRE(sqlite3_column_count(sqlstm) != 0);
    const unsigned char *statement = sqlite3_column_text(sqlstm, 0);
    //Check that CREATE TABLE commands are correct
    BOOST_CHECK(strcmp(reinterpret_cast<const char*>(statement), "CREATE "
                "TABLE tickers(ID INTEGER PRIMARY KEY AUTOINCREMENT, ticker"
                " TEXT)") == 0);
    status = sqlite3_reset(sqlstm);
    status = sqlite3_bind_text(sqlstm, 1, "dailyData", -1, SQLITE_TRANSIENT);
    status = sqlite3_step(sqlstm);
    BOOST_REQUIRE(status == SQLITE_ROW);
    BOOST_REQUIRE(sqlite3_column_count(sqlstm) != 0);
    statement = sqlite3_column_text(sqlstm, 0);
    BOOST_CHECK(strcmp(reinterpret_cast<const char*>(statement), 
                "CREATE TABLE dailyData(ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                " tickerID INTEGER REFERENCES tickers(ID), date TEXT, "
                "open REAL, high REAL, low REAL, close REAL, "
                "AdjClose REAL, volume REAL, coeff REAL, divident REAL)")
            == 0);
    //deallocate sqlite3 connection variable
    sqlite3_finalize(sqlstm);
    sqlite3_close(db);
    boost::filesystem::remove(dummydb);
}

BOOST_AUTO_TEST_CASE(Test_validate_schema)
{
    //Create new connection
    SQLiteManager mngr("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    BOOST_CHECK(mngr.validate_schema());
    boost::filesystem::path dbPath("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    if(boost::filesystem::exists(dbPath))
        boost::filesystem::remove(dbPath);
}

BOOST_AUTO_TEST_CASE(Test_Store_PriceHistory)
{
    //Instatiate a PriceHistory object
    std::string ticker("IVZ");
    PriceHistory data(ticker);
    //Load some dummy data
    data.insert_dayinfo(boost::gregorian::from_simple_string("2018-09-10"),
            1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
    data.insert_dayinfo(boost::gregorian::from_simple_string("2018-09-11"),
            2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
    data.insert_dayinfo(boost::gregorian::from_simple_string("2018-09-12"),
            3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);
    data.insert_dayinfo(boost::gregorian::from_simple_string("2018-09-13"),
            4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0);
    //Instatiate SQLiteManager
    SQLiteManager mngr("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    //Store the DayInfo Object in the database
    mngr.store_PriceHistory(data);
    BOOST_CHECK(true);
    boost::filesystem::path dummydb("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    if(boost::filesystem::exists(dummydb)) boost::filesystem::remove(dummydb);
}
