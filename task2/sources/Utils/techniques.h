/**
 * @file: singleton.h 
 * @defgroup Techniques Programming techniques
 * @brief Various small C++ programming idioms
 * @ingroup Utils
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef UTILS_TECHNIQUES_H
#define UTILS_TECHNIQUES_H

/* Predeclaration */
template <class FinalType> class Final;

/**
 * Dummy base template for 'final' template implementation 
 * @ingroup Techniques
 */
template <class FinalType> class FinalHook
{
public:
    /** Private constructor */
    FinalHook(){};

    /*
     * Decaration of friends so that we could
     * access the constructor from specified classes
     */
    friend class FinalType;
    //friend class Final< FinalType>;
};

/**
 * @brief Template that restricts further subclassing of first derived class
 * @ingroup Techniques
 *
 * @details
 @code

// That's how one can declare class 'final', i.e. class Base cannot be further subclassed
class Base: Final< Base>
{

};

class Derived: public Base
{};

int foo()
{
    // We can create objects of the class Base
    Base b;
    
    // This should trigger compiler error saying that the 
    // FinalHook<Base>() constructor is not accessible from Derived 
    Derived *d = new Derived(); // <-- compilation error
    
    return 0;
}
@endcode
 */
template <class FinalType> class Final: virtual FinalHook< FinalType>
{
    
};


#endif /* UTILS_TECHNIQUES_H */
