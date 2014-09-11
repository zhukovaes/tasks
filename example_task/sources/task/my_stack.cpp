#include "iface.h"

namespace ExampleTask
{
    const char* MyStack::Error::what() const throw()
    {
        return "stack internal error";
    }

    // Stack of integers
    MyStack::MyStack(): 
        _size(0),
        mem_p( new int[INIT_SIZE]),
        _impl_size( INIT_SIZE)
    {
    
    }

    MyStack::~MyStack()
    {
        delete[] mem_p;
    }
    
    void MyStack::toStream( std::ostream &stream) const 
    {}
    
    void MyStack::shrink()
    {
        _size--; // naive implementation
    }

    void MyStack::grow()
    {
        if ( _size + 1 >= _impl_size)
            throw Error();

        _size++; // naive implementation
    }

    int MyStack::pop()
    { 
        if ( size() == 0) throw Error();

        int val = *(mem_p + size() - 1);

        shrink();

        return val;
    }
    
    void MyStack::push(int val)
    {
        *(mem_p + size()) = val;

        grow();
    }
        
    bool uTest( UnitTest *utest_p)
    {
        bool thrown_error = false;

        //Tested instance
        MyStack stack;

        stack.push(1);
        stack.push(2);

        UTEST_CHECK( utest_p, stack.size() == 2);

        int second = stack.pop();
        int first = stack.pop();

        UTEST_CHECK( utest_p, second == 2);
        UTEST_CHECK( utest_p, first == 1);
        
        UTEST_CHECK( utest_p, stack.size() == 0);

        try
        {
            stack.pop();
        } catch ( MyStack::Error)
        {
            thrown_error = true;
        }
        UTEST_CHECK( utest_p, thrown_error);

        return utest_p->result();
    }
};