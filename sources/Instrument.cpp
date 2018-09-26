#include "Instrument.hpp"

void DayInfo::print()
{
    std::cout << "Open: " << open << std::endl;
    std::cout << "High: " << high << std::endl;
    std::cout << "Low: " << low << std::endl;
    std::cout << "Close: " << close << std::endl;
    std::cout << "Adjusted Close: " << adjclose << std::endl;
    std::cout << "Volume: " << volume << std::endl;
    std::cout << "Divident: " << divident << std::endl;
    std::cout << "Split coefficient: " << split_coefficient << std::endl;
}

DayInfo::DayInfo() : open(), high(), low(), close(), adjclose(), volume(),
    divident(), split_coefficient() {}

DayInfo::DayInfo(double open, double high, double low, double close,
            double adjclose, double volume, double divident, double
            split_coefficient) : open(std::move(open)), high(std::move(high)),
    low(std::move(low)), close(std::move(close)), adjclose(std::move(adjclose)),
    volume(std::move(volume)), divident(std::move(divident)),
    split_coefficient(split_coefficient) {}

DayInfo::DayInfo(const DayInfo& other)
{
    open = other.open;
    high = other.high;
    low = other.low;
    close = other.close;
    adjclose = other.adjclose;
    volume = other.volume;
    divident = other.divident;
    split_coefficient = other.split_coefficient;
}

DayInfo& DayInfo::operator=(const DayInfo& other)
{
    open = other.open;
    high = other.high;
    low = other.low;
    close = other.close;
    adjclose = other.adjclose;
    volume = other.volume;
    divident = other.divident;
    split_coefficient = other.split_coefficient;
    return *this;
}

std::size_t DateHash::operator()(boost::gregorian::date const& date) const noexcept
{
    return std::hash<int>{}(date.julian_day());
}

const char* DayInfoNotFound::what()
{
    return "DayInfoNotFound: Lookup operation failed";
}

PriceHistory::PriceHistory() : _data(), ticker() {}

PriceHistory::PriceHistory(std::string ticker) : _data()
    ,ticker(std::move(ticker)) {}

PriceHistory::PriceHistory(const PriceHistory& other) : ticker(other.ticker),
    _data(other._data) {}

PriceHistory::PriceHistory(PriceHistory&& other) :
    ticker(std::move(other.ticker)), 
    _data(std::move(other._data)) {}

PriceHistory& PriceHistory::operator=(const PriceHistory& other)
{
    ticker = other.ticker;
    _data = other._data; 
    return *this;
}

std::string PriceHistory::get_ticker() const
{
    return std::string(ticker);
}

void PriceHistory::set_ticker(std::string nticker)
{
    ticker = nticker;
}

void PriceHistory::insert_dayinfo(boost::gregorian::date date, DayInfo info)
{
    _data.emplace(std::pair<boost::gregorian::date, DayInfo>(
            boost::gregorian::date(std::move(date)),
            DayInfo(std::move(info))));
}

void PriceHistory::insert_dayinfo(boost::gregorian::date date, double open,
        double high, double low, double close, double adjclose,
        double volume, double divident, double split_coefficient)
{
    _data.emplace(std::pair<boost::gregorian::date, DayInfo>(
                boost::gregorian::date(std::move(date)),
                DayInfo(std::move(open), std::move(high), std::move(low),
                std::move(close), std::move(adjclose), std::move(volume),
                std::move(divident), std::move(split_coefficient))));
}

DayInfo& PriceHistory::get_dayinfo(boost::gregorian::date& date)
{
    std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::iterator 
        found = _data.find(date);
    if(found != _data.end()) return found->second;
    else throw DayInfoNotFound();
}

std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::iterator 
    PriceHistory::begin()
{
    return _data.begin();
}

std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::const_iterator 
    PriceHistory::cbegin() const
{
    return _data.cbegin();
}

std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::iterator 
    PriceHistory::end()
{
    return _data.end();
}

std::unordered_map<boost::gregorian::date, DayInfo, DateHash>::const_iterator 
    PriceHistory::cend() const
{
    return _data.cend();
}
