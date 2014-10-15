/**
 * @file: utils_utest.cpp 
 * Implementation of various utils
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

/*
  The ASSERT macro calls this this function when the test fails.
*/
void fatalAssert(const char *assertion, const char *file, int line)
{
    cerr << "ASSERT: " << assertion << " in file " << file << ", line " << line << endl;
    abort();
}

/*
  The ASSERT_X macro calls this this function when the test fails.
*/
void fatalAssertWithMess(const char *where, const char *what, const char *file, int line)
{
    cerr << "ASSERT failure in "<< where <<": \""<< what <<"\", file "<< file <<", line " << line << endl;
    abort();
}