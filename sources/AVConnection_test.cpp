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

//BOOST_AUTO_TEST_CASE(CheckGlobalStockDaily)
//{
//    /*Description: Called with valid parameters, print on the terminal the data
//      returned */
//
//    //Valid API key: L9AUO9JRFTMP694H
//    std::string AVKey("L9AUO9JRFTMP694H");
//    AVConnection conn(AVKey);
//    std::cout << "This is printing from the 'CheckGlobalStockDaily Unit Test"
//        << std::endl;
//    std::cout << "Working on the TIME_SERIES_DAILY" << std::endl;
//    BOOST_CHECK_NO_THROW(
//       conn.Print_GlobalEquity(GlobalEquity::TIME_SERIES_DAILY,
//           "AAPL", outputsize::compact)
//    );
//
//    std::chrono::seconds dura(15);
//    std::this_thread::sleep_for(dura);
//
//    std::cout << "Working on the TIME_SERIES_DAILY_ADJUSTED" << std::endl;
// 
//    BOOST_CHECK_NO_THROW(
//       conn.Print_GlobalEquity(GlobalEquity::TIME_SERIES_DAILY_ADJUSTED,
//           "AAPL", outputsize::compact)
//    );
//
//    std::this_thread::sleep_for(dura);
//
//    std::cout << "Working on the TIME_SERIES_WEEKLY" << std::endl;
// 
//    BOOST_CHECK_NO_THROW(
//       conn.Print_GlobalEquity(GlobalEquity::TIME_SERIES_WEEKLY,
//           "AAPL", outputsize::compact)
//    );
//
//    std::this_thread::sleep_for(dura);
//
//    std::cout << "Working on the TIME_SERIES_WEEKLY_ADJUSTED" << std::endl;
// 
//    BOOST_CHECK_NO_THROW(
//       conn.Print_GlobalEquity(GlobalEquity::TIME_SERIES_WEEKLY_ADJUSTED,
//           "AAPL", outputsize::compact)
//    );
//
//    std::this_thread::sleep_for(dura);
//
//    std::cout << "Working on the TIME_SERIES_MONTHLY" << std::endl;
// 
//    BOOST_CHECK_NO_THROW(
//       conn.Print_GlobalEquity(GlobalEquity::TIME_SERIES_MONTHLY,
//           "AAPL", outputsize::compact)
//    );
//
//    std::this_thread::sleep_for(dura);
//
//    std::cout << "Working on the TIME_SERIES_MONTHLY_ADJUSTED" << std::endl;
// 
//    BOOST_CHECK_NO_THROW(
//       conn.Print_GlobalEquity(GlobalEquity::TIME_SERIES_MONTHLY_ADJUSTED,
//           "AAPL", outputsize::compact)
//    );
//}

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
             std::cout << it->urlComponent << std::endl;
    );
}
