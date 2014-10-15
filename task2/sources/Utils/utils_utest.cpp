/**
 * @file: utils_utest.cpp 
 * Implementation of testing of utils
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

using namespace Utils;

namespace TestSingle
{
    /** Sample class */
    class A
    {
        int a;
        /** private constructors, assignment and destructor */
        A(){};
        A( const A&){};
        A& operator =( const A&){};
        ~A(){};
        /** Needed for singleton creation */
        friend class Single< A>;
    };

    /** Typedef for A encapsulated in singleton */
    typedef Single< A> SingleA;

    /**
     * Test simple singleton
     */
    static bool uTestSingle( UnitTest *utest_p)
    {
        SingleA::init();

        A* a1 = SingleA::instance();
        A* a2 = SingleA::instance();
        UTEST_CHECK( utest_p, areEqP( a1, a2));
        SingleA::deinit();
        return true;
    }
}

#if 0
namespace UTestFinal
{
    class Base: Final< Base>
    {

    };

    class Derived: public Base
    {};
}
#endif

class MyPrintable: public Printable<MyPrintable>
{
public:
    void toStream( std::ostream &stream)
    {
        stream << "MyPrintable class";
    }
};

/**
 * Various tests
 */
static bool uTestMisc()
{
#if 0
    // We can create objects of the class Base
    UTestFinal::Base b;
    
    // This should trigger compiler error saying that the 
    // FinalHook<Base>() constructor is not accessible from Derived 
    UTestFinal::Derived *d = new UTestFinal::Derived(); // <-- compilation error (C2248 in MSVC 10) 
#endif
    //MyPrintable my_p;
    //std::cout << my_p << std::endl;
    return true;
}


/**
 * Test Utils package (logically this should be seen as a 'testing suite'
 * though the concept is not yet supported in utest)
 */
bool Utils::uTest()
{
    /** Various tests of auxiliary routines */
    RUN_TEST( uTestMisc);
    /** test singleton */
    RUN_TEST( TestSingle::uTestSingle);
    /** Tets list classes */
    RUN_TEST( uTestList);

    return true;
}

