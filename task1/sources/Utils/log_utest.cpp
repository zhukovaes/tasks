/**
 * @file: log_utest.cpp 
 * Implementation of logging unit testing
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

using namespace Utils;

/**
 * Test logging package
 */
bool Utils::uTestLogs( UnitTest *utest_p, std::string &name)
{
    Log::init();
    UInt32 message_num = 0;
    
    // Add default log
    log()->add( LOG_UTEST_PARENT, "Default log", 5, name);
    log()->add( LOG_UTEST, "Unit testing", 5, LOG_UTEST_PARENT);

    // Logging is disabled by default so we should not see these messages in the log file
    LOG( LOG_UTEST_PARENT, "Shouldn't be seen 1 %d\n", message_num++);
    LOGS( LOG_UTEST_PARENT, "Shouldn't be seen" << " ever 1 " << message_num++ << endl);
    LOG( LOG_UTEST, "Shouldn't be seen %d\n", message_num++);

    // Test printing
    log()->enable( LOG_UTEST_PARENT);
    LOG( LOG_UTEST_PARENT, "Log message %d\n", message_num++);
    LOG_INC_INDENT( LOG_UTEST_PARENT);

    LOGS( LOG_UTEST_PARENT, "Log message " << "with stream " << message_num++ << endl);
    LOGS( LOG_UTEST_PARENT, "A very very long logging message that needs to be written on multiple lines\n" 
                       << "the number of this long message is "
                       << message_num++ << endl);
    
    LOG_INC_INDENT( LOG_UTEST_PARENT);
    
    LOGV( LOG_UTEST_PARENT, 4, "Fairly verbose message (verb %d) with formatted print %d\n", 4, message_num++);
    LOG_DEC_INDENT( LOG_UTEST_PARENT);
    
    LOGVS( LOG_UTEST_PARENT, 4, "Fairly verbose message(" << 4 << ") with stream interface " << message_num++ << endl);
    LOG_DEC_INDENT( LOG_UTEST_PARENT);

    // These messages are too verbose for currently set level (5) we should not see them in the log file
    LOGV( LOG_UTEST_PARENT, 9, "Very verbose message(%d) with formatted print %d\n", 9, message_num++);
    LOGVS( LOG_UTEST_PARENT, 9, "Very verbose message(" << 9 << ") with stream interface " << message_num++ << endl);

    log()->disable( LOG_UTEST_PARENT);
    LOG( LOG_UTEST_PARENT, "Shouldn't be seen 2 %d\n", message_num++);
    LOGS( LOG_UTEST_PARENT, "Shouldn't be seen" << " ever 2 "  << message_num++ << endl);
    
    log()->enable( LOG_UTEST);

    // Test direct calls to log
    Log::ptr()->log( LOG_UTEST, "Utest log message %d\n", message_num++);
    Log::ptr()->log( LOG_UTEST_PARENT, "Log message %d\n", message_num++);
        
    Log::deinit();
    return utest_p->result();
}
