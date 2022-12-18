//
//  utility.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef utility_hpp
#define utility_hpp

#include "tradebookingservice.hpp"
#include "inquiryservice.hpp"
#include "marketdataservice.hpp"

#include "boost/algorithm/string.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;

// convert a decimal number to bond notation number
string dec2bond(double price){
    
    
    int rounded = floor(price);
    int xy = floor((price - rounded) * 32);
    int z = (price - rounded - xy / 32.0) * 256;
    
    string rounded_str = to_string(rounded);
    
    string xy_str;
    if (xy<10) xy_str = "0";
    xy_str += to_string(xy);
    
    string z_str;
    if (z!=4) z_str = to_string(z);
    else z_str = "+";
    
    return  rounded_str + "-" + xy_str + z_str;
}

// convert a bond notation number to a decimal number
double bond2dec(string price_str){
    
    vector<string> two_parts;
    boost::algorithm::split(two_parts, price_str, boost::algorithm::is_any_of("-"));
    
    double res = stod(two_parts[0]);
    res += stod(two_parts[1].substr(0,2)) / 32.;
    if (two_parts[1].substr(2,1)=="+") res += 4. / 256.;
    else res += stod(two_parts[1].substr(2,1)) / 256.;
    
    return res;
    
}

// convert string to side enum
Side string2side(string side){
    if (side == "BUY") return Side::BUY;
    else return Side::SELL;
}

// convert side enum to string
string side2string(Side side){
    if (side == Side::BUY) return string("BUY");
    else return string("SELL");
}

// convert string to pricingside enum
PricingSide string2pricingside(string side){
    if (side == "BID") return PricingSide::BID;
    else return PricingSide::OFFER;
}

// convert pricingside enum to string
string pricingside2string(PricingSide side){
    if (side == PricingSide::BID) return string("BID");
    else return string("OFFER");
}

// convert string to inquiry enum
InquiryState string2inquiry(string inquiry){
    if (inquiry=="RECEIVED") return InquiryState::RECEIVED;
    if (inquiry=="QUOTED") return InquiryState::QUOTED;
    if (inquiry=="DONE") return InquiryState::DONE;
    if (inquiry=="REJECTED") return InquiryState::REJECTED;
    if (inquiry=="CUSTOMER_REJECTED") return InquiryState::CUSTOMER_REJECTED;
    return InquiryState::RECEIVED;
        
}

// convert inquiry enum to string
string inquiry2string(InquiryState inquiry){
    switch(inquiry){
        case InquiryState::RECEIVED: return string("RECEIVED");
        case InquiryState::QUOTED: return string("QUOTED");
        case InquiryState::DONE: return string("DONE");
        case InquiryState::REJECTED: return string("REJECTED");
        case InquiryState::CUSTOMER_REJECTED: return string("CUSTOMER_REJECTED");
        default: return "NA";
    }
}


// get current time as string
std::string get_local_time(){
    // get string
    string time_string = boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time());
    // remove micro seconds
    return time_string.substr(0, time_string.length()-3);
}

#endif /* utility_hpp */
