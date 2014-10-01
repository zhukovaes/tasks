/**
 * @file: conf.h
 * Implementation of options configuration of SCL (Simple Compiler)
 * @defgroup Opts Options
 * @brief Classes for configuring and parsing command-line options
 * @ingroup Utils
 */
/*
 * Utils library in ShowGraphImpl tool
 * Copyright (C) 2012  Boris Shurygin
 */
#ifndef CONF_H
#define CONF_H

/**
 * Command line option type
 * @ingroup Opts
 */
enum OptType
{
    /** Boolean */
    OPT_BOOL,
    /** Integer */
    OPT_INT,
    /** Float */
    OPT_FLOAT,
    /** String */
    OPT_STRING,
    /** Number of types */
    OPT_TYPES_NUM
};

/**
 * Union type for option's values
 * @ingroup Opts
 */
union OptValues
{
    /** Value for boolean option */
    bool bool_val;
    /** Value for integer option */
    Int32 int_val;
    /** Value for floating point option */
    Double float_val;
}; 

/**
 * Command line option descrition
 * @ingroup Opts
 */
class Option
{
    /** Whether user has set value of this option */
    bool defined;

    /** Type */
    OptType _type;

    /** Option default values */
    OptValues def_values;

    /** Option user's values */
    OptValues values;

    string string_val;
    
    /** Short name */
    string short_name;
    
    /** Long name */
    string long_name;

    /** Description */
    string descr;
public:
    /** Constructor without default val */
    Option( OptType _t, string sname, string lname, string d):
        defined( false),_type( _t), short_name( sname), long_name( lname), descr( d)
    {
        switch ( _t)
        {   
            case OPT_BOOL:
                def_values.bool_val = false;
                break;
            case OPT_INT:
                def_values.int_val = 0;
                break;
            case OPT_FLOAT:
                def_values.float_val = 0;
                break;
            case OPT_STRING:
            default:
                break;
        }
        values = def_values;
    };

    /** Constructor with default bool val */
    Option( string sname, string lname, string d, bool val):
        defined( false), _type( OPT_BOOL), short_name( sname), long_name( lname), descr( d)
    {
        def_values.bool_val = val;
        values = def_values;
    }

    /** Constructor for string option */
    Option( string sname, string lname, string d):
        defined( false), _type( OPT_STRING), short_name( sname), long_name( lname), descr( d){};

    /** Check if the option was defined */
    inline bool isDefined() const
    {
        return defined;    
    }
    /** Set the option to 'defined' state*/
    inline void setDefined( bool def = true)
    {
        defined = def;
    }
    /** Get short name */
    inline string shortName() const
    {
        return short_name;    
    }
    /** Get long name */
    inline string longName() const
    {
        return long_name;    
    }
    /** Get option type */
    inline OptType type() const
    {
        return _type;
    }
    /** Get option default val */
    inline bool defBoolVal() const
    {
        ASSERTD( _type == OPT_BOOL);
        return def_values.bool_val;
    }
    /** Set option boolean value */
    inline void setBoolVal( bool val)
    {
        ASSERTD( _type == OPT_BOOL);
        values.bool_val = val;
    }
    /** Set option boolean value */
    inline void setIntVal( Int32 val)
    {
        ASSERTD( _type == OPT_INT);
        values.int_val = val;
    }
    /** Set option boolean value */
    inline void setFloatVal( Double val)
    {
        ASSERTD( _type == OPT_FLOAT);
        values.float_val = val;
    }
    /** Set option boolean value */
    inline void setStringVal( string val)
    {
        ASSERTD( _type == OPT_STRING);
        string_val = val;
    }
    /** Get string value of option */
    inline string stringVal() const
    {
        ASSERTD( _type == OPT_STRING);
        return string_val;
    }
    /** Get int value of option */
    inline Int32 intVal() const
    {
        ASSERTD( _type == OPT_INT);
        return values.int_val;
    }
    /** Get float value of option */
    inline Double floatVal() const
    {
        ASSERTD( _type == OPT_FLOAT);
        return values.float_val;
    }
    /** Get int value of option */
    inline bool isSet() const
    {
        ASSERTD( _type == OPT_BOOL);
        return values.bool_val;
    }
    /** Print option's synopsis and description */
    void print( ostream &stream);
    /** Print option's synopsis and description */
    void printWithValue( ostream &stream);
};

