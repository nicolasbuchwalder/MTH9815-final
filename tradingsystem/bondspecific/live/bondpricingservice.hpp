//
//  bondpricingservice.hpp
//  MTH9815 final
//
//  This file defines the BondGUIService, which gets the prices from a socket and is then listened by the streaming and gui services
//  No ServiceListener as it does not listen from any service
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondpricingservice_hpp
#define bondpricingservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "pricingservice.hpp"
#include "tradinguniverse.hpp"
#include "utility.hpp"

#include <unordered_map>


using namespace std;

/*
 BONDPRICINGSERVICE CLASS DECLARATION
 */

// service that listens to prices and processes it
class BondPricingService
: public PricingService<Bond>
{
    
public:
    // constructor
    BondPricingService() = default;
    // sends the price of the bond to listeners
    virtual void OnMessage(Price<Bond>& price) override;
};



/*
 BONDPRICINGCONNECTOR CLASS DECLARATION
 */

// class that reads data from socket
class BondPricingConnector
: public SocketReadConnector<Price<Bond>>
{
private:
    // sets how data from connector is processed to service
    virtual Price<Bond> ProcessData(const vector<string>& row) override;
public:
    // constructor
    BondPricingConnector(BondPricingService* _service, const string _raw_address, const int _port_number);
};



/*
 BONDPRICINGSERVICE METHODS DEFINITION
 */

// sends the price of the bond to listeners
void BondPricingService::OnMessage(Price<Bond>& price){
    string bond_id = price.GetProduct().GetProductId();
    if (!ExistingData(bond_id)){
        AddData(bond_id, price);
        for (auto& listener : listeners){
            listener->ProcessAdd(price);
        }
    }
    else {
        UpdateData(bond_id, price);
        for (auto& listener : listeners){
            listener->ProcessUpdate(price);
        };
    }
}



/*
 BONDPRICINGCONNECTOR METHODS DEFINITION
 */

// constructor
BondPricingConnector::BondPricingConnector(BondPricingService* _service, const string _raw_address, const int _port_number)

: SocketReadConnector("BondPricingConnector", _service, _raw_address, _port_number)
{};

// sets how data from connector is processed to service
 Price<Bond> BondPricingConnector::ProcessData(const vector<string>& row){
     return Price<Bond>(productmap.at(row[0]), bond2dec(row[1]), bond2dec(row[2]));
}

#endif /* bondpricingservice_hpp */
