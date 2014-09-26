/**
 * @file: mem_obj.h 
 * Implementation of memory object base class
 */
/*
 * Utils library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_OBJ_H
#define MEM_OBJ_H

/**
 * Namespace for memory manager
 * @ingroup Mem
 */
namespace Mem
{
    /**
     * Base class for all objects allocated in pools
     *
     * @ingroup Mem
     */    
    template < class ClientType, template < class T> class AllocationPolicy = UseDefaultFixedPool>
    class PoolObj: public AllocationPolicy< ClientType>
    {
    public:
        /** Destructor is to be called by 'destroy' routine of pool class */
        virtual ~PoolObj(){};
    };
};

#endif /** MEM_OBJ_H */
