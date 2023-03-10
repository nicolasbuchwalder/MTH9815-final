                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**
 * soa.hpp
 * Definition of our Service Oriented Architecture (SOA) Service base class
 *
 * @author Breman Thuraisingham
 */

#ifndef SOA_HPP
#define SOA_HPP

#include <vector>
#include <unordered_map>
#include <tuple>

using namespace std;

/**
 * Definition of a generic base class ServiceListener to listen to add, update, and remve
 * events on a Service. This listener should be registered on a Service for the Service
 * to notify all listeners for these events.
 */
template<typename V>
class ServiceListener
{

public:

  // Listener callback to process an add event to the Service
  virtual void ProcessAdd(V &data) = 0;

  // Listener callback to process a remove event to the Service
  virtual void ProcessRemove(V &data) = 0;

  // Listener callback to process an update event to the Service
  virtual void ProcessUpdate(V &data) = 0;

};


/**
 * Definition of a Connector class.
 * This will invoke the Service.OnMessage() method for subscriber Connectors
 * to push data to the Service.
 * Services can invoke the Publish() method on this Service to publish data to the Connector
 * for a publisher Connector.
 * Note that a Connector can be publisher-only, subscriber-only, or both publisher and susbcriber.
 */
template<typename V>
class Connector
{

public:

  // Publish data to the Connector
  virtual void Publish(V &data) = 0;
    
  // Read data from the Connector
  virtual void Read() = 0;

};




/**
 * Definition of a generic base class Service.
 * Uses key generic type K and value generic type V.
 */
template<typename K, typename V>
class Service
{
    
protected:
    unordered_map<K, V> map;  // map of identifier K and data V
    
public:
  // checking if key is already in map
  bool ExistingData(K key);
  // adding data to map on key
  void AddData(K key, V& data);
  // updating data to existing key
  void UpdateData(K key, V& data);
  // retriveing data
  V& GetData(K key);
    
  // The callback that a Connector should invoke for any new or updated data
  virtual void OnMessage(V &data) = 0;

};


/*
 * LISTENEDSERVICE CLASS DECLARATION
 */

// Service class that has listeners
template<typename K, typename V>
class ListenedService
    : public Service<K, V>
{
protected:
    vector<ServiceListener<V>*> listeners;  // listeners
    
public:
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<V>* listener);
    
    // Get all listeners on the Service.
    const vector<ServiceListener<V>*>& GetListeners() const;
    
};



/*
 * SERVICE METHODS DEFINITION
 */

// checking if key is already in map
template<typename K, typename V>
bool Service<K, V>::ExistingData(K key){
    return map.find(key) != map.end();
}

// adding data to map on key
template<typename K, typename V>
void Service<K, V>::AddData(K key, V& data){
    map.insert(make_pair(key, data));
}

// updating data to existing key
template<typename K, typename V>
void Service<K, V>::UpdateData(K key, V& data){
    auto it = map.find(key);
    if(it != map.end())
        it->second = data;
}

// retriveing data
template<typename K, typename V>
V& Service<K, V>::GetData(K key){
    return map.at(key);
}



/*
 * LISTENEDSERVICE METHODS DEFINITION
 */

// Add a listener to the Service for callbacks on add, remove, and update events
// for data to the Service.
template<typename K, typename V>
void ListenedService<K, V>::AddListener(ServiceListener<V>* listener){
    listeners.push_back(listener);
};

// Get all listeners on the Service.
template<typename K, typename V>
const vector<ServiceListener<V>*>& ListenedService<K, V>::GetListeners() const{
    return listeners;
};


#endif
