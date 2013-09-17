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

SentenceSimilarityLi2006::SentenceSimilarityLi2006(){}
SentenceSimilarityLi2006::~SentenceSimilarityLi2006(){}

extern void print_graph(const we::UndirectedGraph & g);

float get_cosine_similarity(const std::vector<float> & sim1, const std::vector<float> & si2);

float SentenceSimilarityLi2006::compute_similarity(const std::string & s1, const std::string & s2)
{
	//TODO validate input params

	boost::tokenizer<> tok1(s1), tok2(s2);
	std::vector<std::string> v1(tok1.begin(), tok1.end()), v2(tok2.begin(), tok2.end());

	we we;
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
	print_graph(graph);

	// semilarity vectors
	std::vector<float> sim1(corpus.size()), sim2(corpus.size());

	// build similarity vectors for s1 & s3
	for(std::vector<std::string>::iterator it = corpus.begin(); it != corpus.end(); ++it)
	{
		// s1
		if(std::find(v1.begin(), v1.end(), *it) == v1.end()) 
		{
			// loop all words in s1 to get the min path length(synset distance)
			int min_path_len = INT_MAX;
			for(std::vector<std::string>::iterator it_t1 = v1.begin(); it_t1 != v1.end(); ++it_t1)
			{
				int path_len = we.compute_distance(graph, *it_t1, *it);
				if(path_len < min_path_len)
					min_path_len = path_len;
			}

			// +1 to smooth 0 lenth
			sim1.push_back(exp(0 - min_path_len - 1));
		}
		else{
			sim1.push_back(1);
		}

		// s2
		if(std::find(v2.begin(), v2.end(), *it) == v2.end()) 
		{
			// loop all words in s1 to get the min path length(synset distance)
			int min_path_len = INT_MAX;
			for(std::vector<std::string>::iterator it_t2 = v2.begin(); it_t2 != v2.end(); ++it_t2)
			{
				int path_len = we.compute_distance(graph, *it_t2, *it);
				if(path_len < min_path_len)
					min_path_len = path_len;
			}

			// +1 to smooth 0 lenth
			sim2.push_back(exp(0 - min_path_len - 1));
		}
		else{
			sim2.push_back(1);
		}
	}

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
