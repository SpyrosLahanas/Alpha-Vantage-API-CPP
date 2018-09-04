#ifndef _H_AVCONNECTION
#define _H_AVCONNECTION
#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <utility>
#include <unordered_map>
#include "JSONParser.hpp"

//Custom exception for AlphaVantage API errors
class AVInvalidKey: public std::exception
{
public:
    virtual const char* what();    
};

class AVQueryFailure: public std::exception
{
public:
    virtual const char* what();
};

class AVInvalidParameters: public std::exception
{
    virtual const char *what();
};

//Enum list of the eligible options accepted by AlphaVantage API
enum struct AVFunctions
{
    TIME_SERIES_INTRADAY,
    TIME_SERIES_DAILY,
    TIME_SERIES_DAILY_ADJUSTED,
    TIME_SERIES_WEEKLY,
    TIME_SERIES_WEEKLY_ADJUSTED,
    TIME_SERIES_MONTHLY,
    TIME_SERIES_MONTHLY_ADJUSTED,
    GLOBAL_QUOTE,
    CURRENCY_EXCHANGE_RATE,
    FX_INTRADAY,
    FX_DAILY,
    FX_WEEKLY,
    FX_MONTHLY,
    DIGITAL_CURRENCY_DAILY,
    DIGITAL_CURRENCY_WEEKLY,
    DIGITAL_CURRENCY_MONTHLY,
    SMA,
    EMA,
    WMA,
    DEMA,
    TEMA,
    TRIMA,
    KAMA,
    MAMA,
    T3,
    MACD,
    MACDEXT,
    STOCH,
    STOCHF,
    RSI,
    STOCHRSI,
    WILLR,
    ADX,
    ADXR,
    APO,
    PPO,
    MOM,
    BOP,
    CCI,
    CMO,
    ROC,
    ROCR,
    AROON,
    AROONOSC,
    MFI,
    TRIX,
    ULTOSC,
    DX,
    MINUS_DI,
    PULS_DI,
    MINUS_DM,
    PLUS_DM,
    BBANDS,
    MIDPOINT,
    MIDPRICE,
    SAR,
    TRANGE,
    ATR,
    NATR,
    AD,
    ADOSC,
    OBV,
    HT_TRENDLINE,
    HT_SINE,
    HT_TRENDMODE,
    HT_DCPERIOD,
    HT_DCPHASE,
    HT_PHASOR,
    SECTOR
};

enum struct outputsize{
    compact,
    full
};

class AVConnection
{
    std::string AVKey;
    std::string executeGETRequest(std::string &urlString);
public:
	AVConnection() = delete;
	AVConnection(std::string UserKey);
    void Print_AVFunctions(AVFunctions series, std::string ticker,
                          outputsize size);
    ~AVConnection();
};

struct APIArgument
{
    std::string urlprefix;
    bool optional;
    bool (*validate)(std::string& value);
    APIArgument();
    APIArgument(APIArgument&& other);
    APIArgument(std::string urlprefix,
            bool (*func_ptr)(std::string& value), bool optional = true);
    APIArgument(const APIArgument& other);
    APIArgument& operator=(const APIArgument& other);
    std::string get_URLarg(std::string& input);
};

struct APIFunction
{
    std::vector<APIArgument> args;
    std::string funcName;
    APIFunction();
    APIFunction(const APIFunction& other);
    APIFunction& operator=(const APIFunction& other);
};

struct APISpecs
{
    std::unordered_map<AVFunctions, APIFunction> api_specs; 
    APISpecs();
    APISpecs(APISpecs& other) = delete;
    APISpecs& operator=(APISpecs& other) = delete;
    std::string build_url(AVFunctions func, std::string arg);
};
#endif
