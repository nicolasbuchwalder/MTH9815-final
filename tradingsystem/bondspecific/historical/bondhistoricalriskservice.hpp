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

// class that writes files that it received from the GUIService class
class BondHistoricalRiskConnector
: public FilePublishConnector<PV01<Bond>>
{
public:
    // constructor
    BondHistoricalRiskConnector(const std::string& path);
    // alternative definition of FilePublishConnector::Publish for bucketrisk
    void PublishBucket(PV01<BucketedSector<Bond>>& bucketrisk);
    // sets how data from connector is processed to file
    virtual string ProcessData(PV01<Bond>& risk) override;
    string ProcessData(PV01<BucketedSector<Bond>>& bucketrisk);
};



/*
 * BONDHISTORICALRISKSERVICE CLASS DECLARATION
 */

class BondHistoricalRiskService
    : public HistoricalDataService<PV01<Bond>>
{
    
private:
    BondHistoricalRiskConnector* connector;            // publish connector
    
    
public:
    // constructor
    BondHistoricalRiskService(BondHistoricalRiskConnector* _connector);
                                 
    virtual void PersistData(string persistKey, PV01<Bond>& risk) override;
    
    void PersistData(string persistKey, PV01<BucketedSector<Bond>>& bucketrisk);
    
};



/*
 * BONDHISTORICALRISKSERVICELISTENER CLASS DECLARATION
 */

class BondHistoricalRiskServiceListener
: public ServiceListener<PV01<Bond>>
{
    
private:
    BondHistoricalRiskService* service;  // service to which the listener is attached
    

public:
    // constructor
    BondHistoricalRiskServiceListener(BondHistoricalRiskService* _service);
    // if price does not exist already
    virtual void ProcessAdd(PV01<Bond>& risk) override;
    void ProcessAdd(PV01<BucketedSector<Bond>>& bucketrisk);
    // not used
    virtual void ProcessRemove(PV01<Bond>& risk) override{};
    // if price exist
    virtual void ProcessUpdate(PV01<Bond>& risk) override;
    void ProcessUpdate(PV01<BucketedSector<Bond>>& bucketrisk);
};



/*
 * BONDHISTORICALRISKSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalRiskService::BondHistoricalRiskService(BondHistoricalRiskConnector* _connector)
: connector(_connector)
{};

void BondHistoricalRiskService::PersistData(string persistKey, PV01<Bond>& risk){
    connector->Publish(risk);
}

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

void BondHistoricalRiskServiceListener::ProcessAdd(PV01<Bond>& risk){
    service->PersistData(risk.GetProduct().GetProductId(), risk);
}

void BondHistoricalRiskServiceListener::ProcessUpdate(PV01<Bond>& risk){
    service->PersistData(risk.GetProduct().GetProductId(), risk);
}

void BondHistoricalRiskServiceListener::ProcessAdd(PV01<BucketedSector<Bond>>& bucketrisk){
    service->PersistData(bucketrisk.GetProduct().GetName(), bucketrisk);
}

void BondHistoricalRiskServiceListener::ProcessUpdate(PV01<BucketedSector<Bond>>& bucketrisk){
    service->PersistData(bucketrisk.GetProduct().GetName(), bucketrisk);
}




/*
 * BONDHISTORICALRISKCONNECTOR METHODS DEFINITION
 */

// constructor
BondHistoricalRiskConnector::BondHistoricalRiskConnector(const std::string& _path)
: FilePublishConnector("BondHistoricalRiskConnector", _path)
{
    file << "time,product_id,pv_01,quantity" << std::endl;
}
// alternative definition of FilePublishConnector::Publish for bucketrisk
void BondHistoricalRiskConnector::PublishBucket(PV01<BucketedSector<Bond>>& bucketrisk){
    file << ProcessData(bucketrisk) << endl;
}

// sets how data from connector is processed to file
string BondHistoricalRiskConnector::ProcessData(PV01<Bond>& risk){
    return get_local_time() + "," + risk.GetProduct().GetProductId() + "," + to_string(risk.GetPV01()) + "," + to_string(risk.GetQuantity());
}

string BondHistoricalRiskConnector::ProcessData(PV01<BucketedSector<Bond>>& risk){
    return get_local_time() + "," + risk.GetProduct().GetName() + "," + to_string(risk.GetPV01()) + "," + to_string(risk.GetQuantity());
}

#endif /* bondhistoricalriskservice_hpp */

