//
//  bondalgostreamingservice.hpp
//  MTH9815 final
//
//
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondalgostreamingservice_hpp
#define bondalgostreamingservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "algostream.hpp"
#include "pricingservice.hpp"
#include "marketdataservice.hpp"
#include "streamingservice.hpp"
#include "utility.hpp"

#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <thread>


using namespace std;

/*
 * BONDALGOSTREAMINGSERVICE CLASS DECLARATION
 */

// class that streams prices and sets quantities
class BondAlgoStreamingService
: public ListenedService<string, AlgoStream<Bond>>
{
    
private:
    long visible_quantity;                                  // visible quantity for algostream
    
public:
    // constructor
    BondAlgoStreamingService();
    
    // sends the algo stream to next service
    virtual void OnMessage(AlgoStream<Bond>& algostream) override;
    // create stream from price associated with new bond
    void CreateStream(Price<Bond>& price);
    // update stream from price with existing bond
    void UpdateStream(Price<Bond>& price);
    // return altenating quantity
    long GetQuantity();
    
};



/*
 * BONDALGOSTREAMINGSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to algostreaming service
class BondAlgoStreamingServiceListener
: public ServiceListener<Price<Bond>>
{
    
private:
    BondAlgoStreamingService* service;          // service attached to connector
    // not used
    virtual void ProcessRemove(Price<Bond>& price) override{};
    
public:
    // constructor
    BondAlgoStreamingServiceListener(BondAlgoStreamingService* service);
    // send to service for price associated with new bond
    virtual void ProcessAdd(Price<Bond>& price) override;
    // send to service for price associated with existing bond
    virtual void ProcessUpdate(Price<Bond>& price) override;
};



/*
 * BONDALGOSTREAMINGSERVICE METHODS DEFINITION
 */
// constructor (first visible quantity will be 1000000 as it will first switch with GetQuantity method
BondAlgoStreamingService::BondAlgoStreamingService()
    : visible_quantity(2000000)
{};


// sends the price of the bond to listeners
void BondAlgoStreamingService::OnMessage(AlgoStream<Bond>& algostream){
    string bond_id = algostream.GetPriceStream().GetProduct().GetProductId();
    if (!ExistingData(bond_id)){
        AddData(bond_id, algostream);
        for (auto& listener : listeners){
            listener->ProcessAdd(algostream);
        }
    }
    else {
        UpdateData(bond_id, algostream);
        for (auto& listener : listeners){
            listener->ProcessUpdate(algostream);
        };
    }
};

// create stream from price associated with new bond
void BondAlgoStreamingService::CreateStream(Price<Bond>& price){
    double mid_price = price.GetMid();
    double spread = price.GetBidOfferSpread();
    // getiing alternating quantity
    long quantity = GetQuantity();
    // creating price streaming orders from get and spread (assuming symmetric)
    PriceStreamOrder bidorder(mid_price - spread / 2., quantity, quantity * 2., PricingSide::BID);
    PriceStreamOrder offerorder(mid_price + spread / 2., quantity, quantity * 2., PricingSide::OFFER);
    // creating new algostream
    AlgoStream<Bond> algostream(PriceStream(price.GetProduct(), bidorder, offerorder));

    OnMessage(algostream);
}

// create stream from price associated with existing bond
void BondAlgoStreamingService::UpdateStream(Price<Bond>& price){
    double mid_price = price.GetMid();
    double spread = price.GetBidOfferSpread();
    // getiing alternating quantity
    long quantity = GetQuantity();
    // creating price streaming orders from get and spread (assuming symmetric)
    PriceStreamOrder bidorder(mid_price - spread / 2., quantity, quantity * 2., PricingSide::BID);
    PriceStreamOrder offerorder(mid_price + spread / 2., quantity, quantity * 2., PricingSide::OFFER);
    // updating algostream
    GetData(price.GetProduct().GetProductId()).UpdatePriceStream(bidorder, offerorder);
    OnMessage(GetData(price.GetProduct().GetProductId()));
}

// return altenating quantity
long BondAlgoStreamingService::GetQuantity(){
    visible_quantity = (visible_quantity == 2000000) ? 1000000 : 2000000;
    return visible_quantity;
}

/*
 * BONDALGOSTREAMINGSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondAlgoStreamingServiceListener::BondAlgoStreamingServiceListener(BondAlgoStreamingService* service)
: service(service)
{};

// send to service for price associated with new bond
void BondAlgoStreamingServiceListener::ProcessAdd(Price<Bond>& price){
    service->CreateStream(price);
}
// send to service for price associated with exsting bond
void BondAlgoStreamingServiceListener::ProcessUpdate(Price<Bond>& price){
    service->UpdateStream(price);
}




#endif /* bondalgostreamingservice_hpp */
