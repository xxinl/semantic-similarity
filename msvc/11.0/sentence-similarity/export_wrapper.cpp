//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================

#include "sentence_similarity.h"

extern "C"	__declspec(dllexport)
	float compute_similarity(WordnetExtended* pWne, const char* s1, const char* s2)
{		
	SentenceSimilarityLi2006 ss(*pWne);
	WordnetExtended::UndirectedGraph graph;
	return ss.compute_similarity(s1, s2, graph);
}

extern "C"	__declspec(dllexport)
	float compute_similarity2(const char* s1, const char* s2)
{		
	WordnetExtended wne("C:\\Users\\XinL\\Downloads\\libs\\WordNet-3.0\\dict",
		"C:\\Projects\\sentence-similarity\\dicts\\freq.txt");
	SentenceSimilarityLi2006 ss(wne);
	WordnetExtended::UndirectedGraph graph;
	return ss.compute_similarity(s1, s2, graph);
}

extern "C" __declspec(dllexport)
	WordnetExtended* connect(char* wn_path, char* freq_path) {
		return new WordnetExtended(wn_path, freq_path);
}

extern "C" __declspec(dllexport)
	void disconnect(WordnetExtended* pWne) {
		delete(pWne);
}


