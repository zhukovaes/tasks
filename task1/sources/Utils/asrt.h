/**
 * @file: asrt.h 
 * Assertion related routines of Utils library for SCL (Simple Compiler)
 * @defgroup Asserts Assertions
 * @brief Assertion routines/macros
 * @ingroup Utils
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#ifndef ASRT_H
#define ASRT_H

void fatalAssert(const char *assertion, const char *file, int line);

/* Empty function */
inline void noop() {}

/** 
 * @brief Simple assert macro
 * @ingroup Asserts
 * @param cond The condition
 * @details 
 * Assert macro: in case of failure prints condition, location and dies.
 */
#if !defined(ASSERT)
#  ifndef NO_DEBUG
#    define ASSERT(cond) ((!(cond)) ? fatalAssert(#cond,__FILE__,__LINE__) : noop())
#  else
#    define ASSERT(cond) noop()
#  endif
#endif

void fatalAssertWithMess(const char *where, const char *what, const char *file, int line);

/**
 * @brief Assert macro with description
 * @ingroup Asserts
 * @param cond The condition
 * @param where Description of functionality where failure occured
 * @param what Error message explaining what happened
 *
 * @details
 * Assertion that in case of failure provides additional info about what happened
 */
#if !defined(ASSERT_X)
#  ifndef NO_DEBUG
#    define ASSERT_X(cond, where, what) ((!(cond)) ? fatalAssertWithMess(where, what,__FILE__,__LINE__) : noop())
#  else
#    define ASSERT_X(cond, where, what) noop()
#  endif
#endif


/**
 * @brief Assert macro that works only in debug version
 * @ingroup Asserts
 * @param cond The condition
 * @details 
 * Assert macro that works only in debug version. In case of failure prints condition, location and dies.
 */
#if !defined(ASSERTD)
#  ifdef _DEBUG
#    define ASSERTD(cond) ASSERT(cond)
#  else
#    define ASSERTD(cond) noop()
#  endif
#endif

/**
 * @brief Assert macro with description that works only in debug version
 * @ingroup Asserts
 * @param cond The condition
 * @param where Description of functionality where failure occured
 * @param what Error message explaining what happened
 * @details 
 * Assertion that in case of failure provides additional info about what happened
 */
#if !defined(ASSERT_XD)
#  ifdef _DEBUG
#    define ASSERT_XD(cond, where, what) ASSERT_X(cond, where, what)
#  else
#    define ASSERT_XD(cond, where, what) noop()
#  endif
#endif



/**
 * Generic assertion routine template
 * @ingroup Asserts
 */
template<class Excpt> inline void assert( bool assertion)
{
    if ( !assertion)
    {
        throw Excpt();
    }
}

/**
 * Assertion routine template parameterized with thrown exception type
 * @ingroup Asserts
 */
template<class Excpt> inline void assert( bool asrt, Excpt e)
{
    if ( !asrt)
    {
        throw e;
    }
}

/**
 * Simple assertion routine 
 * @ingroup Asserts
 */
inline void assert( bool asrt)
{
    assert< int>( asrt);
}

/**
 * Assert that works only in debug version
 * @ingroup Asserts
 */
inline void assertd( bool asrt)
{
#ifdef _DEBUG
    assert< int>( asrt);
#endif
}

#endif /* ASRT_H */
