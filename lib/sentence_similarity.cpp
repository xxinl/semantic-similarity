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


float SentenceSimilarityLi2006::compute_similarity(const std::string & s1, const std::string & s2, we::UndirectedGraph & ret_graph)
{
	//TODO validate input params

	boost::tokenizer<> tok1(s1), tok2(s2);
	std::vector<std::string> v1(tok1.begin(), tok1.end()), v2(tok2.begin(), tok2.end());

	we we(_wn_dict_path, _freq_dict_path);
	we.normalization(v1);
	we.normalization(v2);

	// build word corpus from two sentenses
	std::vector<std::string> corpus;
	for(std::vector<std::string>::iterator it_w_corpus = v1.begin(); it_w_corpus != v1.end(); ++it_w_corpus){
		if(std::find(corpus.begin(), corpus.end(), *it_w_corpus) == corpus.end()) {
			corpus.push_back(*it_w_corpus);
		}
	}

	for(std::vector<std::string>::iterator it_w_corpus = v2.begin(); it_w_corpus != v2.end(); ++it_w_corpus){
		if(std::find(corpus.begin(), corpus.end(), *it_w_corpus) == corpus.end()) {
			corpus.push_back(*it_w_corpus);
		}
	}

	we::UndirectedGraph graph;

	// build corpus graph
	we.build_synset_adjacency_list(corpus, graph);

	// semilarity vectors
	std::vector<float> sim1(corpus.size()), sim2(corpus.size());
	
	// loop all words in corpus
	for(std::vector<std::string>::iterator it_w_corpus = corpus.begin(); it_w_corpus != corpus.end(); ++it_w_corpus)
	{
		float freq_weight_w_corpus = we.get_freq_weight(*it_w_corpus);

		// build similarity vectors for s1
		if(std::find(v1.begin(), v1.end(), *it_w_corpus) == v1.end()) 
		{
			// loop all words in s1 to get the synset has max similarity
			float max_sim = 0;
			std::string max_freq_w_s1;
			for(std::vector<std::string>::iterator it_s1_w = v1.begin(); it_s1_w != v1.end(); ++it_s1_w)
			{
				we::vertex_t v_w_s1, v_w_corpus;
				int path_len = we.compute_distance(graph, *it_s1_w, *it_w_corpus, v_w_s1, v_w_corpus);
				// if both *it_t1 and *it are in the graph
				if(path_len != INT_MAX && v_w_s1 != -1 && v_w_corpus != -1)
				{
					int depth = graph[v_w_s1]->depth;
					float sim = calc_word_sim(path_len, depth);
					if(max_sim < sim)
					{
						max_sim = sim;
						max_freq_w_s1 = *it_s1_w;
					}
				}
			}

			float freq_weight_w_s1 = max_sim == 0 ? 0 : we.get_freq_weight(max_freq_w_s1);
			// +1 to smooth 0 lenth
			sim1.push_back(max_sim * freq_weight_w_corpus * freq_weight_w_s1);
		}
		else{
			sim1.push_back(1 * freq_weight_w_corpus * freq_weight_w_corpus);
		}

		// build similarity vectors for s2
		if(std::find(v2.begin(), v2.end(), *it_w_corpus) == v2.end()) 
		{
			// loop all words in s2 to get the synset has max similarity
			float max_sim = 0;
			std::string max_freq_w_s2;
			for(std::vector<std::string>::iterator it_s2_w = v2.begin(); it_s2_w != v2.end(); ++it_s2_w)
			{
				we::vertex_t v_w_s2, v_w_corpus;
				int path_len = we.compute_distance(graph, *it_s2_w, *it_w_corpus, v_w_s2, v_w_corpus);
				// if both *it_t2 and *it are in the graph
				if(path_len != INT_MAX && v_w_s2 != -1 && v_w_corpus != -1)
				{					
					int depth = graph[v_w_s2]->depth;
					float sim = calc_word_sim(path_len, depth);
					if(max_sim < sim)
					{
						max_sim = sim;
						max_freq_w_s2 = *it_s2_w;
					}
				}
			}

			float freq_weight_w_s2 = max_sim == 0 ? 0 : we.get_freq_weight(max_freq_w_s2);
			// +1 to smooth 0 lenth
			sim2.push_back(max_sim * freq_weight_w_corpus * freq_weight_w_s2);
		}
		else{
			sim2.push_back(1* freq_weight_w_corpus * freq_weight_w_corpus);
		}
	}

	ret_graph = graph;
	return calc_cos_similarity(sim1, sim2);
}

float SentenceSimilarityLi2006::calc_cos_similarity(const std::vector<float> & sim1, const std::vector<float> & sim2)
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
float SentenceSimilarityLi2006::calc_word_sim(int length, int depth)
{
	// +1 to smooth 0 lenth
	length ++;

	float length_sim = exp(_S_ALPHA * (0 - length));
	float depth_sim = (exp(_S_BETA * depth) - exp(_S_BETA * (0 - depth))) 
		/ (exp(_S_BETA * depth) + exp(_S_BETA * (0 - depth)));

	return length_sim * depth_sim;
}