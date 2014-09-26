/**
 * @file: mem_ref.h 
 * Implementation of memory reference template
 */
/*
 * Utils library in compiler prototype project
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_REF_H
#define MEM_REF_H

namespace Mem
{
   /**
     * Memory reference template
     * @ingroup Mem
     */
    template < class PtrObj> class Ptr
    {
    public:
        Ptr(); /**< Default constructor */
        Ptr( const Ptr& orig); /** Copy constructor !!! NO uTEST !!! */

        /** Assignement operator overloading */
        Ptr< PtrObj> & operator=( const Ptr& orig);

        /**
         * Constructor from pointer
         * Used in Ptr p( new Obj(...)); initialization
         * !!! No uTEST !!!
         */
        Ptr( PtrObj* p);

        /**
         * Assignement of pointer
         * Used in Ptr ref; ref = new Obj(...) expression
         */
        Ptr& operator=( PtrObj* p);

        /** Destructor */
        ~Ptr();

        /** Member access operator */
        inline PtrObj* operator->();

        /** Equals operator */
        inline bool operator == ( Ptr< PtrObj> &r);

        /** Conversion to boolean */
        inline operator bool();

        /** Delete operator replacement */
        inline void destroy();

        /** Check that pointer is null */
        inline bool isNull() const;

        /** Check that pointer is not null */
        inline bool isNotNull() const;

        /** Set internal pointer to null */
        inline void setNull();

        /**
         * Template for implicit convertions of smart pointers when conversion
         * of plain poinsters is possible
         */
        template<class newType>
        operator Ptr< newType>()
        {
            return Ptr< newType>( ptr);
        }

#ifdef USE_REF_COUNTERS
        /** Routine for checkers */
        inline RefNumber refCount();
#endif
    private:
        /**
         * Conversion to pointer type disabled,
         * this prohibits expressions like
         * @code
         Ptr< MyClass> ptr = new MyClass();
         
         ...//some actions
         
         delete ptr;
        @endcode
         */
        inline operator PtrObj*();
        
        PtrObj *ptr;
    };

    /** Default constructor */
    template < class PtrObj>
    inline
    Ptr< PtrObj>::Ptr(): ptr( NULL){};
    
    
    /** Copy constructor */
    template < class PtrObj>
    inline
    Ptr< PtrObj>::Ptr( const Ptr< PtrObj>& orig): ptr( orig.ptr)
    {
#ifdef USE_REF_COUNTERS
        if ( ptr != 0)
            MemImpl::getDebugInfo( ptr)->incRefCount();
#endif
    }

    /** Assignement operator overloading */
    template < class PtrObj>
    inline Ptr< PtrObj> &
    Ptr< PtrObj>::operator=( const Ptr< PtrObj>& orig)
    {
#ifdef USE_REF_COUNTERS
        /* Self assignment check */
        if ( this != &orig)
        {
            /** Decrement object's ref count */
            if ( ptr != 0)
                MemImpl::getDebugInfo( ptr)->decRefCount();
            ptr = orig.ptr;
            if ( ptr != 0)
                MemImpl::getDebugInfo( ptr)->incRefCount();
        }
#else
        ptr = orig.ptr;
#endif  
        return *this;
    }
    /**
     * Constructor from pointer
     * Used in Ptr ref = new Obj(...) initialization
     * !!! No uTEST !!!
     */
    template < class PtrObj>
    inline
    Ptr< PtrObj>::Ptr( PtrObj* p): ptr( p)
    {
#ifdef USE_REF_COUNTERS
        if ( ptr != 0)
            MemImpl::getDebugInfo( ptr)->incRefCount();
#endif    
    }
    /**
     * Assignement of pointer
     * Used in Ptr ref; ref = new Obj(...) expression
     */
    template < class PtrObj>
    inline Ptr< PtrObj> &
    Ptr< PtrObj>::operator=( PtrObj* p)
    {
#ifdef USE_REF_COUNTERS
        /** Decrement object's ref count */
        if ( ptr != 0)
            MemImpl::getDebugInfo( ptr)->decRefCount();
#endif    
        /** Assign a new pointer */
        ptr = p;
#ifdef USE_REF_COUNTERS
        /** Increment ref count */
        if ( ptr != 0)
            MemImpl::getDebugInfo( ptr)->incRefCount();
#endif    
        return *this;
    }

    /** Destructor */
    template < class PtrObj>
    Ptr< PtrObj>::~Ptr()
    {
#ifdef USE_REF_COUNTERS
        if ( ptr != 0)
            MemImpl::getDebugInfo( ptr)->decRefCount();
#endif
    }

    /** Member access operator */
    template < class PtrObj>
    inline PtrObj*
    Ptr< PtrObj>::operator->()
    {
        return ptr;
    }

    /** Equals operator */
    template < class PtrObj>
    inline bool
    Ptr< PtrObj>::operator == ( Ptr< PtrObj> &r)
    {
        return ptr == r.ptr;
    }

    /** Conversion to boolean */
    template < class PtrObj>
    inline 
    Ptr< PtrObj>::operator bool()
    {
        return ptr != 0;
    }

    /**
     * Conversion to Pointer
     * For using ref as delete operator argument.
     * Use in other expressions is prohibited. Unfortunatelly this is not ( can't be?) enforced.
     */
    template < class PtrObj>
    inline Ptr< PtrObj>::operator PtrObj*()
    {
        PtrObj *ret_val = ptr;
        MEM_ASSERTD( ret_val != NULL, "Convertion of null pointer to plain pointer"
                                      " is prohibited. Use isNull() method");
#ifdef USE_REF_COUNTERS
        /** Decrement object's ref count */
        if ( ptr != 0)
        {
            MemImpl::getDebugInfo( ptr)->decRefCount();    
            ptr = 0;
        }
#endif
        return ret_val;
    }

    /** Delete operator replacement */
    template < class PtrObj>
    inline void 
    Ptr< PtrObj>::destroy()
    {
#ifdef USE_REF_COUNTERS
        if ( ptr != 0)
        {
            MemImpl::DebugInfo *info = MemImpl::getDebugInfo( ptr);
            info->decRefCount();
            
            MEM_ASSERTD( info->refCount() == 0, "Counter should be zero at destruction moment. Indicates hanging pointers");
        }
#endif
        delete ptr;
        ptr = 0;
    }

    /** Check that pointer is null */
    template < class PtrObj>
    inline bool
    Ptr< PtrObj>::isNull() const
    {
        return ptr == 0;
    }

    /** Check that pointer is not null */
    template < class PtrObj>
    inline bool
    Ptr< PtrObj>::isNotNull() const
    {
        return ptr != 0;
    }

    /** Routine for checkers */
    template < class PtrObj>
    inline void
    Ptr< PtrObj>::setNull()
    {
        *this = 0;
    }

#ifdef USE_REF_COUNTERS
    /** Routine for checkers */
    template < class PtrObj>
    inline RefNumber
    Ptr< PtrObj>::refCount()
    {
        return MemImpl::getDebugInfo( ptr)->refCount();
    }
#endif
};


/**
 * Check if smart pointer is not null
 * @ingroup Misc
 */
template< class T> bool isNullP( const Mem::Ptr< T> ptr)
{
    return ptr.isNull();
}


/**
 * Check if smart pointer is not null
 * @ingroup Misc
 */
template< class T> bool isNotNullPtr( const Mem::Ptr< T> ptr)
{
    return ptr.isNotNull();
}

#endif /* MEM_REF_H */
