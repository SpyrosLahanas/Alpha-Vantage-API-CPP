#include "AVConnection.hpp"

size_t read_return(char *bufptr, size_t size, size_t nitems, void *userp)
{
    //=============================DESCRIPTION================================
    /*The is the WRITEFUNCTION passed to curl. It simply writes the data
    returned by the url request in a string. The intention is to use only this
    function with curl. All data manipulation is left for the fuction which
    calls CURL*/
    std::string *return_string = (std::string *) userp;
    for (int i = 0; i < nitems; i++)
    {
        *return_string += bufptr[i]; 
    } 
    return nitems;
}

AVConnection::AVConnection(std::string UserKey) : AVKey(UserKey), specs()
{
    //==============================Description===============================
    /*Class Constructor: Execute a dummy query to test the Alpha Vantage Key
      provided.
    NOTE: It seems that api calls will work as long as the url contains
    and 'apikey=SOMESTRING', regardless of what SOMESTRING is. Inspite of that
    the constructor will run a check on the key, in case Vantage Alpha
    decides to change its policy in the future.*/
    
    std::string queryUrl("https://www.alphavantage.co/query?"
            "function=TIME_SERIES_DAILY&symbol=AAPL&outputsize=compact"
            "&apikey=");
    queryUrl = queryUrl + AVKey;


    /*WARNING: LIBCURL documentation instructs that the curl_global_init function
    must be called once for the whole application. That implies that any
    application using this class must use only one instance of it.*/

    //Intitialise the curl session
    curl_global_init(CURL_GLOBAL_ALL); 

    //Execute the GET request on the URL
    std::string return_string = executeGETRequest(queryUrl);

    //Check if we got an error sting
    std::string error_mess("{\n    \"Error Message\":");
    if(error_mess == return_string.substr(0, 22))
    {
        throw AVInvalidKey(); 
    }
}

bool AVConnection::is_function(const std::string& funcName)
{
    return specs.is_function(funcName);
}

AVFunctions AVConnection::lookup_function(const std::string& funcName)
{
    return specs.lookup_function(funcName);
}

AVConnection::~AVConnection()
{
    curl_global_cleanup();
}

std::string AVConnection::executeGETRequest(std::string &urlString)
{
    //DESCRIPTION: Execute the GET request on the URL provided

    //Get a handle from curl
    CURL *CurlHandle = curl_easy_init();

    //Integer to hold the return status from curl
    CURLcode return_status;

    //Set GET request options
    return_status = curl_easy_setopt(CurlHandle, CURLOPT_URL,
            urlString.c_str());
 
    //Create a string to house the data returned from the GET request
    std::string return_string;
    return_status = curl_easy_setopt(CurlHandle, CURLOPT_WRITEDATA,
            &return_string); 
    return_status = curl_easy_setopt(CurlHandle, CURLOPT_WRITEFUNCTION,
            read_return);

    //Execute the transfer
    return_status = curl_easy_perform(CurlHandle);

    if(return_status != CURLE_OK)
    {
        throw AVQueryFailure();
    } 

    //Clean-up the resources used by CurlHandle
    curl_easy_cleanup(CurlHandle);
    return return_string;
}

bool APISpecs::is_function(const std::string& funcName)
{
    for(std::unordered_map<AVFunctions, APIFunction>::iterator iter =
            api_specs.begin(); iter != api_specs.end(); iter++)
    {
        if(iter->second.funcName == funcName) return true;
    }
    return false;
}

AVFunctions APISpecs::lookup_function(const std::string& funcName)
{
    for(std::unordered_map<AVFunctions, APIFunction>::iterator iter =
            api_specs.begin(); iter != api_specs.end(); iter++)
    {
        if(iter->second.funcName == funcName) return iter->first;
    }
    return AVFunctions::NONE;
}

const char* AVInvalidKey::what()
{
    return "Alpha Vantage API key validation failed";
}

const char *AVQueryFailure::what()
{
    return "Alpha Vantage API call  failed";
}

const char *AVInvalidParameters::what()
{
    return "Alpha Vantage API Error: Wrong call parameteres";
}

APIArgument::APIArgument() : urlprefix(), optional() {}

APIArgument::APIArgument(APIArgument&& other) :
    urlprefix(std::move(other.urlprefix)), optional(std::move(other.optional))
{
    validate = other.validate;
}

APIArgument::APIArgument(std::string urlprefix,
        bool (*func_ptr)(std::string& value), bool optional) :
    urlprefix(urlprefix), optional(optional)
{
    validate = func_ptr;
}

APIArgument::APIArgument(const APIArgument& other) :
    urlprefix(other.urlprefix), optional(other.optional)
{
    validate = other.validate;
}

APIArgument& APIArgument::operator=(const APIArgument& other)
{
    urlprefix = other.urlprefix;
    validate = other.validate;
    optional = other.optional;
    return *this;
}

std::string APIArgument::get_URLarg(std::string input)
{
    if(!validate(input)) throw AVInvalidParameters();

    return urlprefix + input;
}

APIFunction::APIFunction() : args(), funcName() {};

APIFunction::APIFunction(const APIFunction& other) : args(other.args),
                                                 funcName(other.funcName) {}

APIFunction& APIFunction::operator=(const APIFunction& other)
{
    args = other.args;
    funcName = other.funcName;
    return *this;
}

bool always_true(std::string& value){return true;}

bool validate_interval(std::string& value)
{
    if(value == "1min" || value == "5min" || value == "15min" ||
            value == "30min" || value == "60min") return true;
    else return false;
}

