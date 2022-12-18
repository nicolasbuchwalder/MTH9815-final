//
//  bondinquiryservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondinquiryservice_hpp
#define bondinquiryservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "inquiryservice.hpp"
#include "tradinguniverse.hpp"
#include "utility.hpp"

#include <unordered_map>
#include <thread>
#include <chrono>


using namespace std;

/*
 BONDINQUIRYSERVICE CLASS DECLARATION
 */


class BondInquiryService
: public InquiryService<Bond>
{
    
public:
    // constructor
    BondInquiryService() = default;
    // sends the price of the bond to listeners
    virtual void OnMessage(Inquiry<Bond>& data) override;
    // Send a quote back to the client
    virtual void SendQuote(const string &inquiryId, double price) override;
    // Reject an inquiry from the client
    virtual void RejectInquiry(const string &inquiryId) override;
};


/*
 BONDINQUIRYSERVICELISTENER CLASS DECLARATION
 */

class BondInquiryServiceListener
    : public ServiceListener<Inquiry<Bond>>
{
private:
    BondInquiryService* service;                // service to which the listener is attached
public:
    // constructor
    BondInquiryServiceListener(BondInquiryService* _service);
    // not needed
    virtual void ProcessAdd(Inquiry<Bond>& inquiry) override;
    // not used
    virtual void ProcessRemove(Inquiry<Bond>& inquiry) override{};
    // quotes the inquiry if state is received or sets it to done if quoted
    virtual void ProcessUpdate(Inquiry<Bond>& inquiry) override;
};



/*
 BONDINQUIRYCONNECTOR CLASS DECLARATION
 */


class BondInquiryConnector
: public SocketReadConnector<Inquiry<Bond>>
{
public:
    // constructor
    BondInquiryConnector(BondInquiryService* _service, const string _raw_address, const int _port_number);
    // sets the onmessage function of the service

    virtual Inquiry<Bond> ProcessData(const vector<string>& row) override;
};



/*
 BONDINQUIRYSERVICE METHODS DEFINITION
 */

// sends the price of the bond to listeners
void BondInquiryService::OnMessage(Inquiry<Bond>& inquiry){
    
    string inquiry_id = inquiry.GetInquiryId();
    InquiryState inquiry_state = inquiry.GetState();
    switch(inquiry_state){
            
        case InquiryState::RECEIVED:
            AddData(inquiry_id, inquiry);
            for (auto& listener : listeners){
                listener->ProcessAdd(inquiry);
            };
            break;
        case InquiryState::QUOTED:
            for (auto& listener : listeners){
                listener->ProcessUpdate(inquiry);
            };
            break;
        case InquiryState::DONE:
            for (auto& listener : listeners){
                listener->ProcessUpdate(inquiry);
            };
            break;
        default:
            break;
        
    }
}


void BondInquiryService::SendQuote(const string &inquiryId, double price){
    Inquiry inquiry(GetData(inquiryId));
    inquiry.SetPrice(price);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    inquiry.UpdateState(InquiryState::QUOTED);
    OnMessage(inquiry);
}

 void BondInquiryService::RejectInquiry(const string &inquiryId){
     Inquiry inquiry(GetData(inquiryId));
     inquiry.UpdateState(InquiryState::REJECTED);
     OnMessage(inquiry);
}



/*
 BONDINQUIRYSERVICELISTENER METHODS DEFINITION
 */

BondInquiryServiceListener::BondInquiryServiceListener(BondInquiryService* _service)
    : service(_service)
{};


void BondInquiryServiceListener::ProcessAdd(Inquiry<Bond>& inquiry){
    service->SendQuote(inquiry.GetInquiryId(), 100.);
}


void BondInquiryServiceListener::ProcessUpdate(Inquiry<Bond>& inquiry){
    if (inquiry.GetState() == InquiryState::QUOTED) {
        inquiry.UpdateState(InquiryState::DONE);
        service->OnMessage(inquiry);
    }
        
}


/*
 BONDINQUIRYCONNECTOR METHODS DEFINITION
 */


BondInquiryConnector::BondInquiryConnector(BondInquiryService* _service, const string _raw_address, const int _port_number)

: SocketReadConnector("BondInquiryConnector", _service, _raw_address, _port_number)
{};


Inquiry<Bond> BondInquiryConnector::ProcessData(const vector<string>& row){
    return Inquiry(row[0], productmap.at(row[2]), string2side(row[3]), stol(row[4]), 0, string2inquiry(row[1]));
}


#endif /* bondinquiryservice_hpp */
