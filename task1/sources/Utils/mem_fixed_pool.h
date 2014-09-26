/**
 * @file: mem_fixed_pool.h 
 * Implementation of memory pool with fixed entry size
 */
/*
 * Utils library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_FIXED_POOL_H
#define MEM_FIXED_POOL_H

namespace Mem
{
    /**
     * @brief Memory pool with fixed-size entries.
     * @ingroup Mem
     * @param Data Type of objects stored in pool.
     *
     * @details  
     * A <em>Fixed pool</em> is a pool that creates entries of the same size which
     * is defined by template parameter. It uses the knowledge of the entry size for
     * optimization of allocation/deallocation process and for simplifying the internal
     * bookkeeping. This simplification significantly speeds up pool's operation.
     *
     * Internal implementation of a fixed pool is based on allocating memory in so-called
     * <em>chunks</em>. A <em>chunk</em> is a continuous block of memory which holds
     * a number of entries and is allocated with one system call. The allocated memory
     * is managed by the pool and released to system when pool doesn't need it. This
     * policy effectively avoids calling malloc() and free() routines for every entry
     * and prevents fragmentation. FixedPool internal implementattion trades off compactness
     * for quickness and uses additional info for every entry. Every entry keeps:
     *  - Its own number. To calculate pointer to chunk info from pointer to entry.
     *  - Number of next free entry in this chunk. For speeding up the allocation.
     *  - Debug info like allocation/deallocation event number or ``free'' flag for checking 
     *    double deallocation.
     *
     * The chunks are organized in doubly-liked list to have constant time for chunk 
     * creation and destruction. Free chunks are also linked in list to speed up the 
     * allocation. On allocation request pool simply gets its first free chunk and 
     * returns pointer to its first entry. On deallocation, pointer to chunk is calculated
     * from pointer to entry using its own number and entry is simply connected to the
     * head of free entry list in the chunk. The chunk itself is enrolled in the list of
     * free chunks if it isn't already there. If the chunk is completely empty after 
     * this deallocation it may be deleted if there are other free chunks present. Both
     * allocation and deallocation demand constant time unless we have to allocate a new
     * chunk or free existing empty one.
     *
     * To decrease memory usage overhead the numbers in entries are effectively one byte long.
     * Thats two byte per entry overhead if we don't align entries on 8, 16, 32 or 64 bytes.
     * Thus memory overhead is significant if we store small objects in such a pool but for 
     * a list unit this overhead is about 25% and for Graph's node or edge it is 6.2%.
     *
     * When project is built in debug mode the pools keep track of every entry's allocation
     * and deallocation ID. This allows programmer to put in a breakpoint if a memory leak
     * or double-delete occurred. The conditional breakpoint should be placed in
     * MemInfo::allocReg( n) or MemInfo::deallocReg( n) where 'n' should be the ID that
     * can be obtained from suspicious entry.
     */
    template < size_t size> 
    class FixedPool: public Pool
    {
        static const size_t CHUNK_SIZE = sizeof( MemImpl::Chunk< size>) 
            + sizeof( MemImpl::FixedEntry< size>) * MemImpl::MAX_CHUNK_ENTRIES_NUM;
    public:
        /** Create fixed pool with default parameters */
        FixedPool();
        
        /** Destroy the pool */
        ~FixedPool();
                
        /** Allocate new memory block */
        void* allocate( size_t alloc_size);
        
        /**
         * Allocate an array of n elements of size 'size'. Throws std::bad_alloc
         * in case of requsting number of elements greater than the size of a chunk
         */
        void* allocate( size_t alloc_size, UInt32 n);

        /** Free memory entry */
        void deallocate( void *ptr);

        /** Free memory block */
        void deallocate( void *ptr, UInt32 n);

        /** Type of the pool */
        virtual PoolType type() const;
#ifdef _DEBUG
        /** Get first busy chunk */
        inline MemImpl::Chunk< size> *firstBusyChunk();
#endif
        /** Print */
        void stat2Stream( std::ostream &os) const;
    private:
#ifdef COLLECT_POOL_STAT
        UInt64 num_entries_alloced;
        UInt64 num_entries_dealloced;
        UInt64 chunks_alloced;
        UInt64 alive_chunks;
        UInt64 free_chunks;
#endif 
        /** Number of used entries */
        EntryNum entry_count;
        /** First chunk */
        MemImpl::Chunk< size> *first_chunk;
        /** First free chunk */
        MemImpl::Chunk< size> *free_chunk;

        /* Internal routines */
        
        /** Allocate one chunk */
        inline MemImpl::Chunk< size> *allocateChunk();
        /** Deallocate one chunk */
        inline void deallocateChunk( MemImpl::Chunk< size> *chunk);
        /** Get pointer to chunk from pointer to entry */
        inline MemImpl::Chunk< size> *entryChunk( MemImpl::FixedEntry< size> *e);
    };

    /** Create fixed pool with default parameters */
    template < size_t size> 
    FixedPool< size>::FixedPool(): 
        entry_count( 0),
        first_chunk( NULL),
        free_chunk( NULL)
    {
#ifdef COLLECT_POOL_STAT
        num_entries_alloced = 0;
        num_entries_dealloced = 0;
        chunks_alloced = 0;
        alive_chunks = 0;
        free_chunks = 0;
#endif 
    }

    /** Destroy the pool */
    template < size_t size> 
    FixedPool< size>::~FixedPool()
    {
        /** Deallocated cached chunks */
        while ( isNotNullP( first_chunk))
        {
            deallocateChunk( first_chunk);
        }
        /** Check that all entries are freed */
        MEM_ASSERTD( entry_count == 0, "Trying to delete non-empty pool");
    }

    /** Type of the pool */
    template < size_t size> 
    PoolType
    FixedPool< size>::type() const
    {
        return POOL_FIXED;
    }

