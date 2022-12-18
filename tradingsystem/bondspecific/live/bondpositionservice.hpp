//
//  bondpositionservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondpositionservice_hpp
#define bondpositionservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "positionservice.hpp"
#include "tradebookingservice.hpp"
#include "tradinguniverse.hpp"

/*
 * BONDPOSITIONSERVICE CLASS DECLARATION
 */

// class that gets trades and transforms it into positions
class BondPositionService
    : public PositionService<Bond>
{
public:
    // constructor
    BondPositionService() = default;
    // sends the position to next service
    virtual void OnMessage(Position<Bond>& data) override;
    // transforming trade into postion
    virtual void AddTrade(const Trade<Bond>& trade) override;
};



/*
 * BONDPOSITIONSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to position service
class BondPositionServiceListener
    : public ServiceListener<Trade<Bond>>
{
private:
    BondPositionService* service;    // service to which the listener is attached
    // not needed
    virtual void ProcessRemove(Trade<Bond>& Trade) override{};
    // not needed
    virtual void ProcessUpdate(Trade<Bond>& Trade) override{};
    
public:
    BondPositionServiceListener(BondPositionService* _service);
    // send to service for a new trade
    virtual void ProcessAdd(Trade<Bond>& Trade) override;

};

/*
 * BONDPOSITIONSERVICE METHODS DEFINITION
 */

// sends the position to next service
void BondPositionService::OnMessage(Position<Bond>& position){
    string bond_id = position.GetProduct().GetProductId();
    if (!ExistingData(bond_id)){
        AddData(bond_id, position);
        for (auto& listener : listeners){
            listener->ProcessAdd(position);
        }
    }
    else {
        UpdateData(bond_id, position);
        for (auto& listener : listeners){
            listener->ProcessUpdate(position);
        };
    }
}

// transforming trade to postion
void BondPositionService::AddTrade(const Trade<Bond>& trade){
    string bond_id = trade.GetProduct().GetProductId();
    // getting net quantity from side and quantity
    long quantity = trade.GetSide() == Side::BUY ? trade.GetQuantity() : -trade.GetQuantity();
    
    Position<Bond> position(trade.GetProduct());
    // setting position at 0 and adding quantity if new position
    if (!ExistingData(bond_id)){
        position.UpdatePosition(trade.GetBook(), quantity);
    }
    // updates quantity from existing position
    else{
        position = GetData(bond_id);
        position.UpdatePosition(trade.GetBook(), quantity);
    }
    OnMessage(position);
}

// constuctor
BondPositionServiceListener::BondPositionServiceListener(BondPositionService* _service)
    : service(_service)
{};

// send to service for a new trade
void BondPositionServiceListener::ProcessAdd(Trade<Bond>& trade){
    service->AddTrade(trade);
}
        

#endif /* bondpositionservice_hpp */
