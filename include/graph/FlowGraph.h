#ifndef _FlowGraph_H
#define _FlowGraph_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>

// force-directed layout
#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

using namespace boost;

#include "BaseGraph.h"
#include "FlowVertexProperty.h"
#include "FlowEdgeProperty.h"

namespace boost {
    struct SampleGraph {

        typedef adjacency_list< vecS, listS, directedS,
                FlowVertexProperty, FlowEdgeProperty,
                BaseGraphProperty >  FlowGraph;

        typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
        typedef adjacency_list < vecS, vecS, directedS, no_property,
                                 property < edge_capacity_t, long,
                                 property < edge_residual_capacity_t, long,
                                 property < edge_reverse_t, Traits::edge_descriptor,
                                 property < edge_weight_t, long > > > > > MyFlowGraph;
        typedef property_map < MyFlowGraph, vertex_index_t >::type VertexIndex;
        typedef property_map < MyFlowGraph, edge_capacity_t >::type Capacity;
        typedef property_map < MyFlowGraph, edge_residual_capacity_t >::type ResidualCapacity;
        typedef property_map < MyFlowGraph, edge_weight_t >::type Weight;
        typedef property_map < MyFlowGraph, edge_reverse_t>::type Reversed;
        typedef boost::graph_traits< MyFlowGraph >::vertices_size_type size_type;
        typedef Traits::vertex_descriptor vertex_descriptor;

        class EdgeAdder {
        public:
            EdgeAdder( MyFlowGraph & g, Weight & w, Capacity & c, Reversed & rev, ResidualCapacity & residualCapacity )
                    : m_g(g), m_w(w), m_cap(c), m_resCap( residualCapacity ), m_rev( rev ) {}
            void addEdge( vertex_descriptor v, vertex_descriptor w, long weight, long capacity ) {
                Traits::edge_descriptor e,f;
                e = add( v, w, weight, capacity );
                f = add( w, v, -weight, 0 );
                m_rev[e] = f;
                m_rev[f] = e;
            }
        private:
            Traits::edge_descriptor add( vertex_descriptor v, vertex_descriptor w, long weight, long capacity ) {
                bool b;
                Traits::edge_descriptor e;
                boost::tie( e, b ) = add_edge( vertex( v, m_g ), vertex( w, m_g ), m_g );
                if (!b) {
                    std::cerr << "Edge between " << v << " and " << w << " already exists." << std::endl;
                    std::abort();
                }
                m_cap[e] = capacity;
                m_w[e] = weight;
                return e;
            }
            MyFlowGraph & m_g;
            Weight & m_w;
            Capacity & m_cap;
            ResidualCapacity & m_resCap;
            Reversed & m_rev;
        };

        static void getFlowGraph( FlowGraph &f,  MyFlowGraph &g, vertex_descriptor & s, vertex_descriptor & t ) {

            g.clear();
            size_type N( num_vertices( f ) );
            typedef property_map < MyFlowGraph, edge_reverse_t >::type Reversed;

            for( size_type i = 0; i < N; ++i ){
                add_vertex( g );
            }
            Capacity  capacity = get( edge_capacity, g );
            Reversed rev = get( edge_reverse, g );
            ResidualCapacity residual_capacity = get( edge_residual_capacity, g );
            Weight weight = get( edge_weight, g );

            s = vertex( 0, g );
            t = vertex( 1, g );

            EdgeAdder ea( g, weight, capacity, rev, residual_capacity );
#ifdef DEBUG
            BGL_FORALL_VERTICES( vd, f, FlowGraph ){
                cerr << "id = " << f[ vd ].id << endl;
            }
#endif // DEBUG
            BGL_FORALL_EDGES( ed, f, FlowGraph ){
                FlowGraph::vertex_descriptor vdS = source( ed, f );
                FlowGraph::vertex_descriptor vdT = target( ed, f );
                unsigned int idS = f[ vdS ].id;
                unsigned int idT = f[ vdT ].id;
                //cerr << "idS = " << idS << " idT = " << idT << " capacity = " << f[ ed ].capacity << endl;
                ea.addEdge( idS, idT, f[ ed ].weight, f[ ed ].capacity );
            }
        }
    };
} //boost

#ifdef WU
typedef adjacency_list< vecS, vecS, directedS,
        no_property, property< edge_weight_t, double > > DiGraph;

typedef graph_traits< DiGraph > DiGraphTraits;
typedef graph_traits< DiGraph >::vertex_descriptor DiVertexDescriptor;
typedef graph_traits< DiGraph >::edge_descriptor DiEdgeDescriptor;
typedef pair< int, int > DiEdge;
typedef graph_traits< DiGraph >::vertex_iterator DiVertexIterator;
typedef graph_traits< DiGraph >::edge_iterator DiEdgeIterator;
typedef property_map< DiGraph, vertex_index_t >::type DiVertexIndexMap;
//typedef property_map< DiGraph, edge_index_t >::type DiEdgeIndexMap;
typedef property_map< DiGraph, edge_weight_t >::type DiEdgeWeightMap;
#endif // WU

typedef adjacency_list< vecS, listS, directedS,
                        FlowVertexProperty, FlowEdgeProperty,
                        BaseGraphProperty >  FlowGraph;

void printGraph( FlowGraph & graph );
void clearGraph( FlowGraph & graph );

class FlowPath
{
public:
    unsigned int id;
    bool isSelected;                    // if is selected
    bool isNeighbor;                    // if is neighbor selected
    unsigned int subsysID;		        // id in the subsystem
    //unsigned int startReactIDinMetaG;   // start reaction ID in metabolite graph
    //unsigned int endReactIDinMetaG;     // end reaction ID in metabolite graph
    string name;					    // metabolite name
    vector < int > roadIDPath;          // path with roadID
};

class LocalShortestPath
{
public:
    unsigned int subID;
    string metaName;
    unsigned int sourceID;          // id of reaction in metabolite graph
    unsigned int targetID;          // id of reaction in metabolite graph
    unsigned int sourceTID;         // treemap id of reaction in metabolite graph
    unsigned int targetTID;         // treemap id of reaction in metabolite graph
    vector < int > roadIDPath;      // path with roadID
};

#endif  // _FlowGraph_H