/**
 * @brief Configuration represention
 * @ingroup Opts
 *
 * @par
 * Allows for configuring command line options and reading them from C-string array.
 * Usage example:
 @code
 // simple main routine
 int main( int argc, char **argv)
 {
     Conf conf;

     // Configuring possible options and their type
     conf.addOption( new Option( OPT_STRING, "f", "file", "input GraphImpl description file name"));
     conf.addOption( new Option( OPT_STRING, "o", "output", "output image file name"));
     conf.readArgs( argc, argv);

     //checking options and reading their values
     if ( conf.longOption("file")->isDefined() 
          && conf.longOption("output")->isDefined())
     { 
         string xmlname = conf.longOption("file")->string(); // read value passed with '-file ...'
         ...
     } else
     {
         conf.printOpts(); // Print options to console
     }
 }
 @endcode
 */
class Conf
{
    string app_name;
    typedef std::map< string, Option *> OptHash;
    
    OptHash short_opts;
    OptHash long_opts;

    std::list< string> unknown_options;
public:
    /** Constructor */
    Conf();
    
    /** Destructor wipes all the options */
    ~Conf()
    {
        for ( OptHash::iterator it = short_opts.begin(), end = short_opts.end();
              it != end;
              ++it)
        {
            Option *opt = (*it).second;
            delete opt;
        }
    }
    /** Get number of arguments that were not recognized */
    inline int unknownOptsNum() const
    {
        return unknown_options.size();
    }
    /** Add option */
    inline void addOption( Option *opt)
    {
        short_opts[ opt->shortName()] = opt;
        long_opts[ opt->longName()] = opt;
    }

    /** Convenience routine: adds option without default val */
    inline void addOption( OptType _t, string sname, string lname, string d)
    {
        Option *opt = new Option( _t, sname, lname, d);
        addOption( opt);
    }

    /** Convenience routine: adds option with default bool val */
    inline void addOption( string sname, string lname, string d, bool val)
    {
        Option *opt = new Option( sname, lname, d, val);
        addOption( opt);
    }
    /** Convenience routine: adds string option without default value */
    inline void addOption( string sname, string lname, string d)
    {
        Option *opt = new Option( sname, lname, d);
        addOption( opt);
    }
    
    /** Print options */
    void printOpts();

    /** Print options with values */
    void printOptValues();

    /** Print value defaults */
    void printDefaults();

    /**
     * Parse arguments from C-string
     * @param argc number of arguments
     * @param argv pointer to array of C-strings
     */
    void readArgs( int argc, char** argv);

    /** Get option based on its name */
    inline Option* option( string name)
    {
        /* try to look among short options */
        if ( short_opts.find( name) != short_opts.end())
        {
            return short_opts[ name];
        }
        /* try to look among long options */
        if ( long_opts.find( name) != long_opts.end())
        {
            return long_opts[ name];
        }
        /* if nothing's found return NULL */
        return NULL;
    }
    /** Get option based on its short name */
    inline Option* shortOption( string name)
    {
        /* try to look among short options */
        if ( short_opts.find( name) != short_opts.end())
        {
            return short_opts[ name];
        }
        /* if nothing's found return NULL */
        return NULL;
    }
    /** Get option based on its long name */
    inline Option* longOption( string name)
    {
        /* try to look among long options */
        if ( long_opts.find( name) != long_opts.end())
        {
            return long_opts[ name];
        }
        /* if nothing's found return NULL */
        return NULL;
    }
};

#endif