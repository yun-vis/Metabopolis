#ifndef _RoadGraph_H
#define _RoadGraph_H

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

#include "BaseGraph.h"
#include "RoadVertexProperty.h"
#include "RoadEdgeProperty.h"


typedef adjacency_list< vecS, listS, undirectedS,
                        RoadVertexProperty, RoadEdgeProperty,
                        BaseGraphProperty >  RoadGraph;


void optimizeEdgeOrders( RoadGraph & graph );
void printGraph( RoadGraph & graph );
void clearGraph( RoadGraph & graph );


#endif  // _RoadGraph_H
