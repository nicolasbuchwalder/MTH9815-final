//
//  bondhistoricalexecutionservice.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 17.12.22.
//

#ifndef bondhistoricalexecutionservice_hpp
#define bondhistoricalexecutionservice_hpp

#include "historicaldataservice.hpp"

/*
 * BONDHISTORICALEXECUTIONSERVICE CLASS DECLARATION
 */

// class that listens to the execution service to persist data to file
class BondHistoricalExecutionService
    : public HistoricalDataService<ExecutionOrder<Bond>>
{
    
private:
    Connector<ExecutionOrder<Bond>>* connector;            // publish connector
    
public:
    // constructor
    BondHistoricalExecutionService(Connector<ExecutionOrder<Bond>>* _connector);
    // persist execution order to file
    virtual void PersistData(string persistKey, ExecutionOrder<Bond>& exec_order) override;
};



/*
 * BONDHISTORICALEXECUTIONSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to execution service
class BondHistoricalExecutionServiceListener
: public ServiceListener<ExecutionOrder<Bond>>
{
    
private:
    BondHistoricalExecutionService* service;  // service to which the listener is attached
    // not used
    virtual void ProcessRemove(ExecutionOrder<Bond>& exec_order) override{};
    // not used
    virtual void ProcessUpdate(ExecutionOrder<Bond>& exec_order) override{};
    
public:
    // constructor
    BondHistoricalExecutionServiceListener(BondHistoricalExecutionService* _service);
    // send order to service
    virtual void ProcessAdd(ExecutionOrder<Bond>& exec_order) override;
};



/*
 * BONDHISTORICALEXECUTIONCONNECTOR CLASS DECLARATION
 */

// class that writes positions to executions.txt
class BondHistoricalExecutionConnector
: public FilePublishConnector<ExecutionOrder<Bond>>
{
private:
    // sets how data from connector is processed to file
    virtual string ProcessData(ExecutionOrder<Bond>& data) override;
public:
    // constructor
    BondHistoricalExecutionConnector(const std::string& path);

};



/*
 * BONDHISTORICALEXECUTIONSERVICE METHODS DEFINITION
 */

// constuctor
BondHistoricalExecutionService::BondHistoricalExecutionService(Connector<ExecutionOrder<Bond>>* _connector)
: connector(_connector)
{};
// persist execution order to file
void BondHistoricalExecutionService::PersistData(string persistKey, ExecutionOrder<Bond>& exec_order){
    connector->Publish(exec_order);
}



/*
 * BONDHISTORICALEXECUTIONSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondHistoricalExecutionServiceListener::BondHistoricalExecutionServiceListener(BondHistoricalExecutionService* _service)
: service(_service)
{};

// send order to service
void BondHistoricalExecutionServiceListener::ProcessAdd(ExecutionOrder<Bond>& exec_order){
    service->PersistData(exec_order.GetOrderId(), exec_order);
}



/*
 * BONDHISTORICALEXECUTIONCONNECTOR METHODS DEFINITION
 */

// constructor
BondHistoricalExecutionConnector::BondHistoricalExecutionConnector(const std::string& _path)
: FilePublishConnector("BondHistoricalExecutionConnector", _path)
{
    file << "time,exec_order_id,product_id,order_type,pricing_side,price,direction,visible_quantity,hidden_quantity,parent_order_id,is_child_order" << std::endl;
}

// sets how data from connector is processed to file
string BondHistoricalExecutionConnector::ProcessData(ExecutionOrder<Bond>& exec_order){
    return get_local_time() + "," + exec_order.GetOrderId() + "," + exec_order.GetProduct().GetProductId() + "," + "IOC" + "," + pricingside2string(exec_order.GetSide()) + "," + to_string(exec_order.GetPrice()) + "," + to_string(exec_order.GetVisibleQuantity()) + "," + to_string(exec_order.GetHiddenQuantity()) + "," + exec_order.GetParentOrderId() + "," + to_string(exec_order.IsChildOrder());
}


#endif /* bondhistoricalexecutionservice_hpp */
