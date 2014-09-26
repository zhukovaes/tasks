/**
 * @file: conf.cpp 
 * Implementation of configuration tools
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

/** 
 * namespaces import
 */
using namespace Utils;

/** Option's print routine */
void
Option::print( ostream &stream)
{
    stream << "-" << short_name << ", "
           << "--" << long_name << "    "
           << descr << endl;
}

/** Option's print routine */
void
Option::printWithValue( ostream &stream)
{
    stream << "-" << short_name << ": ";

    if ( !isDefined())
    { 
        stream << "undefined. Default value is: ";
    }
    switch( _type)
    {
        case OPT_BOOL:
            if ( isSet())
            {
                stream << "true";
            } else
            {
                stream << "false";
            }
            break;
        case OPT_INT:    stream << intVal();       break;
        case OPT_FLOAT:  stream << floatVal();     break;
        case OPT_STRING: stream << this->stringVal(); break;
        default:
            break;
    }
    stream << endl;
}

/** Default constructor of configuration */
Conf::Conf(): short_opts(), long_opts(), unknown_options()
{
#ifdef _DEBUG
    /* Debug options */

#endif
    /* General purpose options */
}

/** Print options */
void Conf::printOpts()
{
    for ( OptHash::iterator it = short_opts.begin(), end = short_opts.end();
              it != end;
              ++it)
    {
        Option *opt = (*it).second;
        opt->print( cout);
    }
}

/** Print options with defined values*/
void Conf::printOptValues()
{
    for ( OptHash::iterator it = short_opts.begin(), end = short_opts.end();
              it != end;
              ++it)
    {
        Option *opt = (*it).second;
        opt->printWithValue( cout);
    }
}

/** Print value defaults */
void Conf::printDefaults()
{

}

/** Read input arguments */
void Conf::readArgs( int argc, char** argv)
{
    app_name = string( argv[ 0]);
    string short_prefix("-");
    string long_prefix("--");

    for ( int i = 1; i < argc; i++)
    {
        string curr( argv[ i]);
        size_t found_short = curr.find( short_prefix);
        size_t found_long = curr.find( long_prefix);
        Option *opt = NULL;
        if ( found_long == 0)
        {
            /* We look for expression among long option names */
            string name = curr.substr( 2);
            if ( long_opts.find( name) != long_opts.end())
            {
                opt = long_opts[ name];
            } else
            {
                cerr << "No such long option " << name << " in string " << curr << endl;
                unknown_options.push_back( name);
            }
        } else if ( found_short == 0)
        {
            /* We look for expression among short option names */
            string name = curr.substr( 1);
            
            if ( short_opts.find( name) != short_opts.end())
            {
                opt = short_opts[ name];
            } else
            {
                cerr << "No such short option " << name << " in string " << curr << endl;
                unknown_options.push_back( name);
            }
        } else
        {
            /* Is not an option specifier */
            cerr << "Unrecognized argument " << curr << endl;
            unknown_options.push_back( curr);
        }
        if ( isNotNullP( opt))
        {
            OptType tp = opt->type();
            opt->setDefined(); // option is defined now
            switch( tp)
            {
                case OPT_BOOL:
                    /* For bool options we expect argument only if its default value is 'true' */
                    if ( opt->defBoolVal())
                    {
                        i++; // advance in argument array
                        string val( argv[ i]);
                        opt->setBoolVal( convStr2Int32( val) != 0);
                    } else /* However most bool options have default value of 'false' */
                    {
                        opt->setBoolVal( true);
                    }
                    break;
                case OPT_INT:
                {
                    i++; // advance in argument array
                    string val( argv[ i]);
                    opt->setIntVal( convStr2Int32( val));
                    break;
                }
                case OPT_FLOAT:
                {
                    i++; // advance in argument array
                    string val( argv[ i]);
                    opt->setFloatVal( convStr2Double( val));
                    break;
                }
                case OPT_STRING:
                {
                    i++; // advance in argument array
                    string val( argv[ i]);
                    opt->setStringVal( val);
                    break;
                }
                default:
                    break;
            }
        }
    }
}