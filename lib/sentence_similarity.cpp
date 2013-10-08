//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================

#include <iterator>
#include <math.h>
//#include <thread>

#include <boost/tokenizer.hpp>
//#include <boost/thread.hpp>

#include "sentence_similarity.h"


#ifndef _DEBUG
# define DEBUG_PRINT(s)
#else
# define DEBUG_PRINT(s) std::cerr << s << std::endl
#endif


typedef WordnetExtended we;


float SentenceSimilarityLi2006::compute_similarity(const std::string & s1, const std::string & s2, we::UndirectedGraph & graph)
{
	//TODO validate input params

	graph.clear();

	boost::tokenizer<> tok1(s1), tok2(s2);
	std::vector<std::string> v1(tok1.begin(), tok1.end()), v2(tok2.begin(), tok2.end());

	_wne.normalization(v1);
	_wne.normalization(v2);

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

	// build corpus graph
	_wne.build_synset_adjacency_list(corpus, graph);

	// semilarity vectors
	std::vector<float> sim1, sim2;

	build_sim_vector(_wne, graph, v1, corpus, sim1);
	build_sim_vector(_wne, graph, v2, corpus, sim2);

	/*boost::thread t1(boost::bind(&SentenceSimilarityLi2006::build_sim_vector, this, _wne, graph, v1, corpus, boost::ref(sim1)));
	boost::thread t2(boost::bind(&SentenceSimilarityLi2006::build_sim_vector, this, _wne, graph, v2, corpus, boost::ref(sim2)));
	
	t1.join();
	t2.join();*/

	return calc_cos_similarity(sim1, sim2);
}


void SentenceSimilarityLi2006::build_sim_vector(we & wne,  we::UndirectedGraph & graph,
																															const std::vector<std::string> & s, 
																															const std::vector<std::string> & corpus,
																															std::vector<float> & sim)
{
	// loop all words in corpus
	for(std::vector<std::string>::const_iterator it_corpus = corpus.begin(); it_corpus != corpus.end(); ++it_corpus)
	{
		float freq_weight_w_corpus = wne.get_freq_weight(*it_corpus);

		// build similarity vectors for s
		if(std::find(s.begin(), s.end(), *it_corpus) == s.end()) 
		{
			// loop all words in s to get the synset has max similarity
			float max_sim = 0;
			std::string max_sim_word;
			for(std::vector<std::string>::const_iterator it_s = s.begin(); it_s != s.end(); ++it_s)
			{
				we::vertex_t v_w_s, v_w_corpus;
				int path_len = wne.compute_distance(graph, *it_s, *it_corpus, v_w_s, v_w_corpus);
				// if both *it_s and *it_corpus are in the graph
				if(path_len != INT_MAX && v_w_s != -1 && v_w_corpus != -1)
				{
					int depth = graph[v_w_s]->depth;
					float sim = calc_word_sim(path_len, depth);
					if(max_sim < sim)
					{
						max_sim = sim;
						max_sim_word = *it_s;
					}
				}
			}

			float freq_weight_w_s1 = max_sim == 0 ? 0 : wne.get_freq_weight(max_sim_word);
			// +1 to smooth 0 lenth
			sim.push_back(max_sim * freq_weight_w_corpus * freq_weight_w_s1);
		}
		else{
			sim.push_back(1 * freq_weight_w_corpus * freq_weight_w_corpus);
		}
	}
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