/**
 * @file: UnitTest/main.cpp
 * Implementation and entry point for unit testing of SCL (Simple Compiler)
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utest_impl.h"

#undef UNIT_TEST_GUI

/**
 * The entry point for GUI version of SCL (Simple Compiler)
 */
int main(int argc, char **argv)
{
    // Run the example testing
    RUN_TEST( ExampleTask::uTest);

    cout.flush();
    cerr.flush();
    TestDriver::printStats();
    return TestDriver::returnRes();
}
