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

class BondAlgoExecutionService
: public ListenedService<string, AlgoExecution<Bond>>
{
    
private:
    PricingSide side;                                           // pricingside
    
    
public:
    // constructor
    BondAlgoExecutionService();

    // sends the price of the bond to next service
    virtual void OnMessage(AlgoExecution<Bond>& data) override;

    void AggressBook(OrderBook<Bond>& orderbook);
    
    PricingSide GetCurrentSide();
    
    string GenOrderId();
    
};



/*
 * BONDALGOEXECUTIONSERVICELISTENER CLASS DECLARATION
 */

class BondAlgoExecutionServiceListener
: public ServiceListener<OrderBook<Bond>>
{
    
private:
    BondAlgoExecutionService* service;          // service attached to connector

public:
    // constructor
    BondAlgoExecutionServiceListener(BondAlgoExecutionService* service);
    // only needed, we want to get the key
    virtual void ProcessAdd(OrderBook<Bond>& orderbook) override;
    // not used
    virtual void ProcessRemove(OrderBook<Bond>& orderbook) override{};
    // not used
    virtual void ProcessUpdate(OrderBook<Bond>& orderbook) override{};
};



/*
 * BONDALGOEXECUTIONSERVICE METHODS DEFINITION
 */

BondAlgoExecutionService::BondAlgoExecutionService()
    : side(PricingSide::BID)
{};

// sends the price of the bond to listeners
void BondAlgoExecutionService::OnMessage(AlgoExecution<Bond>& algoexec){
    string trade_id = algoexec.GetExecutionOrder().GetOrderId();
    AddData(trade_id, algoexec);
    for (auto& listener : listeners){
        listener->ProcessAdd(algoexec);
    }
};

void BondAlgoExecutionService::AggressBook(OrderBook<Bond>& orderbook){
    if (orderbook.GetTopSpread() <= 1. / 128.){
        PricingSide side = GetCurrentSide();
        ExecutionOrder new_order(orderbook.GetProduct(), side, GenOrderId(), OrderType::IOC, orderbook.GetTopAgainstPrice(side), orderbook.GetTopAgainstQuantity(side), 0, "N/A", false);
        AlgoExecution<Bond> algoexec(new_order);
        OnMessage(algoexec);
    }
    
}

PricingSide BondAlgoExecutionService::GetCurrentSide(){
    side = (side == PricingSide::OFFER) ? PricingSide::BID : PricingSide::OFFER;
    return side;
}
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

void BondAlgoExecutionServiceListener::ProcessAdd(OrderBook<Bond>& orderbook){
    service->AggressBook(orderbook);
}


#endif /* bondalgoexecutionservice_hpp */
