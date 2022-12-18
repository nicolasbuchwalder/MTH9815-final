//
//  bondstreamingservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondstreamingservice_hpp
#define bondstreamingservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "pricingservice.hpp"
#include "streamingservice.hpp"
#include "utility.hpp"

#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>


using namespace std;

/*
 * BONDSTREAMINGSERVICE CLASS DECLARATION
 */

// class that streams prices and distributed it
class BondStreamingService
: public StreamingService<Bond>
{
    
private:
    Connector<PriceStream<Bond>>* connector;            // publish connector

public:
    // constructor
    BondStreamingService(Connector<PriceStream<Bond>>* _connector);
    // sends the stream to next service
    virtual void OnMessage(PriceStream<Bond>& data) override;
    // publish prices
    void PublishPrice(const PriceStream<Bond>& priceStream) override{};
};



/*
 * BONDSTREAMINGSERVICELISTENER CLASS DECLARATION
 */

class BondStreamingServiceListener
: public ServiceListener<AlgoStream<Bond>>
{
    
private:
    BondStreamingService* service;  // service to which the listener is attached
    // not needed
    virtual void ProcessRemove(AlgoStream<Bond>& algostream) override{};
    
public:
    // constructor
    BondStreamingServiceListener(BondStreamingService* _service);
    // send to service for streams associated with new bond
    virtual void ProcessAdd(AlgoStream<Bond>& algostream) override;
    // send to service for streams associated with existing bond
    virtual void ProcessUpdate(AlgoStream<Bond>& algostream) override;
};



/*
 * BONDSTREAMINGCONNECTOR CLASS DECLARATION
 */

// class that publishes executions to socket
class BondStreamingConnector
: public SocketPublishConnector<PriceStream<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(PriceStream<Bond>& pricestream) override;
    
public:
    // constructor
    BondStreamingConnector(const string _raw_address, const int _port_number);

};



/*
 * BONDSTREAMINGSERVICE METHODS DEFINITION
 */

// constuctor
BondStreamingService::BondStreamingService(Connector<PriceStream<Bond>>* _connector)
: connector(_connector)
{};


// sends the stream to next service
void BondStreamingService::OnMessage(PriceStream<Bond>& pricestream){
    AddData(pricestream.GetProduct().GetProductId(), pricestream);
    for (auto& listener : listeners){
        listener->ProcessAdd(pricestream);
    }
    connector->Publish(pricestream);
}



/*
 * BONDSTREAMINGSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondStreamingServiceListener::BondStreamingServiceListener(BondStreamingService* _service)
: service(_service)
{};

// send to service for streams associated with new bond
void BondStreamingServiceListener::ProcessAdd(AlgoStream<Bond>& algostream){
    service->OnMessage(algostream.GetPriceStream());
}

// send to service for streams associated with existing bond
void BondStreamingServiceListener::ProcessUpdate(AlgoStream<Bond>& algostream){
    service->OnMessage(algostream.GetPriceStream());
}



/*
 * BONDSTREAMINGCONNECTOR METHODS DEFINITION
 */

// constructor
BondStreamingConnector::BondStreamingConnector(const string _raw_address, const int _port_number)
: SocketPublishConnector("BondStreamingConnector", _raw_address, _port_number)
{}

// sets how data from connector is processed to file
string BondStreamingConnector::ProcessData(PriceStream<Bond>& pricestream){
    string bond_id = pricestream.GetProduct().GetProductId();
    PriceStreamOrder bid = pricestream.GetBidOrder();
    PriceStreamOrder offer = pricestream.GetOfferOrder();
    return get_local_time() + "," + bond_id + "," + to_string(bid.GetPrice()) + "," + to_string(bid.GetVisibleQuantity()) + "," + to_string(bid.GetHiddenQuantity()) + "," + to_string(offer.GetPrice()) + "," + to_string(offer.GetVisibleQuantity()) + "," + to_string(offer.GetHiddenQuantity());
    
}


#endif /* bondstreamingservice_hpp */