bool validate_tech_interval(std::string& value)
{
    if(value == "1min" || value == "5min" || value == "15min" ||
            value == "30min" || value == "60min" || value == "daily"
            || value == "weekly" || value == "monthly") return true;
    else return false;
}

bool validate_posi_int(std::string& value)
{
    try
    {
        int arg = std::stoi(value);
        return arg > 0;
    }
    catch(...)
    {
        return false;
    }
}
bool validate_ma_type(std::string& value)
{
    try
    {
        int arg = std::stoi(value);
        return (arg >= 0 || arg <= 8);
    }
    catch(...)
    {
        return false;
    }
}

bool validate_dbl(std::string& value)
{
    try
    {
        double arg = std::stod(value);
        return arg > 0;
    }
    catch(...)
    {
        return false;
    }
}

bool validate_series_type(std::string& value)
{
    if(value == "close" || value == "open" || value == "high" || value ==
            "low") return true;
    else return false;
}

bool validate_outputsize(std::string& value)
{
    if (value == "compact" || value == "full") return true;
    else return false;
}

bool validate_datatype(std::string& value)
{
    if(value == "json" || value == "csv") return true;
    else return false;
}

APISpecs::APISpecs() : urlroot("https://www.alphavantage.co/query?")
{
    api_specs.reserve(70);
    //Log the TIME_SERIES_INTRADAY specs
    //Construct the arguments
    APIArgument function("function=", &always_true, false);
    APIArgument symbol("symbol=", &always_true, false);
    APIArgument interval("interval=", &validate_interval, false);
    APIArgument output("outputsize=", &validate_outputsize, true);
    APIArgument datatype("datatype=", &validate_datatype, true);
    APIArgument apikey("apikey=", &always_true, false);

    //Construct the function
    APIFunction intraday;
    intraday.funcName = "TIME_SERIES_INTRADAY";
    intraday.args.push_back(function);
    intraday.args.push_back(symbol);
    intraday.args.push_back(interval);
    intraday.args.push_back(output);
    intraday.args.push_back(datatype);
    intraday.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_INTRADAY] = intraday;

    //Log the TIME_SERIES_DAILY specs
    //All arguments already constructed
    //Construct the function
    APIFunction daily;
    daily.funcName = "TIME_SERIES_DAILY";
    daily.args.push_back(function);
    daily.args.push_back(symbol);
    daily.args.push_back(output);
    daily.args.push_back(datatype);
    daily.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_DAILY] = daily;

    //Log the TIME_SERIES_DAILY_ADJUSTED specs
    //All arguments already constructed
    //Construct the function
    APIFunction daily_adjusted;
    daily_adjusted.funcName = "TIME_SERIES_DAILY_ADJUSTED";
    daily_adjusted.args.push_back(function);
    daily_adjusted.args.push_back(symbol);
    daily_adjusted.args.push_back(output);
    daily_adjusted.args.push_back(datatype);
    daily_adjusted.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_DAILY_ADJUSTED] = daily_adjusted;

    //Log the TIME_SERIES_WEEKLY specs
    //All arguments already constructed
    //Construct the function
    APIFunction weekly;
    weekly.funcName = "TIME_SERIES_WEEKLY";
    weekly.args.push_back(function);
    weekly.args.push_back(symbol);
    weekly.args.push_back(datatype);
    weekly.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_WEEKLY] = weekly;

    //Log the TIME_SERIES_WEEKLY_ADJUSTED specs
    //All arguments already constructed
    //Construct the function
    APIFunction weekly_adjusted;
    weekly_adjusted.funcName = "TIME_SERIES_WEEKLY_ADJUSTED";
    weekly_adjusted.args.push_back(function);
    weekly_adjusted.args.push_back(symbol);
    weekly_adjusted.args.push_back(datatype);
    weekly_adjusted.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_WEEKLY_ADJUSTED] = weekly_adjusted;

    //Log the TIME_SERIES_MONTHLY specs
    //All arguments already constructed
    //Construct the function
    APIFunction monhtly;
    monhtly.funcName = "TIME_SERIES_MONTHLY";
    monhtly.args.push_back(function);
    monhtly.args.push_back(symbol);
    monhtly.args.push_back(datatype);
    monhtly.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_MONTHLY] = monhtly;

    //Log the TIME_SERIES_MONTHLY_ADJUSTED specs
    //All arguments already constructed
    //Construct the function
    APIFunction monthly_adjusted;
    monthly_adjusted.funcName = "TIME_SERIES_MONTHLY_ADJUSTED";
    monthly_adjusted.args.push_back(function);
    monthly_adjusted.args.push_back(symbol);
    monthly_adjusted.args.push_back(datatype);
    monthly_adjusted.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TIME_SERIES_MONTHLY_ADJUSTED] = monthly_adjusted;

    //Log the GLOBAL_QUOTE specs
    //All arguments already constructed
    //Construct the function
    APIFunction global_quote;
    global_quote.funcName = "GLOBAL_QUOTE";
    global_quote.args.push_back(function);
    global_quote.args.push_back(symbol);
    global_quote.args.push_back(datatype);
    global_quote.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::GLOBAL_QUOTE] = global_quote;

    //Log the CURRENCY_EXCHANGE_RATE specs
    //Construct the arguments from_currency and to_currency
    APIArgument from_FX("from_currency=", &always_true, false);
    APIArgument to_FX("to_currency=", &always_true, false);

    //Construct the function
    APIFunction fx_rate;
    fx_rate.funcName = "CURRENCY_EXCHANGE_RATE";
    fx_rate.args.push_back(function);
    fx_rate.args.push_back(from_FX);
    fx_rate.args.push_back(to_FX);
    fx_rate.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::CURRENCY_EXCHANGE_RATE] = fx_rate;

    //Log the FX_INTRADAY specs
    //All argument already constructed
    //Construct the function
    APIFunction fx_intraday;
    fx_intraday.funcName = "FX_INTRADAY";
    fx_intraday.args.push_back(function);
    fx_intraday.args.push_back(from_FX);
    fx_intraday.args.push_back(to_FX);
    fx_intraday.args.push_back(interval);
    fx_intraday.args.push_back(output);
    fx_intraday.args.push_back(datatype);
    fx_intraday.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::FX_INTRADAY] = fx_intraday;

    //Log the FX_DAILY specs
    //All argument already constructed
    //Construct the function
    APIFunction fx_daily;
    fx_daily.funcName = "FX_DAILY";
    fx_daily.args.push_back(function);
    fx_daily.args.push_back(from_FX);
    fx_daily.args.push_back(to_FX);
    fx_daily.args.push_back(output);
    fx_daily.args.push_back(datatype);
    fx_daily.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::FX_DAILY] = fx_daily;

    //Log the FX_WEEKLY specs
    //All argument already constructed
    //Construct the function
    APIFunction fx_weekly;
    fx_weekly.funcName = "FX_WEEKLY";
    fx_weekly.args.push_back(function);
    fx_weekly.args.push_back(from_FX);
    fx_weekly.args.push_back(to_FX);
    fx_weekly.args.push_back(datatype);
    fx_weekly.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::FX_WEEKLY] = fx_weekly;

    //Log the FX_MONTHLY specs
    //All argument already constructed
    //Construct the function
    APIFunction fx_monthly;
    fx_monthly.funcName = "FX_MONTHLY";
    fx_monthly.args.push_back(function);
    fx_monthly.args.push_back(from_FX);
    fx_monthly.args.push_back(to_FX);
    fx_monthly.args.push_back(datatype);
    fx_monthly.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::FX_MONTHLY] = fx_monthly;

    //Log the DIGITAL_CURRENCY_DAILY specs
    //Construct the argument for market.
    APIArgument market("market=", &always_true, false);

    //Construct the function
    APIFunction digital_daily;
    digital_daily.funcName = "DIGITAL_CURRENCY_DAILY";
    digital_daily.args.push_back(function);
    digital_daily.args.push_back(symbol);
    digital_daily.args.push_back(market);
    digital_daily.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::DIGITAL_CURRENCY_DAILY] = digital_daily;

    //Log the DIGITAL_CURRENCY_WEEKLY specs
    //Construct the function
    APIFunction digital_weekly;
    digital_weekly.funcName = "DIGITAL_CURRENCY_WEEKLY";
    digital_weekly.args.push_back(function);
    digital_weekly.args.push_back(symbol);
    digital_weekly.args.push_back(market);
    digital_weekly.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::DIGITAL_CURRENCY_WEEKLY] = digital_weekly;

    //Log the DIGITAL_CURRENCY_MONTHLY specs
    //Construct the function
    APIFunction digital_monthly;
    digital_monthly.funcName = "DIGITAL_CURRENCY_MONTHLY";
    digital_monthly.args.push_back(function);
    digital_monthly.args.push_back(symbol);
    digital_monthly.args.push_back(market);
    digital_monthly.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::DIGITAL_CURRENCY_MONTHLY] = digital_monthly;

    //Log the SMA specs
    //Construct the missing argument (i.e., interval, time_period and
    //series_type).
    APIArgument interval_tech("interval=", &validate_tech_interval, false);
    APIArgument time_period("time_period=", &validate_posi_int, false);
    APIArgument series_type("series_type=", &validate_series_type, false);

    //Construct the function
    APIFunction sma;
    sma.funcName = "SMA";
    sma.args.push_back(function);
    sma.args.push_back(symbol);
    sma.args.push_back(interval_tech);
    sma.args.push_back(time_period);
    sma.args.push_back(series_type);
    sma.args.push_back(datatype);
    sma.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::SMA] = sma;

    //Log the EMA specs
    //All arguments already constructed
    //Construct the function
    APIFunction ema;
    ema.funcName = "EMA";
    ema.args.push_back(function);
    ema.args.push_back(symbol);
    ema.args.push_back(interval_tech);
    ema.args.push_back(time_period);
    ema.args.push_back(series_type);
    ema.args.push_back(datatype);
    ema.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::EMA] = ema;

    //Log the WMA specs
    //All arguments already constructed
    //Construct the function
    APIFunction wma;
    wma.funcName = "WMA";
    wma.args.push_back(function);
    wma.args.push_back(symbol);
    wma.args.push_back(interval_tech);
    wma.args.push_back(time_period);
    wma.args.push_back(series_type);
    wma.args.push_back(datatype);
    wma.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::WMA] = wma;

    //Log the DEMA specs
    //All arguments already constructed
    //Construct the function
    APIFunction dema;
    dema.funcName = "DEMA";
    dema.args.push_back(function);
    dema.args.push_back(symbol);
    dema.args.push_back(interval_tech);
    dema.args.push_back(time_period);
    dema.args.push_back(series_type);
    dema.args.push_back(datatype);
    dema.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::DEMA] = dema;

    //Log the TEMA specs
    //All arguments already constructed
    //Construct the function
    APIFunction tema;
    tema.funcName = "TEMA";
    tema.args.push_back(function);
    tema.args.push_back(symbol);
    tema.args.push_back(interval_tech);
    tema.args.push_back(time_period);
    tema.args.push_back(series_type);
    tema.args.push_back(datatype);
    tema.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TEMA] = tema;

    //Log the TRIMA specs
    //All arguments already constructed
    //Construct the function
    APIFunction trima;
    trima.funcName = "TRIMA";
    trima.args.push_back(function);
    trima.args.push_back(symbol);
    trima.args.push_back(interval_tech);
    trima.args.push_back(time_period);
    trima.args.push_back(series_type);
    trima.args.push_back(datatype);
    trima.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TRIMA] = trima;

    //Log the KAMA specs
    //All arguments already constructed
    //Construct the function
    APIFunction kama;
    kama.funcName = "KAMA";
    kama.args.push_back(function);
    kama.args.push_back(symbol);
    kama.args.push_back(interval_tech);
    kama.args.push_back(time_period);
    kama.args.push_back(series_type);
    kama.args.push_back(datatype);
    kama.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::KAMA] = kama;

    //Log the MAMA specs
    //Construct missing arguments (i.e., fastlimit and slowlimit).
    APIArgument fastlimit("fastlimit=", &validate_dbl, true);
    APIArgument slowlimit("slowlimit=", &validate_dbl, true);

    //Costruct the function
    APIFunction mama;
    mama.funcName = "MAMA";
    mama.args.push_back(function);
    mama.args.push_back(symbol);
    mama.args.push_back(interval_tech);
    mama.args.push_back(series_type);
    mama.args.push_back(fastlimit);
    mama.args.push_back(slowlimit);
    mama.args.push_back(datatype);
    mama.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MAMA] = mama;

    //Log the T3 specs
    //All arguments already constructed
    //Construct the function
    APIFunction t3;
    t3.funcName = "T3";
    t3.args.push_back(function);
    t3.args.push_back(symbol);
    t3.args.push_back(interval_tech);
    t3.args.push_back(time_period);
    t3.args.push_back(series_type);
    t3.args.push_back(datatype);
    t3.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::T3] = t3;

    //Log the MACD specs
    //Construct missing arguments(i.e. fastperiod, slowperiod and signal
    //period).
    APIArgument fastperiod("fastperiod=", &validate_posi_int, true);
    APIArgument slowperiod("slowperiod=", &validate_posi_int, true);
    APIArgument signalperiod("signalperiod=", &validate_posi_int, true);

    //Constructthe function
    APIFunction macd;
    macd.funcName = "MACD";
    macd.args.push_back(function);
    macd.args.push_back(symbol);
    macd.args.push_back(interval_tech);
    macd.args.push_back(series_type);
    macd.args.push_back(fastperiod);
    macd.args.push_back(slowperiod);
    macd.args.push_back(signalperiod);
    macd.args.push_back(datatype);
    macd.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MACD] = macd;

    //Log the MACDEXT specs
    //Construct missing arguments(i.e.fastmatype, slowmatype, signalmatype).
    APIArgument fastmatype("fastmatype=", &validate_ma_type, true);
    APIArgument slowmatype("slowmatype=", &validate_ma_type, true);
    APIArgument signalmatype("signalmatype=", &validate_ma_type, true);

    //Constructthe function
    APIFunction macdext;
    macdext.funcName = "MACDEXT";
    macdext.args.push_back(function);
    macdext.args.push_back(symbol);
    macdext.args.push_back(interval_tech);
    macdext.args.push_back(series_type);
    macdext.args.push_back(fastperiod);
    macdext.args.push_back(slowperiod);
    macdext.args.push_back(signalperiod);
    macdext.args.push_back(fastmatype);
    macdext.args.push_back(slowmatype);
    macdext.args.push_back(signalmatype);
    macdext.args.push_back(datatype);
    macdext.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MACDEXT] = macdext;

    //Log specs for STOCH
    //Construct missing arguments (i.e. fastkperiod, slowkperiod, slowdperiod,
    //slowkmatype, slowdmatype)
    APIArgument fastkperiod("fastkperiod=", &validate_posi_int, true);
    APIArgument slowkperiod("slowkperiod=", &validate_posi_int, true);
    APIArgument slowdperiod("slowdperiod=", &validate_posi_int, true);
    APIArgument slowkmatype("slowkmatype=", &validate_ma_type, true);
    APIArgument slowdmatype("slowdmatype=", &validate_ma_type, true);

    //Construct the function
    APIFunction stoch;
    stoch.funcName = "STOCH";
    stoch.args.push_back(function);
    stoch.args.push_back(symbol);
    stoch.args.push_back(interval_tech);
    stoch.args.push_back(fastkperiod);
    stoch.args.push_back(slowkperiod);
    stoch.args.push_back(slowdperiod);
    stoch.args.push_back(slowkmatype);
    stoch.args.push_back(slowdmatype);
    stoch.args.push_back(datatype);
    stoch.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::STOCH] = stoch;

    //Log specs for STOCHF
    //Construct missing arguments (i.e. fastdperiod, fastdmatype).
    APIArgument fastdperiod("fastdperiod=", &validate_posi_int, true);
    APIArgument fastdmatype("fastdmatype=", &validate_ma_type, true);

    //Construct the function
    APIFunction stochf;
    stochf.funcName = "STOCHF";
    stochf.args.push_back(function);
    stochf.args.push_back(symbol);
    stochf.args.push_back(interval_tech);
    stochf.args.push_back(fastkperiod);
    stochf.args.push_back(fastdperiod);
    stochf.args.push_back(fastdmatype);
    stochf.args.push_back(datatype);
    stochf.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::STOCHF] = stochf;

    //Log specs for RSI
    //All arguments already constructed.

    //Construct the function
    APIFunction rsi;
    rsi.funcName = "RSI";
    rsi.args.push_back(function);
    rsi.args.push_back(symbol);
    rsi.args.push_back(interval_tech);
    rsi.args.push_back(time_period);
    rsi.args.push_back(series_type);
    rsi.args.push_back(datatype);
    rsi.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::RSI] = rsi;

    //Log specs for STOCHRSI
    //All arguments already constructed.

    //Construct the function
    APIFunction stochrsi;
    stochrsi.funcName = "STOCHRSI";
    stochrsi.args.push_back(function);
    stochrsi.args.push_back(symbol);
    stochrsi.args.push_back(interval_tech);
    stochrsi.args.push_back(time_period);
    stochrsi.args.push_back(series_type);
    stochrsi.args.push_back(fastkperiod);
    stochrsi.args.push_back(fastdperiod);
    stochrsi.args.push_back(fastdmatype);
    stochrsi.args.push_back(datatype);
    stochrsi.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::STOCHRSI] = stochrsi;

    //Log specs for WILLR
    //All arguments already constructed.

    //Construct the function
    APIFunction willr;
    willr.funcName = "WILLR";
    willr.args.push_back(function);
    willr.args.push_back(symbol);
    willr.args.push_back(interval_tech);
    willr.args.push_back(time_period);
    willr.args.push_back(datatype);
    willr.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::WILLR] = willr;

    //Log specs for ADX
    //All arguments already constructed.

    //Construct the function
    APIFunction adx;
    adx.funcName = "ADX";
    adx.args.push_back(function);
    adx.args.push_back(symbol);
    adx.args.push_back(interval_tech);
    adx.args.push_back(time_period);
    adx.args.push_back(datatype);
    adx.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::ADX] = adx;

    //Log specs for ADXR
    //All arguments already constructed.

    //Construct the function
    APIFunction adxr;
    adxr.funcName = "ADXR";
    adxr.args.push_back(function);
    adxr.args.push_back(symbol);
    adxr.args.push_back(interval_tech);
    adxr.args.push_back(time_period);
    adxr.args.push_back(datatype);
    adxr.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::ADXR] = adxr;

    //Log specs for APO
    //Construct missing argument (i.e., matype).
    APIArgument matype("matype=", &validate_ma_type, true);

    //Construct the function
    APIFunction apo;
    apo.funcName = "APO";
    apo.args.push_back(function);
    apo.args.push_back(symbol);
    apo.args.push_back(interval_tech);
    apo.args.push_back(series_type);
    apo.args.push_back(fastperiod);
    apo.args.push_back(slowperiod);
    apo.args.push_back(matype);
    apo.args.push_back(datatype);
    apo.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::APO] = apo;

    //Log specs for PPO
    //All arguments already constructed.
    //Construct the function
    APIFunction ppo;
    ppo.funcName = "PPO";
    ppo.args.push_back(function);
    ppo.args.push_back(symbol);
    ppo.args.push_back(interval_tech);
    ppo.args.push_back(series_type);
    ppo.args.push_back(fastperiod);
    ppo.args.push_back(slowperiod);
    ppo.args.push_back(matype);
    ppo.args.push_back(datatype);
    ppo.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::PPO] = ppo;

    //Log specs for MOM
    //All arguments already constructed.
    //Construct the function
    APIFunction mom;
    mom.funcName = "MOM";
    mom.args.push_back(function);
    mom.args.push_back(symbol);
    mom.args.push_back(interval_tech);
    mom.args.push_back(time_period);
    mom.args.push_back(series_type);
    mom.args.push_back(datatype);
    mom.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MOM] = mom;

    //Log specs for BOP
    //All arguments already constructed.
    //Construct the function
    APIFunction bop;
    bop.funcName = "BOP";
    bop.args.push_back(function);
    bop.args.push_back(symbol);
    bop.args.push_back(interval_tech);
    bop.args.push_back(datatype);
    bop.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::BOP] = bop;

    //Log specs for CCI
    //All arguments already constructed.
    //Construct the function
    APIFunction cci;
    cci.funcName = "CCI";
    cci.args.push_back(function);
    cci.args.push_back(symbol);
    cci.args.push_back(interval_tech);
    cci.args.push_back(time_period);
    cci.args.push_back(datatype);
    cci.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::CCI] = cci;

    //Log specs for CMO
    //All arguments already constructed.
    //Construct the function
    APIFunction cmo;
    cmo.funcName = "CMO";
    cmo.args.push_back(function);
    cmo.args.push_back(symbol);
    cmo.args.push_back(interval_tech);
    cmo.args.push_back(time_period);
    cmo.args.push_back(series_type);
    cmo.args.push_back(datatype);
    cmo.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::CMO] = cmo;

    //Log specs for ROC
    //All arguments already constructed.
    //Construct the function
    APIFunction roc;
    roc.funcName = "ROC";
    roc.args.push_back(function);
    roc.args.push_back(symbol);
    roc.args.push_back(interval_tech);
    roc.args.push_back(time_period);
    roc.args.push_back(series_type);
    roc.args.push_back(datatype);
    roc.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::ROC] = roc;

    //Log specs for ROCR
    //All arguments already constructed.
    //Construct the function
    APIFunction rocr;
    rocr.funcName = "ROCR";
    rocr.args.push_back(function);
    rocr.args.push_back(symbol);
    rocr.args.push_back(interval_tech);
    rocr.args.push_back(time_period);
    rocr.args.push_back(series_type);
    rocr.args.push_back(datatype);
    rocr.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::ROCR] = rocr;

    //Log specs for AROON
    //All arguments already constructed.
    //Construct the function
    APIFunction aroon;
    aroon.funcName = "AROON";
    aroon.args.push_back(function);
    aroon.args.push_back(symbol);
    aroon.args.push_back(interval_tech);
    aroon.args.push_back(time_period);
    aroon.args.push_back(datatype);
    aroon.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::AROON] = aroon;

    //Log specs for AROONOSC
    //All arguments already constructed.
    //Construct the function
    APIFunction aroonosc;
    aroonosc.funcName = "AROONOSC";
    aroonosc.args.push_back(function);
    aroonosc.args.push_back(symbol);
    aroonosc.args.push_back(interval_tech);
    aroonosc.args.push_back(time_period);
    aroonosc.args.push_back(datatype);
    aroonosc.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::AROONOSC] = aroonosc;

    //Log specs for MFI
    //All arguments already constructed.
    //Construct the function
    APIFunction mfi;
    mfi.funcName = "MFI";
    mfi.args.push_back(function);
    mfi.args.push_back(symbol);
    mfi.args.push_back(interval_tech);
    mfi.args.push_back(time_period);
    mfi.args.push_back(datatype);
    mfi.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MFI] = mfi;

    //Log specs for TRIX
    //All arguments already constructed.
    //Construct the function
    APIFunction trix;
    trix.funcName = "TRIX";
    trix.args.push_back(function);
    trix.args.push_back(symbol);
    trix.args.push_back(interval_tech);
    trix.args.push_back(time_period);
    trix.args.push_back(series_type);
    trix.args.push_back(datatype);
    trix.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TRIX] = trix;

    //Log specs for ULTOSC
    //Construct missing argument (i.e. timeperiod1, timeperiod2 and
    //timeperiod3).
    APIArgument timeperiod1("timeperiod1=", &validate_posi_int, true);
    APIArgument timeperiod2("timeperiod2=", &validate_posi_int, true);
    APIArgument timeperiod3("timeperiod3=", &validate_posi_int, true);

    //Construct the function
    APIFunction ultosc;
    ultosc.funcName = "ULTOSC";
    ultosc.args.push_back(function);
    ultosc.args.push_back(symbol);
    ultosc.args.push_back(interval_tech);
    ultosc.args.push_back(timeperiod1);
    ultosc.args.push_back(timeperiod2);
    ultosc.args.push_back(timeperiod3);
    ultosc.args.push_back(datatype);
    ultosc.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::ULTOSC] = ultosc;

    //Log specs for DX
    //All arguments already constructed.
    //Construct the function
    APIFunction dx;
    dx.funcName = "DX";
    dx.args.push_back(function);
    dx.args.push_back(symbol);
    dx.args.push_back(interval_tech);
    dx.args.push_back(time_period);
    dx.args.push_back(datatype);
    dx.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::DX] = dx;

    //Log specs for MINUS_DI
    //All arguments already constructed.
    //Construct the function
    APIFunction minus_di;
    minus_di.funcName = "MINUS_DI";
    minus_di.args.push_back(function);
    minus_di.args.push_back(symbol);
    minus_di.args.push_back(interval_tech);
    minus_di.args.push_back(time_period);
    minus_di.args.push_back(datatype);
    minus_di.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MINUS_DI] = minus_di;

    //Log specs for PLUS_DI
    //All arguments already constructed.
    //Construct the function
    APIFunction plus_di;
    plus_di.funcName = "PLUS_DI";
    plus_di.args.push_back(function);
    plus_di.args.push_back(symbol);
    plus_di.args.push_back(interval_tech);
    plus_di.args.push_back(time_period);
    plus_di.args.push_back(datatype);
    plus_di.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::PLUS_DI] = plus_di;

    //Log specs for MINUS_DM
    //All arguments already constructed.
    //Construct the function
    APIFunction minus_dm;
    minus_dm.funcName = "MINUS_DM";
    minus_dm.args.push_back(function);
    minus_dm.args.push_back(symbol);
    minus_dm.args.push_back(interval_tech);
    minus_dm.args.push_back(time_period);
    minus_dm.args.push_back(datatype);
    minus_dm.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MINUS_DM] = minus_dm;

    //Log specs for PLUS_DM
    //All arguments already constructed.
    //Construct the function
    APIFunction plus_dm;
    plus_dm.funcName = "PLUS_DM";
    plus_dm.args.push_back(function);
    plus_dm.args.push_back(symbol);
    plus_dm.args.push_back(interval_tech);
    plus_dm.args.push_back(time_period);
    plus_dm.args.push_back(datatype);
    plus_dm.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::PLUS_DM] = plus_dm;

    //Log specs for BBANDS
    //Construct missing arguments (i.e., nbdevup, nbdevdn).
    APIArgument nbdevup("nbdevup=", &validate_posi_int, true);
    APIArgument nbdevdn("nbdevdn=", &validate_posi_int, true);

    //Construct the function
    APIFunction bbands;
    bbands.funcName = "BBANDS";
    bbands.args.push_back(function);
    bbands.args.push_back(symbol);
    bbands.args.push_back(interval_tech);
    bbands.args.push_back(time_period);
    bbands.args.push_back(series_type);
    bbands.args.push_back(nbdevup);
    bbands.args.push_back(nbdevdn);
    bbands.args.push_back(matype);
    bbands.args.push_back(datatype);
    bbands.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::BBANDS] = bbands;

    //Log specs for MIDPOINT
    //All arguments already constructed
    //Construct the function
    APIFunction midpoint;
    midpoint.funcName = "MIDPOINT";
    midpoint.args.push_back(function);
    midpoint.args.push_back(symbol);
    midpoint.args.push_back(interval_tech);
    midpoint.args.push_back(time_period);
    midpoint.args.push_back(series_type);
    midpoint.args.push_back(datatype);
    midpoint.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MIDPOINT] = midpoint;

    //Log specs for MIDPRICE
    //All arguments already constructed
    //Construct the function
    APIFunction midprice;
    midprice.funcName = "MIDPRICE";
    midprice.args.push_back(function);
    midprice.args.push_back(symbol);
    midprice.args.push_back(interval_tech);
    midprice.args.push_back(time_period);
    midprice.args.push_back(datatype);
    midprice.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::MIDPRICE] = midprice;

    //Log specs for SAR
    //Construct missing arguments (i.e, acceleration and maximum).
    APIArgument acceleration("acceleration=", &validate_dbl, true);
    APIArgument maximum("maximum=", &validate_dbl, true);

    //Construct the function
    APIFunction sar;
    sar.funcName = "SAR";
    sar.args.push_back(function);
    sar.args.push_back(symbol);
    sar.args.push_back(interval_tech);
    sar.args.push_back(acceleration);
    sar.args.push_back(maximum);
    sar.args.push_back(datatype);
    sar.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::SAR] = sar;

    //Log specs for TRANGE
    //All arguments already constructed
    //Construct the function
    APIFunction trange;
    trange.funcName = "TRANGE";
    trange.args.push_back(function);
    trange.args.push_back(symbol);
    trange.args.push_back(interval_tech);
    trange.args.push_back(datatype);
    trange.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::TRANGE] = trange;

    //Log specs for ATR
    //All arguments already constructed
    //Construct the function
    APIFunction atr;
    atr.funcName = "ATR";
    atr.args.push_back(function);
    atr.args.push_back(symbol);
    atr.args.push_back(interval_tech);
    atr.args.push_back(time_period);
    atr.args.push_back(datatype);
    atr.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::ATR] = atr;

    //Log specs for NATR
    //All arguments already constructed
    //Construct the function
    APIFunction natr;
    natr.funcName = "NATR";
    natr.args.push_back(function);
    natr.args.push_back(symbol);
    natr.args.push_back(interval_tech);
    natr.args.push_back(time_period);
    natr.args.push_back(datatype);
    natr.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::NATR] = natr;

    //Log specs for AD
    //All arguments already constructed
    //Construct the function
    APIFunction ad;
    ad.funcName = "AD";
    ad.args.push_back(function);
    ad.args.push_back(symbol);
    ad.args.push_back(interval_tech);
    ad.args.push_back(datatype);
    ad.args.push_back(apikey);

    //ADD the function in specs
    api_specs[AVFunctions::AD] = ad;

    //Log specs for ADOSC
    //All arguments alreadoscy constructed
    //Construct the function
    APIFunction adosc;
    adosc.funcName = "ADOSC";
    adosc.args.push_back(function);
    adosc.args.push_back(symbol);
    adosc.args.push_back(interval_tech);
    adosc.args.push_back(fastperiod);
    adosc.args.push_back(slowperiod);
    adosc.args.push_back(datatype);
    adosc.args.push_back(apikey);

    //ADOSCD the function in specs
    api_specs[AVFunctions::ADOSC] = adosc;

    //Log specs for OBV
    //All arguments alreobvy constructed
    //Construct the function
    APIFunction obv;
    obv.funcName = "OBV";
    obv.args.push_back(function);
    obv.args.push_back(symbol);
    obv.args.push_back(interval_tech);
    obv.args.push_back(datatype);
    obv.args.push_back(apikey);

    //OBVD the function in specs
    api_specs[AVFunctions::OBV] = obv;

    //Log specs for HT_TRENDLINE
    //All arguments alreht_trendliney constructed
    //Construct the function
    APIFunction ht_trendline;
    ht_trendline.funcName = "HT_TRENDLINE";
    ht_trendline.args.push_back(function);
    ht_trendline.args.push_back(symbol);
    ht_trendline.args.push_back(interval_tech);
    ht_trendline.args.push_back(series_type);
    ht_trendline.args.push_back(datatype);
    ht_trendline.args.push_back(apikey);

    //HT_TRENDLINED the function in specs
    api_specs[AVFunctions::HT_TRENDLINE] = ht_trendline;

    //Log specs for HT_SINE
    //All arguments alreht_siney constructed
    //Construct the function
    APIFunction ht_sine;
    ht_sine.funcName = "HT_SINE";
    ht_sine.args.push_back(function);
    ht_sine.args.push_back(symbol);
    ht_sine.args.push_back(interval_tech);
    ht_sine.args.push_back(series_type);
    ht_sine.args.push_back(datatype);
    ht_sine.args.push_back(apikey);

    //HT_SINED the function in specs
    api_specs[AVFunctions::HT_SINE] = ht_sine;

    //Log specs for HT_TRENDMODE
    //All arguments alreht_trendmodey constructed
    //Construct the function
    APIFunction ht_trendmode;
    ht_trendmode.funcName = "HT_TRENDMODE";
    ht_trendmode.args.push_back(function);
    ht_trendmode.args.push_back(symbol);
    ht_trendmode.args.push_back(interval_tech);
    ht_trendmode.args.push_back(series_type);
    ht_trendmode.args.push_back(datatype);
    ht_trendmode.args.push_back(apikey);

    //HT_TRENDMODED the function in specs
    api_specs[AVFunctions::HT_TRENDMODE] = ht_trendmode;

    //Log specs for HT_DCPERIOD
    //All arguments alreht_dcperiody constructed
    //Construct the function
    APIFunction ht_dcperiod;
    ht_dcperiod.funcName = "HT_DCPERIOD";
    ht_dcperiod.args.push_back(function);
    ht_dcperiod.args.push_back(symbol);
    ht_dcperiod.args.push_back(interval_tech);
    ht_dcperiod.args.push_back(series_type);
    ht_dcperiod.args.push_back(datatype);
    ht_dcperiod.args.push_back(apikey);

    //HT_DCPERIODD the function in specs
    api_specs[AVFunctions::HT_DCPERIOD] = ht_dcperiod;

    //Log specs for HT_DCPHASE
    //All arguments alreht_dcphasey constructed
    //Construct the function
    APIFunction ht_dcphase;
    ht_dcphase.funcName = "HT_DCPHASE";
    ht_dcphase.args.push_back(function);
    ht_dcphase.args.push_back(symbol);
    ht_dcphase.args.push_back(interval_tech);
    ht_dcphase.args.push_back(series_type);
    ht_dcphase.args.push_back(datatype);
    ht_dcphase.args.push_back(apikey);

    //HT_DCPHASED the function in specs
    api_specs[AVFunctions::HT_DCPHASE] = ht_dcphase;

    //Log specs for HT_PHASOR
    //All arguments alreht_phasory constructed
    //Construct the function
    APIFunction ht_phasor;
    ht_phasor.funcName = "HT_PHASOR";
    ht_phasor.args.push_back(function);
    ht_phasor.args.push_back(symbol);
    ht_phasor.args.push_back(interval_tech);
    ht_phasor.args.push_back(series_type);
    ht_phasor.args.push_back(datatype);
    ht_phasor.args.push_back(apikey);

    //HT_PHASORD the function in specs
    api_specs[AVFunctions::HT_PHASOR] = ht_phasor;

    //Log specs for SECTOR
    //All arguments alreht_phasory constructed
    //Construct the function
    APIFunction sector;
    sector.funcName = "SECTOR";
    sector.args.push_back(function);
    sector.args.push_back(apikey);

    //ADD the function is specs
    api_specs[AVFunctions::SECTOR] = sector;
}

