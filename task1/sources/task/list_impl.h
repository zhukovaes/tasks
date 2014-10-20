/**
 * @file: task/list_impl.h
 * Solution implementation of the programming task 1
 */

namespace Task
{
// ---- DList::Unit class implementation ----

    // This is how template class members are defined outside of the template class
    template <class T> // template keyword and template parameter
    typename DList<T>::Unit *   // the return value. Note that we have to use the template parameter T to specify return type.
                                // "typename" is needed here because DList<T>::Unit is a
                                // dependent name and we should hint the compiler that it is a type
    DList<T>::Unit::next()
    {
        if(this)
        {
            return unit_next;
        }

    }

    template <class T> typename DList<T>::Unit *
    DList<T>::Unit::prev()
    {
        if (this)
        {
            return unit_prev;
        }

    }
    template <class T>  T&
    DList<T>::Unit::val(){
	    if(this){
		    return unit_value;
	    }
    }

    template <class T> void
	    DList<T>::Unit::addPrev(Unit *prev) {
	    unit_prev = prev;
    }
    template <class T> void
    DList<T>::Unit::addNext(Unit *next) {
	    unit_next = next;
    }


    template <class T>
    DList<T>::Unit::Unit(const T value)
    {unit_value = value;
    unit_prev = NULL;
    unit_next = NULL;
    }


    // ---- DList template class implementation ----


    template <class T> DList<T>::DList()
    {
		head = NULL;
		tail = NULL;
	    size_of_list = 0;
    }

        template <class T> DList<T>::~DList()
    {
		    while (size_of_list > 0)
                pop_front();
    }

    template <class T> void DList<T>::push_front (const T& val)
    {
        if(head == NULL)
        {
            head = new Unit(val);
            tail = head;
            size_of_list = 1;
        }
        else
        {
            size_of_list++;
            head->addPrev(new Unit(val));//new element is previous for current first
            head->prev()->addNext(head);
            head = head->prev();//new front element
            head->addPrev(NULL);

        }
    }
     template <class T> void DList<T>::pop_front()
    {
	    if (size_of_list) {
		    if (head->unit_next()){
		        head = head->unit_next();
			    head->addPrev(NULL);
		    }
		    else
            {
                head = NULL;
                tail = NULL;
            }
	    }
    }
    template <class T> void DList<T>::push_back(const T &val)
    {

	    if (tail) {
		    tail->addNext(new Unit(val));//new Unit
		    tail->unit_next()->addPrev(tail);//prev of new Unit == last
		    tail = tail->unit_next();//new last
		    tail->addNext(NULL);
	    } else
	    {
		    tail = new Unit(val);
		    head = tail;
	    }
        size_of_list++;
    }

    template <class T> void DList<T>::pop_back()
    {
	    if (size_of_list){
	        size_of_list--;

		    if (tail->unit_prev()) {
			    tail = tail->unit_prev();
			    tail->addNext(NULL);
		    }
	        else
            {
                tail = NULL;
                head = NULL;
	        }
	    }
    }

    template <class T>
    typename    DList<T>::Unit* DList<T>:: insert (Unit* u, const T& val)
    {
	    DList<T>:: Unit* previous = u->unit_prev();//remember previous
	    u->addPrev(new Unit(val)) ;//add new previous
	    u->unit_prev()->addPrev(previous);
	    u->unit_prev->addNext(u);
	    size_of_list++;
	    return u->unit_prev();
    }
        template <class T>
    typename DList<T>::Unit* DList<T>::  first()
    {
	    return head;
    }

    template <class T>
    typename DList<T>:: Unit* DList<T>:: last()
    {
	    return tail;
    }

     template <class T>
    typename DList<T>:: Unit* DList<T>::erase(Unit *u)
    {
        if(size_of_list)
        {
            if (u->unit_prev())
            {
                if (u->unit_next())
                {
                    u->unit_prev()->addNext(u->unit_next);
                    u->unit_next()->addPrev(u->unit_prev);
                    size_of_list--;
                    return u->unit_prev;
                }
                else
                {
                    tail = u->unit_prev();
                    size_of_list--;
                    return tail;
                }

            }
            else
            {
               if (u->unit_next())
                {
                    head = u->unit_next();
                    size_of_list--;
                    return NULL;
                }
                head = NULL;
                tail = NULL;
                size_of_list--;
                return head;

            }
        }
    }

     template <class T>
    void DList<T>::clear()
    {
	   while(size_of_list)
       {
           pop_front();
       }
    }


	template <class T>
	bool DList<T>::empty()
	{
		return (!size_of_list);
	}

	template <class T>
	unsigned DList<T>::size()
	{
		return size_of_list;
	}

	 template <class T>
    void DList<T>::reverse()
    {
	    DList<T>:: Unit* current = head;
	    for (int i = 0; i < size_of_list; i++)
            {
			    DList<T>::Unit* prev = current->unit_next(), next = current->unit_prev();// remember prev and next
			    current->addNext(next);//swap prev and next
			    current->addPrev(prev);
			    current = current->prev;
		    }

	    Unit* new_head = tail;//
	    Unit* new_tail = head;//swaping first_ and next
	    head = new_head;
	    tail = new_tail;

    }



// ---- End of DList template class implementation ----
};
