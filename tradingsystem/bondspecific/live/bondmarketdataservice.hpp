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


class BondMarketDataService
: public MarketDataService<Bond>
{
    
public:
    // constructor
    BondMarketDataService() = default;
    // sends the price of the bond to listeners
    virtual void OnMessage(OrderBook<Bond>& orderbook) override;
    // Get the best bid/offer order
    virtual const BidOffer& GetBestBidOffer(const string &productId) override;
    // Aggregate the order book
    virtual const OrderBook<Bond>& AggregateDepth(const string &productId) override;
};



/*
 BONDMARKETDATACONNECTOR CLASS DECLARATION
 */


class BondMarketDataConnector
: public SocketReadConnector<OrderBook<Bond>>
{
public:
    // constructor
    BondMarketDataConnector(BondMarketDataService* _service, const string _raw_address, const int _port_number);
    // sets the onmessage function of the service

    virtual OrderBook<Bond> ProcessData(const vector<string>& row) override;
};



/*
 BONDMARKETDATASERVICE METHODS DEFINITION
 */

// sends the price of the bond to listeners
void BondMarketDataService::OnMessage(OrderBook<Bond>& orderbook){
    AddData(orderbook.GetProduct().GetProductId(), orderbook);
    for (auto& listener : listeners){
        listener->ProcessAdd(orderbook);
    }
}

const BidOffer& BondMarketDataService::GetBestBidOffer(const string &productId){
    OrderBook<Bond> current_book = GetData(productId);
    BidOffer bestbidoffer(current_book.GetBidStack().front(), current_book.GetOfferStack().front());
    return bestbidoffer;
}

// NOT FINISHED, what is this?
const OrderBook<Bond>& BondMarketDataService::AggregateDepth(const string &productId){
    return GetData(productId);
}



/*
 BONDMARKETDATACONNECTOR METHODS DEFINITION
 */


BondMarketDataConnector::BondMarketDataConnector(BondMarketDataService* _service, const string _raw_address, const int _port_number)

: SocketReadConnector("BondMarketDataConnector", _service, _raw_address, _port_number)
{};


OrderBook<Bond> BondMarketDataConnector::ProcessData(const vector<string>& row){
    
    vector<Order> bidStack;
    vector<Order> offerStack;
    for (int i = 0; i < 5; i++){
        bidStack.push_back(Order(bond2dec(row[3 * i + 1]), stol(row[3 * i + 3]), PricingSide::BID));
        offerStack.push_back(Order(bond2dec(row[3 * i + 2]), stol(row[3 * i + 3]), PricingSide::OFFER));
    };
     return OrderBook<Bond>(productmap.at(row[0]), bidStack, offerStack);
}

#endif /* bondmarketdataservice_hpp */
