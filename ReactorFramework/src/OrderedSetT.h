#ifndef __ORDERED_SET_T_H__
#define __ORDERED_SET_T_H__

#include "Public.h"

class Allocator;

template<class TYPE>
class OrderedSetDefaultComparator
{
public:
    bool operator ()( const TYPE &lhs, const TYPE &rhs ) const;
};
template<class TYPE>
inline bool OrderedSetDefaultComparator<TYPE>::operator ()( const TYPE &lhs, const TYPE &rhs ) const
{
    return lhs == rhs;
}

template<class TYPE, class COMPARATOR>
class OrderedSet;

template<class TYPE, class COMPARATOR = OrderedSetDefaultComparator>
class OrderSetIterator
{
public:
    typedef OrderedSet<TYPE,COMPARATOR> ContainerType;

    typedef std::forward_iterator_tag               iterator_category;
    typedef typename ContainerType::value_type      value_type;
    typedef typename ContainerType::pointer         pointer;
    typedef typename ContainerType::reference       reference;
    typedef typename ContainerType::difference_type difference_type;
};

#endif

