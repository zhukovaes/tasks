/**
 * @file: mem_pool_alloc.h 
 * Implementation of adapter of pools for using them as STL-like allocator
 */
/*
 * Utils library in compiler prototype project
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_POOL_ALLOC_H
#define MEM_POOL_ALLOC_H

namespace Mem
{

/**
 * @brief Allocator that uses TypedPool for allocation logic implementation
 * @ingroup Mem
 *
 * @details  
 * Allocator that uses TypedPool for allocation logic implementation. Due to usage 
 * chunks in the pool implementation this allocator is only suitable for containers
 * that only allocate one object at a time (tested with std::list and std::map).
 */
template <class T>
class PoolAllocator
{
public:
    // type definitions
    typedef T        value_type;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    /** Rebind allocator to type U */
    template <class U> struct rebind
    {
        typedef PoolAllocator<U> other;
    };

    /** Get the address of value */
    pointer address( reference value) const;

    /** Get the constant pointer to the value */
    const_pointer address( const_reference value) const;

    /* Contructor, copy constructor and destructor */
    PoolAllocator() throw();
    PoolAllocator( const PoolAllocator&) throw();
    ~PoolAllocator() throw();

    /** Conversion from any other pool allocator */
    template < class U> PoolAllocator(const PoolAllocator<U>&) throw() {};

    /**
     * Return maximum number of elements that can be allocated( on Linux)
     * and the maximum size of the container on Windows
     */
    size_type max_size() const throw();

    /** Allocation of memory for the given number of objects */
    pointer allocate( size_type num, const void* hint = 0);
   
    /** Call copy costructor on the given pointer */
    inline void construct( pointer p, const T& value);
    
    /** Destroy one object by directly calling its destructor */
    inline void destroy( pointer p);
   
    /** Free memory at given pointer for a number of elements */
    inline void deallocate( pointer p, size_type num);
private:
    /** Get reference to the underliyng pool */
    inline TypedPool<T> &pool();
};


/**
 * Get reference to the underliyng pool
 * The pool is under automatic control of singleton
 */
template <class T>
TypedPool<T> &
PoolAllocator<T>::pool()
{
    return AutoSingle< TypedPool<T> >::instance(); 
}

/**
 * Get the address of value
 */
template <class T>
typename PoolAllocator<T>::pointer 
PoolAllocator<T>::address( typename PoolAllocator<T>::reference value) const
{
    return &value;
}
 
/**
 * Get the constant pointer to the value
 */
template <class T>
typename PoolAllocator<T>::const_pointer 
PoolAllocator<T>::address( typename PoolAllocator<T>::const_reference value) const
{
    return &value;
}

/** Constructor. Empty as the allocator has no state */
template <class T>
PoolAllocator<T>::PoolAllocator() throw() {}           

/** Copy constructor. Empty as the allocator has no state */
template <class T>
PoolAllocator<T>::PoolAllocator(const PoolAllocator&) throw() {}

template <class T>
PoolAllocator<T>::~PoolAllocator() throw() {}

/**
 * Return maximum number of elements that can be allocated( on Linux) and the maximum size of the container on Windows
 */
template <class T>
typename PoolAllocator<T>::size_type PoolAllocator<T>::max_size() const throw() 
{
#if defined ( OS_LINUX)
        return MemImpl::MAX_CHUNK_ENTRIES_NUM;
#elif defined ( OS_WIN)
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
#endif
}

/**
 * Allocation of memory for the given number of objects. 
 * NOTE: The hint is ignored in this implementation.
 */
template <class T> 
typename PoolAllocator<T>::pointer 
PoolAllocator<T>::allocate( typename PoolAllocator<T>::size_type num,
                            const void* hint) 
{
    return (pointer) pool().allocate( sizeof(T), num);
}

/**
 * Call copy costructor on the given pointer
 */
template <class T> 
void
PoolAllocator<T>::construct( typename PoolAllocator<T>::pointer p,
                             const T& value)
{
    // use placement new to call the constructor
    new ( (void*)p) T(value);
}
 
/**
 * Destroy one object by directly calling its destructor
 */
template <class T> 
void
PoolAllocator<T>::destroy( typename PoolAllocator<T>::pointer p)
{
    p->~T();
}

/**
 * Free memory at given pointer for a number of elements
 */
template <class T> 
void
PoolAllocator<T>::deallocate( typename PoolAllocator<T>::pointer p,
                              typename PoolAllocator<T>::size_type num)
{
    pool().deallocate( p, num);
}

/**
 * Two instantiations of Pool allocator are interchangeable
 */
template <class T1, class T2>
bool operator== ( const PoolAllocator<T1>&,
                  const PoolAllocator<T2>&) throw()
{
    return true;
}

template <class T1, class T2>
bool operator!= (const PoolAllocator<T1>&,
                const PoolAllocator<T2>&) throw()
{
    return false;
}

};

#endif