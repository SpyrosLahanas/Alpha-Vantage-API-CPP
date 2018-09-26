#ifndef _H_INSTRUMENT
#define _H_INSTRUMENT
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>

struct DayInfo
{
    double open;
    double high;
    double low;
    double close;
    double adjclose;
    double volume;
    double divident;
    double split_coefficient;
    void print();
    DayInfo();
    DayInfo(double open, double high, double low, double close,
            double adjclose, double volume, double divident, double
            split_coefficient);
    DayInfo(const DayInfo& other);
    DayInfo& operator=(const DayInfo& other);
};

struct DateHash
{
    std::size_t operator()(boost::gregorian::date const& date) const noexcept;
};

class DayInfoNotFound : public std::exception
{
    virtual const char* what();
};

class PriceHistory
{
    std::unordered_map<boost::gregorian::date, DayInfo, DateHash> _data;
    std::string ticker;
public:
    PriceHistory();
    PriceHistory(std::string ticker);
    PriceHistory(const PriceHistory& other);
    PriceHistory(PriceHistory&& other);
    PriceHistory& operator=(const PriceHistory& other);
    std::string get_ticker() const;
    void set_ticker(std::string ticker);
    void insert_dayinfo(boost::gregorian::date date, DayInfo info);
    void insert_dayinfo(boost::gregorian::date date, double open,
            double high, double low, double close, double adjclose,
            double volume, double divident, double split_coefficient);
    DayInfo& get_dayinfo(boost::gregorian::date& date);
    std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::iterator begin();
    std::unordered_map<boost::gregorian::date, DayInfo,
        DateHash>::const_iterator cbegin() const;
    std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::iterator end();
    std::unordered_map<boost::gregorian::date, DayInfo,
        DateHash>::const_iterator cend() const;
};
#endif
