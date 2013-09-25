//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================

#include <iterator>
#include <math.h>

#include <boost/tokenizer.hpp>

#include "sentence_similarity.h"

typedef WordnetExtended we;


///Li et al.(2006) 3.1.3
const float _S_ALPHA = 0.2;
const float _S_BETA = 0.45;


float get_cosine_similarity(const std::vector<float> & sim1, const std::vector<float> & si2);
float calc_sim(int length, int depth);


float SentenceSimilarityLi2006::compute_similarity(const std::string & s1, const std::string & s2, WordnetExtended::UndirectedGraph & ret_graph)
{
	//TODO validate input params

	boost::tokenizer<> tok1(s1), tok2(s2);
	std::vector<std::string> v1(tok1.begin(), tok1.end()), v2(tok2.begin(), tok2.end());

	we we(WN_DICT_PATH);
	we.normalization(v1);
	we.normalization(v2);

	// build word corpus from two sentenses
	std::vector<std::string> corpus;
	for(std::vector<std::string>::iterator it = v1.begin(); it != v1.end(); ++it){
		if(std::find(corpus.begin(), corpus.end(), *it) == corpus.end()) {
			corpus.push_back(*it);
		}
	}

	for(std::vector<std::string>::iterator it = v2.begin(); it != v2.end(); ++it){
		if(std::find(corpus.begin(), corpus.end(), *it) == corpus.end()) {
			corpus.push_back(*it);
		}
	}

	we::UndirectedGraph graph;

	// build corpus graph
	we.build_synset_adjacency_list(corpus, graph);

	// semilarity vectors
	std::vector<float> sim1(corpus.size()), sim2(corpus.size());
	
	// build similarity vectors for s1
	for(std::vector<std::string>::iterator it = corpus.begin(); it != corpus.end(); ++it)
	{
		// build similarity vectors for s1
		if(std::find(v1.begin(), v1.end(), *it) == v1.end()) 
		{
			// loop all words in s1 to get the synset has max similarity
			float max_sim = 0;
			for(std::vector<std::string>::iterator it_t1 = v1.begin(); it_t1 != v1.end(); ++it_t1)
			{
				we::vertex_t v_it_t1, v_it;
				int path_len = we.compute_distance(graph, *it_t1, *it, v_it_t1, v_it);
				// if both *it_t1 and *it are in the graph
				if(path_len != INT_MAX && v_it_t1 != -1 && v_it != -1)
				{
					int depth = graph[v_it_t1]->depth;
					float sim = calc_sim(path_len, depth);
					if(max_sim < sim)
						max_sim = sim;
				}
			}

			// +1 to smooth 0 lenth
			sim1.push_back(max_sim);
		}
		else{
			sim1.push_back(1);
		}

		// build similarity vectors for s2
		if(std::find(v2.begin(), v2.end(), *it) == v2.end()) 
		{
			// loop all words in s1 to get the synset has max similarity
			float max_sim = 0;
			for(std::vector<std::string>::iterator it_t2 = v2.begin(); it_t2 != v2.end(); ++it_t2)
			{
				we::vertex_t v_it_t2, v_it;
				int path_len = we.compute_distance(graph, *it_t2, *it, v_it_t2, v_it);
				// if both *it_t1 and *it are in the graph
				if(path_len != INT_MAX && v_it_t2 != -1 && v_it != -1)
				{					
					int depth = graph[v_it_t2]->depth;
					float sim = calc_sim(path_len, depth);
					if(max_sim < sim)
						max_sim = sim;
				}
			}

			// +1 to smooth 0 lenth
			sim2.push_back(max_sim);
		}
		else{
			sim2.push_back(1);
		}
	}

	ret_graph = graph;
	return get_cosine_similarity(sim1, sim2);
}

float get_cosine_similarity(const std::vector<float> & sim1, const std::vector<float> & sim2)
{
	//TODO exception handling
	if(sim1.size() != sim2.size()) return -1;

	float numerator = 0, denominator1 = 0, denominator2 = 0;
	for(auto i = 0; i < sim1.size(); i++)
	{
		numerator += sim1[i] * sim2[i];
		denominator1 += sim1[i] * sim1[i];
		denominator2 += sim2[i] * sim2[i];
	}

	return numerator / (sqrt(denominator1) * sqrt(denominator2));
}

// Li et al.(2006) 3.1.3
float calc_sim(int length, int depth)
{
	// +1 to smooth 0 lenth
	length ++;

	float length_sim = exp(_S_ALPHA * (0 - length));
	float depth_sim = (exp(_S_BETA * depth) - exp(_S_BETA * (0 - depth))) 
		/ (exp(_S_BETA * depth) + exp(_S_BETA * (0 - depth)));

	return length_sim * depth_sim;
}