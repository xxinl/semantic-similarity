//=======================================================================
// Copyright 2013
// Authors: Xin Liu tonyxliu@hotmail.co.uk
//
// Distributed under GPL v2
//=======================================================================


#ifndef _SENTENCE_SIMILARITY
#define _SENTENCE_SIMILARITY

#include <string>

#include "wordnet_extended.h"

class SentenceSimilarity {

 private:      
      

 public:
  ///// Constructor 
  //ISentenceSimilarity();
  /// Destructor
	 virtual ~SentenceSimilarity(){};

  ///
	virtual float compute_similarity(const std::string &, const std::string &) = 0;
};


///Li et al.(2006)
///http://ants.iis.sinica.edu.tw/3bkmj9ltewxtsrrvnoknfdxrm3zfwrr/55/Sentence%20Similarity%20Based%20on%20Semantic%20Nets%20and%20corpus%20statistics.pdf
class SentenceSimilarityLi2006 : public SentenceSimilarity{
	
	private:      
      

	public:
		/// Constructor 
		SentenceSimilarityLi2006();
		/// Destructor
		~SentenceSimilarityLi2006();

		/// compute similarity between two sentences
		float compute_similarity(const std::string &, const std::string &);
};

#endif