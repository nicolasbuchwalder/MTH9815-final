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

// class that listens to the position service to persist data to file
class BondHistoricalPositionService
    : public HistoricalDataService<Position<Bond>>
{
    
private:
    Connector<Position<Bond>>* connector;           // publish connector
    vector<string> trading_books;                   // list of all trading books
    
public:
    // constructor
    BondHistoricalPositionService(Connector<Position<Bond>>* _connector, const vector<string>& _trading_books);
    // persist position to file
    virtual void PersistData(string persistKey, Position<Bond>& position) override;
};



/*
 * BONDHISTORICALPOSITIONSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to position service
class BondHistoricalPositionServiceListener
: public ServiceListener<Position<Bond>>
{
    
private:
    BondHistoricalPositionService* service;  // service to which the listener is attached
    // not used
    virtual void ProcessRemove(Position<Bond>& price) override{};
    
public:
    // constructor
    BondHistoricalPositionServiceListener(BondHistoricalPositionService* _service);
    // send to service for position associated with new bond
    virtual void ProcessAdd(Position<Bond>& price) override;
    // send to service for position associated with existing bond
    virtual void ProcessUpdate(Position<Bond>& price) override;
};



/*
 * BONDHISTORICALPOSITIONCONNECTOR CLASS DECLARATION
 */

// class that writes positions to positions.txt
class BondHistoricalPositionConnector
: public FilePublishConnector<Position<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(Position<Bond>& data) override;
    
public:
    // constructor
    BondHistoricalPositionConnector(const std::string& path);
};



/*
 * BONDHISTORICALPOSITIONSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalPositionService::BondHistoricalPositionService(Connector<Position<Bond>>* _connector, const vector<string>& _trading_books)
: connector(_connector), trading_books(_trading_books)
{};

// persist position to file
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

// send to service for position associated with new bond
void BondHistoricalPositionServiceListener::ProcessAdd(Position<Bond>& position){
    service->PersistData(position.GetProduct().GetProductId(), position);
}

// send to service for position associated with existing bond
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
    // add position for every book
    string pos_str = "";
    for (auto book: trading_books){
        pos_str += "," + book + "_size";
    }
    file << "time,product_id" << pos_str << ",total_size" << std::endl;
}

// sets how data from connector is processed to file
string BondHistoricalPositionConnector::ProcessData(Position<Bond>& position){
    string out = get_local_time() + "," + position.GetProduct().GetProductId();
    // iterate over all books
    for (auto book: trading_books){
        try {
            // position related to that book exist
            out += "," + to_string(position.GetPosition(book));
        }
        catch (...){
            // position related to hat book does not exist
            out += ",0";
        };
        
    }
    // aggregate position
    out += "," + to_string(position.GetAggregatePosition());
    return out;
}

#endif /* bondhistoricalpositionservice_hpp */
