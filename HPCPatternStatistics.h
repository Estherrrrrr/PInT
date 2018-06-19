#pragma once

#include "HPCParallelPattern.h"
#include <string>
#include "TreeAlgorithms.h"

#define CSV_SEPARATOR_CHAR ","



class HPCPatternStatistic
{
public:
	virtual void Calculate() = 0;

	virtual void Print() = 0;

	virtual void CSVExport(std::string FileName) = 0;
};



class SimilarityMeasure
{
public:
	/* Datastructure to save a sequence of PatternOccurences and link them with similarities */
	struct SimilarityPair;

	struct PatternOccurenceSequence
	{
		std::vector<PatternOccurence*> PatternOccs;
		std::vector<SimilarityPair*> Similarities;
	
		/* A little fork operator, so that we can branch one sequence from another */
		PatternOccurenceSequence* Fork()
		{
			PatternOccurenceSequence* POS;
			POS = new PatternOccurenceSequence;
		
			POS->PatternOccs = this->PatternOccs;
			return POS;
		}
	};

	/* Datastructure to save the similarity between two sequences */
	struct SimilarityPair
	{
		PatternOccurenceSequence* Seq1;
		PatternOccurenceSequence* Seq2;
		float Similarity;
	};


protected:
	SimilarityMeasure(HPCParallelPattern* RootPattern, int maxlength, SearchDirection dir);


	std::vector<SimilarityPair*> SortBySimilarity(std::vector<SimilarityPair*> Sims);


	std::vector<PatternOccurenceSequence*> PatternOccSequences;

	std::vector<SimilarityPair*> Similarities;

	/* Functions to build the sequences of pattern occurences */
	std::vector<PatternOccurenceSequence*> FindPatternOccSeqs(PatternOccurence* PatternOccNode, SearchDirection dir, int maxdepth);

	void VisitPatternTreeNode(PatternTreeNode* CurrentNode, PatternOccurenceSequence* CurrentSequence, std::vector<PatternOccurenceSequence*>* Sequences, SearchDirection dir, int depth, int maxdepth);


	HPCParallelPattern* RootPattern;

	int maxlength;
	
	SearchDirection dir;
};



class JaccardSimilarityStatistic : public HPCPatternStatistic, public SimilarityMeasure
{
public:
	JaccardSimilarityStatistic(HPCParallelPattern* RootPattern, int length, SearchDirection dir);

	void Calculate();

	void Print();

	void CSVExport();

private:
	/* Functions to calculate the Jaccard Similarity */
	float Similarity(PatternOccurenceSequence* Seq1, PatternOccurenceSequence* Seq2);

	std::vector<PatternOccurence*> IntersectByDesignSp(std::vector<PatternOccurence*> Seq1, std::vector<PatternOccurence*> Seq2);

	std::vector<PatternOccurence*> IntersectByPatternName(std::vector<PatternOccurence*> Seq1, std::vector<PatternOccurence*> Seq2);

	std::vector<PatternOccurence*> UnionByDesignSp(std::vector<PatternOccurence*> Seq1, std::vector<PatternOccurence*> Seq2);

	std::vector<PatternOccurence*> UnionByPatternName(std::vector<PatternOccurence*> Seq1, std::vector<PatternOccurence*> Seq2); 	
};



class CyclomaticComplexityStatistic : public HPCPatternStatistic
{
public:
	CyclomaticComplexityStatistic();
	
	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	void SetNodeVisited(PatternTreeNode* Node);

	bool IsNodeVisited(PatternTreeNode* Node);

	std::vector<PatternTreeNode*> VisitedNodes;

	int CountEdges();

	int CountEdges(PatternTreeNode* Root);

	int CountNodes();

	int CountConnectedComponents();

	void MarkConnectedComponent(PatternTreeNode * Node, int ComponentID);

	int Nodes, Edges, ConnectedComponents = 0;

	int CyclomaticComplexity = 0;
};



class LinesOfCodeStatistic : public HPCPatternStatistic
{
public:
	void Calculate();
	
	void Print();

	void CSVExport(std::string FileName);
};



class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic();	

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);
};



/*
 * This statistic calculates the fan-in and fan-out numbers for every pattern which occurs in the code.
 * The fan-in number is calculated as the number of patterns which are a direct parent to the pattern in question.
 * The fan-out number is the number of patterns which are direct children to this pattern.
 */
class FanInFanOutStatistic : public HPCPatternStatistic
{
public:
	FanInFanOutStatistic(int maxdepth);

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	struct FanInFanOutCounter
	{
		HPCParallelPattern* Pattern;
		int FanIn = 0;
		int FanOut = 0;
	};

	void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth);

	void VisitPattern(PatternOccurence* PatternOcc, int depth, int maxdepth);

	FanInFanOutCounter* LookupFIFOCounter(HPCParallelPattern* Pattern);

	FanInFanOutCounter* AddFIFOCounter(HPCParallelPattern* Pattern);

	std::vector<PatternOccurence*> GetUniquePatternOccList(std::vector<PatternOccurence*> PatternOccs);

	void FindParentPatterns(PatternOccurence* Start, std::vector<PatternOccurence*>& Parents, int maxdepth);
	
	void FindChildPatterns(PatternOccurence* Start, std::vector<PatternOccurence*>& Children, int maxdepth);

	void FindNeighbourPatternsRec(PatternTreeNode* Current, std::vector<PatternOccurence*>& Results, SearchDirection dir, int depth, int maxdepth);

	int maxdepth;

	std::vector<FanInFanOutCounter*> FIFOCounter;
};
