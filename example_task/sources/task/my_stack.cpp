#include "iface.h"

namespace ExampleTask
{
    bool uTest( UnitTest *utest_p)
    {
        return utest_p->result();
    }
};