//
//  bondexecutionservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondexecutionservice_hpp
#define bondexecutionservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "executionservice.hpp"
#include "utility.hpp"

#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>


using namespace std;

/*
 * BONDEXECUTIONSERVICE CLASS DECLARATION
 */

// class that executes orders and distributed it
class BondExecutionService
: public ExecutionService<Bond>
{
    
private:
    Connector<ExecutionOrder<Bond>>* connector;                 // publish connector

public:
    // constructor
    BondExecutionService(Connector<ExecutionOrder<Bond>>* _connector);
    // sends the order to next service
    virtual void OnMessage(ExecutionOrder<Bond>& exec_order) override;
    // executing order
    void ExecuteOrder(ExecutionOrder<Bond>& order, Market market) override;
};



/*
 * BONDEXECUTIONSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to execution service
class BondExecutionServiceListener
: public ServiceListener<AlgoExecution<Bond>>
{
    
private:
    BondExecutionService* service;  // service to which the listener is attached
    // not used
    virtual void ProcessRemove(AlgoExecution<Bond>& algoexec) override{};
    // not used
    virtual void ProcessUpdate(AlgoExecution<Bond>& algoexec) override{};
public:
    // constructor
    BondExecutionServiceListener(BondExecutionService* _service);
    // send to service for a new algo exec
    virtual void ProcessAdd(AlgoExecution<Bond>& algoexec) override;
    
};



/*
 * BONDEXECUTIONCONNECTOR CLASS DECLARATION
 */

// class that publishes executions to socket
class BondExecutionConnector
: public SocketPublishConnector<ExecutionOrder<Bond>>
{
private:
    // sets how data from connector is processed to socket
    virtual string ProcessData(ExecutionOrder<Bond>& pricestream) override;
    
public:
    // constructor
    BondExecutionConnector(const string _raw_address, const int _port_number);

};



/*
 * BONDEXECUTIONSERVICE METHODS DEFINITION
 */

// constuctor
BondExecutionService::BondExecutionService(Connector<ExecutionOrder<Bond>>* _connector)
: connector(_connector)
{};


// sends the order to next service and signals connector
void BondExecutionService::OnMessage(ExecutionOrder<Bond>& exec_order){
    AddData(exec_order.GetProduct().GetProductId(), exec_order);
    for (auto& listener : listeners){
        listener->ProcessAdd(exec_order);
    }
    connector->Publish(exec_order);
}
// executing order
void BondExecutionService::ExecuteOrder(ExecutionOrder<Bond>& order, Market market){
    OnMessage(order);
}


/*
 * BONDEXECUTIONSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondExecutionServiceListener::BondExecutionServiceListener(BondExecutionService* _service)
: service(_service)
{};

// send to service for a new algo exec
void BondExecutionServiceListener::ProcessAdd(AlgoExecution<Bond>& algoexec){
    // we assume only one market
    service->ExecuteOrder(algoexec.GetExecutionOrder(), Market::CME);
}



/*
 * BONDEXECUTIONCONNECTOR METHODS DEFINITION
 */

// constructor
BondExecutionConnector::BondExecutionConnector(const string _raw_address, const int _port_number)
: SocketPublishConnector("BondExecutionConnector", _raw_address, _port_number)
{}

// sets how data from connector is processed to file
string BondExecutionConnector::ProcessData(ExecutionOrder<Bond>& exec_order){
    return get_local_time() + "," + exec_order.GetOrderId() + "," + exec_order.GetProduct().GetProductId() + "," + "IOC" + "," + pricingside2string(exec_order.GetSide()) + "," + to_string(exec_order.GetPrice()) + "," + to_string(exec_order.GetVisibleQuantity()) + "," + to_string(exec_order.GetHiddenQuantity()) + "," + exec_order.GetParentOrderId() + "," + to_string(exec_order.IsChildOrder());
}


#endif /* bondexecutionservice_hpp */
