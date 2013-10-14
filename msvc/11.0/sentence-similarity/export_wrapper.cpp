//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================

#include <oaidl.h>
#include <comutil.h>

#include "sentence_similarity.h"


extern "C"	__declspec(dllexport)
	float compute_similarity(WordnetExtended* pWne, const char* s1, const char* s2, SAFEARRAY** vertices, SAFEARRAY** edges)
{		
	SentenceSimilarityLi2006 ss(*pWne);
	WordnetExtended::UndirectedGraph graph;

	float sim = ss.compute_similarity(s1, s2, graph);
	
	// construct vertices array
	SAFEARRAY *pV = SafeArrayCreateVector(VT_BSTR, 0, graph.m_vertices.size());

	boost::graph_traits < WordnetExtended::UndirectedGraph >::vertex_iterator vi, vend;

	LONG i = 0;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) 
	{
		int wslen = MultiByteToWideChar(CP_ACP, 0, graph[*vi]->words[0], strlen(graph[*vi]->words[0]), 0, 0);
		BSTR bstr = SysAllocStringLen(0, wslen);
		MultiByteToWideChar(CP_ACP, 0, graph[*vi]->words[0], strlen(graph[*vi]->words[0]), bstr, wslen);
		SafeArrayPutElement(pV, &i, SysAllocString(bstr));
		SysFreeString(bstr);

		i++;
	}
	*vertices = pV;

	*edges = pV;

	// construct edges array
	SAFEARRAY *pE = SafeArrayCreateVector(VT_BSTR, 0, graph.m_edges.size());

	typedef boost::property_map<WordnetExtended::UndirectedGraph, boost::vertex_index_t>::type VertexIndex;
	VertexIndex index = get(boost::vertex_index, graph);
	boost::graph_traits<WordnetExtended::UndirectedGraph>::edge_iterator ei, ei_end;

	i = 0;
	for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei)
	{		
		std::string edge = std::to_string(index[source(*ei, graph)]) + "-" + std::to_string(index[target(*ei, graph)]);
		const char* str = edge.c_str();
		int wslen = MultiByteToWideChar(CP_ACP, 0, str, strlen(str), 0, 0);
		BSTR bstr = SysAllocStringLen(0, wslen);
		MultiByteToWideChar(CP_ACP, 0, str, strlen(str), bstr, wslen);
		SafeArrayPutElement(pE, &i, SysAllocString(bstr));
		SysFreeString(bstr);

		i++;
	}
	*edges = pE;

	return sim;
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
	WordnetExtended* connect_wne(char* wn_path, char* freq_path) {
		return new WordnetExtended(wn_path, freq_path);
}

extern "C" __declspec(dllexport)
	void disconnect_wne(WordnetExtended* pWne) {
		delete(pWne);
}


