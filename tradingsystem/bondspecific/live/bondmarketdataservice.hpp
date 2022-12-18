//
//  bondmarketdataservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondmarketdataservice_hpp
#define bondmarketdataservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "marketdataservice.hpp"
#include "tradinguniverse.hpp"
#include "utility.hpp"

#include <unordered_map>


using namespace std;

/*
 BONDMARKETDATASERVICE CLASS DECLARATION
 */

// service that listens to marketdata and processes it
class BondMarketDataService
: public MarketDataService<Bond>
{
    
public:
    // constructor
    BondMarketDataService() = default;
    // sends the order book to next service
    virtual void OnMessage(OrderBook<Bond>& orderbook) override;
    // Get the best bid/offer order
    virtual const BidOffer& GetBestBidOffer(const string &productId) override;
    // Aggregate the order book
    virtual const OrderBook<Bond>& AggregateDepth(const string &productId) override;
};



/*
 BONDMARKETDATACONNECTOR CLASS DECLARATION
 */

// class that reads data from socket
class BondMarketDataConnector
: public SocketReadConnector<OrderBook<Bond>>
{
private:
    // sets how data from connector is processed to service
    virtual OrderBook<Bond> ProcessData(const vector<string>& row) override;
    
public:
    // constructor
    BondMarketDataConnector(BondMarketDataService* _service, const string _raw_address, const int _port_number);
    // sets the onmessage function of the service

};



/*
 BONDMARKETDATASERVICE METHODS DEFINITION
 */

// sends the order book to next service
void BondMarketDataService::OnMessage(OrderBook<Bond>& orderbook){
    string bond_id = orderbook.GetProduct().GetProductId();
    if (!ExistingData(bond_id)){
        AddData(bond_id, orderbook);
        for (auto& listener : listeners){
            listener->ProcessAdd(orderbook);
        }
    }
    else {
        UpdateData(bond_id, orderbook);
        for (auto& listener : listeners){
            listener->ProcessUpdate(orderbook);
        };
    }
}

// Get the best bid/offer order
const BidOffer& BondMarketDataService::GetBestBidOffer(const string &productId){
    OrderBook<Bond> current_book = GetData(productId);
    BidOffer bestbidoffer(current_book.GetBidStack().front(), current_book.GetOfferStack().front());
    return bestbidoffer;
}

// Aggregate the order book
const OrderBook<Bond>& BondMarketDataService::AggregateDepth(const string &productId){
    return GetData(productId);
}



/*
 BONDMARKETDATACONNECTOR METHODS DEFINITION
 */

// constructor
BondMarketDataConnector::BondMarketDataConnector(BondMarketDataService* _service, const string _raw_address, const int _port_number)

: SocketReadConnector("BondMarketDataConnector", _service, _raw_address, _port_number)
{};


// sets how data from connector is processed to service
OrderBook<Bond> BondMarketDataConnector::ProcessData(const vector<string>& row){
    vector<Order> bidStack;
    vector<Order> offerStack;
    // getting each depth of order book
    for (int i = 0; i < 5; i++){
        bidStack.push_back(Order(bond2dec(row[3 * i + 1]), stol(row[3 * i + 3]), PricingSide::BID));
        offerStack.push_back(Order(bond2dec(row[3 * i + 2]), stol(row[3 * i + 3]), PricingSide::OFFER));
    };
     return OrderBook<Bond>(productmap.at(row[0]), bidStack, offerStack);
}

#endif /* bondmarketdataservice_hpp */
