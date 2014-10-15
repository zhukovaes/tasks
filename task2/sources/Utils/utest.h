/**
 * @file: Utils/utest.h 
 * @defgroup uTest
 * @brief Definition of unit testing tools
 * @ingroup Utils
 */
/*
 * Copyright (C) 2013  Boris Shurygin
 */
#pragma once

#ifndef UTEST_H
#define UTEST_H

#include <string>

namespace Utils
{
    using namespace PrintUtils;
/**
 * Possible test outcomes
 */
enum UTestResult
{
    //Full pass
    UTEST_SUCCESS = 0x0,

    //Assertions failed
    UTEST_ASSERT_FAIL = 0x1,

    //Comparison failed
    UTEST_CMP_FAIL = 0x2
};

//Forward declaration of UnitTest class
class UnitTest;

/**
 * Type of simple testing function pointer
 */
typedef bool (*SimpleTestFuncPtr)();

/**
 * Type of elaborated testing function pointer
 */
typedef bool (*TestFuncPtr)( UnitTest *utest_p);

/**
 * Type of elaborated testing function pointer with out file name
 */
typedef bool (*TestFuncWFileNamePtr)( UnitTest *utest_p, std::string &name);

/**
 * Class for representation of individual check results
 */
class UTestCheck: public Printable<UTestCheck>
{
public:
    /** The full constructor */
    UTestCheck( bool result, const std::string &check, const std::string &place):
        check_text( check), location( place), res( result)
    {}
    void toStream( std::ostream &stm) const
    {
        stm << location << ": " << check_text << " = " << (res ? "pass" : "fail") << endl;
    }
private:
    std::string location;
    std::string check_text;
    bool res;
};

/**
 * Unit test description class
 */
class UnitTest: public Printable<UnitTest>
{
public:
    
    /** Create the empty unit test object and set it up for the work with given streams */
    UnitTest( std::string &tst_name, std::string &o_name, std::ostream& log_strm = std::cerr, std::ostream& out_strm = std::cout);

    /* text stream accessors */
    inline std::ostream& out(){ return out_stream;}; /**< Get stream for test output */
    inline std::ostream& log(){ return log_stream;}; /**< Get stream for log output  */
    
    /**
     * Remember results of individual check along with the textual
     * representation of the checked assertion and its location in the code
     */
    void saveCheckResult( bool res, const char *assertion, const char *file, int line);
    
    bool result() { return main_res == UTEST_SUCCESS;} /**< Get the overall result of the test */
    
    void setAssertFailed() { main_res = main_res | UTEST_ASSERT_FAIL;}/**< Remember that some assertions failed */
    void setCmpFailed() { main_res = main_res | UTEST_CMP_FAIL;}      /**< Remember that comparison with ref data is failed */

    /** Check if some assertions failed */
    bool assertFailed() const { return UTEST_SUCCESS != (main_res & UTEST_ASSERT_FAIL);}

    /** Check if comparison with ref data is failed */
    bool cmpFailed() const { return UTEST_SUCCESS != (main_res & UTEST_CMP_FAIL);}      
    
    UInt32 numFails() const { return num_fail;}     /**< Get the number of failed assertions */
    UInt32 numPasses() const { return num_success;} /**< Get the number of passed assertions */

    const std::string &name() const { return test_name;}    /**< Get the test name string        */
    const std::string &filename() const { return out_name;} /**< Get the output file name string */

    void setRunTime( UInt32 t) { run_time = t; }/**< Set the runtime value */
    UInt32 runTime() const { return run_time; } /**< Get the runtime value */

    /** Get list of individual check results */
    std::list<UTestCheck>& results()
    {
        return checks;
    }

    /** Print the test results */
    void toStream( std::ostream &stm) const
    {
        //stm << "Test " << test_name << ": " << num_fail << " failed, " << num_success << " succeed"<< endl;
        std::list<UTestCheck>::const_iterator it = checks.begin();
        std::list<UTestCheck>::const_iterator end_it = checks.end();
        while ( it != end_it)
        {
            stm << (*it);
            ++it;
        }

    }

private:
    std::string test_name;
    std::string out_name;
    std::ostream& out_stream;
    std::ostream& log_stream;
    std::list<UTestCheck> checks;
    UInt32 num_fail;
    UInt32 num_success;
    UInt8 main_res;
    UInt32 run_time;
};


#define UTEST_CHECK( utest_p, cond)\
     ( utest_p->saveCheckResult(cond, #cond,__FILE__,__LINE__))

/**
 * List of UnitTests with auto-deletion of referenced objects on list destruction
 */
class TestList: public std::list<UnitTest *> 
{
    typedef std::list<UnitTest *>::iterator Iterator;
public:
    
    ~TestList()
    {
        /* Delete all UnitTest objects */
        for ( Iterator it = begin(), end_it = end();
              it != end_it;
              ++it)
        {
            delete *it;        
        }
    }
};

/**
 * Test driver implementation
 */
class TestDriver
{
public:
    /** Run one particular test in simple mode */
    static bool runTest( std::string name, SimpleTestFuncPtr func);
    
    /** Run one particular test in elaborated mode */
    static bool runTest( std::string name, TestFuncPtr func, std::string out_file_name = std::string() );
    static bool runTest( std::string name, TestFuncWFileNamePtr func, std::string out_file_name = std::string() );

    /** Configure output stream */
    static void setLog( std::ostream &log_strm)
    {
        log_stream = &log_strm;
    }
    
    /** Configure output stream */
    static void setRefPath( std::string &ref_path);
    static const std::string &refPath();

    static int returnRes();   /**< Generate return result for main */
    static void printStats(); /**< Print test statistics */
    static void useLogFile( std::string name); /**< Set output file name and open it */
private:
    static std::string ref_data_path;
    static void testHeader( std::string name);
    static void testFooter( std::string name);
    static void compareOut( UnitTest *test_p);
    static void processResult( bool res);//deprecated
    static void processResult( UnitTest *utest);
    static std::ostream *log_stream;
    static std::ofstream file_log;
    static int fail_num;
    static int success_num;
    static TestList tests;
};

} // namespace Utils

#define RUN_TEST( name) Utils::TestDriver::runTest( #name, name)
#define RUN_TEST_OUT_FILE_CHECK( name, file) Utils::TestDriver::runTest( #name, name, std::string(file))

#endif // UTEST_H
