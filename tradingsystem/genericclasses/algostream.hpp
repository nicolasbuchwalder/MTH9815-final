//
//  algostream.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 13.12.22.
//

#ifndef algostream_hpp
#define algostream_hpp

#include "streamingservice.hpp"

template<typename T>
class AlgoStream{

private:
    PriceStream<T> price_stream;

public:
    AlgoStream(const PriceStream<T>& _price_stream);
    PriceStream<T>& GetPriceStream();
    void UpdatePriceStream(const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);
    
};

template<typename T>
AlgoStream<T>::AlgoStream(const PriceStream<T>& _price_stream)
: price_stream(_price_stream)
{};

template<typename T>
PriceStream<T>& AlgoStream<T>::GetPriceStream(){
    return price_stream;
}

template<typename T>
void AlgoStream<T>::UpdatePriceStream(const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder){
    price_stream.UpdateStreamOrders(_bidOrder, _offerOrder);
}

#endif /* algostream_hpp */
