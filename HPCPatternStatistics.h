#pragma once

#include "HPCParallelPattern.h"



class HPCPatternStatistic
{
public:
	virtual void Calculate() = 0;

	virtual void Print() = 0;
};



class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic();	

	void Calculate();

	void Print();

private:
	struct PatternOccurenceCounter
	{
		DesignSpace PatternDesignSp;
		std::string PatternName;
		int Count = 0;
	};

	void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth);

	void VisitPattern(HPCParallelPattern* Pattern, int depth, int maxdepth);

	PatternOccurenceCounter* LookupPatternOcc(HPCParallelPattern* Pattern);

	PatternOccurenceCounter* AddPatternOcc(HPCParallelPattern* Pattern);
	
	std::vector<PatternOccurenceCounter*> PatternOccCounter;
};



class FanInFanOutStatistic : public HPCPatternStatistic
{
public:
	FanInFanOutStatistic();

	void Calculate();

	void Print();

private:
	struct FanInFanOutCounter
	{
		DesignSpace PatternDesignSp;
		std::string PatternName;
		int FanIn = 0;
		int FanOut = 0;
	};

	enum SearchDirection
	{
		DIR_Children, DIR_Parents
	};

	void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth);

	void VisitPattern(HPCParallelPattern* Pattern, int depth, int maxdepth);

	FanInFanOutCounter* LookupFIFOCounter(HPCParallelPattern* Pattern);

	FanInFanOutCounter* AddFIFOCounter(HPCParallelPattern* Pattern);

	void FindParentPatterns(HPCParallelPattern* Start, std::vector<HPCParallelPattern*>& Parents, int maxdepth);
	
	void FindChildPatterns(HPCParallelPattern* Start, std::vector<HPCParallelPattern*>& Children, int maxdepth);

	void FindNeighbourPatternsRec(PatternOccurence* Current, std::vector<HPCParallelPattern*>& Results, SearchDirection dir, int depth, int maxdepth);

	std::vector<FanInFanOutCounter*> FIFOCounter;
};
