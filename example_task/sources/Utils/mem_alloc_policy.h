/**
 * @file: mem_alloc_policy.h 
 * Implementation of memory allocatin policy classes
 */
/*
 * Utils library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_ALLOC_POLICY_H
#define MEM_ALLOC_POLICY_H

/**
 * Namespace for memory manager
 * @ingroup Mem
 */
namespace Mem
{
    /**
     * Base class of alloction policy. Mostly everything is disabled.
     */
    class ObjAllocPolicy
    {
    public:
        /** Default constructor */
        ObjAllocPolicy(){};

        /** Destructor */
        virtual ~ObjAllocPolicy(){};

        /** Default operator 'new' */
        void *operator new ( size_t size){};
        /** Default operator 'delete' */
        void operator delete( void *ptr){};
        
        /** Default operator 'new' */
        void *operator new[] ( size_t size){};
        /** Default operator 'delete' */
        void operator delete[] ( void *ptr){};

        /** Placement new */
        inline void *operator new ( size_t size, Pool* pool){};
        /**
         * Operator 'delete' corresponding to placement new
         * WARNING: Compiler won't call this for deletion. 
         *          It is needed for freeing memory in case of exceptions in constructor
         */
        inline void operator delete( void *ptr, Pool* pool){};
    private:
        /** Copy constructor disabled*/
        ObjAllocPolicy( ObjAllocPolicy &obj){};
        /** Assignment disabled */
        ObjAllocPolicy& operator = ( const ObjAllocPolicy& obj){ return *this;};
    };


    /**
     * Object allocation policy that uses default fixed pool to allocate memory
     */
    template < class T> class UseDefaultFixedPool: public ObjAllocPolicy
    {
        public:
        /** Default operator 'new' is disabled */
        void *operator new( size_t size)
        {
             MEM_ASSERTD( sizeof( T) == size, "Size passed as new parameter must be equal to the one that"
                                              "was selected for the object with specialization of PoolObj");
             return DefaultPool< T>::ptr()->allocate( size);
        }
        /** Operator 'delete' deallocationg from default pool */
        void operator delete( void *ptr)
        {
            DefaultPool< T>::ptr()->deallocate( ptr);
        }
    };
       
    /**
     * Object allocation policy that uses default generic pool to allocate memory, T parameter is ignored
     */
    template < class T> class UseGenericPool: public ObjAllocPolicy
    {
        public:
        /** Operator 'new' */
        void *operator new( size_t size)
        {
             return DefaultGenericPool::ptr()->allocate( size);
        }
        /** Operator 'delete' */
        void operator delete( void *ptr)
        {
            DefaultGenericPool::ptr()->deallocate( ptr);
        }
    };

    /**
     * Object allocation policy that defines placement new/delete to operate on user-defined pool
     */
    template < class T> class UseCustomFixedPool: public ObjAllocPolicy
    {
        public:
        /**
         * Placement new
         */
        inline void *
        operator new ( size_t size, Pool* pool)
        {
            return static_cast< FixedPool< sizeof( T)>*>(pool)->allocate( size);
        }
        /**
         * Operator 'delete' corresponding to placement new
         * WARNING: Compiler won't call this for deletion. 
         *          It is needed for freeing memory in case of exceptions in constructor
         */
        inline void
        operator delete( void *ptr, Pool* pool)
        {
             static_cast< FixedPool< sizeof( T)>*>(pool)->deallocate( ptr);
        }

        /** Default operator 'delete' */
        void operator delete( void *ptr){};
    };
};

#endif /* MEM_ALLOC_POLICY_H */
