#include "AlphaVantage_main.hpp"

int main(int argc, char* argv[])
{
    //Construct an options object
    boost::program_options::options_description desc("All options:");
    //Log the options in key, value, description tuples
    desc.add_options()
        ("help,h", "produce help message.")
        ("apikey,k", boost::program_options::value<std::string>(),
         "AlphaVantage ApiKey\nThat is you personal ApiKey\nIf you don't have"
         "one please visit:\nhttps://www.alphavantage.co/")
        ("function,f", boost::program_options::value<std::string>(), "Alpha"
         "Vantage API function")
        ("args,a", boost::program_options::value<std::vector<std::string>>(),
         "Function arguments");

    boost::program_options::positional_options_description posop;
    posop.add("args", -1);

    boost::program_options::variables_map vm;
    boost::program_options::store(
            boost::program_options::command_line_parser(argc,
                argv).options(desc).positional(posop).run(), vm);
    boost::program_options::notify(vm);

    if(vm.count("help"))
    {
            std::cout << desc << std::endl;
            return 1;
    }

    if(vm.count("apikey") == 0)
    {
        std::cout << "AlphaVantage API key is required\n"
            "Please claim your personal API Key at:\n"
            "www.alphavantage.co";
        return 1;
    }
    else if (vm.count("apikey") > 1)
    {
        std::cout << "Multiple Api keys have been provided.\nPlease provide "
            "only one.\n";
        return 1;
    }

    if(vm.count("function") == 0)
    {
        std::cout <<"No function has been provided.\nPlease specify the "
            "name of the function you want to call\n";
        return 1;
    }
    else if(vm.count("function") >1)
    {
        std::cout << "Multiple function names have been provided.\nPlease "
            "provide only one.\n";
        return 1;
    }
    try
    {
        AVConnection conn(vm["apikey"].as<std::string>());
        if(!conn.is_function(vm["function"].as<std::string>()))
        {
            std::cout << "Error: Wrong function name.\n"
                "Please consult the documentation at www.alphavanate.co for "
                "the list of supported functions.\n";
            return 1;
        }
        std::vector<std::string> 
            arguments(vm["args"].as<std::vector<std::string>>());
        AVFunctions function = conn.lookup_function(
                vm["function"].as<std::string>());
        conn.Print_AVFunction(function, arguments);
    }
    catch(AVInvalidKey& ex)
    {
        std::cout << "Invalid API key: " << ex.what() << std::endl;
        return 1;
    }
    catch(AVInvalidParameters& ex)
    {
        std::cout << "Invalid Parameter: " << ex.what() << std::endl;
        return 1;
    }
    catch(AVQueryFailure& ex)
    {
        std::cout << "Query failure: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
