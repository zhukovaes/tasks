/**
 * @file: list_utest.cpp 
 * Implementation of testing for lists 
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#include "utils_iface.h"

using namespace Utils;

static const int LIST_TEST_NUM_NODES = 1000;


/** Define the  lists we use */
enum TestListType
{
    LIST_ONE,
    LIST_TWO,
    LIST_THREE,
    LISTS_NUM
};

static const TestListType TEST_LIST_ID = LIST_THREE; 

/** Derive class of linked objects */
class ClassAList: public MListIface< ClassAList, LISTS_NUM>
{

};

/** Derive class from A */
class B: public ClassAList
{

};

/** MList testing */
static bool uTestMList( UnitTest *utest_p)
{
    B *obj1 = new B();
    B *obj2 = new B();
    B *obj3 = new B();
    obj1->attach( LIST_ONE, obj2);
    obj1->attach( LIST_TWO, obj3);
    UTEST_CHECK( utest_p, areEqP( obj1->next( LIST_ONE), obj2));
    UTEST_CHECK( utest_p, areEqP( obj1->next( LIST_TWO), obj3));
    UTEST_CHECK( utest_p, isNullP( obj1->prev( LIST_ONE)));
    UTEST_CHECK( utest_p, isNullP( obj1->prev( LIST_TWO)));
    UTEST_CHECK( utest_p, areEqP( obj2->prev( LIST_ONE), obj1));
    UTEST_CHECK( utest_p, areEqP( obj3->prev( LIST_TWO), obj1));
    UTEST_CHECK( utest_p, isNullP( obj3->prev( LIST_ONE)));
    UTEST_CHECK( utest_p, isNullP( obj2->prev( LIST_TWO)));
    UTEST_CHECK( utest_p, isNullP( obj2->next( LIST_ONE)));
    UTEST_CHECK( utest_p, isNullP( obj3->next( LIST_TWO)));
    obj2->detachAll();
    delete obj2;
    delete obj1;
    delete obj3;
    return utest_p->result();
}


struct MyMListDummy: public MListIface<MyMListDummy, LISTS_NUM>
{
public:
    long long i;
    long long j;
    MyMListDummy( MyMListDummy* ptr): MListIface<MyMListDummy, LISTS_NUM>( TEST_LIST_ID, ptr){};
};

struct MySListDummy: public SListIface<MySListDummy>
{
public:
    long long i;
    long long j;
    MySListDummy( MySListDummy* ptr): SListIface<MySListDummy>( ptr){};
};

class TListAList{};
class TListBList{};

class TListA: public ListItem< TListA, TListAList>
{
    int a;
};

class TListB: public TListA, public ListItem< TListB, TListBList>
{
    typedef ListItem< TListB, TListBList> List;
public:
    int i;

    TListB( TListB* ptr): ListItem< TListB, TListBList>( ptr){};

    TListB *nextB(){ return static_cast< TListB *>( List::next() );}
    TListB *prevB(){ return static_cast< TListB *>( List::prev() );}
};

/**
 * Test tagged list
 */
static bool 
uTestTagList( UnitTest *utest_p)
{
    TListB *list = NULL;
    TListB *list_temp = NULL;

    /* Create */
    for ( int i = 0; i < LIST_TEST_NUM_NODES; i++)
    {
        list = new TListB( list);
    }
    /* Iterate and access data */
    list_temp = list;
    while ( isNotNullP( list_temp))
    {
        list_temp->i = 10;
        list_temp = list_temp->nextB();
    }
    /* Iterate and access data */
    list_temp = list;
    while ( isNotNullP( list_temp))
    {
        TListB *next = list_temp->nextB();
        delete list_temp;
        list_temp = next;
    }
    return utest_p->result();
}

/**
 * Test list classes operation
 */
bool Utils::uTestList( UnitTest *utest_p)
{
    uTestMList( utest_p);
    uTestTagList( utest_p);

    return utest_p->result();
}

