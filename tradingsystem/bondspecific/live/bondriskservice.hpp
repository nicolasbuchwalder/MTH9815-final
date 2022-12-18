//
//  bondriskservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondriskservice_hpp
#define bondriskservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "riskservice.hpp"
#include "positionservice.hpp"
#include "tradinguniverse.hpp"
#include "bondhistoricalriskservice.hpp"

#include <numeric>
#include <functional>

using namespace std;
using namespace std::placeholders;

// class that gets positions and transforms it into risk
class BondRiskService
    : public RiskService<Bond>
{
private:
    // need to update the listener as the historical listener has the ProcessAdd/ProcessUpdate methods also for bucket sectors, which do not exist in the base ServiceListener class
    vector<BondHistoricalRiskServiceListener*> listeners;  // listeners
    
public:
    // constructor
    BondRiskService() = default;
    // sends the risk of the bond position to listeners
    virtual void OnMessage(PV01<Bond>& risk) override;
    // sends the risk of the bucket position to listeners
    void OnMessageBucket(PV01<BucketedSector<Bond>>& bucketrisk, bool newbucket);
    // updated methods for the same reasons as above
    void AddListener(BondHistoricalRiskServiceListener* listener);
    const vector<BondHistoricalRiskServiceListener*>& GetListeners() const;
    // transforming position into risk
    virtual void AddPosition(Position<Bond>& position) override;
    // get risk of bucket sector
    virtual PV01<BucketedSector<Bond>> GetBucketedRisk(const BucketedSector<Bond> &sector) override;
};

// service listener attached to risk service
class BondRiskServiceListener
    : public ServiceListener<Position<Bond>>
{
private:
    BondRiskService* service;    // service to which the listener is attached
    // not needed
    virtual void ProcessRemove(Position<Bond>& position) override{};
    
public:
    // constructor
    BondRiskServiceListener(BondRiskService* _service);
    // send to service for a new position
    virtual void ProcessAdd(Position<Bond>& position) override;
    // send to service for a existing position
    virtual void ProcessUpdate(Position<Bond>& position) override;
};

// sends the risk of the bond position to listeners
void BondRiskService::OnMessage(PV01<Bond>& risk){
    string bond_id = risk.GetProduct().GetProductId();
    if (!ExistingData(bond_id)){
        AddData(bond_id, risk);
        for (auto& listener : listeners){
            listener->ProcessAdd(risk);
        }
    }
    else {
        UpdateData(bond_id, risk);
        for (auto& listener : listeners){
            listener->ProcessUpdate(risk);
        };
    }
}

// sends the risk of the bucket position to listeners
void BondRiskService::OnMessageBucket(PV01<BucketedSector<Bond>>& bucketrisk, bool newbucket){
    if (newbucket){
        for (auto& listener : listeners){
            listener->ProcessAdd(bucketrisk);
        }
    }
    else {
        for (auto& listener : listeners){
            listener->ProcessUpdate(bucketrisk);
        };
    }
}

// updated listener methods
void BondRiskService::AddListener(BondHistoricalRiskServiceListener* listener){
    listeners.push_back(listener);
};
const vector<BondHistoricalRiskServiceListener*>& BondRiskService::GetListeners() const{
    return listeners;
};

// transforming position into risk
void BondRiskService::AddPosition(Position<Bond>& position){
    string bond_id = position.GetProduct().GetProductId();
    PV01<Bond> risk(position.GetProduct(), PV01map.at(bond_id), 0);
    BucketedSector<Bond> sector = bond2bucketmap.at(position.GetProduct().GetProductId());
    PV01<BucketedSector<Bond>> bucketrisk(sector, 0, 0);
    
    if (!ExistingData(bond_id)){
        risk.UpdateQuantity(position.GetAggregatePosition());
    }
    else{
        risk = GetData(bond_id);
        risk.UpdateQuantity(position.GetAggregatePosition());
    }
    
    OnMessage(risk);
    bucketrisk = GetBucketedRisk(sector);
    OnMessageBucket(bucketrisk, !ExistingData(bond_id));
}

// get risk of bucket sector
PV01<BucketedSector<Bond>> BondRiskService::GetBucketedRisk(const BucketedSector<Bond> &sector){
    vector<double> bucket_pv01;
    vector<double> bucket_quantity;
    // storing characteristics for all bonds in sector (only if they already exist)
    for (auto product : sector.GetProducts()){
        string bond_id = product.GetProductId();
        if (ExistingData(bond_id)) {
            PV01<Bond> current_risk = GetData(bond_id);
            bucket_pv01.push_back(current_risk.GetPV01());
            bucket_quantity.push_back(current_risk.GetQuantity());
        }
    }
    // total quantity
    double total_quantity = std::accumulate(bucket_quantity.begin(), bucket_quantity.end(), 0.);
    // returning 0 bucket risk if quantity is 0
    if (total_quantity == 0.){
        PV01<BucketedSector<Bond>> bucket_risk = PV01<BucketedSector<Bond>>(sector, 0, 0);
        return bucket_risk;
    }
    // computing weights (proportion of size) of each product in bucket
    vector<double> weights(bucket_quantity.size());
    std::transform(bucket_quantity.begin(), bucket_quantity.end(), weights.begin(), bind(divides<double>(), _1, total_quantity));
    // calculating the weighted average of PV01 to get bucket PV01
    vector<double> weighted_PV01(weights.size());
    std::transform(weights.begin(), weights.end(), bucket_pv01.begin(), weighted_PV01.begin(), std::multiplies<double>());
    double total_pv01 = std::accumulate(weighted_PV01.begin(), weighted_PV01.end(), 0.);
    
    PV01<BucketedSector<Bond>> bucket_risk = PV01<BucketedSector<Bond>>(sector, total_pv01, total_quantity);
    return bucket_risk;
}

// constuctor
BondRiskServiceListener::BondRiskServiceListener(BondRiskService* _service)
    : service(_service)
{};

// send to service for a new position
void BondRiskServiceListener::ProcessAdd(Position<Bond>& position){
    service->AddPosition(position);
}

// send to service for an existing position
void BondRiskServiceListener::ProcessUpdate(Position<Bond>& position){
    service->AddPosition(position);
}

#endif /* bondriskservice_hpp */
