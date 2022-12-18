//
//  algoexecution.hpp
//  tradingsystem
//
//  Created by Nicolas Buchwalder on 13.12.22.
//

#ifndef algoexecution_hpp
#define algoexecution_hpp

#include "executionservice.hpp"

template<typename T>
class AlgoExecution{

private:
    ExecutionOrder<T> execution_order;

public:
    AlgoExecution(const ExecutionOrder<T>& _execution_order);
    ExecutionOrder<T>& GetExecutionOrder();
    
};

template<typename T>
AlgoExecution<T>::AlgoExecution(const ExecutionOrder<T>& _execution_order)
: execution_order(_execution_order)
{};

template<typename T>
ExecutionOrder<T>& AlgoExecution<T>::GetExecutionOrder(){
    return execution_order;
}

#endif /* algoexecution_hpp */
