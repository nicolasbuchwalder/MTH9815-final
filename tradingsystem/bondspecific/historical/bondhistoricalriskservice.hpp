//
//  bondhistoricalriskservice.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 16.12.22.
//

#ifndef bondhistoricalriskservice_hpp
#define bondhistoricalriskservice_hpp

#include "historicaldataservice.hpp"


/*
 * BONDHISTORICALRISKCONNECTOR CLASS DECLARATION
 */

// class that writes risk to risk.txt
// this connector has two publish functions as it needs to publish both the risk of bond and bucket
class BondHistoricalRiskConnector
: public FilePublishConnector<PV01<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(PV01<Bond>& risk) override;
    // alternative definition of FilePublishConnector::ProcessData for bucketrisk
    virtual string ProcessData(PV01<BucketedSector<Bond>>& risk);
    
public:
    // constructor
    BondHistoricalRiskConnector(const std::string& path);

    // alternative definition of FilePublishConnector::Publish for bucketrisk
    void PublishBucket(PV01<BucketedSector<Bond>>& bucketrisk);

};



/*
 * BONDHISTORICALRISKSERVICE CLASS DECLARATION
 */

// class that listens to the risk service to persist data to file
class BondHistoricalRiskService
    : public HistoricalDataService<PV01<Bond>>
{
    
private:
    // set dervied connector to get PublishBucket method (that does not exist for the base class)
    BondHistoricalRiskConnector* connector;            // publish connector
    
public:
    // constructor
    BondHistoricalRiskService(BondHistoricalRiskConnector* _connector);
    // persist pv01 of bond to file
    virtual void PersistData(string persistKey, PV01<Bond>& risk) override;
    // persist pv01 of bucket to file
    void PersistData(string persistKey, PV01<BucketedSector<Bond>>& bucketrisk);
};



/*
 * BONDHISTORICALRISKSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to risk service
class BondHistoricalRiskServiceListener
: public ServiceListener<PV01<Bond>>
{
    
private:
    BondHistoricalRiskService* service;  // service to which the listener is attached
    // not used
    virtual void ProcessRemove(PV01<Bond>& risk) override{};

public:
    // constructor
    BondHistoricalRiskServiceListener(BondHistoricalRiskService* _service);
    // send to service for risk associated with new bond
    virtual void ProcessAdd(PV01<Bond>& risk) override;
    // send to service if risk associated with new bucket
    void ProcessAdd(PV01<BucketedSector<Bond>>& bucketrisk);
    // send to service for risk associated with existing bond
    virtual void ProcessUpdate(PV01<Bond>& risk) override;
    // send to service for risk associated with existing bucket
    void ProcessUpdate(PV01<BucketedSector<Bond>>& bucketrisk);
};


/*
 * BONDHISTORICALRISKCONNECTOR METHODS DEFINITION
 */

// constructor
BondHistoricalRiskConnector::BondHistoricalRiskConnector(const std::string& _path)
: FilePublishConnector("BondHistoricalRiskConnector", _path)
{
    file << "time,product_id,pv_01,quantity" << std::endl;
}
// sets how data from connector is processed to file
string BondHistoricalRiskConnector::ProcessData(PV01<Bond>& risk){
    return get_local_time() + "," + risk.GetProduct().GetProductId() + "," + to_string(risk.GetPV01()) + "," + to_string(risk.GetQuantity());
}
// alternative definition of FilePublishConnector::ProcessData for bucketrisk
string BondHistoricalRiskConnector::ProcessData(PV01<BucketedSector<Bond>>& risk){
    return get_local_time() + "," + risk.GetProduct().GetName() + "," + to_string(risk.GetPV01()) + "," + to_string(risk.GetQuantity());
}
// alternative definition of FilePublishConnector::Publish for bucketrisk
void BondHistoricalRiskConnector::PublishBucket(PV01<BucketedSector<Bond>>& bucketrisk){
    file << ProcessData(bucketrisk) << endl;
}




/*
 * BONDHISTORICALRISKSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalRiskService::BondHistoricalRiskService(BondHistoricalRiskConnector* _connector)
: connector(_connector)
{};

// persist pv01 of bond to file
void BondHistoricalRiskService::PersistData(string persistKey, PV01<Bond>& risk){
    connector->Publish(risk);
}

// persist pv01 of bucket to file
void BondHistoricalRiskService::PersistData(string persistKey, PV01<BucketedSector<Bond>>& bucketrisk){
    connector->PublishBucket(bucketrisk);
}



/*
 * BONDHISTORICALRISKSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondHistoricalRiskServiceListener::BondHistoricalRiskServiceListener(BondHistoricalRiskService* _service)
: service(_service)
{};

// send to service for risk associated with new bond
void BondHistoricalRiskServiceListener::ProcessAdd(PV01<Bond>& risk){
    service->PersistData(risk.GetProduct().GetProductId(), risk);
}

// send to service if risk associated with new bucket
void BondHistoricalRiskServiceListener::ProcessUpdate(PV01<Bond>& risk){
    service->PersistData(risk.GetProduct().GetProductId(), risk);
}
// send to service for risk associated with existing bond
void BondHistoricalRiskServiceListener::ProcessAdd(PV01<BucketedSector<Bond>>& bucketrisk){
    service->PersistData(bucketrisk.GetProduct().GetName(), bucketrisk);
}

// send to service for risk associated with existing bucket
void BondHistoricalRiskServiceListener::ProcessUpdate(PV01<BucketedSector<Bond>>& bucketrisk){
    service->PersistData(bucketrisk.GetProduct().GetName(), bucketrisk);
}



#endif /* bondhistoricalriskservice_hpp */

