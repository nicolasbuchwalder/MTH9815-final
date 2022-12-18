//
//  main.cpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 13.12.22.
//

#include "bondalgoexecutionservice.hpp"
#include "bondalgostreamingservice.hpp"
#include "bondexecutionservice.hpp"
#include "bondguiservice.hpp"
#include "bondinquiryservice.hpp"
#include "bondmarketdataservice.hpp"
#include "bondpositionservice.hpp"
#include "bondpricingservice.hpp"
#include "bondriskservice.hpp"
#include "bondstreamingservice.hpp"
#include "bondtradebookingservice.hpp"

#include "bondhistoricalinquiryservice.hpp"
#include "bondhistoricalpositionservice.hpp"
#include "bondhistoricalriskservice.hpp"
#include "bondhistoricalexecutionservice.hpp"
#include "bondhistoricalstreamingservice.hpp"

#include <iostream>
#include <thread>



int main(int argc, const char * argv[]) {
    
    // publish connectors
    BondExecutionConnector con_pub_exec("127.0.0.1", 10004);                                    // connector to write to execution socket
    BondStreamingConnector con_pub_stream("127.0.0.1", 10005);                                  // connector to write to streaming socket
    BondGUIConnector con_pub_gui("tradingsystem/data/gui.txt");                                 // connector to write to gui file
    BondHistoricalInquiryConnector con_pub_hist_inquiry("tradingsystem/data/allinquiries.txt"); // connector to write to allinquiries file
    BondHistoricalPositionConnector con_pub_hist_position("tradingsystem/data/positions.txt");  // connector to write to positions file
    BondHistoricalRiskConnector con_pub_hist_risk("tradingsystem/data/risk.txt");               // connector to write to risk file
    BondHistoricalExecutionConnector con_pub_hist_exec("tradingsystem/data/executions.txt");    // connector to write to executions file
    BondHistoricalStreamingConnector con_pub_hist_stream("tradingsystem/data/streaming.txt");   // connector to write to streaming file
    
    
    // services
    BondStreamingService ser_stream(&con_pub_stream);
    BondExecutionService ser_execution(&con_pub_exec);
    BondGUIService ser_gui(&con_pub_gui, 15, 100);
    BondPricingService ser_pricing;
    BondTradeBookingService ser_trade(trading_books);
    BondMarketDataService ser_marketdata;
    BondInquiryService ser_inquiry;
    BondPositionService ser_position;
    BondRiskService ser_risk;
    BondAlgoStreamingService ser_algostr;
    BondAlgoExecutionService ser_algoexec;
    
    // historical services
    BondHistoricalInquiryService ser_hist_inquiry(&con_pub_hist_inquiry);
    BondHistoricalPositionService ser_hist_position(&con_pub_hist_position, trading_books);
    BondHistoricalRiskService ser_hist_risk(&con_pub_hist_risk);
    BondHistoricalExecutionService ser_hist_exec(&con_pub_hist_exec);
    BondHistoricalStreamingService ser_hist_stream(&con_pub_hist_stream);
    
    
    // read connectors
    BondPricingConnector con_read_price(&ser_pricing, "127.0.0.1", 10000);
    BondTradeBookingConnector con_read_trades(&ser_trade, "127.0.0.1", 10001);
    //BondMarketDataConnector con_read_market(&ser_marketdata, "127.0.0.1", 10002);
    BondInquiryConnector con_read_inquiry(&ser_inquiry, "127.0.0.1", 10003);
    
    // listeners
    BondGUIServiceListener lis_gui(&ser_gui);
    BondPositionServiceListener lis_position(&ser_position);
    BondRiskServiceListener lis_risk(&ser_risk);
    BondAlgoStreamingServiceListener lis_algostr(&ser_algostr);
    BondStreamingServiceListener lis_stream(&ser_stream);
    BondAlgoExecutionServiceListener lis_algoexec(&ser_algoexec);
    BondExecutionServiceListener lis_exection(&ser_execution);
    BondTradeBookingServiceListener lis_trades(&ser_trade);
    BondInquiryServiceListener lis_inquiry(&ser_inquiry);
    
    // historical listeners
    BondHistoricalInquiryServiceListener lis_hist_inquiry(&ser_hist_inquiry);
    BondHistoricalPositionServiceListener lis_hist_position(&ser_hist_position);
    BondHistoricalRiskServiceListener lis_hist_risk(&ser_hist_risk);
    BondHistoricalExecutionServiceListener lis_hist_exec(&ser_hist_exec);
    BondHistoricalStreamingServiceListener lis_hist_stream(&ser_hist_stream);
    
    // link historical services
    ser_inquiry.AddListener(&lis_hist_inquiry);     // HistoricalInquiry listens to Inquiry
    ser_position.AddListener(&lis_hist_position);   // HistoricalPosition listens to Position
    ser_risk.AddListener(&lis_hist_risk);           // HistoricalRisk listens to Risk
    ser_execution.AddListener(&lis_hist_exec);      // HistoricalExecution listens to Execution
    ser_stream.AddListener(&lis_hist_stream);       // HistoricalStreaming listens to Streaming
    
    
    // link other services between each other
    ser_inquiry.AddListener(&lis_inquiry);      // Inquiry listens to itself
    ser_pricing.AddListener(&lis_gui);          // GUI listens to Pricing
    ser_pricing.AddListener(&lis_algostr);      // AlgoStreaming listens to Pricing
    ser_algostr.AddListener(&lis_stream);       // Streaming listens to AlgoStreaming
    ser_execution.AddListener(&lis_trades);     // BookTrading listens to Execution
    ser_trade.AddListener(&lis_position);       // Position listens to BookTrading
    ser_position.AddListener(&lis_risk);        // Risk listens to Position
    ser_marketdata.AddListener(&lis_algoexec);  // AlgoExecution listens to MarketData
    ser_algoexec.AddListener(&lis_exection);    // Execution listens to AlgoExecution
    

    
    thread pricing_thread(&BondPricingConnector::Read, &con_read_price);
    thread trades_thread(&BondTradeBookingConnector::Read, &con_read_trades);
    //thread marketdata_thread(&BondMarketDataConnector::Read, &con_read_market);
    thread inquiry_thread(&BondInquiryConnector::Read, &con_read_inquiry);
    
    if (inquiry_thread.joinable()) inquiry_thread.join();
    //if (marketdata_thread.joinable()) marketdata_thread.join();
    if (trades_thread.joinable()) trades_thread.join();
    if (pricing_thread.joinable()) pricing_thread.join();
    
    
}
