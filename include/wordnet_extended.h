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

public:
	typedef std::shared_ptr<Synset> SynsetPtrS;

	//graph types
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,	SynsetPtrS, 
		boost::property < boost::edge_weight_t, int > > UndirectedGraph;
	typedef boost::graph_traits < UndirectedGraph >::vertex_descriptor vertex_t;
	typedef boost::graph_traits < UndirectedGraph >::edge_descriptor edge_t;

private:      
	// frequency dict path
	std::string _freq_dict_path;
	// wordnet dict path
	std::string _wn_dict_path;

	bool _freq_dict_loaded;
	long _freq_total;
	std::map<std::string, int> _freq_dict;

	void mark_depth(const vertex_t v, const UndirectedGraph & g, int depth,
		std::vector<vertex_t> & passed_vertices);

	void load_freq_dict();

public:
	///// Constructor 
	WordnetExtended(std::string wn_path, std::string freq_path) 
		: _wn_dict_path(wn_path), _freq_dict_path(freq_path), _freq_dict_loaded(false){};

	/// Destructor
	~WordnetExtended(){};


	///
	/// c++ notes: use pointers if you want to do arithmetic with them or if you ever have to pass a NULL-pointer.
	///							use references otherwise.
	void build_synset_adjacency_list(const std::vector<std::string> & words, UndirectedGraph &adj_list);

	/// return shortest path
	///		v1, v2 is the vertex contains word that being used for calculation
	int compute_distance(const UndirectedGraph & adj_list, const std::string & w1, const std::string & w2,
		vertex_t & v1, vertex_t & v2);

	/// return whether a synset already exist in graph
	vertex_t find_vertex(const UndirectedGraph & adj_list, long hereiam, char *pos);

	void normalization(std::vector<std::string> &);

	///Li et al.(2006) 4.2.2
	float get_freq_weight(const std::string &);
};

#endif