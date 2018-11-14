#include <boost/program_options.hpp>
#include "AVConnection.hpp"
#include "SQLiteManager.hpp"
#include <string>
#include <vector>
#include <iostream>


int main(int argc, char* args[])
{
    //Construct an options object
    boost::program_options::options_description desc("All options:");
    //Log the options in key, value, description tuples
    desc.add_options()
        ("help,h", "produce help message.")
        ("apikey,k", boost::program_options::value<std::string>(),
         "AlphaVantage ApiKey\nThat is you personal ApiKey\nIf you don't have"
         "one please visit:\nhttps://www.alphavantage.co/")
        ("file,f", boost::program_options::value<std::string>(),
         "database file to store the data.")
        ("tickers,t",
         boost::program_options::value<std::vector<std::string>>(),
         "The symbols for which data are to be downloaded");

    boost::program_options::positional_options_description posop;
    posop.add("tickers", -1);

    boost::program_options::variables_map vm;
    boost::program_options::store(
            boost::program_options::command_line_parser(argc,
                args).options(desc).positional(posop).run(), vm);
    boost::program_options::notify(vm);

    if(vm.count("help"))
    {
        std::cout << desc <<std::endl;
        return 1;
    }

    if(vm.count("apikey") == 0)
    {
        std::cout << "Alphavantage API key is required.\nPlease claim your "
            "personal API key at:\nwww.alphavantage.co\n";
        return 1;
    }
    else if(vm.count("apikey") >1)
    {
        std::cout << "AlphaVantage API key is required\nPlease claim your "
            "personal API Key at:\nwww.alphavantage.co";
        return 1;
    }

    if(vm.count("file") == 0)
    {
        std::cout << "Please specify the file where the data should be stored."
            << std::endl;
        return 1;
    }
    else if(vm.count("file") == 1)
    {
        boost::filesystem::path file(vm["file"].as<std::string>());
        if(boost::filesystem::exists(file.parent_path()) == false) 
        {
            std::cout << "Invalid parent directory. Please provide a valid "
                "file path\n";
            return 1;
        }
    }
    else if (vm.count("file") > 1)
    {
        std::cout << "No file was specified. Please use the -f flag to give "
            "the file where the data are to be stored\n";
        return 1;
    }
    
    try
    {
        AVConnection conn(vm["apikey"].as<std::string>());
        SQLiteManager mngr(vm["file"].as<std::string>());
        std::vector<std::string> tickers =
            vm["tickers"].as<std::vector<std::string>>();
        std::vector<std::string>::iterator biter = tickers.begin();
        std::vector<std::string>::iterator eiter = tickers.end();
        for(biter; biter != eiter; biter++)
        {
            //TODO: Write exception handling to manage query frequency
            PriceHistory prices = conn.fetch_prices(*biter);
            mngr.store_PriceHistory(prices);
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "Program execution interrupted because of the following "
            "error:\n" << e.what() << std::endl;
    }

    return 0;
}
