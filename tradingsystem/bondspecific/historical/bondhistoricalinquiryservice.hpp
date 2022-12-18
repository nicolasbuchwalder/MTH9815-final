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

// class that listens to the inquiry service to persist data to file
class BondHistoricalInquiryService
    : public HistoricalDataService<Inquiry<Bond>>
{
private:
    Connector<Inquiry<Bond>>* connector;            // publish connector
    
public:
    // constructor
    BondHistoricalInquiryService(Connector<Inquiry<Bond>>* _connector);
    // persist inquiry to file
    virtual void PersistData(string persistKey, Inquiry<Bond>& inquiry) override;
};



/*
 * BONDHISTORICALINQUIRYSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to inquiry service
class BondHistoricalInquiryServiceListener
: public ServiceListener<Inquiry<Bond>>
{
private:
    BondHistoricalInquiryService* service;  // service to which the listener is attached
    // not used
    virtual void ProcessRemove(Inquiry<Bond>& inquiry) override{};
    
public:
    // constructor
    BondHistoricalInquiryServiceListener(BondHistoricalInquiryService* _service);
    // send to service if inquiry just received
    virtual void ProcessAdd(Inquiry<Bond>& inquiry) override;
    // send to service if inquiry already received
    virtual void ProcessUpdate(Inquiry<Bond>& inquiry) override;
};



/*
 * BONDHISTORICALINQUIRYCONNECTOR CLASS DECLARATION
 */

// class that writes inquiries to inquiries.txt
class BondHistoricalInquiryConnector
: public FilePublishConnector<Inquiry<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(Inquiry<Bond>& data) override;
    
public:
    // constructor
    BondHistoricalInquiryConnector(const std::string& path);

};



/*
 * BONDHISTORICALINQUIRYSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalInquiryService::BondHistoricalInquiryService(Connector<Inquiry<Bond>>* _connector)
: connector(_connector)
{};

// persit inquiry to file
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

// send to service if inquiry just received
void BondHistoricalInquiryServiceListener::ProcessAdd(Inquiry<Bond>& inquiry){
    service->PersistData(inquiry.GetInquiryId(), inquiry);
}

// send to service if inquiry already received
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
