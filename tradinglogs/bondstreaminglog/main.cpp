//
//  main.cpp
//  bondstreaminglog
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

string read_socket(tcp::socket& socket){
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    string row = boost::asio::buffer_cast<const char*>(buf.data());
    return row;
}


int main(int argc, const char * argv[]) {
    
    const int port_number = 10005;
    string raw_address = "127.0.0.1";
    
    io_service io_service;
    tcp::socket socket(io_service);
    
    cout << "BondStreamingLogger will try to connect to BondStreamingConnector at address " << raw_address << ", port " << port_number << endl;
    
    try{
        socket.connect(tcp::endpoint(address::from_string(raw_address), port_number));
    }
    catch (boost::system::system_error err) {
        cout << "BondStreamingLogger couldn't connect on socket:  " << err.code().message() << ", shutting down..." << endl;
        return 1;
    }
    cout << "BondStreamingLogger sucessfully connected " << endl;
    

    while(true){
    	try{
            cout << read_socket(socket) << endl;
            
    	}
    	catch (boost::system::system_error err) {
            if (err.code() == boost::asio::error::eof){
                cout << "BondStreamingLogger sucessfully read all the data, shutting down... " << endl;
                break;
            }
        	cout << "BondStreamingLogger lost connexion on socket: " << err.code().message() << ", shutting down..." << endl;
            return 1;
    	}
        
    };
    return 0;
}
