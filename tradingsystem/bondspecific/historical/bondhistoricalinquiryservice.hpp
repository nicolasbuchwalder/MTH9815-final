//
//  bondhistoricalinquiryservice.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 16.12.22.
//

#ifndef bondhistoricalinquiryservice_hpp
#define bondhistoricalinquiryservice_hpp

#include "historicaldataservice.hpp"

/*
 * BONDHISTORICALINQUIRYSERVICE CLASS DECLARATION
 */

class BondHistoricalInquiryService
    : public HistoricalDataService<Inquiry<Bond>>
{
    
private:
    Connector<Inquiry<Bond>>* connector;            // publish connector
    
    
public:
    // constructor
    BondHistoricalInquiryService(Connector<Inquiry<Bond>>* _connector);
                                 
    virtual void PersistData(string persistKey, Inquiry<Bond>& inquiry) override;
};



/*
 * BONDHISTORICALINQUIRYSERVICELISTENER CLASS DECLARATION
 */

class BondHistoricalInquiryServiceListener
: public ServiceListener<Inquiry<Bond>>
{
    
private:
    BondHistoricalInquiryService* service;  // service to which the listener is attached

public:
    // constructor
    BondHistoricalInquiryServiceListener(BondHistoricalInquiryService* _service);
    // if price does not exist already
    virtual void ProcessAdd(Inquiry<Bond>& price) override;
    // not used
    virtual void ProcessRemove(Inquiry<Bond>& price) override{};
    // if price exist
    virtual void ProcessUpdate(Inquiry<Bond>& price) override;
};



/*
 * BONDHISTORICALINQUIRYCONNECTOR CLASS DECLARATION
 */

// class that writes files that it received from the GUIService class
class BondHistoricalInquiryConnector
: public FilePublishConnector<Inquiry<Bond>>
{
public:
    // constructor
    BondHistoricalInquiryConnector(const std::string& path);
    // sets how data from connector is processed to file
    virtual string ProcessData(Inquiry<Bond>& data) override;
};



/*
 * BONDHISTORICALINQUIRYSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalInquiryService::BondHistoricalInquiryService(Connector<Inquiry<Bond>>* _connector)
: connector(_connector)
{};

void BondHistoricalInquiryService::PersistData(string persistKey, Inquiry<Bond>& inquiry){
    connector->Publish(inquiry);
}



/*
 * BONDHISTORICALINQUIRYSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondHistoricalInquiryServiceListener::BondHistoricalInquiryServiceListener(BondHistoricalInquiryService* _service)
: service(_service)
{};

void BondHistoricalInquiryServiceListener::ProcessAdd(Inquiry<Bond>& inquiry){
    service->PersistData(inquiry.GetInquiryId(), inquiry);
}

void BondHistoricalInquiryServiceListener::ProcessUpdate(Inquiry<Bond>& inquiry){
    service->PersistData(inquiry.GetInquiryId(), inquiry);
}



/*
 * BONDHISTORICALINQUIRYCONNECTOR METHODS DEFINITION
 */

// constructor
BondHistoricalInquiryConnector::BondHistoricalInquiryConnector(const std::string& _path)
: FilePublishConnector("BondHistoricalInquiryConnector", _path)
{
    file << "time,inquiry_id,state,product_id,direction,quantity,price" << std::endl;
}

// sets how data from connector is processed to file
string BondHistoricalInquiryConnector::ProcessData(Inquiry<Bond>& inquiry){
    return get_local_time() + "," + inquiry.GetInquiryId() + "," + inquiry2string(inquiry.GetState()) + "," + inquiry.GetProduct().GetProductId() + "," + side2string(inquiry.GetSide()) + "," +  to_string(inquiry.GetQuantity())  + "," + to_string(inquiry.GetPrice());
}


#endif /* bondhistoricalinquiryservice_hpp */
