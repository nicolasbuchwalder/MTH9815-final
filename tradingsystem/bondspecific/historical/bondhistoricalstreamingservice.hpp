//
//  bondhistoricalstreamingservice.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 17.12.22.
//

#ifndef bondhistoricalstreamingservice_hpp
#define bondhistoricalstreamingservice_hpp


#include "historicaldataservice.hpp"

/*
 * BONDHISTORICALSTREAMINGSERVICE CLASS DECLARATION
 */

// class that listens to the streaming service to persist data to file
class BondHistoricalStreamingService
    : public HistoricalDataService<PriceStream<Bond>>
{

private:
    Connector<PriceStream<Bond>>* connector;            // publish connector
    
public:
    // constructor
    BondHistoricalStreamingService(Connector<PriceStream<Bond>>* _connector);
    // persist price stream to file
    virtual void PersistData(string persistKey, PriceStream<Bond>& pricestream) override;
};



/*
 * BONDHISTORICALSTREAMINGSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to streaming service
class BondHistoricalStreamingServiceListener
: public ServiceListener<PriceStream<Bond>>
{
    
private:
    BondHistoricalStreamingService* service;  // service to which the listener is attached
    // not used
    virtual void ProcessRemove(PriceStream<Bond>& pricestream) override{};
    
public:
    // constructor
    BondHistoricalStreamingServiceListener(BondHistoricalStreamingService* _service);
    // send to service for pricestream associated with new bond
    virtual void ProcessAdd(PriceStream<Bond>& pricestream) override;
    // send to service for pricestream associated with existing bond
    virtual void ProcessUpdate(PriceStream<Bond>& pricestream) override;
};



/*
 * BONDHISTORICALSTREAMINGCONNECTOR CLASS DECLARATION
 */

// class that writes streams to streaming.txt
class BondHistoricalStreamingConnector
: public FilePublishConnector<PriceStream<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(PriceStream<Bond>& data) override;
    
public:
    // constructor
    BondHistoricalStreamingConnector(const std::string& path);
};



/*
 * BONDHISTORICALSTREAMINGSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalStreamingService::BondHistoricalStreamingService(Connector<PriceStream<Bond>>* _connector)
: connector(_connector)
{};

// persist price stream to file
void BondHistoricalStreamingService::PersistData(string persistKey, PriceStream<Bond>& pricestream){
    connector->Publish(pricestream);
}



/*
 * BONDHISTORICALSTREAMINGSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondHistoricalStreamingServiceListener::BondHistoricalStreamingServiceListener(BondHistoricalStreamingService* _service)
: service(_service)
{};

// send to service for price stream associated with new bond
void BondHistoricalStreamingServiceListener::ProcessAdd(PriceStream<Bond>& pricestream){
    service->PersistData(pricestream.GetProduct().GetProductId(), pricestream);
}

// send to service for price stream associated with existing bond
void BondHistoricalStreamingServiceListener::ProcessUpdate(PriceStream<Bond>& pricestream){
    service->PersistData(pricestream.GetProduct().GetProductId(), pricestream);
}



/*
 * BONDHISTORICALSTREAMINGCONNECTOR METHODS DEFINITION
 */

// constructor
BondHistoricalStreamingConnector::BondHistoricalStreamingConnector(const std::string& _path)
: FilePublishConnector("BondHistoricalStreamingConnector", _path)
{
    file << "time,product_id,bid_price,bid_visible_quantity,bid_hidden_quantity,offer_price,offer_visible_quantity,offer_hidden_quantity" << std::endl;
}

// sets how data from connector is processed to file
string BondHistoricalStreamingConnector::ProcessData(PriceStream<Bond>& pricestream){
    string bond_id = pricestream.GetProduct().GetProductId();
    PriceStreamOrder bid = pricestream.GetBidOrder();
    PriceStreamOrder offer = pricestream.GetOfferOrder();
    return get_local_time() + "," + bond_id + "," + to_string(bid.GetPrice()) + "," + to_string(bid.GetVisibleQuantity()) + "," + to_string(bid.GetHiddenQuantity()) + "," + to_string(offer.GetPrice()) + "," + to_string(offer.GetVisibleQuantity()) + "," + to_string(offer.GetHiddenQuantity());
}

#endif /* bondhistoricalstreamingservice_hpp */
