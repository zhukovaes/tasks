/**
 * @file: log.cpp 
 * Implementation of logging
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

using namespace Utils;

/**
 * Constructor for logs
 */
LogControl::LogControl() 
{
    for ( Int8 id = 0; id < LOGS_NUM; id++)
    {
        indent_size[ id]  = 0;
        indent_width[ id] = DEFAULT_INDENT_WIDTH;
        prefix[ id] = std::string(); // no prefix by default
        enabled[ id] = false;
        registered[ id] = false;
        verbosity[ id] = max_verbosity;
        stream[ id] = NULL;
        unique_name[ id] = false;
        insert_newline[ id] = true;
    }
}

/**
 * Constructor for logs
 */
LogControl::~LogControl()
{
    for ( Int8 id = 0; id < LOGS_NUM; id++)
    {
        if ( registered[ id] && unique_name[ id])
        {
            stream[ id]->flush();
            delete stream[ id];
            fb[ id].close();
        }
    }
}

/**
 * Register log that writes to given file
 */
void 
LogControl::add( LogId id, string prefix_str, UInt8 verbosity_level, string filename, bool enable_log)
{
    LOG_ASSERTD( id < LOGS_NUM, "Id is out of range");
    LOG_ASSERTD( !filename.empty(), "Log file name is not specified");

    /* Check if we already have opened the file for some other log */
    registered[ id] = true;
    enabled[ id] = enable_log;
    prefix[ id] = prefix_str;
    verbosity[ id] = verbosity_level;
    fname[ id] = filename;
    if ( enable_log)
        enable( id);
}

/**
 * Register log that is redirected to given parent log
 */
void
LogControl::add( LogId id, string prefix_str, UInt8 verbosity_level, LogId parent_id, bool enable_log)
{
    LOG_ASSERTD( id < LOGS_NUM, "Id is out of range");
    LOG_ASSERTD( parent_id < LOGS_NUM, "Parent id is out of range");
    LOG_ASSERTD( registered[ parent_id], "Parent log is not registered");
    LOG_ASSERTD( !fname[ parent_id].empty(), "Parent log file name is not specified");

    /* Check if we already have opened the file for some other log */
    stream[ id] = stream[ parent_id];
    registered[ id] = true;
    enabled[ id] = enable_log;
    prefix[ id] = prefix_str;
    verbosity[ id] = verbosity_level;
    fname[ id] = fname[ parent_id];
        
    if ( enable_log)
        enable( id);
}

/**
 * Enable log with given id 
 */
void LogControl::enable( LogId id)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    enabled[ id] = true;

    /** Check if we need to open file */
    if ( isNullP( stream[ id]))
    {
        bool already_opened = false;
        for ( Int8 tmp_id = 0; tmp_id < LOGS_NUM; ++tmp_id)
        {
            if ( registered[ tmp_id] )
            {
                if ( fname[ tmp_id] == fname[ id] 
                     && isNotNullP( stream[ tmp_id]))
                {
                    already_opened = true;
                    stream[ id] = stream[ tmp_id];
                }
            }
        }
        
        if ( !already_opened)
        {
            fb[ id].open ( fname[ id].c_str(), ios::out);
            stream[ id] = new ostream(&fb[ id]);
            unique_name[ id] = true;
        }
    }
} 