/**
 * @file: mem_pool.h 
 * Implementation of memory pool
 */
/*
 * Utils library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_POOL_H
#define MEM_POOL_H

namespace Mem
{
    /**
     * Type for  memory entry size  
     * @ingroup Mem
     */
    typedef UInt32 EntrySize;

    /** 
     * Type for number of memory entries 
     * @ingroup Mem
     */
    typedef UInt32 EntryNum;

    /**
     * Pool types
     * @ingroup Mem
     */
    enum PoolType
    {
        /** Fixed-size entry pool */
        POOL_FIXED,
        /** Variable size entry pool */
        POOL_GENERIC,
        /** Number of types */
        POOL_TYPES_NUM
    };

    /**
     * Base class for memory pools
     *
     * @ingroup Mem
     */
    class Pool
    {
    public:
        /** Type of the pool */
        virtual PoolType type() const = 0;

        /** Destructor */
        virtual ~Pool(){};
 
    };

}; /* namespace Mem */
#endif /* MEM_POOL_H */
