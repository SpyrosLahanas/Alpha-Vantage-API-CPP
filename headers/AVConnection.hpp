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
    PLUS_DI,
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
    std::string get_URLarg(std::string input);
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
    std::string urlroot;
    APISpecs();
    APISpecs(APISpecs& other) = delete;
    APISpecs& operator=(APISpecs& other) = delete;
    bool is_function(std::string funcName);
    std::string build_url(AVFunctions func, std::vector<std::string>& args);
    template<class ...Ts>
        std::string build_url(AVFunctions func, Ts ...args)
        {
            //FETCH from the function specs
            std::unordered_map<AVFunctions, APIFunction>::iterator iter =
                api_specs.find(func);
            if(iter == api_specs.end()) throw AVInvalidParameters();

            //GET iterators for function arguments
            std::vector<APIArgument>::iterator beginit =
                iter->second.args.begin();
            std::vector<APIArgument>::iterator endit =
                iter->second.args.end();

            return urlroot +
                build_element(beginit, endit, iter->second.funcName, args...);
        }

    template<class T>
        std::string build_element(std::vector<APIArgument>::iterator beginit,
            std::vector<APIArgument>::iterator endit, T arg)
        {
            if(beginit == endit) throw AVInvalidParameters();
            return beginit->get_URLarg(arg);
        }

    template<class T, class ...Ts>
        std::string build_element(std::vector<APIArgument>::iterator beginit,
                std::vector<APIArgument>::iterator endit, T arg, Ts ...args)
        {
            if(beginit == endit) throw AVInvalidParameters();
            std::string urlstr(beginit->get_URLarg(arg));
            beginit++;
            std::string urlstr1 = build_element(beginit, endit, args...);
            return urlstr + "&" + urlstr1;
        }
};

class AVConnection
{
    std::string AVKey;
    std::string executeGETRequest(std::string &urlString);
    APISpecs specs;
public:
	AVConnection() = delete;
	AVConnection(std::string UserKey);
    void Print_AVFunction(AVFunctions function, std::vector<std::string>& args);
    template<class ...Ts>
        void Print_AVFunction(AVFunctions function, Ts ...args)
        /*Description: Prints on the terminal the data Alpha Vantage returns
          for the query parameters provided.*/
        {
            //Prepare the URL
            std::string url = specs.build_url(function, args..., AVKey);
            //Execute the request
            std::string datareturned = executeGETRequest(url);

            //Send data to the console
            std::unique_ptr<JsonObject> NewJson(read_from_str(datareturned));
            NewJson->console_print();
        }
    ~AVConnection();
};
#endif
