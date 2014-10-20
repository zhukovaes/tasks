/**
 * @file: task/iface.h
 * Interface of a programming task
 */
//#include "iface.h"

/* namespaces import */
//using namespace Utils;

//Debug assert with diagnostic info
#if !defined(DLIST_ASSERTXD)
#    define DLIST_ASSERTXD(cond, what) ASSERT_XD(cond, "DList", what)
#endif

// Simple debug assert
#if !defined(DLIST_ASSERTD)
#    define DLIST_ASSERTD(cond) ASSERT_XD(cond, "DList", "")
#endif

/**
 * Namespace for the programming task
 */
namespace Task
{
    //
    // Doubly connected list
    //
    template <class T> class DList
    {
    public:
        //
        // List unit
        //
        class Unit
        {
            friend class DList;

        public:
            // ---- This interface is part of the task ---
            Unit *next(); // Get the next unit in list
            Unit *prev(); // Get the previous unit in list
            T& val();     // Get the reference to the unit's value
        private:
        // ---- Implementation routines ----
        Unit();//constructor
        void glue(Unit *u); //
        // ---- Data involved in the implementation ----
        T unit_value;
        Unit *unit_prev;
        Unit *unit_next;
        };

        // ---- Public interface of DList ----
        DList(); //< Constructor
        ~DList();//< Destructor

        void push_front (const T& val);// Insert one unit with given value at front
        void pop_front();// Remove one unit at front of the list
        void push_back (const T& val);        // Insert one unit with given value to back
        void pop_back();                      // Remove one unit from the back of the list
        Unit* insert (Unit* u, const T& val); // Insert one unit before the given one

        Unit* first(); // Get first unit
        Unit* last();  // Get last unit

        Unit* erase (Unit* u); // Remove given unit from list, return next unit or null
        void clear();          // Remove all units
        bool empty();          // Check if list is empty. Returns true if empty, false otherwise
        unsigned size();       // Get the number of units in the list
        void reverse();        // Reverse the order of units in the list
private:
        // ---- The internal implementation routines ----

        // ---- The data involved in the implementation ----
        Unit *head;
        Unit *tail;
        int size_of_list;
    };

//    bool uTest( UnitTest *utest_p);
};

// Since we have defined list as a template - we should implement the solution in a header
// But to keep interface clean we've separated the implementation into list_impl.h header
#include "list_impl.h"
