#define BOOST_TEST_MODULE AVConnection Tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "AVConnection.hpp"
#include <chrono>
#include <thread>

BOOST_AUTO_TEST_CASE(ValidKeyNoException)
{
    //Description: Called with a valid key, the construction succeeds 

    //Valid API key: L9AUO9JRFTMP694H
    //AlphaVantage key is required for the connection
    std::string AVKey("L9AUO9JRFTMP694H");
    BOOST_CHECK_NO_THROW(AVConnection conn(AVKey));
} 

BOOST_AUTO_TEST_CASE(InvalidKeyThrowException)
{
    //Description: Called with a invalid key, the construction fails 
    //AlphaVantage key is required for the connection
    std::string AVKey("");
    BOOST_CHECK_THROW(AVConnection conn(AVKey), AVInvalidKey);
} 

BOOST_AUTO_TEST_CASE(Test_AVConnection)
{
    BOOST_CHECK_NO_THROW(
    AVConnection conn(std::string("L9AUO9JRFTMP694H"));
    conn.Print_AVFunction(AVFunctions::TIME_SERIES_INTRADAY,
            "IVZ", "60min", "compact", "json", "apikey1");
    );
}

BOOST_AUTO_TEST_CASE(Test_APIArgument)
{
    BOOST_CHECK_NO_THROW(
    APIArgument arg("function=", [](std::string& value) -> bool {return value
            == "TIME_SERIES_INTRADAY";});

    APIArgument arg1("symbol=", [](std::string& value) ->bool {return true;});
    APIArgument arg2("interval=", [](std::string& value) -> bool {
                return (value == "1min" || value == "5min" || value == "15min"
                        || value == "30min" || "60min");
            });
    APIArgument arg3("outputsize=", [](std::string& value) -> bool
            {
                return (value == "compact" || value == "full");
            });
    std::string input("RandomName");
    std::cout << std::boolalpha << arg.validate(input) <<
        std::noboolalpha << std::endl;
    );
}

BOOST_AUTO_TEST_CASE(Test_APIFunction)
{
    BOOST_CHECK_NO_THROW(
    APIArgument arg("function=", [](std::string& value) -> bool {return value
            == "TIME_SERIES_INTRADAY";});

    APIArgument arg1("symbol=", [](std::string& value) ->bool {return true;});
    APIArgument arg2("interval=", [](std::string& value) -> bool {
                return (value == "1min" || value == "5min" || value == "15min"
                        || value == "30min" || "60min");
            });
    APIArgument arg3("outputsize=", [](std::string& value) -> bool
            {
                return (value == "compact" || value == "full");
            });
    APIFunction func;
    func.args.push_back(arg);
    func.args.push_back(arg1);
    func.args.push_back(arg2);
    func.args.push_back(arg3);

    for (std::vector<APIArgument>::iterator it = func.args.begin();
        it != func.args.end(); it++)
             std::cout << it->urlprefix << std::endl;
    );
}

