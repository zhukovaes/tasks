/**
 * @file: mem_chunk.h 
 * Implementation of memory chunk
 */
/*
 * Utils/Mem library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#    error
#endif

#ifndef MEM_CHUNK_H
#define MEM_CHUNK_H

namespace MemImpl
{
    /**
     * Chunk lists identificators
     * @ingroup MemImpl
     */
    enum ChunkListType
    {
        CHUNK_LIST_ALL,
        CHUNK_LIST_FREE,
        CHUNK_LISTS_NUM
    };

    /**
     * Memory chunk representation
     * @ingroup MemImpl
     */
    template< size_t size> class Chunk: 
        public MListIface< Chunk< size>, // List client data
                           CHUNK_LISTS_NUM > // Lists number
    {
        typedef MListIface< Chunk< size>, CHUNK_LISTS_NUM > List;

        //void *dummy_ptr; //for alignment
        /** position of first free entry */
        ChunkPos free_entry;
        /** busy entries num */
        ChunkPos busy;
        /** maximum block size */
        ChunkPos max_block_size;
        /** Get entry for given number */
        inline FixedEntry< size> *entry( ChunkPos pos) const;
    public:
#ifdef CHECK_CHUNKS
        Mem::Pool *pool;       
        /** Get chunk's first busy entry */
        inline FixedEntry< size> *firstBusyEntry();
#endif
        /** Constructor */
        inline Chunk();
        /** Check if this chunk has free entries */
        inline bool isFree() const;
        /** Check if this chunk is empty */
        inline bool isEmpty() const;
        /** Return number of free entries */
        inline ChunkPos numFree() const;
        /** Initialization of chunk */
        inline void initialize();
        /** Allocate one entry */
        inline void *allocateEntry();
        /** Allocate a number of continuos entries */
        inline void *allocateBlock( ChunkPos num_entries);
        /** Find maximum number of continuos free entries */
        inline ChunkPos maxBlockSize() const;
        /** Deallocate one entry */
        inline void deallocateEntry( FixedEntry< size> *e);
        /** Deallocate one entry */
        inline void deallocateBlock( FixedEntry< size> *e, ChunkPos num_entries);
        /** For some reason GCC asks for it :( */
        inline void operator delete( void* mem);
        /** Placement new */
        inline void *operator new ( size_t alloc_size, void* mem);
        /**
         * Operator 'delete' corresponding to placement new
         * WARNING: Compiler won't call this for deletion. 
         *          It is needed for freeing memory in case of exceptions in constructor
         */
        inline void operator delete( void* ptr, void* mem);

        inline void toStream( std::ostream& os) const;
        inline void toStdErr() const;
        inline void toStdOut() const;

        /** Check that the given pointer belongs to this chunk */
        inline bool checkPtr( void *ptr) const;
    };
    
    /** Constructor */
    template < size_t size> 
    Chunk< size>::Chunk()
    {
        FixedEntry< size> *e = NULL;
        max_block_size = 0;

        /*
         * This loop basicly does new ( mem_ptr)[MAX_CHUNK_ENTRIES_NUM] for entries 
         * should be rewritten in explicit manner, with operator new overriding for Entry class
         */
        for ( int i = 0; i < MAX_CHUNK_ENTRIES_NUM; i++)
        {
            e = ( FixedEntry< size> *)( (UInt8 *) this 
                                       + sizeof( Chunk< size>) 
                                       + sizeof( FixedEntry< size>) * i);
            /** Initialization of every entry */
            e->setPos( i);
            e->setNextFree( i + 1);
#ifdef CHECK_ENTRY
            e->setBusy( false);
#endif
#ifdef USE_MEM_EVENTS        
            e->debugInfo().setAllocEvent( 0);
            e->debugInfo().setDeallocEvent( 0);
#endif 
#ifdef USE_REF_COUNTERS
            e->debugInfo().resetCount();
#endif
            max_block_size++;
        }
        MEM_ASSERTD( e->nextFree() == UNDEF_POS, "Chunk size constant and undefined value do not match");
        free_entry = 0;
        busy = 0;
    }
    /** Placement new */
    template < size_t size> 
    void*
    Chunk< size>::operator new ( size_t alloc_size, void* mem)
    {
        return mem;
    }
    
    /** For some reason GCC asks for it :( */
    template < size_t size>
    void
    Chunk< size>::operator delete( void* mem)
    {

    }
    /**
     * Operator 'delete' corresponding to placement new
     * WARNING: Compiler won't call this for deletion. 
     *          It is needed for freeing memory in case of exceptions in constructor
     */
    template < size_t size> 
    void
    Chunk< size>::operator delete( void* ptr, void* mem)
    {
    
    }
  
    /** Get entry by number */
    template< size_t size> 
    FixedEntry< size>*
    Chunk< size>::entry( ChunkPos pos) const
    {
        MEM_ASSERTD( pos != UNDEF_POS, "Requested entry with undefined number");
        return ( FixedEntry< size> *)( (UInt8 *) this 
                                  + sizeof( Chunk< size>) 
                                  + sizeof( FixedEntry< size>) * pos);
    }
   
