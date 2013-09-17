//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================

#ifndef _WORDNET
#define _WORDNET

#include <string>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/tokenizer.hpp>

#include "wn.h"

class WordnetExtended {

private:      

public:
	///// Constructor 
	WordnetExtended(){};
	/// Destructor
	~WordnetExtended(){};

	typedef std::shared_ptr<Synset> SynsetPtrS;

	//graph types
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,	SynsetPtrS, 
																	boost::property < boost::edge_weight_t, int > > UndirectedGraph;
	typedef boost::graph_traits < UndirectedGraph >::vertex_descriptor vertex_t;
	typedef boost::graph_traits < UndirectedGraph >::edge_descriptor edge_t;

	///
	/// c++ notes: use pointers if you want to do arithmetic with them or if you ever have to pass a NULL-pointer.
	///							use references otherwise.
	void build_synset_adjacency_list(const std::vector<std::string> & words, UndirectedGraph &adj_list);

	/// return shortest path
	int compute_distance(const UndirectedGraph & adj_list, const std::string & w1, const std::string & w2);

	/// return whether a synset already exist in graph
	vertex_t find_vertex(const UndirectedGraph & adj_list, long hereiam, char *pos);

	void normalization(std::vector<std::string> & v);
};

#endif