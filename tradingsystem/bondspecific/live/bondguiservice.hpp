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
    virtual void OnMessage(Price<Bond>& data) override;
    
};



/*
 * BONDGUISERVICELISTENER CLASS DECLARATION
 */

class BondGUIServiceListener
: public ServiceListener<Price<Bond>>
{
    
private:
    BondGUIService* service;                // service to which the listener is attached


public:
    // constructor
    BondGUIServiceListener(BondGUIService* _service);
    // if price does not exist already
    virtual void ProcessAdd(Price<Bond>& price) override;
    // not used
    virtual void ProcessRemove(Price<Bond>& price) override{};
    // if price exist
    virtual void ProcessUpdate(Price<Bond>& price) override;
};



/*
 * BONDGUICONNECTOR CLASS DECLARATION
 */

// class that writes files that it received from the GUIService class
class BondGUIConnector
: public FilePublishConnector<Price<Bond>>
{
public:
    // constructor
    BondGUIConnector(const std::string& path);
    // sets how data from connector is processed to file
    virtual string ProcessData(Price<Bond>& data) override;
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
    if (counter < max_count){
        connector->Publish(price);
        counter++;
    }
    else{
        if (wait){
            if ((std::chrono::steady_clock::now() - wait_start) >= throttle){
                connector->ClearFile();
                wait = false;
                wait_start = std::chrono::steady_clock::now();
                counter = 0;
            }
        }
        else {
            if (counter == -1){
                connector->Publish(price);
                counter = 1;
            }
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

void BondGUIServiceListener::ProcessAdd(Price<Bond>& price){
    service->OnMessage(price);
}

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
