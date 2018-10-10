#include <boost/program_options.hpp>
#include "AVConnection.hpp"
#include "SQLiteManager.hpp"


int main(int argc, char* args[])
{
    //Construct an options object
    //Log the options in key, value, description tuples
    AVConnection conn("L9AUO9JRFTMP694H");
    PriceHistory prices = conn.fetch_prices("IVZ");
    SQLiteManager mngr("/home/spyroslahanas/Documents/Programming/"
            "PortfolioAnalyst/Other/new.db");
    mngr.store_PriceHistory(prices);
    return 0;
}
