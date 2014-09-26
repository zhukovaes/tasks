#include "iface.h"

// Implementation of the task's unit testing
namespace Task
{
    bool uTest( UnitTest *utest_p)
    {
        return utest_p->result();
    }
};