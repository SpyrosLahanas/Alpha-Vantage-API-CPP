#define BOOST_TEST_MODULE Instrument Tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Instrument.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

BOOST_AUTO_TEST_CASE(Test_DayInfo)
{
    BOOST_CHECK_NO_THROW(
    DayInfo line1;
    line1.open = 225.75;
    line1.high = 226.84;
    line1.low = 225.522;
    line1.close = 223.84;
    line1.adjclose = 223.84;
    line1.volume = 31999289.0;
    line1.divident = 0.0;
    line1.split_coefficient = 1.0;
    std::cout << "Print fist instance " << std::endl;
    line1.print();
    //Use copy constructor
    DayInfo line2(line1);
    //Print new Instance
    std::cout << "Printing copy constructed instance...\n";
    line2.print();
    //Use copy assigment
    DayInfo line3;
    line3 = line2;
    std::cout << "Print copy assignment dinstance " << std::endl;
    line3.print();
    //Call parametrized constructor
    DayInfo line4(225.75, 226.84, 225.522, 223.84, 223.84, 31999289.0, 0.0,
        1.0);
    //Print new instance
    std::cout << "Print isntance initialised with paramatetrised "
        "constructor\n";
    line4.print();
    );
}

BOOST_AUTO_TEST_CASE(Test_PriceHistory_costructors)
{
    PriceHistory history;
    PriceHistory history2(history);
    PriceHistory history3(std::move(history2));
}

BOOST_AUTO_TEST_CASE(Test_PriceHistory_Set_Get)
{
    PriceHistory data(std::string("APLL"));
    data.insert_dayinfo(boost::gregorian::from_simple_string(
                std::string("2018-09-15")), DayInfo(225.75, 226.84, 225.522,
                223.84, 223.84, 31999289.0, 0.0, 1.0));
    data.insert_dayinfo(boost::gregorian::from_simple_string(
                std::string("2018-09-16")), 225.75, 226.84, 225.522,
                223.84, 223.84, 31999289.0, 0.0, 1.0);
    boost::gregorian::date date = 
        boost::gregorian::from_simple_string(std::string("2018-09-17"));
    double open = 225.75;
    double high = 226.84;
    double low = 225.522;
    double close = 223.84;
    double adjclose = 223.84;
    double volume = 31999289.0;
    double divident = 0.0;
    double split = 1.0;

    data.insert_dayinfo(date, open, high, low, close, adjclose, volume,
        divident, split);
    DayInfo& line = data.get_dayinfo(date);
    line.print();

    std::string ticker = data.get_ticker();
    std::cout << "Ticker: " << ticker << std::endl;
    data.set_ticker(std::string("IVZ"));
    std::cout << "Ticker changed to: " << data.get_ticker() << std::endl;
}
