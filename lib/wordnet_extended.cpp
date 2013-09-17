//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================

#include <math.h>  

#include "wordnet_extended.h"


// TODO should check and ensure all pointers get deleted at appropriate time, and if any memory leak
void WordnetExtended::build_synset_adjacency_list(const std::vector<std::string> & words, UndirectedGraph &adj_list)
{	
	if(!OpenDB)
	{
		// wninit return 0 as no error
		if(wninit())
		{			
			std::cout << "Failed to open wordnet files" << std::endl;
			throw;
		}
	}

	// http://stackoverflow.com/questions/8274451/well-how-does-the-custom-deleter-of-stdunique-ptr-work
	//auto delSynset = [](Synset * p) { free_synset(p); };

	for(std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
	{
		std::string word = *it;

		// get all avalible poses e
		int all_poses = in_wn(&word[0], ALL_POS);
		//loop 4 poses available in wordnet
		for(int i_pos = 2; i_pos <= 8; i_pos = i_pos << 1)
		{
			if(all_poses & i_pos)
			{	
				SynsetPtr synset_sense = findtheinfo_ds2(&word[0], std::sqrt(i_pos), HYPERPTR, ALLSENSES, 1);	
				// c++ notes: http://stackoverflow.com/questions/106508/what-is-a-smart-pointer-and-when-should-i-use-one
				// TODO consider custom deleter here - auto delSynset = [](Synset * p) { free_synset(p); };
				SynsetPtrS synset_ptr(synset_sense);

				// loop all senses
				while(synset_ptr != nullptr)
				{
					SynsetPtrS synset_hyper_ptr = synset_ptr;

					vertex_t pre_vertex = -1;
					// loop all hyper for each sense
					while(synset_hyper_ptr != nullptr)
					{							
						vertex_t v = find_vertex(adj_list, synset_hyper_ptr->hereiam, synset_hyper_ptr->pos);
						if(v == -1)
						{
							//http://www.boost.org/doc/libs/1_54_0/libs/graph/example/undirected_adjacency_list.cpp
							v = boost::add_vertex(adj_list);
							adj_list[v] = synset_hyper_ptr;
						}

						if(pre_vertex != -1 && !boost::edge(pre_vertex, v, adj_list).second)
						{							
							edge_t e; bool b;
							boost::tie(e, b) = boost::add_edge(pre_vertex, v, 1, adj_list);
						}

						pre_vertex = v;

						// TODO handle the case when more than one HYPERPTR synset. see http://wordnet.princeton.edu/man/wnsearch.3WN.html#sect3 
						// move to next hyper sense
						synset_hyper_ptr = SynsetPtrS(synset_hyper_ptr->ptrlist);
					}

					// move to next sibling sense
					synset_ptr = SynsetPtrS(synset_ptr->nextss);
				}
			}
		}
	}
}

int WordnetExtended::compute_distance(const UndirectedGraph &adj_list, const std::string & w1, const std::string & w2)
{
	// processed vertex index map
	std::vector<vertex_t> p(boost::num_vertices(adj_list));
	// distance map
	std::vector<int> d(boost::num_vertices(adj_list));

	// find the synset vertex contains word1	
	// TODO when word exists in more than one synset
	vertex_t v1;
	boost::graph_traits < UndirectedGraph >::vertex_iterator vi, vend;	
	bool found = 0;
	for (boost::tie(vi, vend) = boost::vertices(adj_list); vi != vend; ++vi) 
	{
		// loop all words in current synset vertex
		for(auto i = 0; i < adj_list[*vi] -> wcount; ++i)
		{
			if(adj_list[*vi] -> words[i] == w1)
			{
				v1 = *vi;
				found = 1;
				break;
			}
		}

		if(found) break;
	}

	int ret_dis = INT_MAX;
	// word not found in graph (doesn't exist in wordnet when excuting build_synset_adjacency_list)
	if(!found) return ret_dis;

	// calculate distance to all other vertex
	// TODO check if possible to only calculate distance to vertex contains word2
	boost::dijkstra_shortest_paths(adj_list, v1,
		predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, adj_list))).
		distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, adj_list))));

	// find distance to synset vertex contains word2
	// TODO when word exists in more than one synset
	for (boost::tie(vi, vend) = boost::vertices(adj_list); vi != vend; ++vi) 
	{
		bool found = 0;
		//loop all words in current synset vertex
		for(auto i = 0; i < adj_list[*vi] -> wcount; ++i)
		{
			if(adj_list[*vi] -> words[i] == w2)
			{
				ret_dis = d[*vi];
				found = 1;
				break;
			}
		}

		if(found) break;
	}

	return ret_dis;
}

WordnetExtended::vertex_t WordnetExtended::find_vertex(const UndirectedGraph & adj_list, long hereiam, char *pos)
{
	boost::graph_traits < UndirectedGraph >::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(adj_list); vi != vend; ++vi) 
	{
		if(adj_list[*vi]->hereiam == hereiam && *adj_list[*vi]->pos == *pos)
			return *vi;
	}

	return -1;
}

void WordnetExtended::normalization(std::vector<std::string> & v)
{
	if(!OpenDB)
	{
		// wninit return 0 as no error
		if(wninit())
		{			
			std::cout << "Failed to open wordnet files" << std::endl;
			throw;
		}
	}

	morphinit();

	std::string word;
	for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		word = *it;
		char* morph;
		if(!in_wn(&word[0], ALL_POS))
		{
			for(auto i = 1; i <= 4; ++i)
			{
				morph = morphword(&word[0], i);
				if(morph != NULL)
				{
					*it = morph;
					// TODO have morph in more than one pos
					break;
				}
			}
		}
	}
}