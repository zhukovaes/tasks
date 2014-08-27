/**
 * @file: mem_generic_pool.h 
 * Implementation of generic memory pool with variable entry size
 */
/*
 * Utils library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_GENERIC_POOL_H
#define MEM_GENERIC_POOL_H

namespace Mem
{
    /**
     * @brief Memory pool with arbitrary-sized entries.
     * @ingroup Mem
     *
     * @details  
     * A <em>Generic pool</em> is a pool that allocates entries of the arbitrary size 
     */
    class GenericPool: public Pool
    {
    public:
        
        inline GenericPool();  /**< Create pool with default parameters */
        inline ~GenericPool(); /**< Destroy the pool                    */
                
        inline void* allocate( size_t size); /**< Allocate new memory block */
        inline void deallocate( void *ptr);  /**< Free memory block         */

        inline PoolType type() const; /**< Type of the pool */
    private:        
        /** Number of used entries */
        EntryNum entry_count;
    };

    /** Create pool with default parameters */
    GenericPool::GenericPool(): 
        entry_count( 0)
    {

    }

    /** Destroy the pool */
    GenericPool::~GenericPool()
    {
        /** Check that all entries are freed */
        MEM_ASSERTD( entry_count == 0, "Trying to delete non-empty pool");
    }

    /* Allocate new memory block */
    void* 
    GenericPool::allocate( size_t size)
    {
        MemImpl::Entry< 1> *entry = (MemImpl::Entry< 1> *) malloc( sizeof( MemImpl::Entry< 1>) + size - 1);
        void *ptr = entry->dataMem();
        entry_count++;
        return ptr;
    }

    /** Free memory block */
    void
    GenericPool::deallocate( void *ptr)
    {
        /* 1. Check pointer */
        MEM_ASSERTD( isNotNullP( ptr), "Deallocation tried on NULL pointer");
        
        /* 2. Check entry count */
        MEM_ASSERTD( entry_count > 0, "Trying deallocate entry of an empty pool"); 

        MemImpl::Entry< 1> *entry = MemImpl::Entry< 1>::getEntryPtr( ptr);
        free( ptr);
        entry_count--;
    }

    /** Type of the pool */
    PoolType
    GenericPool::type() const
    {
        return POOL_GENERIC;
    }

    /** Default generic pool */
    typedef Single< GenericPool> DefaultGenericPool;
}; /* namespace Mem */
#endif /* MEM_GENERIC_POOL_H */
