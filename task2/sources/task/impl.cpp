#include "iface.h"

// Implementation of the task 2 unit testing
namespace Task
{
    struct Node;
    struct Edge;
    typedef Graph<Node, Edge> G;

    struct Node: public G::Node
    {
        int weight;
        Node( G& g): G::Node( g), weight(0){} 
    };

    struct Edge: public G::Edge
    {
        int weight;
        Edge( Node& p, Node& s): G::Edge(p, s), weight(0){} 
    };

    bool uTest( UnitTest *utest_p)
    {
        G graph;
        Node& n1 = graph.create_node();
        Node& n2 = graph.create_node();
        Node& n3 = graph.create_node();

        Edge& e1 = graph.create_edge( n1, n2);

        n1.weight = 10;
        n2.weight = 20;

        UTEST_CHECK( utest_p, &n1 == &e1.pred()); 
        UTEST_CHECK(utest_p, n1.succs_begin() != n1.succs_end());
        UTEST_CHECK(utest_p, n1.preds_begin() == n1.preds_end());
        UTEST_CHECK(utest_p, n2.succs_begin() == n2.succs_end());
        UTEST_CHECK(utest_p, n2.preds_begin() != n2.preds_end());
        UTEST_CHECK(utest_p, n1.uid() != n2.uid());
        UTEST_CHECK(utest_p, n1.uid() != n3.uid());
        UTEST_CHECK(utest_p, n2.uid() != n3.uid());

        graph.remove(e1); // remove the only edge

        UTEST_CHECK(utest_p, n1.succs_begin() == n1.succs_end());
        UTEST_CHECK(utest_p, n1.preds_begin() == n1.preds_end());
        UTEST_CHECK(utest_p, n2.succs_begin() == n2.succs_end());
        UTEST_CHECK(utest_p, n2.preds_begin() == n2.preds_end());

        Edge& e2 = graph.create_edge(n1, n2);
        Edge& e3 = graph.create_edge(n2, n3);
        UTEST_CHECK(utest_p, e2.uid() != e3.uid());
        UTEST_CHECK(utest_p, &n1 == &(n2.first_pred().pred()));
        UTEST_CHECK(utest_p, &n2 == &(n3.first_pred().pred()));
        UTEST_CHECK(utest_p, &n2 == &(n1.first_succ().succ()));
        UTEST_CHECK(utest_p, &n3 == &(n2.first_succ().succ()));
        UTEST_CHECK(utest_p, 0 == n1.num_preds());
        UTEST_CHECK(utest_p, 1 == n1.num_succs());

        UTEST_CHECK(utest_p, 3 == graph.num_nodes());
        UTEST_CHECK(utest_p, 2 == graph.num_edges());

        bool throws_correctly = false;

        try
        {
            n1.first_pred();
        } catch (G::Error e)
        {
            throws_correctly = true; 
        }

        UTEST_CHECK(utest_p, throws_correctly);

        e2.weight = 1;
        e3.weight = 2;
        int sum = 0;
        
        // Iterate through the nodes
        for ( auto n_iter = graph.nodes_begin(), n_end = graph.nodes_end(); 
              n_iter != n_end; ++n_iter)
        {
            sum += (*n_iter).weight;
        }
        // Iterate through the edges
        for ( auto e_iter = graph.edges_begin(), n_end = graph.edges_end();
              e_iter != n_end; ++e_iter)
        {
            sum += (*e_iter).weight;
        }

        UTEST_CHECK(utest_p, sum == 33);
      
        return utest_p->result();
    }
};