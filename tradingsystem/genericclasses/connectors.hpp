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



/*
 * SOCKETREADCONNECTOR CLASS DECLARATION
 */

// abstract class for connectors to read from a boost::asio socket
template<typename V>
class SocketReadConnector
    : public Connector<V>
{
private:
    const string display_name;          // name that is diplayed when priting to cout
    Service<string, V>* service;        // service to which the connector is attached to
    io_service io_service;              // boost io service
    tcp::socket socket;                 // boost socket
    const string raw_address;           // address of socket
    const int port_number;              // port number of socket

    // useless function for a read connector
    virtual void Publish(V& data) final{};
    
protected:
    // reads socket line and returns it
    vector<string> ReadSocket();
    // transforms string of input to data
    virtual V ProcessData(const vector<string>& row) = 0;
    
public:
    // constructor
    SocketReadConnector(const string _display_name, Service<string, V>* _service, const string _raw_address, const int _port_number);
    // read socket continuously
    virtual void Read() final;
};



/*
 * SOCKETPUBLISHCONNECTOR CLASS DECLARATION
 */

// abstract class for connectors to publish to a boost::asio socket
template<typename V>
class SocketPublishConnector
    : public Connector<V>
{
private:
    const string display_name;          // name that is diplayed when priting to cout
    Service<string, V>* service;        // service to which the connector is attached to
    io_service io_service;              // boost io service
    tcp::acceptor acceptor;             // boost acceptor (waiting on connection instead of trying to connect)
    tcp::socket socket;                 // boost socket
    const string raw_address;           // address of socket
    const int port_number;              // port number of socket

    // useless function for a publish connector
    virtual void Read() final{};
    
protected:
    // publish message to socket
    void PublishSocket(const string& message);
    // transforms data to message of output
    virtual string ProcessData(V& data) = 0;
    
public:
    // constructor
    SocketPublishConnector(const string _display_name, const string _raw_address, const int _port_number);
    // publish to socket
    virtual void Publish(V& data) final;
};


/*
 * FILEPUBLISHCONNECTOR CLASS DECLARATION
 */

// abstract class for connectors to publish to a file at path
template<typename V>
class FilePublishConnector
    : public Connector<V>
{
private:
    const string display_name;      // name that is diplayed when priting to cout

protected:
    fstream file;                   // file to write to
    const string path;              // file of path
    
private:
    // useless function for a publish connector
    virtual void Read() final{};

protected:
    // transforms data to message of output
    virtual string ProcessData(V& data) = 0;
    
public:
    // constructor
    FilePublishConnector(const string _display_name, const string _path);
    // publish to socket
    virtual void Publish(V& data) final;
    // clear content of file and rewrite header
    void ClearFile();
    

    
};



/*
 * SOCKETREADCONNECTOR METHODS DEFINITION
 */

// constructor
template<typename V>
SocketReadConnector<V>::SocketReadConnector(const string _display_name, Service<string, V>* _service, const string _raw_address, const int _port_number)
    : display_name(_display_name), service(_service), raw_address(_raw_address), port_number(_port_number), socket(io_service)
{
    // tries to connect to socket
    cout<< display_name << " will try to connect to socket at address " << raw_address << ", port " << port_number << endl;
    try {
        socket.connect(tcp::endpoint(address::from_string(raw_address), port_number));
        cout << display_name << " successfully connected " << endl;
    }
    catch (boost::system::system_error err){
        cout << display_name << " lost connexion on socket: " << err.code().message() << ", shutting down..." << endl;
    }
}

// read socket continuously
template<typename V>
void SocketReadConnector<V>::Read(){
    // continuous loop until lost connexion or no more data to read
    while (true) {
        try {
            // read line and process it to data
            V data = ProcessData(ReadSocket());
            // run OnMessage of service to propagate new data
            service->OnMessage(data); 
        }
        catch (boost::system::system_error err){
            // if end of file error, then print success
            if (err.code() == boost::asio::error::eof){
                cout << display_name << " sucessfully read all the data" << endl;
                break;
            }
            cout << display_name << " lost connexion on socket: " << err.code().message() << ", shutting down..." << endl;
            break;
        }
    }
}

// reads socket line and returns it
template<typename V>
vector<string> SocketReadConnector<V>::ReadSocket(){
    boost::asio::streambuf buf;
    // only reads one line
    boost::asio::read_until(socket, buf, "\n");
    string row = boost::asio::buffer_cast<const char*>(buf.data());
    vector<string> data;
    // split elements into vector
    boost::split(data, row, boost::is_any_of(","));
    return data;
}



/*
 * SOCKETPUBLISHCONNECTOR METHODS DEFINITION
 */

// constructor
template<typename V>
SocketPublishConnector<V>::SocketPublishConnector(const string _display_name, const string _raw_address, const int _port_number)
    : display_name(_display_name), raw_address(_raw_address), port_number(_port_number), socket(io_service), acceptor(io_service, tcp::endpoint(address::from_string(_raw_address), _port_number))
{
    // waits on socket connexion through the acceptor
    try {
        cout << display_name << " is waiting for socket connection at address " << raw_address << ", port " << port_number << endl;
        acceptor.accept(socket);
        cout << display_name << " successfully connected " << endl;
    }
    catch (boost::system::system_error err){
        cout << display_name << " could not connect to socket: " << err.code().message() << endl;
    }
}

// publish to socket (needs to be called, not continuous)
template<typename V>
void SocketPublishConnector<V>::Publish(V& data){
    PublishSocket(ProcessData(data));
};


// Publish one message (line)
template<typename V>
void SocketPublishConnector<V>::PublishSocket(const string& message){
    const string msg = message + "\n";
    boost::asio::write(socket, boost::asio::buffer(msg));
}



/*
 * FILEPUBLISHCONNECTOR METHODS DEFINITION
 */

// constructor
template<typename V>
FilePublishConnector<V>::FilePublishConnector(const string _display_name, const string _path)
: display_name(_display_name), path(_path)
{
    // removes content if there was some
    file.open(path, fstream::out | fstream::trunc);
    // checks that file is correctly opened
    if (file.fail()){
        cout << display_name << "could not open file, please make sure path is correct" << endl;
    }
};

// publish to socket (needs to be called, not continuous)
template<typename V>
void FilePublishConnector<V>::Publish(V& data){
    file << ProcessData(data) << endl;
};

// clear content of file and rewrite header
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
