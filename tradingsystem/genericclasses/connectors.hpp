//
//  connectors.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 13.12.22.
//

#ifndef connectors_hpp
#define connectors_hpp

#include "soa.hpp"

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <thread>
#include <chrono>


using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;



template<typename V>
class SocketReadConnector
    : public Connector<V>
{
private:
    const string display_name;
    Service<string, V>* service;
    io_service io_service;
    tcp::socket socket;
    const string raw_address;
    const int port_number;
    
public:
    SocketReadConnector(const string _display_name, Service<string, V>* _service, const string _raw_address, const int _port_number);
    virtual void Publish(V& data) final;
    virtual void Read() final;
    vector<string> ReadSocket();
    virtual V ProcessData(const vector<string>& row) = 0;
    
};



template<typename V>
class SocketPublishConnector
    : public Connector<V>
{
private:
    const string display_name;
    io_service io_service;
    tcp::acceptor acceptor;
    tcp::socket socket;
    const string raw_address;
    const int port_number;
    
public:
    SocketPublishConnector(const string _display_name, const string _raw_address, const int _port_number);
    virtual void Publish(V& data) final;
    virtual void Read() final;
    void PublishSocket(const string& message);
    virtual string ProcessData(V& data) = 0;
    
};




template<typename V>
class FilePublishConnector
    : public Connector<V>
{
protected:
    const string display_name;
    fstream file;
    const string path;
    
public:
    FilePublishConnector(const string _display_name, const string _path);
    virtual void Publish(V& data) final;
    virtual void Read() final;
    void ClearFile();
    virtual string ProcessData(V& data) = 0;
    
};








template<typename V>
SocketReadConnector<V>::SocketReadConnector(const string _display_name, Service<string, V>* _service, const string _raw_address, const int _port_number)
    : display_name(_display_name), service(_service), raw_address(_raw_address), port_number(_port_number), socket(io_service)
{
    cout<< display_name << " will try to connect to socket at address " << raw_address << ", port " << port_number << endl;
    try {
        socket.connect(tcp::endpoint(address::from_string(raw_address), port_number));
        cout << display_name << " successfully connected " << endl;
    }
    catch (boost::system::system_error err){
        cout << display_name << " lost connexion on socket: " << err.code().message() << ", shutting down..." << endl;
    }
}

template<typename V>
void SocketReadConnector<V>::Publish(V& data){};

template<typename V>
void SocketReadConnector<V>::Read(){
    string str;
    while (true) {
        try {
            V data = ProcessData(ReadSocket());
            service->OnMessage(data); 
        }
        catch (boost::system::system_error err){
            if (err.code() == boost::asio::error::eof){
                cout << display_name << " sucessfully read all the data, shutting down... " << endl;
                break;
            }
            cout << display_name << " lost connexion on socket: " << err.code().message() << ", shutting down..." << endl;
            break;
        }
    }
}

template<typename V>
vector<string> SocketReadConnector<V>::ReadSocket(){
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    string row = boost::asio::buffer_cast<const char*>(buf.data());
    vector<string> data;
    boost::split(data, row, boost::is_any_of(","));
    return data;
}


template<typename V>
SocketPublishConnector<V>::SocketPublishConnector(const string _display_name, const string _raw_address, const int _port_number)
    : display_name(_display_name), raw_address(_raw_address), port_number(_port_number), socket(io_service), acceptor(io_service, tcp::endpoint(address::from_string(_raw_address), _port_number))
{
    try {
        cout << display_name << " is waiting for socket connection at address " << raw_address << ", port " << port_number << endl;
        acceptor.accept(socket);
        cout << display_name << " successfully connected " << endl;
    }
    catch (boost::system::system_error err){
        cout << display_name << " could not connect to socket: " << err.code().message() << endl;
    }
}

template<typename V>
void SocketPublishConnector<V>::Publish(V& data){
    PublishSocket(ProcessData(data));
};

template<typename V>
void SocketPublishConnector<V>::Read(){};

template<typename V>
void SocketPublishConnector<V>::PublishSocket(const string& message){
    const string msg = message + "\n";
    boost::asio::write(socket, boost::asio::buffer(msg));
}







template<typename V>
FilePublishConnector<V>::FilePublishConnector(const string _display_name, const string _path)
: display_name(_display_name), path(_path)
{
    file.open(path, fstream::out | fstream::trunc);
};

template<typename V>
void FilePublishConnector<V>::Publish(V& data){
    file << ProcessData(data) << endl;
};
    
template<typename V>
void FilePublishConnector<V>::Read(){};

template<typename V>
void FilePublishConnector<V>::ClearFile(){
    // closing file
    file.close();
    // reading header
    ifstream reader(path);
    string header;
    getline(reader, header);
    
    // reopening file and suppressing content and inputting header
    file.open(path, fstream::out | fstream::trunc);
    file << header << endl;
    
}

#endif /* connectors_hpp */