#ifdef CHECK_CHUNKS        
    /** Get chunk's first busy entry */
    template< size_t size> 
    FixedEntry< size>*
    Chunk< size>::firstBusyEntry()
    {
        FixedEntry< size> *e = NULL;
        
        for ( int i = 0; i < MAX_CHUNK_ENTRIES_NUM; i++)
        {
            e = ( FixedEntry< size> *)( (UInt8 *) this 
                                       + sizeof( Chunk< size>) 
                                       + sizeof( FixedEntry< size>) * i);
            if ( e->isBusy())
                return e;
        }
        return NULL;
    }
#endif

    /** Check if this chunk has free entries */
    template< size_t size> 
    bool 
    Chunk< size>::isFree() const
    {
        return free_entry != UNDEF_POS;
    }
    /** Check if this chunk is empty */
    template< size_t size> 
    bool 
    Chunk< size>::isEmpty() const
    {
        return busy == 0;
    }      

    /** Check if this chunk is empty */
    template< size_t size> 
    ChunkPos 
    Chunk< size>::numFree() const
    {
        MEM_ASSERTD( busy <= MAX_CHUNK_ENTRIES_NUM,
                     "Internal error: number of busy chunks is greater then max");
        return MAX_CHUNK_ENTRIES_NUM - busy;
    }    
    
    /** Allocate one entry */
    template< size_t size> 
    void*
    Chunk< size>::allocateEntry()
    {
        MEM_ASSERTD( this->isFree(), "Trying to allocated entry in a full chunk");
        
        FixedEntry< size> *e = entry( free_entry);
#ifdef CHECK_ENTRY
        e->setBusy( true);
#endif
#ifdef MEM_CHECK_POOL
        e->debugInfo().setPool( this->pool);
#endif
#ifdef USE_MEM_EVENTS   
        e->debugInfo().setAllocEvent( Mem::MemMgr::instance()->allocEvent());
#endif        
        void *res = e->dataMem();
        free_entry = e->nextFree();
        busy++;
        max_block_size--;
        return res;
    }

    template< size_t size> 
    ChunkPos
    Chunk< size>::maxBlockSize() const
    {
        return max_block_size;
    }

    /** Allocate a number of continuos entries */
    template< size_t size> 
    void*
    Chunk< size>::allocateBlock( ChunkPos num_entries)
    {
        MEM_ASSERTD( this->isFree(), "Trying to allocated entry in a full chunk");
        MEM_ASSERTD( maxBlockSize() >= num_entries, "Trying to allocate more entries than we have");
        
        FixedEntry< size> *e = entry( free_entry);
        void *res = e->dataMem();
        
        for ( ChunkPos num_alloc = 0,
                       pos = free_entry; 
              num_alloc < num_entries;
              num_alloc++)
        {
            FixedEntry< size> *e = entry( pos);
#ifdef CHECK_ENTRY
            MEM_ASSERTD( !e->isBusy(), 
                         "Entry should be free, otherwise we have "
                         "wrongly estimated number of free entries");
            e->setBusy( true);
#endif
#ifdef MEM_CHECK_POOL
            e->debugInfo().setPool( this->pool);
#endif
#ifdef USE_MEM_EVENTS        
            e->debugInfo().setAllocEvent( Mem::MemMgr::instance()->allocEvent());
#endif        
            free_entry = e->nextFree();
            pos++;
            busy++;
            max_block_size--;
        }
        
        return res;
    }

    /** Deallocate one entry */
    template< size_t size> 
    void
    Chunk< size>::deallocateEntry( FixedEntry< size> *e)
    {
        MEM_ASSERTD( busy > 0, "Trying to deallocate entry of an empty chunk");
#ifdef CHECK_ENTRY
        MEM_ASSERTD( e->isBusy(), 
                     "Trying to deallocate entry that is free. Check deallocation event ID");
        e->setBusy( false);
#endif
#ifdef USE_MEM_EVENTS        
        e->debugInfo().setDeallocEvent( Mem::MemMgr::instance()->deallocEvent());
#endif 
        if ( e->pos() + 1 == free_entry)
        {
            max_block_size++;
        } else
        {
            max_block_size = 1;
        }

        e->setNextFree( free_entry);
        free_entry = e->pos();
        busy--;
    }

    /** Deallocate one entry */
    template< size_t size> 
    void
    Chunk< size>::deallocateBlock( FixedEntry< size> *e, ChunkPos num_entries)
    {
        MEM_ASSERTD( busy >= num_entries, "Trying to deallocate too many entries");
        
        // Freeing elements in reverse order 
        for ( ChunkPos num_dealloc = 0,
                       pos = e->pos() + num_entries - 1; 
              num_dealloc < num_entries;
              num_dealloc++)
        {
            e = entry( pos);
#ifdef CHECK_ENTRY
            MEM_ASSERTD( e->isBusy(), 
                         "Trying to deallocate entry that is free. Check deallocation event ID");
            e->setBusy( false);
#endif
#ifdef USE_MEM_EVENTS        
            e->debugInfo().setDeallocEvent( Mem::MemMgr::instance()->deallocEvent());
#endif 
            if ( e->pos() + 1 == free_entry)
            {
                max_block_size++;
            } else
            {
                max_block_size = 1;
            }
            e->setNextFree( free_entry);
            free_entry = e->pos();

            pos--;
            busy--;
        } 
    }

    /** Print chunk to stream for debug purposes */
    template < size_t size> 
    void
    Chunk< size>::toStream( std::ostream& os) const
    {
        os << "Chunk " << this
           << " next " << List::next( CHUNK_LIST_ALL)
           << ", prev " << List::prev( CHUNK_LIST_ALL)
           << ", next_free " << List::next( CHUNK_LIST_FREE)
           << ", prev_free " << List::next( CHUNK_LIST_FREE) << endl;
        os << "Entries\n";
        /* Traverse and print entries */
        for ( int i = 0; i < MAX_CHUNK_ENTRIES_NUM; i++)
        {
            FixedEntry< size> *e = entry( i);
            os << (UInt32)e->pos() << ": ";
#ifdef CHECK_ENTRY
            if ( e->isBusy() ) os << "Busy ";
            else os << "Free ";
#endif
            os << "Next free " << (UInt32) e->nextFree();
            os << "\n";
        }
    }
    /** Print chunk to stream for debug purposes */
    template < size_t size> 
    void
    Chunk< size>::toStdErr() const
    {
        toStream( std::cerr);
    }

    template < size_t size> 
    void
    Chunk< size>::toStdOut() const
    {
        toStream( std::cout);
    }

    /** Check that the given pointer belongs to this chunk */
    template < size_t size> 
    bool 
    Chunk< size>::checkPtr( void *ptr) const
    {
        //First entry ptr
        FixedEntry< size> *first_entry_ptr = entry( 0);
        FixedEntry< size> *last_entry_ptr = entry( MAX_CHUNK_ENTRIES_NUM - 1) + 1;

        // Check range
        if ( ptr < first_entry_ptr || ptr >= last_entry_ptr)
            return false;
        return true;
    }

    template < size_t size> 
    std::ostream& operator<<(std::ostream& os, const Chunk< size> &chunk) 
    {   
        if ( chunk.numFree() == 0 && chunk.isEmpty())
            chunk.toStdOut();
        chunk.toStream(os);
        return os;
    } 

}; /* namespace MemImpl */
#endif /* MEM_CHUNK_H */