std::string APISpecs::build_url(AVFunctions func,
        std::vector<std::string>& args)
{
    //FETCH the function specifications
    std::unordered_map<AVFunctions, APIFunction>::iterator iter =
        api_specs.find(func);

    //IF the APIFunction doesn't exist throw error
    if(iter == api_specs.end()) throw AVInvalidParameters();
    //Push at the front the string correpsonding to the function argument
    args.insert(args.begin(), iter->second.funcName);
    //IF the number of provided arguments doesn't equal the required ones throw
    //error.
    if(iter->second.args.size() != args.size()) throw AVInvalidParameters();

    //GET iterator for the functions specs and user input
    std::vector<APIArgument>::iterator arg_beginit = iter->second.args.begin();
    std::vector<APIArgument>::iterator arg_endit = iter->second.args.end();
    std::vector<std::string>::iterator input_beginit = args.begin();
    std::string url("https://www.alphavantage.co/query?");

    for(arg_beginit; arg_beginit != arg_endit; arg_beginit++)
    {
        url += arg_beginit->get_URLarg(*input_beginit).append("&");
        input_beginit++;
    }
    url.pop_back();
    return url;
}
void AVConnection::Print_AVFunction(AVFunctions function,
        std::vector<std::string>& args)
{
    args.push_back(AVKey);
    std::string url(specs.build_url(function, args));
    std::string data = executeGETRequest(url);

    std::unique_ptr<JsonObject> NewJson(read_from_str(data));
    NewJson->console_print();

}

