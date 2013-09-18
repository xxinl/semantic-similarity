Sentence-Similarity
===================
C++ tool lib to calculate semantic similarity between two sentences using method described in paper "Sentence Similarity Based on Semantic Nets
and Corpus Statistics" Yuhua Li et al, 2006.


Still working in progress...
----------------------------
Current version can only calculate path length distance similarity in synsets graph


Dependencies
------------
Boost and Wordnet C API


Demo
----
    SentenceSimilarityLi2006 ss;
	  s = ss.compute_similarity("i am a boy", "he has cars");
	  s = ss.compute_similarity("i run into a hole", "John drinks beer");
	
	
Setup
-----
You will need to download Wordnet3.0/3.1 dictionary and config path in wn.h

  e.g. 
   
    #define DEFAULTPATH	"C:\\Users\\XinL\\Downloads\\libs\\WordNet-3.0\\dict"
    
    
Contact
-------
tonyxliu at hotmail.co.uk

