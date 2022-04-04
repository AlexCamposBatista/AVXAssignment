#include "Quicksort.h"
#include <chrono>
#include <random>
#include <iostream>
#include <immintrin.h>


int QuickSort::avxPartition(int* pData, __m256i p, int* writeLeft, int* writeRight) {
	__m256i* p_data = (__m256i*)pData;
	__m256i pivot = p;
	__m256i data = _mm256_loadu_si256(p_data);
	__m256i greaterThanPivot = _mm256_cmpgt_epi32(data, pivot);
	int mask = _mm256_movemask_epi8(greaterThanPivot);

	int fmask = 0;
	int popCnt = 0;
	for (int i = 0; i < 8; i++) {
		if ((1 & mask) == 1) {
			fmask = (fmask << 1) | 1;
			popCnt++;
		}
		else {
			fmask = fmask << 1; 
		}
		mask = mask >> 4;
	}

	

	__m256i permTable = _mm256_loadu_si256((__m256i*) permutationTable[fmask]);
	data = _mm256_permutevar8x32_epi32(data, permTable);

	_mm256_storeu_si256((__m256i*)writeLeft, data);
	_mm256_storeu_si256((__m256i*)writeRight, data);


	return popCnt;
	
}

int QuickSort::avxPartitionInplace(int* left, int* right) {
	const int N = 8;
	int count = 0;
	int* writeLeft = left;
	int* writeRight = right - N+1;

	int temp[3 * N];
	int* tempLeft = &temp[0];
	int* tempRight = &temp[2 * N];
	int* mid = ((right -left)/2) + left;
	int popCnt = 0;


	__m256i* p_data = (__m256i*) left;
	__m256i pivot = _mm256_set1_epi32(*mid);
	
	

	popCnt = avxPartition(left, pivot, tempLeft, tempRight);
	tempLeft += 8 - popCnt;
	tempRight -= popCnt;


	
	popCnt = avxPartition(right - N + 1, pivot, tempLeft, tempRight);
	tempLeft += 8 - popCnt;
	tempRight -= popCnt;



	
	int* readLeft = left + N;
	int* readRight = right - 2 * N + 1;


	while (readRight >= readLeft) {
		
		int* nextPointer;
		if ((readLeft - writeLeft) <= (writeRight - readRight)) {
			nextPointer = readLeft;
			readLeft += N;
		} else {
			nextPointer = readRight;
			readRight -= N;
		}


		popCnt = avxPartition(nextPointer, pivot, writeLeft, writeRight);		
		writeLeft += 8 - popCnt;
		writeRight -= popCnt;


	}
	readRight += N;
	tempRight += N;

	while (readLeft < readRight) {

		int currP = *readLeft++;
		if (currP <= *mid) {
			*tempLeft++ = currP;
		} else{
			*--tempRight = currP;
		}
	}

	
	int leftTempSize = (uint32_t)(int)(tempLeft - &temp[0]);

	for (int i = 0; i < leftTempSize; i++) {
		writeLeft[i] = temp[i];
	}
	writeLeft += leftTempSize;
	int rightTempSize = (&temp[3 * N - 1] - tempRight);
	for (int i = 0; i <= rightTempSize; i++) {
		writeLeft[i] = tempRight[i];
	}


	return writeLeft -left;
}

void QuickSort::avxSort(int* left, int* right) {
	int length = right - left;

	if (length <= 0) {
		return;
	}else if (length <= 16) {

		for (int i = 1; i <= length; i++) {
			int cursor = left[i];
			int j = i;
			while ((j > 0) && (left[j - 1] > cursor)) {
				left[j] = left[j-1];
				j--;
			}
			left[j] = cursor;
		}
		return;
	} 

	int middle = avxPartitionInplace(left, right);
	if (middle == 0) {
		return;
	}

	avxSort(left, (middle - 1) + left);
	avxSort(middle + left, right);

}

void QuickSort::avxTopLevelSort(std::vector<int>& data) {
	avxSort(&data[0], &data[data.size()-1]);
}


double QuickSort::avxQuickSort(std::vector<int>& data) {
	return measuredQuickSort(data, &QuickSort::avxTopLevelSort);
}

void QuickSort::nSort(std::vector<int>& data, int beg, int end) {
	if (beg < end) {
		int i = beg;
		int j = end;
		int x = data[(beg + end) / 2];
		while (i <= j) {
			while (data[i] < x) {
				i++;
			}
			while (data[j] > x) {
				j--;
			}
			if (i <= j) {
				int temp = data[i];
				data[i] = data[j];
				data[j] = temp;
				i++;
				j--;
			}
		}
		nSort(data, beg, j);
		nSort(data, i, end);
	}
}

void QuickSort::nQuickSort(std::vector<int>& data) {
	nSort(data, 0, data.size() - 1);
}


double QuickSort::normalQuickSort(std::vector<int>& data) {
	return measuredQuickSort(data, &QuickSort::nQuickSort);
}


double QuickSort::measuredQuickSort(std::vector<int>& a, void(QuickSort::* sortFunc)(std::vector<int>& a))
{
	auto start = std::chrono::high_resolution_clock::now();

	(this->*sortFunc)(a);

	auto stop = std::chrono::high_resolution_clock::now();

	return (double)(stop - start).count();
}

bool QuickSort::proove(std::vector<int>& a) {
	for (uint64_t i = 0; i < a.size() - 1; ++i) {
		if (a[i] > a[i + 1])
			return false;
	}

	return true;
}

std::vector<int> QuickSort::createRandomData(uint64_t size) {

	std::vector<int> data(size, 0);

	// random gen
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, (int)size);
	generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

	for (int i = 0; i < size; ++i) {
		data[i] = distribution(generator);
	}

	return data;
}

QuickSort::QuickSort() {
	
	//generates a permutationtable for the use of the QuickSort in avx mode
	
	int count;
	int bitPointer;
	//for every possible 8 bit constelation creates an array with permutation indexes
	for (int i = 0; i < 256; i++) {
		count = 0;
		bitPointer = 128;
		for (int j = 0; j < 8; j++) {
			if ((i&bitPointer) == 0) {
				permutationTable[i][count++] = j;
			}
			bitPointer = bitPointer >> 1;
		}
		bitPointer = 128;
		for (int j = 0; j < 8; j++) {
			if ((i & bitPointer) == bitPointer) {
				permutationTable[i][count++] = j;
			}
			bitPointer = bitPointer >> 1;
		}
	}
}
