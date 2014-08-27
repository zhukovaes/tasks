/**
 * @file: log.h 
 * Implementation of logging functionality
 * @defgroup Logs Stream-based log class
 *
 * Implementation of logging functionality
 * @ingroup Utils
 */
/*
 * Utils library in SCL (Simple Compiler)
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef UTILS_LOG_H
#define UTILS_LOG_H

//#define NO_LOGS

/**
 * Debug assert for logs
 * @ingroup GraphBase
 */
#if !defined(LOG_ASSERTD)
#    define LOG_ASSERTD(cond, what) ASSERT_XD(cond, "Logs", what)
#endif

/* Add log to Utils namespace */
namespace Utils
{

/**
 * Log identificator
 * @ingroup Logs
 */
enum LogId
{
    /** Default log */
    LOG_DEFAULT,
    /** Parent log used for testing purposes */
    LOG_UTEST_PARENT,
    /** Child log used for testing purposes */
    LOG_UTEST,
    /** Frontend log */
    LOG_FE,
    /** Lexical analyzer log */
    LOG_FE_LEXER,
    /** Regular expressions debug log */
    LOG_REG_EXP,
    /** Parser log */
    LOG_FE_PARSER,
    /** AST->IR lowering log */
    LOG_FE_AST_IR,
    /** High-level optimizations log */
    LOG_HLO,
    /** Backend log */
    LOG_BE,
    /** Machine-specific optimizations log */
    LOG_BE_OPT,
    /** Scheduler log */
    LOG_BE_SCH,
    /** Register allocator log */
    LOG_BE_RA,
    /** MEMORY log */
    LOG_UTILS_MEM,
    /** Number of logs */
    LOGS_NUM
};

/**
 * @brief Logging control class
 *
 * @ingroup Logs
 * @details
 * Log class allows for structured output of internal/debug info
 * 
 */
class LogControl
{
    /** Default indentation unit width */
    static const UInt32 DEFAULT_INDENT_WIDTH = 2;
public:
    
    /** Log colntrol contructor. 
     *  By default all logs are disabled, use add routine to register them.
     *  Use enable/disable to turn it on/off.
     */
    LogControl();
    
    /** Destructor */
    virtual ~LogControl();
    
    /**
     * Register log that writes to file with given name.
     * File is not open until enable() is called.
     */
    void add( LogId id, string prefix_str, UInt8 verbosity_level, string filename, bool enable_log = false); 
    
    /** Register log that writes to file with given name */
    void add( LogId id, string prefix_str, UInt8 verbosity_level, LogId parent_id, bool enable_log = false);
    
    /* Control of logs suppression */
    void enable( LogId id);  /**< Enable log with given id */
    inline void disable( LogId id); /**< Disable log with given id */

    /* Log printing interface */
    inline void log( LogId id, std::ostringstream& os); /**< Print string stream to log */
    inline void log( LogId id, const char *mess, ...);        /**< Print formatted string to log */
    
    inline UInt8 verb( LogId id);     /**< Get verbosity level of log with given id */
    inline bool isEnabled( LogId id); /**< Check that log is enabled */

    inline UInt32 indent( LogId id) const;        /**< Get indentation factor for the given log id (in units) */
    inline void incIndent( LogId id);             /**< Increase indentation for the given log id */
    inline void decIndent( LogId id);             /**< Decrease indentation for the given log id */
    inline void setIndent( LogId id, UInt32 sz);  /**< Set indentation for the given log id */

    inline UInt32 indentWidth( LogId id) const;         /**< Get indentation unit width for the given log id */
    inline void setIndentWidth( LogId id, UInt32 width);/**< Set indentation unit width for the given log id */
private:
    
    /**
     * Convert incoming message string into several strings 
     * with the correct prefixes and indentation
     */
    inline std::string prepareString( LogId id, const std::string &str);

    static const UInt8 max_verbosity = (UInt8)(-1);
    static const UInt32 max_buf_size = 256;

    bool insert_newline[ LOGS_NUM];
    bool registered[ LOGS_NUM];
    bool enabled[ LOGS_NUM];
    bool unique_name[ LOGS_NUM];
    UInt32 indent_size[ LOGS_NUM];
    UInt32 indent_width[ LOGS_NUM];
    string fname[ LOGS_NUM];
    ostream* stream[ LOGS_NUM];
    string prefix[ LOGS_NUM];
    UInt8 verbosity[ LOGS_NUM];
    filebuf fb[ LOGS_NUM];
};

/** Get indentation factor for the given log id (in units) */
inline UInt32
LogControl::indent( LogId id) const
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    return indent_size[ id];
}

/** Increase indentation for the given log id */
inline void
LogControl::incIndent( LogId id)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    indent_size[ id]++;
}      

/** Decrease indentation for the given log id */
inline void
LogControl::decIndent( LogId id)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    if ( indent_size[ id] > 0)
    {
        indent_size[ id]--;
    }
}


/** Set indentation for the given log id */
inline void
LogControl::setIndent( LogId id, UInt32 sz)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    indent_size[ id] = sz;
}

/** Get indentation unit width for the given log id */
inline UInt32 LogControl::indentWidth( LogId id) const
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    return indent_width[ id];
}

/** Set indentation unit width for the given log id */
inline void LogControl::setIndentWidth( LogId id, UInt32 width)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    indent_width[ id] = width;
}

