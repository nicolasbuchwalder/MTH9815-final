//
//  tradinguniverse.hpp
//  MTH9815 final
//
//
//  RATES AND PV01 COME FROM THE FOLLOWING WEBSITE
//  https://eiptrading.com/risk-management/
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef tradinguniverse_hpp
#define tradinguniverse_hpp

#include "products.hpp"
#include "riskservice.hpp"

#include <unordered_map>

using namespace std;

// set here the trading books
vector<string> trading_books = {"TRSY1", "TRSY2", "TRSY3"};

// set here the universe of bonds
Bond yr2("91282CFX4", CUSIP, "T", 0.00375, boost::gregorian::date(2024, 11, 30));
Bond yr3("91282CFW6", CUSIP, "T", 0.00625, boost::gregorian::date(2025, 11, 15));
Bond yr5("91282CFZ9", CUSIP, "T", 0.150, boost::gregorian::date(2027, 11, 30));
Bond yr7("91282CFY2", CUSIP, "T", 0.225, boost::gregorian::date(2029, 11, 30));
Bond yr10("91282CFV8", CUSIP, "T", 0.3125, boost::gregorian::date(2032, 11, 15));
Bond yr20("912810TM0", CUSIP, "T", 0.375, boost::gregorian::date(2042, 11, 30));
Bond yr30("912810TL2", CUSIP, "T", 0.4375, boost::gregorian::date(2052, 11, 15));

// include here are the bonds of the universe
vector<Bond> products = { yr2, yr3, yr5, yr7, yr10, yr20, yr30 };

// include here the map of the product id and product instance
unordered_map<string, Bond> productmap{
    {"91282CFX4", yr2},
    {"91282CFW6", yr3},
    {"91282CFZ9", yr5},
    {"91282CFY2", yr7},
    {"91282CFV8", yr10},
    {"912810TM0", yr20},
    {"912810TL2", yr30},
        
};

// include here the PV01 of each product
unordered_map<string, double> PV01map{
    {"91282CFX4", 0.019851},
    {"91282CFW6", 0.029309},
    {"91282CFZ9", 0.048643},
    {"91282CFY2", 0.065843},
    {"91282CFV8", 0.087939},
    {"912810TM0", 0.140980},
    {"912810TL2", 0.184698},
        
};

// set here the different sectors
BucketedSector<Bond> frontend(vector<Bond>{yr2, yr3}, "frontend");
BucketedSector<Bond> belly(vector<Bond>{yr5, yr7, yr10}, "belly");
BucketedSector<Bond> backend(vector<Bond>{yr20, yr30}, "backend");

// set here the map of the product id and sector instance
unordered_map<string, BucketedSector<Bond>> bond2bucketmap{
    {"91282CFX4", frontend},
    {"91282CFW6", frontend},
    {"91282CFZ9", belly},
    {"91282CFY2", belly},
    {"91282CFV8", belly},
    {"912810TM0", backend},
    {"912810TL2", backend},
};


#endif /* tradinguniverse_hpp */
