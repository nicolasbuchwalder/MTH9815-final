//
//  bondhistoricalpositionservice.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 16.12.22.
//

#ifndef bondhistoricalpositionservice_hpp
#define bondhistoricalpositionservice_hpp

#include "historicaldataservice.hpp"

/*
 * BONDHISTORICALPOSITIONSERVICE CLASS DECLARATION
 */

class BondHistoricalPositionService
    : public HistoricalDataService<Position<Bond>>
{
    
private:
    Connector<Position<Bond>>* connector;           // publish connector
    vector<string> trading_books;                   // list of all trading books
    
public:
    // constructor
    BondHistoricalPositionService(Connector<Position<Bond>>* _connector, const vector<string>& _trading_books);
                                 
    virtual void PersistData(string persistKey, Position<Bond>& position) override;
};



/*
 * BONDHISTORICALPOSITIONSERVICELISTENER CLASS DECLARATION
 */

class BondHistoricalPositionServiceListener
: public ServiceListener<Position<Bond>>
{
    
private:
    BondHistoricalPositionService* service;  // service to which the listener is attached

public:
    // constructor
    BondHistoricalPositionServiceListener(BondHistoricalPositionService* _service);
    // if price does not exist already
    virtual void ProcessAdd(Position<Bond>& price) override;
    // not used
    virtual void ProcessRemove(Position<Bond>& price) override{};
    // if price exist
    virtual void ProcessUpdate(Position<Bond>& price) override;
};



/*
 * BONDHISTORICALPOSITIONCONNECTOR CLASS DECLARATION
 */

// class that writes files that it received from the GUIService class
class BondHistoricalPositionConnector
: public FilePublishConnector<Position<Bond>>
{
public:
    // constructor
    BondHistoricalPositionConnector(const std::string& path);
    // sets how data from connector is processed to file
    virtual string ProcessData(Position<Bond>& data) override;
};



/*
 * BONDHISTORICALPOSITIONSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalPositionService::BondHistoricalPositionService(Connector<Position<Bond>>* _connector, const vector<string>& _trading_books)
: connector(_connector), trading_books(_trading_books)
{};

void BondHistoricalPositionService::PersistData(string persistKey, Position<Bond>& position){
    connector->Publish(position);
}



/*
 * BONDHISTORICALPOSITIONSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondHistoricalPositionServiceListener::BondHistoricalPositionServiceListener(BondHistoricalPositionService* _service)
: service(_service)
{};

void BondHistoricalPositionServiceListener::ProcessAdd(Position<Bond>& position){
    service->PersistData(position.GetProduct().GetProductId(), position);
}

void BondHistoricalPositionServiceListener::ProcessUpdate(Position<Bond>& position){
    service->PersistData(position.GetProduct().GetProductId(), position);
}



/*
 * BONDHISTORICALPOSITIONCONNECTOR METHODS DEFINITION
 */

// constructor
BondHistoricalPositionConnector::BondHistoricalPositionConnector(const std::string& _path)
: FilePublishConnector("BondHistoricalPositionConnector", _path)
{
    string pos_str = "";
    for (auto book: trading_books){
        pos_str += "," + book + "_size";
    }
    file << "time,product_id" << pos_str << ",total_size" << std::endl;
}

// sets how data from connector is processed to file
string BondHistoricalPositionConnector::ProcessData(Position<Bond>& position){
    string out = get_local_time() + "," + position.GetProduct().GetProductId();
    for (auto book: trading_books){
        try {
            out += "," + to_string(position.GetPosition(book));
        }
        catch (...){
            out += ",0";
        };
        
    }
    out += "," + to_string(position.GetAggregatePosition());
    return out;
}

#endif /* bondhistoricalpositionservice_hpp */
