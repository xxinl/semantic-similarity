[Semantic-Similarity](http://xxinl.github.io/semantic-similarity)
===================
C++ tool lib to calculate semantic similarity between two sentences based on method described in paper 

*"Sentence Similarity Based on Semantic Nets and Corpus Statistics" Yuhua Li et al, 2006.*


###[Live Demo](http://semantic-similarity.azurewebsites.net/demo/demo.html)


###Dependencies
Boost and Wordnet C API


###Demo
    SentenceSimilarityLi2006 ss;
    s = ss.compute_similarity("it is a dog", "it is a log");
    s = ss.compute_similarity("it is a dog", "it is a pig");
    
    ===============================
    results:
    "i like that bachelor" vs "i like that unmarrided man" sim:0.386069
    "John is very nice" vs "is John very nice" sim:1
    "I have a pen" vs "where do you live" sim:0.0203516
    "it is a dog" vs "it is a log" sim:0.122288
    "it is a dog" vs "it is a pig" sim:0.442149
    "red alcoholic drink" vs "a bottle of wine" sim:0.105169
    "red alcoholic drink" vs "fresh orange juice" sim:0.0963826
    "red alcoholic drink" vs "an english dictionary" sim:0.0467486
    "it is a dog" vs "that must be your dog" sim:0.720558
    "dogs are animals" vs "they are common pets" sim:0.753427
    "i have a hammer" vs "take some nails" sim:0.0763042
    "canis familiaris are animals" vs "dogs are common pets" sim:0.349904
    "i have a pen" vs "where is ink" sim:0.119264
    "red alcoholic drink" vs "fersh apple juice" sim:0.0762678
    "a glass of cider" vs "a full cup of apple juice" sim:0.534483
    "i have a hammer" vs "take some apples" sim:0.0757834
	
	
###Setup
You will need to download Wordnet3.0/3.1 dictionary
    
    
###Contact
*tonyxliu at hotmail.co.uk*

