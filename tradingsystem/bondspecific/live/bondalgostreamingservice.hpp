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

class BondAlgoStreamingService
: public ListenedService<string, AlgoStream<Bond>>
{
    
private:

    long visible_quantity;                                  // visible quantity for algostream
    
    
public:
    // constructor
    BondAlgoStreamingService();
    
    // sends the price of the bond to next service
    virtual void OnMessage(AlgoStream<Bond>& data) override;
    
    void CreateStream(Price<Bond>& price);
    
    void UpdateStream(Price<Bond>& price);
    //
    long GetQuantity();
    
};



/*
 * BONDALGOSTREAMINGSERVICELISTENER CLASS DECLARATION
 */

class BondAlgoStreamingServiceListener
: public ServiceListener<Price<Bond>>
{
    
private:
    BondAlgoStreamingService* service;          // service attached to connector

public:
    // constructor
    BondAlgoStreamingServiceListener(BondAlgoStreamingService* service);
    // only needed, we want to get the key
    virtual void ProcessAdd(Price<Bond>& price) override;
    // not used
    virtual void ProcessRemove(Price<Bond>& price) override{};
    // not used
    virtual void ProcessUpdate(Price<Bond>& price) override;
};



/*
 * BONDALGOSTREAMINGSERVICE METHODS DEFINITION
 */

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


void BondAlgoStreamingService::CreateStream(Price<Bond>& price){
    double mid_price = price.GetMid();
    double spread = price.GetBidOfferSpread();
    long quantity = GetQuantity();
    PriceStreamOrder bidorder(mid_price - spread / 2., quantity, quantity * 2., PricingSide::BID);
    PriceStreamOrder offerorder(mid_price + spread / 2., quantity, quantity * 2., PricingSide::OFFER);
    AlgoStream<Bond> algostream(PriceStream(price.GetProduct(), bidorder, offerorder));

    OnMessage(algostream);
}

void BondAlgoStreamingService::UpdateStream(Price<Bond>& price){
    double mid_price = price.GetMid();
    double spread = price.GetBidOfferSpread();
    long quantity = GetQuantity();
    PriceStreamOrder bidorder(mid_price - spread / 2., quantity, quantity * 2., PricingSide::BID);
    PriceStreamOrder offerorder(mid_price + spread / 2., quantity, quantity * 2., PricingSide::OFFER);
    GetData(price.GetProduct().GetProductId()).UpdatePriceStream(bidorder, offerorder);
    OnMessage(GetData(price.GetProduct().GetProductId()));
}

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

void BondAlgoStreamingServiceListener::ProcessAdd(Price<Bond>& price){
    service->CreateStream(price);
}

void BondAlgoStreamingServiceListener::ProcessUpdate(Price<Bond>& price){
    service->UpdateStream(price);
}




#endif /* bondalgostreamingservice_hpp */
