//
//  bondtradebookingservice.hpp
//  MTH9815 final
//
//  Created by Nicolas Buchwalder on 12.12.22.
//

#ifndef bondtradebookingservice_hpp
#define bondtradebookingservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include "connectors.hpp"
#include "tradebookingservice.hpp"
#include "executionservice.hpp"
#include "tradinguniverse.hpp"
#include "utility.hpp"

#include <vector>
#include <memory>


using namespace std;



/*
 * BONDTRADEBOOKINGSERVICE CLASS DECLARATION
 */

// service that listens to trades and processes it
class BondTradeBookingService
    : public TradeBookingService<Bond>
{
private:
    const vector<string> trading_books;                 // list of trading books
    const int num_books;                                // number of books
    int current_book_idx;                               // current book index
    
public:
    // constructor
    BondTradeBookingService(const vector<string>& _trading_books);
    // sends the bond trades to listeners
    virtual void OnMessage(Trade<Bond>& data) override;
    // book the trade
    virtual void BookTrade(Trade<Bond> &trade) override;
    // returns the book on which the trade will be done
    string GetCurrentBook();
    
};



/*
 * BONDTRADEBOOKINGSERVICELISTENER CLASS DECLARATION
 */

// service listener attached to tradebooking service
class BondTradeBookingServiceListener
: public ServiceListener<ExecutionOrder<Bond>>
{
    
private:
    BondTradeBookingService* service;   // service to which the listener is attached
    // not used
    virtual void ProcessRemove(ExecutionOrder<Bond>& exec_order) override{};
    // not used
    virtual void ProcessUpdate(ExecutionOrder<Bond>& exec_order) override{};
public:
    // constructor
    BondTradeBookingServiceListener(BondTradeBookingService* service);
    // send to service for a new order
    virtual void ProcessAdd(ExecutionOrder<Bond>& exec_order) override;
};



/*
 * BONDTRADEBOOKINGCONNECTOR CLASS DECLARATION
 */

// class that reads data from socket
class BondTradeBookingConnector
    : public SocketReadConnector<Trade<Bond>>
{
private:
    // sets how data from connector is processed to service
    Trade<Bond> ProcessData(const vector<string>& row) override;
    
public:
    // constructor
    BondTradeBookingConnector(BondTradeBookingService* _service, const string _raw_address, const int _port_number);

};



/*
 * BONDTRADEBOOKINGSERVICE METHODS DEFINITION
 */

// constructor (trading books is needed to attribute trades to books)
BondTradeBookingService::BondTradeBookingService(const vector<string>& _trading_books)
    : trading_books(_trading_books), num_books(trading_books.size()), current_book_idx(0)
{};

// sends the bond trades to listeners
void BondTradeBookingService::OnMessage(Trade<Bond>& trade){
    AddData(trade.GetTradeId(), trade);
    for (auto& listener : listeners){
        listener->ProcessAdd(trade);
    };
}

// book the trade
void BondTradeBookingService::BookTrade(Trade<Bond>& trade){
    OnMessage(trade);
};

// returns the book on which the trade will be done
string BondTradeBookingService::GetCurrentBook(){
    string current_book = trading_books[current_book_idx];
    current_book_idx = (current_book_idx + 1) % num_books;
    return current_book;
}



/*
 * BONDTRADEBOOKINGSERVICELISTENER METHODS DEFINITION
 */

// constructor
BondTradeBookingServiceListener::BondTradeBookingServiceListener(BondTradeBookingService* _service)
    : service(_service)
{};

// send to service for a new order
void BondTradeBookingServiceListener::ProcessAdd(ExecutionOrder<Bond>& exec_order){
    Trade<Bond> executed_trade(exec_order, service->GetCurrentBook());
    service->BookTrade(executed_trade);
};
  


/*
 * BONDTRADEBOOKINGCONNECTOR METHODS DEFINITION
 */

// constructor
BondTradeBookingConnector::BondTradeBookingConnector(BondTradeBookingService* _service, const string _raw_address, const int _port_number)

: SocketReadConnector("BondTradeBookingConnector", _service, _raw_address, _port_number)
{};

// sets how data from connector is processed to service
Trade<Bond> BondTradeBookingConnector::ProcessData(const vector<string>& row){
    return Trade<Bond>(productmap.at(row[1]), row[0], stod(row[4]), row[2], stoi(row[5]), string2side(row[3]));
}



#endif /* bondtradebookingservice_hpp */
