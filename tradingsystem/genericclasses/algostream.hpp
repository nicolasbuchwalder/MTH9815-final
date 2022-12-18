//
//  algostream.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 13.12.22.
//

#ifndef algostream_hpp
#define algostream_hpp

#include "streamingservice.hpp"

/*
 * ALGOSTREAM CLASS DECLARATION
 */

// Template class that keeps a price stream
template<typename T>
class AlgoStream{

private:
    PriceStream<T> price_stream;    // pricestream

public:
    // constructor
    AlgoStream(const PriceStream<T>& _price_stream);
    // retrieving stream
    PriceStream<T>& GetPriceStream();
    // updating stream
    void UpdatePriceStream(const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);
    
};

/*
 * ALGOSTREAM METHODS DEFINITION
 */

// constructor
template<typename T>
AlgoStream<T>::AlgoStream(const PriceStream<T>& _price_stream)
: price_stream(_price_stream)
{};

// retrieving stream
template<typename T>
PriceStream<T>& AlgoStream<T>::GetPriceStream(){
    return price_stream;
}

// updating stream
template<typename T>
void AlgoStream<T>::UpdatePriceStream(const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder){
    price_stream.UpdateStreamOrders(_bidOrder, _offerOrder);
}

#endif /* algostream_hpp */