/**
 * Get verbosity level of log with given id
 */
inline UInt8 
LogControl::verb( LogId id)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    return verbosity[ id];
}

/**
 * Check that log with given id is enabled
 */
inline bool 
LogControl::isEnabled( LogId id)
{
    return registered[ id] && enabled[ id];
}

/**
 * Print formated message into log
 */
inline void LogControl::log( LogId id, const char *mess, ...)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    if ( enabled[ id])
    {
        char buf[ max_buf_size];
        
        /* Fill buffer with formatted string */
        va_list args;
        va_start( args, mess);

 #if defined (OS_WIN32) && !defined(CC_GNU) /* Windows */
        vsnprintf_s( buf, max_buf_size - 1, mess, args);
#else /* Linux */
        vsnprintf( buf, max_buf_size - 1, mess, args);
#endif
        va_end( args);

        /* Print string to stream */
        ostream &output_stream = *(stream[ id]);
        output_stream << prepareString( id, buf);
    }
}

/**
 * Print string stream to log
 */
inline void
LogControl::log( LogId id, std::ostringstream& os)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    if ( enabled[ id])
    {
        ostream &output_stream = *(stream[ id]);
        output_stream << prepareString( id, os.str());
    }
}
  
/**
 * Disable log with given id
 */
inline void LogControl::disable( LogId id)
{
    LOG_ASSERTD( registered[ id], "log id is not registered");
    enabled[ id] = false;
}

/**
 * Convert incoming message string into several strings with correct prefixes and indentation
 */
inline std::string
LogControl::prepareString( LogId id, const std::string &str)
{
    std::ostringstream tmp;
    std::stringstream ss(str);
    UInt32 padding_width = indent_size[ id] * indent_width[ id];

    std::string::const_reverse_iterator at_end = str.rbegin();
    bool first = true;

    // Split the incoming message string into a bunch of one-line strings separated by the '\n'
    std::string::const_iterator it = str.begin();
    std::string::const_iterator end = str.end();

    while ( it != end)
    {
        char c = *it;
        if ( insert_newline[ id])
        {
            std::string one_line_str;
    
            // Insert correct number of whitespaces
            one_line_str.insert( one_line_str.begin(), padding_width, ' ');
        
            // Print one string to the ostream with the prefix
            tmp << prefix[ id] << ": " << one_line_str;
        }
        if ( c == '\n')
        {
            tmp << endl;
            insert_newline[ id] = true;
        } else
        {
            tmp << c;
            insert_newline[ id] = false;
        }
        ++it;
    }

    return tmp.str();
}


/**
 * @brief Singleton of logging control class
 *
 * @ingroup Logs
 * @details
 * Log class allows for structured output of internal/debug info
 * 
 * Usage example:
 * @code
    // Add default log
    Log::ptr()->add( LOG_DEFAULT, "Default log", 5, "log.txt"); // Default log
    Log::ptr()->add( LOG_FE, "Default log", 5, "log.txt"); // FE log

    // Logging is disabled by default so we should enable logs we want to see
    // Calls like this should be put into options parsing code to control logging by options
    Log::ptr()->enable( LOG_DEFAULT);

    LOG( LOG_DEFAULT, "Log message %d", 1);              // Simple formatted message
    LOGS( LOG_DEFAULT, "Log message " << "with stream"); // Message using C++ stream syntax (uses ostringstream internally)
    LOG( LOG_FE, "Log message %d", 2);                   // Won be printed since LOG_FE is not enabled
@endcode
 *
 * @sa LogControl
 */
typedef Single< LogControl> Log;

/** Convenience routine */
inline LogControl* log()
{
    return Log::ptr();
}

#ifndef NO_LOGS
#  define LOGV( log_id, verbosity, message, ...) \
     if ( Utils::Log::ptr()->isEnabled( log_id)\
          && verbosity <= Log::ptr()->verb( log_id) ) Utils::Log::ptr()->log( log_id, message, __VA_ARGS__)
#  define LOGVS( log_id, verbosity, message)\
    if ( bool cond = true\
         && Utils::Log::ptr()->isEnabled( log_id)\
         && verbosity <= Utils::Log::ptr()->verb( log_id) ) for ( std::ostringstream os; cond; cond = false)\
         {os << message;Utils::Log::ptr()->log( log_id, os);}
#  define LOG( log_id, message, ...) LOGV( log_id, 0, message, __VA_ARGS__)
#  define LOGS( log_id, message) LOGVS( log_id, 0, message)
#  define LOG_INC_INDENT( log_id)\
            { if ( Utils::Log::ptr()->isEnabled( log_id)) Utils::Log::ptr()->incIndent( log_id);}
#  define LOG_DEC_INDENT( log_id)\
            { if ( Utils::Log::ptr()->isEnabled( log_id)) Utils::Log::ptr()->decIndent( log_id) ;}
#else
#  define LOGV( log_id, verbosity, message, ...)
#  define LOGVS( log_id, verbosity, message)
#  define LOG( log_id, message, ...)
#  define LOGS( log_id, message)
#  define LOG_INC_INDENT( log_id)
#  define LOG_DEC_INDENT( log_id)
#endif

    /** Unit testing routine for logs */
    bool uTestLogs( UnitTest *utest_p, std::string &name);

}; /* namespace Utils*/
#endif /* UTILS_LOG_H */