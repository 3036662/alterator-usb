#pragma once

#include "types.hpp"

/// @brief CRTP base class for serializable objects
/// @tparam Impl Implementation class
template<typename Impl>
class SerializableForLisp{
public:
    vecPairs SerializeForLisp() const{
       return static_cast<const Impl*> (this)->SerializeForLisp();
    }
};