PriceHistory AVConnection::fetch_prices(std::string ticker)
{
    std::string url = specs.build_url(AVFunctions::TIME_SERIES_DAILY_ADJUSTED,
                                      ticker, "full", "json", AVKey); 
    std::string rawdata = executeGETRequest(url);
    std::unique_ptr<JsonObject> NewJson(read_from_str(rawdata));
    PriceHistory prices(ticker);
    //CHECK the meta data section of the JSON returned and confirm
    std::vector<std::unique_ptr<JsonPair>>::iterator biter = NewJson->begin();
    std::string temp_str((*biter)->get_name());
    if(temp_str != std::string("Meta Data")) throw AVQueryFailure();
    JsonObject obj_ref;
    (*biter)->get_value(obj_ref);
    std::vector<std::unique_ptr<JsonPair>>::iterator biter2 = obj_ref.begin();
    if((*biter2)->get_name() != std::string("1. Information")) throw
        AVQueryFailure();
    std::string value_str;
    (*biter2)->get_value(value_str);
    if( value_str != std::string("Daily Time Series with Splits and Dividend "
                "Events"))
        throw AVQueryFailure();
    //Iterate through the pairs in the Time Series Section
    biter++;
    JsonObject obj_ref2;
    (*biter)->get_value(obj_ref2);
    biter2 = obj_ref2.begin();
    std::vector<std::unique_ptr<JsonPair>>::iterator eiter = obj_ref2.end();
    for(biter2; biter2 != eiter; biter2++)
    {
        boost::gregorian::date date = boost::gregorian::from_simple_string(
                (*biter2)->get_name());
        JsonObject obj_ref3;
        (*biter2)->get_value(obj_ref3);
        std::vector<std::unique_ptr<JsonPair>>::iterator biter3 =
            obj_ref3.begin();
        double open;
        (*biter3)->get_value(open);
        biter3++;
        double high;
        (*biter3)->get_value(high);
        biter3++;
        double low;
        (*biter3)->get_value(low);
        biter3++;
        double close;
        (*biter3)->get_value(close);
        biter3++;
        double adjclose;
        (*biter3)->get_value(adjclose);
        biter3++;
        double volume;
        (*biter3)->get_value(volume);
        biter3++;
        double divident;
        (*biter3)->get_value(divident);
        biter3++;
        double split;
        (*biter3)->get_value(split);
        prices.insert_dayinfo(date, open, high, low, close, adjclose, volume,
                divident, split);
    }
    return prices;
}
