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

class BondPositionService
    : public PositionService<Bond>
{
public:
    // constructor
    BondPositionService() = default;
    virtual void OnMessage(Position<Bond>& data) override;
    // adding trade to postion
    virtual void AddTrade(const Trade<Bond>& trade) override;
};


class BondPositionServiceListener
    : public ServiceListener<Trade<Bond>>
{
private:
    BondPositionService* service;    // service to which the listener is attached
    
public:
    BondPositionServiceListener(BondPositionService* _service);
    virtual void ProcessAdd(Trade<Bond>& Trade) override;
    virtual void ProcessRemove(Trade<Bond>& Trade) override{};
    virtual void ProcessUpdate(Trade<Bond>& Trade) override{};
};

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


void BondPositionService::AddTrade(const Trade<Bond>& trade){
    string bond_id = trade.GetProduct().GetProductId();
    long quantity = trade.GetSide() == Side::BUY ? trade.GetQuantity() : -trade.GetQuantity();
    Position<Bond> position(trade.GetProduct());
    if (!ExistingData(bond_id)){
        position.UpdatePosition(trade.GetBook(), quantity);
    }
    else{
        position = GetData(bond_id);
        position.UpdatePosition(trade.GetBook(), quantity);
    }
    OnMessage(position);
}


BondPositionServiceListener::BondPositionServiceListener(BondPositionService* _service)
    : service(_service)
{};


void BondPositionServiceListener::ProcessAdd(Trade<Bond>& trade){
    service->AddTrade(trade);
}
        

#endif /* bondpositionservice_hpp */
