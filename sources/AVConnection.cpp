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

bool validate_time_period(std::string& value)
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

bool validate_limit(std::string& value)
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
    APIArgument time_period("time_period=", &validate_time_period, false);
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
    //Construct missing arguments (i.e., fastlimi and slowlimit).
    APIArgument fastlimit("fastlimit=", &validate_limit, true);
    APIArgument slowlimit("slowlimit=", &validate_limit, true);

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
}
