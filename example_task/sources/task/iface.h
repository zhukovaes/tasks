/**
 * @file: task/iface.h 
 * Interface of an example programming task
 */
#include "../Utils/utils_iface.h"

/* namespaces import */
using namespace Utils;

/**
 * Namespace for the example programming task
 */
namespace ExampleTask
{
    // Stack of integers
    class MyStack: public Printable<MyStack>
    {
    public:
        typedef UInt32 Size;
        
        class Error: public std::exception
        {
        public:
            virtual const char* what() const throw();
        };

        static const Size INIT_SIZE = 20;

        MyStack();
        ~MyStack();
        void toStream( std::ostream &stream) const;
        
        int pop();
        void push(int val);

        inline Size size() const { return _size; }
    private:
        Size _size;
        Size _impl_size;
        int *mem_p;

        void grow();
        void shrink();

        MyStack( const MyStack &orig);
        MyStack &operator =( const MyStack &orig);
    };


    bool uTest( UnitTest *utest_p);
};
