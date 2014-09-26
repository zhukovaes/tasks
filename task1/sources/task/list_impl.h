/**
 * @file: task/list_impl.h 
 * Solution implementation of the programming task 1
 */

namespace Task
{
    // This is how template class members are defined outside of the template class
    template <class T> // template keyword and template parameter
    typename DList<T>::Unit *   // the return value. Note that we have to use the template parameter T to specify return type.
                                // "typename" is needed here because DList<T>::Unit is a 
                                // dependent name and we should hint the compiler that it is a type
    DList<T>::Unit::next()      
    {
        // implementation should be here
        return 0;
    }
    
    //One more example of template class members definition
    template <class T> void DList<T>::push_front (const T& val)
    {
        //implementation should be here
    }
        
};
