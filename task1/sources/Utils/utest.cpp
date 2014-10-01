/**
 * @file: utest.cpp 
 * Implementation of unit testing tools
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

namespace Utils
{

/** Create the empty unit test object and set it up for the work with given streams */
UnitTest::UnitTest( std::string &tst_name,
                    std::string &o_name,
                    std::ostream& log_strm,
                    std::ostream& out_strm):
    test_name( tst_name),
    out_name( o_name),
    log_stream( log_strm), 
    out_stream( out_strm),
    main_res( UTEST_SUCCESS),
    num_fail( 0), 
    num_success( 0)
{

}

/**
 * Remember results of individual check along with the textual
 * representation of the checked assertion and its location in the code
 */
void UnitTest::saveCheckResult( bool res, const char *assertion, const char *file, int line)
{
    if ( res)
    {
        ++num_success;
    } else
    {
        ++num_fail;
        setAssertFailed();
        std::string check_text(assertion);
        std::ostringstream place_str_strm;
        place_str_strm << file << " line " << line;    
        UTestCheck check( res, check_text, place_str_strm.str() );
        checks.push_back( check);
    }
}

/* Static data definitions for TestDriver class */
int TestDriver::fail_num = 0;
int TestDriver::success_num = 0;
std::ostream *TestDriver::log_stream( &std::cerr);
TestList TestDriver::tests;
std::ofstream TestDriver::file_log;
std::string TestDriver::ref_data_path("ref"); 


void TestDriver::setRefPath( std::string &ref_path)
{
    ref_data_path = ref_path;
}

const std::string &
    TestDriver::refPath()
{
    return ref_data_path;
}

bool TestDriver::runTest( std::string name, SimpleTestFuncPtr func)
{
    testHeader( name);
    bool res = func();
    processResult( res);
    testFooter( name);
    return res;
}

bool TestDriver::runTest( std::string name, TestFuncPtr func, std::string out_file_name)
{
    //Print header line
    testHeader( name);
    
    ofstream out_file_strm;//will close automatically
    bool use_file = false;
    
    if ( !out_file_name.empty())
    {
        //NOTE: the c_str is used because on Linux in C++98 the 
        //      open routine is defined with const char * argument only
        out_file_strm.open( out_file_name.c_str());
        if ( out_file_strm.is_open() )
            use_file= true;
    }

    UnitTest *test_p;
    if ( use_file)
    {
        test_p = new UnitTest( name, out_file_name, *log_stream, out_file_strm);
    } else
    {
        test_p = new UnitTest( name, out_file_name, *log_stream);
    }

    //Invoke the testing routine 
    Timer timer(true);
    bool res = func( test_p);//results are saved to test
    tests.push_back( test_p);
    test_p->setRunTime( timer.elapsedUSec() ); 

    if ( use_file)
    {
        out_file_strm.close();
    }

    compareOut( test_p);
    processResult( test_p);
    testFooter( name);
    return res;
}

//NOTE: The invoked testing function is responsible for opening and closing the file
bool TestDriver::runTest( std::string name,
                          TestFuncWFileNamePtr func,
                          std::string out_file_name) 
{
    //Print header line
    testHeader( name);
    UnitTest *test_p = new UnitTest( name, out_file_name, *log_stream);
    
    //Invoke the testing routine 
    Timer timer(true);
    bool res = func( test_p, out_file_name);//results are saved to test
    test_p->setRunTime( timer.elapsedUSec() );    

    tests.push_back( test_p);
    compareOut( test_p);
    processResult( test_p);
    testFooter( name);
    return res;
}

void TestDriver::testHeader( std::string name)
{
    *log_stream << "Running " << std::left << std::setw(FUNC_NAME_WIDTH) << std::setfill('.') <<  name <<' ';
}

void TestDriver::testFooter( std::string name){ /* Not implemented - do nothing */}

void TestDriver::processResult( bool res)
{
    if ( res )
    {
        success_num++;
        *log_stream << " success" << endl;
    } else
    {
        *log_stream << " fail" <<endl;
        fail_num++;
    } 
}

void TestDriver::compareOut( UnitTest *test_p)
{
    std::string test_name = test_p->filename();
    std::string ref_name = Host::concatPaths( refPath(), test_name);
    
    ifstream ref_stream( ref_name.c_str());
    ifstream test_stream( test_name.c_str());
    UInt32 line_num = 0;

    std::string ref_line;
    std::string test_line;

    while ( getline( ref_stream, ref_line) )
    {
        if ( !getline( test_stream, test_line))
        {
            //Test file ended but ref file continues
            test_p->setCmpFailed();
            break;
        } else if ( ref_line != test_line)
        {
            //lines differ
            test_p->setCmpFailed();
            break;
        }
    }
    //Ref file ended
    if ( getline( test_stream, test_line))
    {
        test_p->setCmpFailed();
    }
}

void TestDriver::processResult( UnitTest* utest)
{
    bool res = utest->result();
    
    if ( res )
    {
        success_num++;
        
        *log_stream << " success ("
                    << std::right << std::setw(3) << std::setfill(' ')
                    << utest->numPasses() 
                    << " assertions checked)";
    } else
    {
        *log_stream << " fail (";
            
        if ( utest->assertFailed() )
        {
            *log_stream << "assertions summary: " 
                        << utest->numFails()  << " failed, "
                        << utest->numPasses() << " passed" << endl;
            *log_stream << *utest;
        }
        if ( utest->cmpFailed() )
        {
            *log_stream << "comparison failed";
        }
        *log_stream << ")";
        fail_num++;
    } 
    *log_stream << " " << utest->runTime() << " uSec" << endl;
}

/** Print test statistics */
void TestDriver::printStats()
{
    *log_stream << "Tests failed: " << fail_num << endl;
    *log_stream << "Tests passed: " << success_num << endl;
}

/** Set output file name and open it */
void TestDriver::useLogFile( std::string name)
{

}

/** Generate return result for main */
int TestDriver::returnRes()
{
    if ( fail_num)
        return -1;
    return 0;
}

}