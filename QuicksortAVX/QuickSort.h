#pragma once
#include <vector>
#include <immintrin.h>
#include <cmath>

class QuickSort {

private:
	void nSort(std::vector<int>& data, int beg, int end);
	void nQuickSort(std::vector<int>& data);
	double measuredQuickSort(std::vector<int>& data, void(QuickSort::* sortFunc)(std::vector<int>& data));

public:
	QuickSort();
	int permutationTable[256][8];
	int avxPartition(int* pdata, __m256i pivot, int* writeLeft, int* writeRight);
	int avxPartitionInplace(int* left, int* right);
	void avxTopLevelSort(std::vector<int>& data);
	void avxSort(int* left, int* right);
	double normalQuickSort(std::vector<int>& data);
	double avxQuickSort(std::vector<int>& data);
	bool proove(std::vector<int>& data);
	std::vector<int> createRandomData(uint64_t size);
};