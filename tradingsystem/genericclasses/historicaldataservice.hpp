/**
 * historicaldataservice.hpp
 * historicaldataservice.hpp
 *
 * @author Breman Thuraisingham
 * Defines the data types and Service for historical data.
 *
 * @author Breman Thuraisingham
 */
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class HistoricalDataService
    : public Service<string,T>
{
private:
    // OnMessage is changed by PersistData for historicaldata services
    virtual void OnMessage(T &data) final{};

public:

  // Persist data to a store
  virtual void PersistData(string persistKey, T& data) = 0;

};

#endif
