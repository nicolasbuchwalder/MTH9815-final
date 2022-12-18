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

// service that listens to inquiries and processes it
class BondInquiryService
: public InquiryService<Bond>
{
    
public:
    // constructor
    BondInquiryService() = default;
    // sends the inquiries to next service
    virtual void OnMessage(Inquiry<Bond>& inquiry) override;
    // send a quote back to the client
    virtual void SendQuote(const string &inquiryId, double price) override;
    // reject an inquiry from the client
    virtual void RejectInquiry(const string &inquiryId) override;
};


/*
 BONDINQUIRYSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to inquiry service
class BondInquiryServiceListener
    : public ServiceListener<Inquiry<Bond>>
{
private:
    BondInquiryService* service;                // service to which the listener is attached
    // not used
    virtual void ProcessRemove(Inquiry<Bond>& inquiry) override{};
public:
    // constructor
    BondInquiryServiceListener(BondInquiryService* _service);
    // if new inquiry arrives
    virtual void ProcessAdd(Inquiry<Bond>& inquiry) override;
    // sets it to done if quoted, ignores it if done
    virtual void ProcessUpdate(Inquiry<Bond>& inquiry) override;
};



/*
 BONDINQUIRYCONNECTOR CLASS DECLARATION
 */

// class that reads data from socket
class BondInquiryConnector
: public SocketReadConnector<Inquiry<Bond>>
{
private:
    // sets how data from connector is processed to service
    virtual Inquiry<Bond> ProcessData(const vector<string>& row) override;
    
public:
    // constructor
    BondInquiryConnector(BondInquiryService* _service, const string _raw_address, const int _port_number);

};



/*
 BONDINQUIRYSERVICE METHODS DEFINITION
 */

// sends the inquiries to next service
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

// send a quote back to the client
void BondInquiryService::SendQuote(const string &inquiryId, double price){
    Inquiry inquiry(GetData(inquiryId));
    inquiry.SetPrice(price);
    // emulate the trader that needs to quote
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    inquiry.UpdateState(InquiryState::QUOTED);
    OnMessage(inquiry);
}

// reject an inquiry from the client
 void BondInquiryService::RejectInquiry(const string &inquiryId){
     Inquiry inquiry(GetData(inquiryId));
     inquiry.UpdateState(InquiryState::REJECTED);
     OnMessage(inquiry);
}



/*
 BONDINQUIRYSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondInquiryServiceListener::BondInquiryServiceListener(BondInquiryService* _service)
    : service(_service)
{};

// if new inquiry arrives
void BondInquiryServiceListener::ProcessAdd(Inquiry<Bond>& inquiry){
    service->SendQuote(inquiry.GetInquiryId(), 100.);
}

// sets it to done if quoted, ignores it if done
void BondInquiryServiceListener::ProcessUpdate(Inquiry<Bond>& inquiry){
    if (inquiry.GetState() == InquiryState::QUOTED) {
        inquiry.UpdateState(InquiryState::DONE);
        service->OnMessage(inquiry);
    }
        
}


/*
 BONDINQUIRYCONNECTOR METHODS DEFINITION
 */

// constructor
BondInquiryConnector::BondInquiryConnector(BondInquiryService* _service, const string _raw_address, const int _port_number)

: SocketReadConnector("BondInquiryConnector", _service, _raw_address, _port_number)
{};

// sets how data from connector is processed to service
Inquiry<Bond> BondInquiryConnector::ProcessData(const vector<string>& row){
    return Inquiry(row[0], productmap.at(row[2]), string2side(row[3]), stol(row[4]), 0, string2inquiry(row[1]));
}


#endif /* bondinquiryservice_hpp */