#ifdef _DEBUG
    /** Get first busy chunk */
    template < size_t size> 
    MemImpl::Chunk< size> *
    FixedPool< size>::firstBusyChunk()
    {
        MemImpl::Chunk< size> *chunk = first_chunk;
        while ( isNotNullP( chunk))
        {
            if ( !chunk->isEmpty())
                return chunk;
            chunk = chunk->next( MemImpl::CHUNK_LIST_ALL);
        }
        return NULL;
    }
#endif
    /** Allocate one chunk */
    template < size_t size> 
    MemImpl::Chunk< size> *
    FixedPool< size>::allocateChunk()
    {
        /* We should only allocate chunk if there are no free chunks left */
        //MEM_ASSERTD( isNullP( free_chunk ), "Tried to deallocate chunk while there is a free chunk");
        
        /* Allocate memory for chunk */
        void *chunk_mem = 
#ifdef MEM_USE_MALLOC              
            ( MemImpl::Chunk< size> *) malloc( sizeof( UInt8) * CHUNK_SIZE);
#else
            ( MemImpl::Chunk< size> *) new UInt8[ CHUNK_SIZE];
#endif
        MemImpl::Chunk< size> * chunk = new ( chunk_mem) MemImpl::Chunk< size>();

        /* Add this chunk to pool */
        chunk->attach( MemImpl::CHUNK_LIST_ALL, first_chunk);
        chunk->attach( MemImpl::CHUNK_LIST_FREE, free_chunk);
        first_chunk = chunk;
        free_chunk = chunk;
        
#ifdef CHECK_CHUNKS
        chunk->pool = this;
#endif

#ifdef COLLECT_POOL_STAT
        chunks_alloced++;
        alive_chunks++;
        free_chunks++;
#endif 
        return chunk;
    }
    
    /** Deallocate one chunk */
    template < size_t size> 
    void
    FixedPool< size>::deallocateChunk( MemImpl::Chunk< size> *chunk)
    {
#ifdef CHECK_CHUNKS
        if ( !chunk->isEmpty())
        {
            MEM_ASSERTD( isNotNullP( chunk->firstBusyEntry()),
                         "Can't get first busy entry of non-empty chunk");
            MEM_ASSERTD( 0, "Deallocated chunk is not empty. Check allocation ID of some busy entry");
        }
        MEM_ASSERTD( areEqP( chunk->pool, this), "Deallocated chunk does not belong to this pool");
#endif
        if ( areEqP( first_chunk, chunk))
        {
            first_chunk = chunk->next( MemImpl::CHUNK_LIST_ALL);
        }
        chunk->MemImpl::Chunk< size>::~Chunk();
#ifdef MEM_USE_MALLOC 
        free( chunk);
#else
        delete[] (UInt8 *)chunk;
#endif
#ifdef COLLECT_POOL_STAT
        alive_chunks--;
        free_chunks--;
#endif 
    }

    /* Calculate pointer to chunk from pointer to entry */
    template < size_t size> 
    MemImpl::Chunk< size> *
    FixedPool< size>::entryChunk( MemImpl::FixedEntry< size> *e)
    {
        MemImpl::ChunkPos e_pos = e->pos();
        UInt8 *ptr = ( UInt8 *) e;
        ptr = ptr - sizeof( MemImpl::FixedEntry< size>) * e_pos - sizeof ( MemImpl::Chunk< size>);
        return (MemImpl::Chunk< size> *) ptr;
    }

    /* Allocate new memory block */
    template < size_t size> 
    void* 
    FixedPool< size>::allocate( size_t sz)
    {
        MEM_ASSERTD( size == sz,
                     "Allocation size doesn't match FixedPool's template parameter size");
        void *ptr = NULL;
        /* If we don't have a free chunk */
        if ( isNullP( free_chunk))
        {
            /* We need to create new chunk */
            allocateChunk();
        } 
        MEM_ASSERTD( free_chunk->isFree(), "Pool's first free chunk is not free");
        /* allocate one entry */
        ptr = ( void *)free_chunk->allocateEntry();
        /* if no more entries left */
        if ( !free_chunk->isFree())
        {
#ifdef COLLECT_POOL_STAT
        free_chunks--;
#endif 
            MemImpl::Chunk< size> *chunk = free_chunk;
            free_chunk = chunk->next( MemImpl::CHUNK_LIST_FREE);
            chunk->detach( MemImpl::CHUNK_LIST_FREE);
        }
        entry_count++;

#ifdef COLLECT_POOL_STAT
        num_entries_alloced++;
#endif 
        return ptr;
    }

    /**
     * Allocate new memory block
     * TODO: should be further optimized. Namely the call to maxBlockSize() significantly slows down the allocation
     */
    template < size_t size> 
    void* 
    FixedPool< size>::allocate( size_t sz, UInt32 n)
    {
        /* Optimized for single allocation */
        if ( n == 1)
            return allocate( sz);

        MEM_ASSERTD( size == sz,
                     "Allocation size doesn't match FixedPool's template parameter size");
        void *ptr = NULL;
        
        MEM_LOG( "Allocating block of size " << n << "\n");
        MEM_LOG_INC_INDENT;

        /*
         * We can only allocate MemImpl::MAX_CHUNK_ENTRIES_NUM elements at once
         * so we fail if we are asked to allocate more
         */
        if ( n > MemImpl::MAX_CHUNK_ENTRIES_NUM)
            throw std::bad_alloc();

        /* If we don't have a free chunk */
        if ( isNullP( free_chunk) || free_chunk->maxBlockSize() < n)
        {
            /* We need to create new chunk */
            MEM_LOG( "Allocating new chunk\n");
        
            allocateChunk();
        
            MEM_LOG( "Allocated at " << free_chunk << "\n");
        } 

        MEM_ASSERTD( free_chunk->isFree(), "Pool's first free chunk is not free");

        /* allocate requsted number of entries */
        ptr = ( void *)free_chunk->allocateBlock( n);
       
        MEM_LOG( "Allocated block of size " << n << " at " << ptr << "\n");
       
        /* if no more entries left */
        if ( !free_chunk->isFree())
        {
#ifdef COLLECT_POOL_STAT
            free_chunks--;
#endif 
            MemImpl::Chunk< size> *chunk = free_chunk;
            free_chunk = chunk->next( MemImpl::CHUNK_LIST_FREE);
            chunk->detach( MemImpl::CHUNK_LIST_FREE);
        }
        entry_count += n;
        
        MEM_LOG_DEC_INDENT;

#ifdef COLLECT_POOL_STAT
        num_entries_alloced += n;
#endif         
        return ptr;
    }

    /** Free memory block */
    template < size_t size> 
    void
    FixedPool< size>::deallocate( void *ptr)
    {
        /* 1. Check pointer */
        MEM_ASSERTD( isNotNullP( ptr), "Deallocation tried on NULL pointer");
        
        /* 2. Check entry count */
        MEM_ASSERTD( entry_count > 0, "Trying deallocate entry of an empty pool"); 

        MemImpl::FixedEntry< size> *e =static_cast< MemImpl::FixedEntry< size> *>( MemImpl::Entry< size>::getEntryPtr( ptr));
        
        /* 3. Get chunk of the deallocated entry */
        MemImpl::Chunk< size> *chunk = entryChunk( e);

#ifdef CHECK_CHUNKS
        /* 4. Check that we are deleting entry from this pool */
        MEM_ASSERTD( areEqP( this, chunk->pool), "Trying deallocate entry from a wrong pool");
        MEM_ASSERTD( chunk->checkPtr( ptr), "Pointer is not valid for the chunk");
#endif
        /*
         * 5. If chunk is free already - it must be in free list 
         * no need to add it again
         */
        bool add_to_free_list = !chunk->isFree();

        /* 6. Free entry in chunk */
        chunk->deallocateEntry( e);

#ifdef COLLECT_POOL_STAT
        num_entries_dealloced++;
#endif        
        /*
         * 7. If this chunk is not the same as the current 'free chunk' 
         *     add it to free list or deallocate it if it is empty
         */
        if ( areNotEqP( chunk, free_chunk))
        {
            if ( add_to_free_list)
            {
#ifdef COLLECT_POOL_STAT
                free_chunks++;
#endif   
                /* Add the chunk to free list if it is not already there */
                chunk->attach( MemImpl::CHUNK_LIST_FREE, free_chunk);
                /* Deallocate previous free chunk if it is empty */
                if ( isNotNullP( free_chunk) && free_chunk->isEmpty())
                {
                    deallocateChunk( free_chunk);
                }
                free_chunk = chunk;
            } else if ( chunk->isEmpty())
            {
                /* Deallocate this chunk if it is empty and not the first free chunk */
                deallocateChunk( chunk);
            }
        }
        entry_count--;
    }

    /** Free memory block */
    template < size_t size> 
    void
    FixedPool< size>::deallocate( void *ptr, UInt32 n)
    {
        if ( n == 0)
            return;
        if ( n == 1)
        {
            deallocate( ptr);
            return;
        }

        /* 1. Check pointer */
        MEM_ASSERTD( isNotNullP( ptr), "Deallocation tried on NULL pointer");
        
        /* 2. Check entry count */
        MEM_ASSERTD( entry_count > 0, "Trying deallocate entry of an empty pool"); 

        MemImpl::FixedEntry< size> *e =static_cast< MemImpl::FixedEntry< size> *>( MemImpl::Entry< size>::getEntryPtr( ptr));
        
        /* 3. Get chunk of the deallocated entry */
        MemImpl::Chunk< size> *chunk = entryChunk( e);

#ifdef CHECK_CHUNKS
        /* 4a. Check that we are deleting entry from this pool */
        MEM_ASSERTD( areEqP( this, chunk->pool), "Trying deallocate entry from a wrong pool"); 

        /* 4b. Check that all of the deallocated entries belong to one chunk */
        MEM_ASSERTD( chunk->checkPtr( ptr), "Pointer is not valid for the chunk");
        MEM_ASSERTD( chunk->checkPtr( (UInt8 *)ptr + size * (n - 1)), "Last object pointer is not valid for the chunk");
#endif
        /*
         * 5. If chunk is free already - it must be in free list 
         * no need to add it again
         */
        bool add_to_free_list = !chunk->isFree();

        /* 6. Free block in chunk */
        chunk->deallocateBlock( e, n);

#ifdef COLLECT_POOL_STAT
        num_entries_dealloced+= n;
#endif        
        /*
         * 7. If this chunk is not the same as the current 'free chunk' 
         *     add it to free list or deallocate it if it is empty
         */
        if ( areNotEqP( chunk, free_chunk))
        {
            if ( add_to_free_list)
            {
#ifdef COLLECT_POOL_STAT
                free_chunks++;
#endif   
                /* Add the chunk to free list if it is not already there */
                chunk->attach( MemImpl::CHUNK_LIST_FREE, free_chunk);
                /* Deallocate previous free chunk if it is empty */
                if ( isNotNullP( free_chunk) && free_chunk->isEmpty())
                {
                    deallocateChunk( free_chunk);
                }
                free_chunk = chunk;
            } else if ( chunk->isEmpty())
            {
                /* Deallocate this chunk if it is empty and not the first free chunk */
                deallocateChunk( chunk);
            }
        }
        entry_count-= n;
    }

    /**
     * Pool for objects of the given type
     */
    template < class Type> 
    class TypedPool: public FixedPool< sizeof( Type)>
    {
    public:
        /** Functionality of 'operator delete' for pooled objects */
        inline void destroy( Type *ptr); 
    };

    /** Functionality of 'operator delete' for pooled objects */
    template < class Type> 
    void
    TypedPool< Type>::destroy( Type *ptr)
    {
        /* 1. Check null pointer( in DEBUG mode) */
        MEM_ASSERTD( isNotNullP( ptr), "Destruction tried on NULL pointer");
               
        /* 2. Call destructor */
        ptr->~Type();
        
        /* 3. Free memory */
        this->deallocate( ptr);
    }

    /** Create fixed pool with default parameters */
    template < size_t size> 
    void
    FixedPool< size>::stat2Stream( std::ostream &os) const
    {
#ifdef COLLECT_POOL_STAT
        os << "Pool " << this << "( entry size " << size << " ) statistics:\n";
        os << "  Memory used " << entry_count * size << " ( " << entry_count << " entries)\n"; 
        os << "  entries alloced   : " << num_entries_alloced << "\n";
        os << "  entries dealloced : " << num_entries_dealloced << "\n";
        os << "  chunks alloced    : " << chunks_alloced << "\n";
        os << "  alive chunks      : " << alive_chunks << "\n";
        os << "  free chunks       : " << free_chunks << endl;
#endif 
    }

    /**
     * Default pool for a typed objects
     */
    template < class T> class DefaultPool: public Single< TypedPool< T> >
    {
    
    };
}; /* namespace Mem */
#endif /* MEM_FIXED_POOL_H */
