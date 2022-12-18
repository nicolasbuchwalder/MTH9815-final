//
//  bondguiservice.hpp
//  MTH9815 final
//
//  This file defines the BondGUIService, which listens to the BondPricingService and writes to gui.txt every throttle miliseconds
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondguiservice_hpp
#define bondguiservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "pricingservice.hpp"
#include "utility.hpp"

#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <thread>


using namespace std;

/*
 * BONDGUISERVICE CLASS DECLARATION
 */

// class that listens to the prices and post it to the GUI
class BondGUIService
: public Service<string, Price<Bond>>
{
    
private:
    FilePublishConnector<Price<Bond>>* connector;           // publish connector
    std::chrono::seconds throttle;                          // throttle
    int max_count;                                          // maximum number of lines
    int counter;                                            // current number of lines
    std::chrono::steady_clock::time_point wait_start;       // start of throttle
    bool wait;                                              // flag to say that GUI is in waiting mode
    
    
    
public:
    // constructor
    BondGUIService(FilePublishConnector<Price<Bond>>* _connector, int _throttle, int _max_count);
    // sends the price of the bond to connector
    virtual void OnMessage(Price<Bond>& price) override;
    
};



/*
 * BONDGUISERVICELISTENER CLASS DECLARATION
 */

class BondGUIServiceListener
: public ServiceListener<Price<Bond>>
{
    
private:
    BondGUIService* service;                // service to which the listener is attached
    // not used
    virtual void ProcessRemove(Price<Bond>& price) override{};
    
public:
    // constructor
    BondGUIServiceListener(BondGUIService* _service);
    // send to service for price associated with new bond
    virtual void ProcessAdd(Price<Bond>& price) override;
    // send to service for price associated with existing bond
    virtual void ProcessUpdate(Price<Bond>& price) override;
};



/*
 * BONDGUICONNECTOR CLASS DECLARATION
 */

// class that writes files that it received from the GUIService class
class BondGUIConnector
: public FilePublishConnector<Price<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(Price<Bond>& data) override;
    
public:
    // constructor
    BondGUIConnector(const std::string& path);
};



/*
 * BONDGUISERVICE METHODS DEFINITION
 */

// constuctor
BondGUIService::BondGUIService(FilePublishConnector<Price<Bond>>* _connector, int _throttle, int _max_count)
: connector(_connector), throttle(_throttle), max_count(_max_count), wait(false), counter(0)
{
};

// sends the price of the bond to listeners
void BondGUIService::OnMessage(Price<Bond>& price){
    AddData(price.GetProduct().GetProductId(), price);
    // constructing new lines for gui until max lines
    if (counter < max_count){
        connector->Publish(price);
        counter++;
    }
    else{
        // if already waiting, checking if wait is not over
        if (wait){
            if ((std::chrono::steady_clock::now() - wait_start) >= throttle){
                // clearing file and resetting everything
                connector->ClearFile();
                wait = false;
                wait_start = std::chrono::steady_clock::now();
                counter = 0;
            }
        }
        else {
            // if connected just initialized now, begin loop
            if (counter == -1){
                connector->Publish(price);
                counter = 1;
            }
            // if gui got to max lines and needs to start to wait
            else {
                wait = true;
            }
        }
    }
}



/*
 * BONDGUISERVICELISTENER METHODS DEFINITION
 */

// constructor
BondGUIServiceListener::BondGUIServiceListener(BondGUIService* _service)
: service(_service)
{}

// send to service for price associated with new bond
void BondGUIServiceListener::ProcessAdd(Price<Bond>& price){
    service->OnMessage(price);
}

// send to service for price associated with existing bond
void BondGUIServiceListener::ProcessUpdate(Price<Bond>& price){
    service->OnMessage(price);
}



/*
 * BONDGUICONNECTOR METHODS DEFINITION
 */

// constructor
BondGUIConnector::BondGUIConnector(const std::string& _path)
: FilePublishConnector("BondGUIConnector", _path)
{
    file << "time,CUSIN,mid_price,spread" << std::endl;
}

// sets how data from connector is processed to file
string BondGUIConnector::ProcessData(Price<Bond>& price){
    // getting the time, the CUSIP and both mid and spread
    return get_local_time() + "," + price.GetProduct().GetProductId() + "," + dec2bond(price.GetMid()) + "," + dec2bond(price.GetBidOfferSpread());
}


#endif /* bondguiservice_hpp */
