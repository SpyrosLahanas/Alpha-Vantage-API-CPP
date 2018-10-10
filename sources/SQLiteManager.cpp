#include "SQLiteManager.hpp"

SQLiteManager::SQLiteManager(std::string dbFilePath)
{
    boost::filesystem::path dbPath(dbFilePath);
    //Assess the validity of path and take appropriate actions
    if(boost::filesystem::exists(dbPath))//Check if the database already exists
    {
        //Since the file exists open db and validate data schema
        if(sqlite3_open(dbFilePath.c_str(), &db) != SQLITE_OK) throw
            SQLiteError();
        //If the data schema is wrong throw exception
        if(!validate_schema()) throw SQLiteSchemaError();
    }
    else
    {
        //Check if the parent folder exists
        if(boost::filesystem::is_directory(dbPath.parent_path()))
        {
            //Create a new database
            if(sqlite3_open(dbFilePath.c_str(), &db) != SQLITE_OK) throw 
                SQLiteError();
            sqlite3_stmt *create_commands;
            if(sqlite3_prepare_v2(db,
                "CREATE TABLE tickers(ID INTEGER PRIMARY KEY "
                "AUTOINCREMENT, ticker TEXT);", -1, &create_commands, NULL)
                    != SQLITE_OK) throw SQLiteError();
            if(sqlite3_step(create_commands) != SQLITE_DONE) throw
                SQLiteError();
            sqlite3_finalize(create_commands);
            if(sqlite3_prepare_v2(db,
                "CREATE TABLE dailyData(ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                " tickerID INTEGER REFERENCES tickers(ID), date TEXT, "
                "open REAL, high REAL, low REAL, close REAL, "
                "AdjClose REAL, volume REAL, coeff REAL, divident REAL);", -1,
                &create_commands, NULL) != SQLITE_OK) throw SQLiteError();
            if(sqlite3_step(create_commands) != SQLITE_DONE) throw
                SQLiteError();
            sqlite3_finalize(create_commands);
        } else {
            //Since the parent direcotry doesn't exist throw invalid path
            //exception
            throw SQLiteInvalidPath();
        }
    } 
}

bool SQLiteManager::validate_schema()
{
    sqlite3_stmt* stm;
    if(sqlite3_prepare_v2(db, "SELECT sql FROM sqlite_master WHERE "
                "type = 'table' AND tbl_name = ?1", -1, &stm, NULL)
            != SQLITE_OK) throw SQLiteError();
    if(sqlite3_bind_text(stm, 1, "tickers", -1, SQLITE_TRANSIENT) != SQLITE_OK)
        throw SQLiteError();
    if(sqlite3_step(stm) != SQLITE_ROW) return false;
    if(std::strcmp(reinterpret_cast<const char*>(sqlite3_column_text(stm, 0)),
                "CREATE TABLE tickers(ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                "ticker TEXT)") != 0) return false;
    sqlite3_reset(stm);
    if(sqlite3_bind_text(stm, 1, "dailyData", -1, SQLITE_TRANSIENT) !=
            SQLITE_OK) throw SQLiteError();
    if(sqlite3_step(stm) != SQLITE_ROW) return false;
    if(std::strcmp(reinterpret_cast<const char*>(sqlite3_column_text(stm,0)),
                "CREATE TABLE dailyData(ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                "tickerID INTEGER REFERENCES tickers(ID), date TEXT, "
                "open REAL, high REAL, low REAL, close REAL, AdjClose REAL, "
                "volume REAL, coeff REAL, divident REAL)") != 0) return false;
    return true;
}

void SQLiteManager::store_PriceHistory(const PriceHistory& data)
{
    sqlite3_stmt* stmt;
    int ticker_ID;
    if(sqlite3_prepare_v2(db, "SELECT ID FROM tickers WHERE ticker = ?1", -1,
                &stmt, NULL) != SQLITE_OK) throw SQLiteError();
    if(sqlite3_bind_text(stmt, 1, data.get_ticker().c_str(), -1,
                SQLITE_TRANSIENT) != SQLITE_OK) throw SQLiteError();
    int rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW)
    {
        ticker_ID = sqlite3_column_int(stmt, 1);
        sqlite3_finalize(stmt);
    }
    else
    {
        if(sqlite3_finalize(stmt) != SQLITE_OK) throw SQLiteError();
        rc = sqlite3_prepare_v2(db, "INSERT INTO tickers(ticker) VALUES(?1)",
                -1, &stmt, NULL);
        if(rc != SQLITE_OK) throw SQLiteError();
        if(sqlite3_bind_text(stmt, 1, data.get_ticker().c_str(), -1,
                    SQLITE_TRANSIENT) != SQLITE_OK) throw SQLiteError();
        if(sqlite3_step(stmt) != SQLITE_DONE) throw SQLiteError();
        ticker_ID = sqlite3_last_insert_rowid(db);
        if(sqlite3_finalize(stmt) != SQLITE_OK) throw SQLiteError();
    }
    if(sqlite3_prepare_v2(db, "INSERT INTO dailyData(tickerID , date, "
                "open, high, low, close, AdjClose, "
                "volume, coeff, divident) VALUEs(?1, ?2, ?3, ?4, ?5, ?6, ?7, "
                "?8, ?9, ?10)", -1, &stmt, NULL) != SQLITE_OK) throw
        SQLiteError();
    for(std::unordered_map<boost::gregorian::date, DayInfo, 
            DateHash>::const_iterator biter =
             data.cbegin(); biter != data.cend(); biter++)
    {
       sqlite3_bind_int(stmt, 1, ticker_ID);
       sqlite3_bind_text(stmt, 2,
               boost::gregorian::to_simple_string(biter->first).c_str(),
               -1, NULL);
       sqlite3_bind_double(stmt, 3, biter->second.open);
       sqlite3_bind_double(stmt, 4, biter->second.high);
       sqlite3_bind_double(stmt, 5, biter->second.low);
       sqlite3_bind_double(stmt, 6, biter->second.close);
       sqlite3_bind_double(stmt, 7, biter->second.adjclose);
       sqlite3_bind_double(stmt, 8, biter->second.volume);
       sqlite3_bind_double(stmt, 9, biter->second.split_coefficient);
       sqlite3_bind_double(stmt, 10, biter->second.divident);
       sqlite3_step(stmt);
       sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
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

const char* SQLiteError::what()
{
    return "SQLite Error";
}

