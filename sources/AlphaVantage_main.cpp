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

    if(vm.count("apikey"))
    {
        std::cout <<  vm["apikey"].as<std::string>() << std::endl;
    }
    if(vm.count("function"))
    {
        std::cout << "Function: " << vm["function"].as<std::string>() << "\n";
    }

    if(vm.count("args"))
    {
        for(std::vector<std::string>::const_iterator iter =
                vm["args"].as<std::vector<std::string>>().begin();
                iter != vm["args"].as<std::vector<std::string>>().end();
                iter++)
            std::cout << "Argument= " <<  *iter << "\n";
    }
    //Lookup the function specs
    AVConnection conn(vm["apikey"].as<std::string>());
    return 0;
}
