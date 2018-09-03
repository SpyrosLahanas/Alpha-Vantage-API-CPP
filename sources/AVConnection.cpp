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

AVConnection::AVConnection(std::string UserKey) : AVKey(UserKey)
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

void AVConnection::Print_AVFunctions(AVFunctions series,
                                         std::string ticker, outputsize size)
    /*Description: Prints on the terminal the data Alpha Vantage returns for
    the query parameters provided. These parameters era:
    AVFunctions series ---> the API function
    std::string ticker     ---> the global equity identifier
    outputsize size        ---> either compact or full
    */
{
    //Prepare the function clause of the query
    std::string function;
    if(series == AVFunctions::TIME_SERIES_DAILY)
    {
       function = "function=TIME_SERIES_DAILY"; 
    } else if(series ==  AVFunctions::TIME_SERIES_DAILY_ADJUSTED)
    {
       function = "function=TIME_SERIES_DAILY_ADJUSTED"; 
    } else if(series == AVFunctions::TIME_SERIES_WEEKLY)
    {
       function = "function=TIME_SERIES_WEEKLY"; 
    } else if(series == AVFunctions::TIME_SERIES_WEEKLY_ADJUSTED)
    {
       function = "function=TIME_SERIES_WEEKLY_ADJUSTED"; 
    } else if(series == AVFunctions::TIME_SERIES_MONTHLY)
    {
       function = "function=TIME_SERIES_MONTHLY"; 
    } else if(series == AVFunctions::TIME_SERIES_MONTHLY_ADJUSTED)
    {
       function = "function=TIME_SERIES_MONTHLY_ADJUSTED"; 
    } else if(series == AVFunctions::GLOBAL_QUOTE)
    {
        function = "function=GLOBAL_QUOTE";    
    } else {
        throw AVInvalidParameters();
    }

    //Prepare the outputsize clause of the query, if relevant
    std::string outputsize(""); 
    if(series == AVFunctions::TIME_SERIES_DAILY ||
                 series == AVFunctions::TIME_SERIES_DAILY_ADJUSTED)
    {
        outputsize =+ "&outputsize=" + outputsize
            += (size == outputsize::compact ? "compact" : "full");
    }
    
    //Prepare the URL
    std::string url =+ "https://www.alphavantage.co/query?" + function
    + "&symbol=" + ticker + outputsize + "&apikey=" + AVKey;
    
    //Execute the request
    std::string datareturned = executeGETRequest(url);
    
    //Print to the console
    std::unique_ptr<JsonObject> NewJSON(read_from_str(datareturned));
    NewJSON->console_print();
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

APIArgument::APIArgument() : urlComponent() {}

APIArgument::APIArgument(APIArgument&& other) :
    urlComponent(std::move(other.urlComponent))
{
    validate = other.validate;
}

APIArgument::APIArgument(std::string urlComponent,
        bool (*func_ptr)(std::string& value)) : urlComponent(urlComponent)
{
    validate = func_ptr;
}

APIArgument::APIArgument(const APIArgument& other) :
    urlComponent(other.urlComponent)
{
    validate = other.validate;
}

APIArgument& APIArgument::operator=(const APIArgument& other)
{
    urlComponent = other.urlComponent;
    validate = other.validate;
    return *this;
}

std::string APIArgument::get_URLarg(std::string& input)
{
    if(!validate(input)) throw AVInvalidParameters();

    return urlComponent + input;
}

APIFunction::APIFunction() : args() {};

APIFunction::APIFunction(const APIFunction& other) : args(other.args) {}

APIFunction& APIFunction::operator=(const APIFunction& other)
{
    args = other.args;
    return *this;
}