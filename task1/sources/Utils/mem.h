/**
 * @file: mem.h 
 * Some experiments with memory manager
 * @defgroup Mem Memory Manager
 *
 * Implementation of memory manager, In Process of Design&Implementation.
 *
 * Memory manager should solve the following memory-related problems
 * - Dangling pointers. Solution: reference counting via smart pointers
 * - Memory leaks. Solution: Various checks in pools and in pointers
 * - Fragmentation. Solution: Fixed-sized chunks used in pools
 * - Poor locality. Solution: not clear yet, except for pooled objects :(
 * @ingroup Utils
 */
/*
 * Utils library in compiler prototype project tool
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef MEM_H
#define MEM_H

#include <new>

#undef CHECK_CHUNKS
#undef CHECK_ENTRY
#undef USE_REF_COUNTERS
#undef USE_MEM_EVENTS
#undef CHECK_DELETE

#ifdef _DEBUG
#  define USE_DEBUG_INFO
#  define CHECK_CHUNKS
#  define CHECK_ENTRY
#  define USE_REF_COUNTERS
#  define USE_MEM_EVENTS
#  define CHECK_DELETE
#  define MEM_CHECK_POOL
#  define COLLECT_POOL_STAT
#endif

#define MEM_USE_MALLOC
//#define MEM_NO_ASSERTS

/**
 * Debug assert in memory manager
 * @ingroup Mem
 */
#if !defined(MEM_ASSERTD)
#  ifndef MEM_NO_ASSERTS
#    define MEM_ASSERTD(cond, what) ASSERT_XD(cond, "Memory manager", what)
#  else
#    define MEM_ASSERTD(cond, what)
#  endif
#endif

#define MEM_LOG( message) LOGS( Utils::LOG_UTILS_MEM, message)
#define MEM_LOG_INC_INDENT LOG_INC_INDENT( Utils::LOG_UTILS_MEM)
#define MEM_LOG_DEC_INDENT LOG_DEC_INDENT( Utils::LOG_UTILS_MEM)

/**
 * Namespace for memory-related routines
 * @ingroup Mem
 */
namespace Mem
{
    /* Class pool predeclaration */
    class Pool;
    
    /** Type of reference count */
    typedef unsigned int RefNumber;
}



/**
 * Low level functinality for Mem package 
 * @defgroup MemImpl Memory Manager Low Level
 * @ingroup Mem
 */
namespace MemImpl
{
    /* Predeclaration on MemInfo */
    class MemInfo;
    /* Predeclaration of mem entry class */
    template < size_t size> class Entry;

    /* Predeclaration of mem entry class */
    template < size_t size> class FixedEntry;
    
    /* Predeclaration of chunk class */
    template < size_t size> class Chunk;
    /**
     * Position in chunk type
     * @ingroup MemImpl
     */
    typedef UInt8 ChunkPos;
    /** 
     * Max number of entries in chunk
     * @ingroup MemImpl
     */
#ifndef MEM_SMALL_CHUNKS
    const UInt8 MAX_CHUNK_ENTRIES_NUM = ( UInt8)( -1);
#else
    const UInt8 MAX_CHUNK_ENTRIES_NUM = 2;
#endif
    /**
     * 'NULL' equivalent for ChunkPos
     * @ingroup MemImpl
     */
    const ChunkPos UNDEF_POS = MAX_CHUNK_ENTRIES_NUM; 
};

namespace Mem
{
    /**
     * Singleton for memory manager
     * @ingroup MemImpl
     */
    typedef Single< MemImpl::MemInfo> MemMgr;
    
    class Pool;
};



#include "mem_mgr.h"          /** Memory manager */
#include "mem_chunk.h"        /** Memory chunk class */
#include "mem_pool.h"         /** Memory pool */
#include "mem_entry.h"        /** Memory entry class */
#include "mem_ref.h"          /** Memory reference */
#include "mem_fixed_pool.h"   /** Memory pool */
#include "mem_generic_pool.h" /** Memory pool */
#include "mem_alloc_policy.h" /** Allocation policies */
#include "mem_obj.h"          /** Memory object base class */
#include "mem_pool_alloc.h"   /** Pool-based allocator */

#endif /* MEM_H */
