/**
 * File: print.h - Wrapper for printing routines in compiler prototype project
 * Copyright (C) 2012  Boris Shurygin
 */
#ifndef PRINT_H
#define PRINT_H

namespace PrintUtils
{
    /** Print to file */
    
    /** Print to console's STDOUT */
    inline void out( const char* format, ...)
    {
#ifdef _DEBUG
        va_list args;
        va_start( args, format);
        vfprintf( stdout, format, args);
        va_end( args);
        fprintf( stdout, "\n");
#endif
    }
    /** Print to console's STDERR */
    inline void err( const char* format, ...)
    {
#ifdef _DEBUG
        va_list args;
        va_start( args, format);
        vfprintf( stderr, format, args);
        va_end( args);
        fprintf( stderr, "\n");
#endif
    }

    template<class T>
    std::string toString(T i)
    {
        std::stringstream ss;
        std::string s;
        ss << i;
        s = ss.str();

        return s;
    }
    
    /**
     * @class Printable
     * @brief Printing interface for child classes
     * @ingroup Utils
     *
     * @details
     * The Printable template provides means for automatic overloading of operator<< for its subclasses.
     * By reimplementing the toStream routine in subclass one enables printing via operator <<.
     * Example:
     * @code
    
    //User-defined class with printing
    class MyPrintable: public Printable<MyPrintable> //Sublcass of Printable
    {
    public:
        //Custom implementation of toStream routine 
        void toStream( std::ostream &stream) const
        {
            stream << "MyPrintable class";
        }
    };

    void foo()
    {
        MyPrintable my_p;
        std::cout << my_p << std::endl;
        return true;
    }

    @endcode
    */
    template< class T> class Printable
    {
    public:
        /** Calls user-defined printing from the subclass */
        inline void toStream( std::ostream &stream) const
        {
            static_cast<const T *>(this)->T::toStream( stream);
        }
    };

    /**
     * Output operator overload for Parser printing
     * @ingroup Utils
     */
    template <class T> 
    inline std::ostream & operator <<( std::ostream &stream, Printable<T> &p)
    {
        p.toStream( stream);
        return stream;
    }
    /**
     * Output operator overload for Parser printing
     * @ingroup Utils
     */
    template <class T> 
    inline std::ostream & operator <<( std::ostream &stream, const Printable<T> &p)
    {
        p.toStream( stream);
        return stream;
    }
};


 
#endif