BOOST_AUTO_TEST_CASE(Test_APISpecs)
{
    //VALIDATE urls returned
    APISpecs specs = APISpecs();

    //CHECK the TIME_SERIES_INTRADAY
    std::string url = specs.build_url(AVFunctions::TIME_SERIES_INTRADAY,
            "IVZ", "60min", "compact", "json", "apikey1");
    std::string teststr(
    "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&"
    "symbol=IVZ&interval=60min&outputsize=compact&datatype=json&"
    "apikey=apikey1");
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the TIME_SERIES_DAILY
    url = specs.build_url(AVFunctions::TIME_SERIES_DAILY, "IVZ", "compact",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&"
        "symbol=IVZ&outputsize=compact&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the TIME_SERIES_DAILY_ADJUSTED
    url = specs.build_url(AVFunctions::TIME_SERIES_DAILY_ADJUSTED,
            "IVZ", "compact", "json", "apikey");
    teststr =
      "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY_ADJUSTED&"
      "symbol=IVZ&outputsize=compact&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the TIME_SERIES_WEEKLY
    url = specs.build_url(AVFunctions::TIME_SERIES_WEEKLY, "IVZ", "json",
            "apikey");
    teststr =
      "https://www.alphavantage.co/query?function=TIME_SERIES_WEEKLY&"
      "symbol=IVZ&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the TIME_SERIES_WEEKLY_ADJUSTED
    url = specs.build_url(AVFunctions::TIME_SERIES_WEEKLY_ADJUSTED,
            "IVZ", "json", "apikey");
    teststr =
      "https://www.alphavantage.co/query?function=TIME_SERIES_WEEKLY_ADJUSTED&"
      "symbol=IVZ&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the TIME_SERIES_MONTHLY
    url = specs.build_url(AVFunctions::TIME_SERIES_MONTHLY,
            "IVZ", "json", "apikey");
    teststr =
      "https://www.alphavantage.co/query?function=TIME_SERIES_MONTHLY&"
      "symbol=IVZ&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the TIME_SERIES_MONTHLY_ADJUSTED
    url = specs.build_url(AVFunctions::TIME_SERIES_MONTHLY_ADJUSTED,
            "IVZ", "json", "apikey");
    teststr =
      "https://www.alphavantage.co/query?function=TIME_SERIES_MONTHLY_ADJUSTED&"
      "symbol=IVZ&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the GLOBAL_QUOTE
    url = specs.build_url(AVFunctions::GLOBAL_QUOTE,
            "IVZ", "json", "apikey");
    teststr =
      "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&"
      "symbol=IVZ&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url,teststr);

    //CHECK the CURRENCY_EXCHANGE_RATE
    url = specs.build_url(AVFunctions::CURRENCY_EXCHANGE_RATE, "USD", "GBP",
            "apikey");

    teststr =
        "https://www.alphavantage.co/query?function=CURRENCY_EXCHANGE_RATE&"
        "from_currency=USD&to_currency=GBP&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the FX_INTRADAY
    url = specs.build_url(AVFunctions::FX_INTRADAY, "USD", "GBP", "60min",
            "compact", "json", "apikey");

    teststr =
        "https://www.alphavantage.co/query?function=FX_INTRADAY&"
        "from_currency=USD&to_currency=GBP&interval=60min&outputsize=compact&"
        "datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the FX_DAILY
    url = specs.build_url(AVFunctions::FX_DAILY, "USD", "GBP", "compact",
            "json", "apikey");
    teststr =
        "https://www.alphavantage.co/query?function=FX_DAILY&from_currency=USD"
        "&to_currency=GBP&outputsize=compact&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the FX_WEEKLY
    url = specs.build_url(AVFunctions::FX_WEEKLY, "USD", "GBP", "json",
            "apikey");
    teststr =
        "https://www.alphavantage.co/query?function=FX_WEEKLY&from_currency=USD"
        "&to_currency=GBP&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the FX_MONTHLY
    url = specs.build_url(AVFunctions::FX_MONTHLY, "USD", "GBP", "json",
            "apikey");
    teststr =
        "https://www.alphavantage.co/query?function=FX_MONTHLY&from_currency=USD"
        "&to_currency=GBP&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //The first function in the Cryptocurrencies secion is the same as the
    //first in the fx one.

    //CHECK the DIGITAL_CURRENCY_DAILY
    url = specs.build_url(AVFunctions::DIGITAL_CURRENCY_DAILY, "BTC", "GBP",
            "apikey");
    teststr = "https://www.alphavantage.co/query?"
        "function=DIGITAL_CURRENCY_DAILY&symbol=BTC&market=GBP&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the DIGITAL_CURRENCY_WEEKLY
    url = specs.build_url(AVFunctions::DIGITAL_CURRENCY_WEEKLY, "BTC", "GBP",
            "apikey");
    teststr = "https://www.alphavantage.co/query?"
        "function=DIGITAL_CURRENCY_WEEKLY&symbol=BTC&market=GBP&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the DIGITAL_CURRENCY_MONTHLY
    url = specs.build_url(AVFunctions::DIGITAL_CURRENCY_MONTHLY, "BTC", "GBP",
            "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=DIGITAL_CURRENCY_MONTHLY&symbol=BTC&market=GBP&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the SMA
    url = specs.build_url(AVFunctions::SMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=SMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the EMA
    url = specs.build_url(AVFunctions::EMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=EMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the WMA
    url = specs.build_url(AVFunctions::WMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=WMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the DEMA
    url = specs.build_url(AVFunctions::DEMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=DEMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the TEMA
    url = specs.build_url(AVFunctions::TEMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=TEMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the TRIMA
    url = specs.build_url(AVFunctions::TRIMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=TRIMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the KAMA
    url = specs.build_url(AVFunctions::KAMA, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=KAMA&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK MAMA
    url = specs.build_url(AVFunctions::MAMA, "IVZ", "monthly", "close", "0.02",
            "0.01", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
        "function=MAMA&symbol=IVZ&interval=monthly&series_type=close&"
        "fastlimit=0.02&slowlimit=0.01&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the T3
    url = specs.build_url(AVFunctions::T3, "IVZ", "monthly","60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=T3&symbol=IVZ&interval=monthly&time_period=60&"
       "series_type=close&datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MACD
    url = specs.build_url(AVFunctions::MACD, "IVZ", "monthly", "close",
            "12", "26", "9", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=MACD&symbol=IVZ&interval=monthly&"
       "series_type=close&fastperiod=12&slowperiod=26&signalperiod=9&"
       "datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MACDEXT
    url = specs.build_url(AVFunctions::MACDEXT, "IVZ", "monthly", "close",
            "12", "26", "9", "0", "8", "4", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?"
       "function=MACDEXT&symbol=IVZ&interval=monthly&"
       "series_type=close&fastperiod=12&slowperiod=26&signalperiod=9&"
       "fastmatype=0&slowmatype=8&signalmatype=4&"
       "datatype=json&apikey=apikey";

    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the STOCH
    url = specs.build_url(AVFunctions::STOCH, "IVZ", "monthly", "5", "3", "3",
            "4", "0", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=STOCH&symbol=IVZ&"
        "interval=monthly&fastkperiod=5&slowkperiod=3&slowdperiod=3&"
        "slowkmatype=4&slowdmatype=0&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the STOCHF
    url = specs.build_url(AVFunctions::STOCHF, "IVZ", "monthly", "5", "3",
            "4", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=STOCHF&symbol=IVZ&"
        "interval=monthly&fastkperiod=5&fastdperiod=3&"
        "fastdmatype=4&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the RSI
    url = specs.build_url(AVFunctions::RSI, "IVZ", "monthly",
            "60", "close", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=RSI&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&"
        "datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the STOCHRSI
    url = specs.build_url(AVFunctions::STOCHRSI, "IVZ", "monthly", "60",
            "close", "4", "3", "0", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=STOCHRSI&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&fastkperiod=4&"
        "fastdperiod=3&fastdmatype=0&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the WILLR
    url = specs.build_url(AVFunctions::WILLR, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=WILLR&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ADX
    url = specs.build_url(AVFunctions::ADX, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=ADX&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ADXR
    url = specs.build_url(AVFunctions::ADXR, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=ADXR&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the APO
    url = specs.build_url(AVFunctions::APO, "IVZ", "monthly", "close", "12",
            "26", "0", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=APO&symbol=IVZ&"
        "interval=monthly&series_type=close&fastperiod=12&slowperiod=26&"
        "matype=0&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the PPO
    url = specs.build_url(AVFunctions::PPO, "IVZ", "monthly", "close", "12",
            "26", "0", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=PPO&symbol=IVZ&"
        "interval=monthly&series_type=close&fastperiod=12&slowperiod=26&"
        "matype=0&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MOM
    url = specs.build_url(AVFunctions::MOM, "IVZ", "monthly", "60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=MOM&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&datatype=json"
        "&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the BOP
    url = specs.build_url(AVFunctions::BOP, "IVZ", "monthly", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=BOP&symbol=IVZ&"
        "interval=monthly&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the CCI
    url = specs.build_url(AVFunctions::CCI, "IVZ", "monthly", "60", "json",
            "apikey");
    teststr = "https://www.alphavantage.co/query?function=CCI&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the CMO
    url = specs.build_url(AVFunctions::CMO, "IVZ", "monthly", "60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=CMO&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ROC
    url = specs.build_url(AVFunctions::ROC, "IVZ", "monthly", "60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=ROC&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ROCR
    url = specs.build_url(AVFunctions::ROCR, "IVZ", "monthly", "60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=ROCR&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the AROON
    url = specs.build_url(AVFunctions::AROON, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=AROON&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the AROONOSC
    url = specs.build_url(AVFunctions::AROONOSC, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=AROONOSC&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MFI
    url = specs.build_url(AVFunctions::MFI, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=MFI&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the TRIX
    url = specs.build_url(AVFunctions::TRIX, "IVZ", "monthly", "60", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=TRIX&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ULTOSC
    url = specs.build_url(AVFunctions::ULTOSC, "IVZ", "monthly", "7", "14",
            "28", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=ULTOSC&symbol=IVZ&"
        "interval=monthly&timeperiod1=7&timeperiod2=14&timeperiod3=28&"
        "datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the DX
    url = specs.build_url(AVFunctions::DX, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=DX&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MINUS_DI
    url = specs.build_url(AVFunctions::MINUS_DI, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=MINUS_DI&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the PLUS_DI
    url = specs.build_url(AVFunctions::PLUS_DI, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=PLUS_DI&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MINUS_DM
    url = specs.build_url(AVFunctions::MINUS_DM, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=MINUS_DM&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the PLUS_DM
    url = specs.build_url(AVFunctions::PLUS_DM, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=PLUS_DM&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the BBANDS
    url = specs.build_url(AVFunctions::BBANDS, "IVZ", "monthly", "60", "close",
            "2", "2", "0", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=BBANDS&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&nbdevup=2&nbdevdn=2&"
        "matype=0&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MIDPOINT
    url = specs.build_url(AVFunctions::MIDPOINT, "IVZ", "monthly", "60",
            "close", "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=MIDPOINT&symbol=IVZ&"
        "interval=monthly&time_period=60&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the MIDPRICE
    url = specs.build_url(AVFunctions::MIDPRICE, "IVZ", "monthly", "60",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=MIDPRICE&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the SAR
    url = specs.build_url(AVFunctions::SAR, "IVZ", "monthly", "0.05", "0.25",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=SAR&symbol=IVZ&"
        "interval=monthly&acceleration=0.05&maximum=0.25&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the TRANGE
    url = specs.build_url(AVFunctions::TRANGE, "IVZ", "monthly", "json",
            "apikey");
    teststr = "https://www.alphavantage.co/query?function=TRANGE&symbol=IVZ&"
        "interval=monthly&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ATR
    url = specs.build_url(AVFunctions::ATR, "IVZ", "monthly", "60", "json",
            "apikey");
    teststr = "https://www.alphavantage.co/query?function=ATR&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the NATR
    url = specs.build_url(AVFunctions::NATR, "IVZ", "monthly", "60", "json",
            "apikey");
    teststr = "https://www.alphavantage.co/query?function=NATR&symbol=IVZ&"
        "interval=monthly&time_period=60&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the AD
    url = specs.build_url(AVFunctions::AD, "IVZ", "monthly", "json",
            "apikey");
    teststr = "https://www.alphavantage.co/query?function=AD&symbol=IVZ&"
        "interval=monthly&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the ADOSC
    url = specs.build_url(AVFunctions::ADOSC, "IVZ", "monthly", "3", "10",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=ADOSC&symbol=IVZ&"
        "interval=monthly&fastperiod=3&slowperiod=10&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the OBV
    url = specs.build_url(AVFunctions::OBV, "IVZ", "monthly", "json",
            "apikey");
    teststr = "https://www.alphavantage.co/query?function=OBV&symbol=IVZ&"
        "interval=monthly&datatype=json&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the HT_TRENDLINE
    url = specs.build_url(AVFunctions::HT_TRENDLINE, "IVZ", "monthly", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=HT_TRENDLINE&"
        "symbol=IVZ&interval=monthly&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the HT_SINE
    url = specs.build_url(AVFunctions::HT_SINE, "IVZ", "monthly", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=HT_SINE&"
        "symbol=IVZ&interval=monthly&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the HT_TRENDMODE
    url = specs.build_url(AVFunctions::HT_TRENDMODE, "IVZ", "monthly", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=HT_TRENDMODE&"
        "symbol=IVZ&interval=monthly&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the HT_DCPERIOD
    url = specs.build_url(AVFunctions::HT_DCPERIOD, "IVZ", "monthly", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=HT_DCPERIOD&"
        "symbol=IVZ&interval=monthly&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the HT_DCPHASE
    url = specs.build_url(AVFunctions::HT_DCPHASE, "IVZ", "monthly", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=HT_DCPHASE&"
        "symbol=IVZ&interval=monthly&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the HT_PHASOR
    url = specs.build_url(AVFunctions::HT_PHASOR, "IVZ", "monthly", "close",
            "json", "apikey");
    teststr = "https://www.alphavantage.co/query?function=HT_PHASOR&"
        "symbol=IVZ&interval=monthly&series_type=close&datatype=json&"
        "apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);

    //CHECK the SECTOR
    url = specs.build_url(AVFunctions::SECTOR, "apikey");
    teststr = "https://www.alphavantage.co/query?function=SECTOR&apikey=apikey";
    BOOST_CHECK_EQUAL(url, teststr);
}

BOOST_AUTO_TEST_CASE(Test_dyn_build_url)
{
    //Construct a paremeter pack
    std::vector<std::string> args;
    args.push_back(std::string("IVZ"));
    args.push_back(std::string("compact"));
    args.push_back(std::string("json"));
    args.push_back(std::string("apikey"));

    //Call the build URL function
    APISpecs specs; 
    std::string url = specs.build_url(AVFunctions::TIME_SERIES_DAILY, args);
    std::string correcturl("https://www.alphavantage.co/query?"
            "function=TIME_SERIES_DAILY&symbol=IVZ&outputsize=compact&"
            "datatype=json&apikey=apikey");
    BOOST_CHECK_EQUAL(url, correcturl);
}

BOOST_AUTO_TEST_CASE(Test_is_function)
{
    APISpecs specs;
    std::string strtest("TIME_SERIES_DAILY");
    bool isvalid = specs.is_function(strtest);
    
    BOOST_CHECK_EQUAL(isvalid, true);

    strtest = "randomstring";
    isvalid = specs.is_function(strtest);
    BOOST_CHECK_EQUAL(isvalid, false);

    AVConnection conn("L9AUO9JRFTMP694H");
    strtest = "TIME_SERIES_DAILY";
    isvalid = conn.is_function(strtest);

    BOOST_CHECK_EQUAL(isvalid, true);

    strtest = "randomstring";
    isvalid = conn.is_function(strtest);

    BOOST_CHECK_EQUAL(isvalid, false);
}

BOOST_AUTO_TEST_CASE(Test_lookup_function)
{
    AVConnection conn("L9AUO9JRFTMP694H");
    APISpecs specs;
    std::string funcName("TIME_SERIES_DAILY");
    AVFunctions funcNum = specs.lookup_function(funcName);
    BOOST_CHECK(funcNum == AVFunctions::TIME_SERIES_DAILY);
    funcNum = conn.lookup_function(funcName);
    BOOST_CHECK(funcNum == AVFunctions::TIME_SERIES_DAILY);
    funcName = "Random String";
    funcNum = specs.lookup_function(funcName);
    BOOST_CHECK(funcNum == AVFunctions::NONE);
    funcNum = conn.lookup_function(funcName);
    BOOST_CHECK(funcNum == AVFunctions::NONE);
}
