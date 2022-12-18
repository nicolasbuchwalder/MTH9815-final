//
//  main.cpp
//  bondinquiryprovider
//
//  Created by Nicolas Buchwalder on 16.12.22.
//

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

void publish_socket(tcp::socket& socket, string message){
    cout << message << endl;
	string msg = message + '\n';
    boost::asio::write(socket, boost::asio::buffer(msg));
}


int main(int argc, const char * argv[]) {
    
    const int port_number = 10003;
    string raw_address = "127.0.0.1";
    string path = "dataproviders/bondinquiryprovider/inquiries.txt";
    int refresh_rate_ms = 1000;
    
    io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(address::from_string(raw_address), port_number));
    tcp::socket socket(io_service);
    cout << "BondInquiryProvider is waiting on BondInquiryConnector at address " << raw_address << ", port " << port_number << endl;
    acceptor.accept(socket);
    cout << "BondInquiryProvider sucessfully connected " << endl;
    
    ifstream file(path);
    if (file.fail()){
        cout << path << " does not exist, make sure to set right path" << endl;
        return 1;
    }
    
    string line;
    bool header = true;
    while(getline(file, line)){
        if (header) {
            cout << "BondInquiryProvider started publishing from " << path << endl;
            header = false;
            continue;
        }
    	try{
            publish_socket(socket, line);
            
    	}
    	catch (boost::system::system_error err) {
        	cout << "BondInquiryProvider couldn't connect on socket " << err.code().message() << ", shutting down..." << endl;
            return 1;
    	}
        std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
    };
    cout << "BondInquiryProvider finished publishing from" << path << endl;
    return 0;
}
