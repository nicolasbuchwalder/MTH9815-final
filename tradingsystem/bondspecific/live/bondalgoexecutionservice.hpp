//
//  bondalgoexecutionservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondalgoexecutionservice_hpp
#define bondalgoexecutionservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "algoexecution.hpp"
#include "marketdataservice.hpp"
#include "tradebookingservice.hpp"
#include "utility.hpp"

#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <thread>


using namespace std;

/*
 * BONDALGOEXECUTIONSERVICE CLASS DECLARATION
 */

// class that signals orders algorithmically based on marketdata
class BondAlgoExecutionService
: public ListenedService<string, AlgoExecution<Bond>>
{
    
private:
    PricingSide side;                                           // pricingside
    
public:
    // constructor
    BondAlgoExecutionService();
    // sends the algo execution to next service
    virtual void OnMessage(AlgoExecution<Bond>& algoexec) override;
    // algorithm of execution, which consists of aggressing the book
    void AggressBook(OrderBook<Bond>& orderbook);
    // return alternative pricing side
    PricingSide GetCurrentSide();
    // generate alphanumeric random id
    string GenOrderId();
    
};



/*
 * BONDALGOEXECUTIONSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to algoexecution service
class BondAlgoExecutionServiceListener
: public ServiceListener<OrderBook<Bond>>
{
    
private:
    BondAlgoExecutionService* service;          // service attached to connector
    // not used
    virtual void ProcessRemove(OrderBook<Bond>& orderbook) override{};
    
public:
    // constructor
    BondAlgoExecutionServiceListener(BondAlgoExecutionService* service);
    // send to service for orderbook associated with new bond
    virtual void ProcessAdd(OrderBook<Bond>& orderbook) override;
    // send to service for orderbook associated with existing bond
    virtual void ProcessUpdate(OrderBook<Bond>& orderbook) override;
};



/*
 * BONDALGOEXECUTIONSERVICE METHODS DEFINITION
 */

// constructor
BondAlgoExecutionService::BondAlgoExecutionService()
    : side(PricingSide::BID)
{};

// sends the algo execution of the bond to listeners
void BondAlgoExecutionService::OnMessage(AlgoExecution<Bond>& algoexec){
    string trade_id = algoexec.GetExecutionOrder().GetOrderId();
    AddData(trade_id, algoexec);
    for (auto& listener : listeners){
        listener->ProcessAdd(algoexec);
    }
};
// algorithm of execution, which consists of aggressing the book
void BondAlgoExecutionService::AggressBook(OrderBook<Bond>& orderbook){
    // checking if spread tight enough
    if (orderbook.GetTopSpread() <= 1. / 128.){
        // attributing side
        PricingSide side = GetCurrentSide();
        // generating order
        ExecutionOrder new_order(orderbook.GetProduct(), side, GenOrderId(), OrderType::IOC, orderbook.GetTopAgainstPrice(side), orderbook.GetTopAgainstQuantity(side), 0, "N/A", false);
        AlgoExecution<Bond> algoexec(new_order);
        OnMessage(algoexec);
    }
    
}

// return alternative pricing side
PricingSide BondAlgoExecutionService::GetCurrentSide(){
    side = (side == PricingSide::OFFER) ? PricingSide::BID : PricingSide::OFFER;
    return side;
}

// generate alphanumeric random id
string BondAlgoExecutionService::GenOrderId(){
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(10,0);
    std::generate_n( str.begin(), 10, randchar );
    return str;
    
}

/*
 * BONDALGOEXECUTIONSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondAlgoExecutionServiceListener::BondAlgoExecutionServiceListener(BondAlgoExecutionService* service)
: service(service)
{};

// send to service for orderbook associated with new bond
void BondAlgoExecutionServiceListener::ProcessAdd(OrderBook<Bond>& orderbook){
    service->AggressBook(orderbook);
}
// send to service for orderbook associated with existing bond
void BondAlgoExecutionServiceListener::ProcessUpdate(OrderBook<Bond>& orderbook){
    service->AggressBook(orderbook);
}



#endif /* bondalgoexecutionservice_hpp */
