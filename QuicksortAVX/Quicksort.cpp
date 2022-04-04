#include "Quicksort.h"
#include <chrono>
#include <random>
#include <iostream>
#include <immintrin.h>


int QuickSort::avxPartition(int* pData, __m256i p, int* writeLeft, int* writeRight) {
	__m256i* p_data = (__m256i*)pData;
	__m256i pivot = p;

	//loads data from pData
	__m256i data = _mm256_loadu_si256(p_data);

	//checks which value stored in data is bigger than the pivot element
	__m256i greaterThanPivot = _mm256_cmpgt_epi32(data, pivot);

	//gathers the result from greaterThanPivot into one scalar value
	int mask = _mm256_movemask_epi8(greaterThanPivot);

	//This section converts the values in mask. Every bit from movemask is represented 4 times.
	//With the for-loop all the redundant bits get removed. The reason for this is to use it as index for permutationtable in fmask.
	//Also counts popCount and there is no need to use the avx method
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

	
	//load permutation table
	__m256i permTable = _mm256_loadu_si256((__m256i*) permutationTable[fmask]);

	//apply permutation table to data
	data = _mm256_permutevar8x32_epi32(data, permTable);

	//store the data in the writeLeft pointer and in the writeRight pointer
	_mm256_storeu_si256((__m256i*)writeLeft, data);
	_mm256_storeu_si256((__m256i*)writeRight, data);


	return popCnt;
	
}

int QuickSort::avxPartitionInplace(int* left, int* right) {
	
	//8 Elemente pro avx2 register
	const int N = 8;

	int count = 0;
	int* writeLeft = left;
	int* writeRight = right - N+1;

	//temp register to be able to use the same data array
	int temp[3 * N];
	int* tempLeft = &temp[0];
	int* tempRight = &temp[2 * N];

	//Pivot element
	int* mid = ((right -left)/2) + left;
	int popCnt = 0;


	//load pivot value to a 8 value register 
	__m256i* p_data = (__m256i*) left;
	__m256i pivot = _mm256_set1_epi32(*mid);
	
	
	//Partitionate the most left and most right 8 value block and store them in the temp register
	popCnt = avxPartition(left, pivot, tempLeft, tempRight);
	tempLeft += 8 - popCnt;
	tempRight -= popCnt;

	popCnt = avxPartition(right - N + 1, pivot, tempLeft, tempRight);
	tempLeft += 8 - popCnt;
	tempRight -= popCnt;



	
	int* readLeft = left + N;
	int* readRight = right - 2 * N + 1;

	//As long as there is a 8 Value block, partitionate them and store the smaller values than pivot left in data
	//and bigger values than pivot right in data
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

	//If the size of the block is smaller than 8, partition them and store them in the middle block of the temp register
	while (readLeft < readRight) {

		int currP = *readLeft++;
		if (currP <= *mid) {
			*tempLeft++ = currP;
		} else{
			*--tempRight = currP;
		}
	}

	//get the amount of smaller values from the temp register
	int leftTempSize = (uint32_t)(int)(tempLeft - &temp[0]);

	//write all smaller temp register to the data array
	for (int i = 0; i < leftTempSize; i++) {
		writeLeft[i] = temp[i];
	}

	writeLeft += leftTempSize;
	
	//get the amount of bigger values from the temp register
	int rightTempSize = (&temp[3 * N - 1] - tempRight);
	
	//write all bigger temp register to the data array
	for (int i = 0; i <= rightTempSize; i++) {
		writeLeft[i] = tempRight[i];
	}


	return writeLeft -left;
}

void QuickSort::avxSort(int* left, int* right) {

	//current length of the block
	int length = right - left;
	
	//if there is no data to be sorted
	if (length <= 0) {
		return;

	//for performance usage, insertion sort is used for partitions which are smaller than 17 bits
	}else if (length <= 16) {

		//insertionsort 
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
	
	//partition the current partition in two partitions
	int middle = avxPartitionInplace(left, right);

	//checks if all elements in the current partition
	if (middle <= 0) {
		return;
	}

	//sorts the both partitions created from the avxPartitionInplace
	avxSort(left, (middle - 1) + left);
	avxSort(middle + left, right);

}

//Wrappermethod to call the avx Quicksort
void QuickSort::avxTopLevelSort(std::vector<int>& data) {
	avxSort(&data[0], &data[data.size()-1]);
}

//Wrappermethod to call the measurement method and the avx Quicksort
double QuickSort::avxQuickSort(std::vector<int>& data) {
	return measuredQuickSort(data, &QuickSort::avxTopLevelSort);
}

//Normal Quicksort
void QuickSort::nSort(std::vector<int>& data, int beg, int end) {

	//checks if there is more than 1 element
	if (beg < end) {
		int i = beg;
		int j = end;

		//Pivot element
		int x = data[(beg + end) / 2];
		
		//Checks if the i and j pointer did cross eachother. Swaps elements if they are in the wrong position
		while (i <= j) {
			
			//Skip until data in data[i] is not smaller than the pivot element
			while (data[i] < x) {
				i++;
			}
			//Skip until data in data[j] is not bigger than the pivot element
			while (data[j] > x) {
				j--;
			}
			//Swap data[i] with data[j]
			if (i <= j) {
				int temp = data[i];
				data[i] = data[j];
				data[j] = temp;
				i++;
				j--;
			}
		}

		//create two partitions on the current left partition
		nSort(data, beg, j);

		//create two partitions on the current right partition
		nSort(data, i, end);
	}
}

//Wrappermethod to call the normal Quicksort
void QuickSort::nQuickSort(std::vector<int>& data) {
	nSort(data, 0, data.size() - 1);
}

//Wrappermethod to call the measurement method and the normal Quicksort
double QuickSort::normalQuickSort(std::vector<int>& data) {
	return measuredQuickSort(data, &QuickSort::nQuickSort);
}

//measure time of the sorting method. This method was from parallelMergesort.
double QuickSort::measuredQuickSort(std::vector<int>& a, void(QuickSort::* sortFunc)(std::vector<int>& a))
{
	auto start = std::chrono::high_resolution_clock::now();

	(this->*sortFunc)(a);

	auto stop = std::chrono::high_resolution_clock::now();

	return (double)(stop - start).count();
}

//proves if the array is sorted by checking if any values has a right neighbour which is bigger than himself. 
//This method was from parallelMergesort.
bool QuickSort::proove(std::vector<int>& a) {
	for (uint64_t i = 0; i < a.size() - 1; ++i) {
		if (a[i] > a[i + 1])
			return false;
	}

	return true;
}

//creates an vector<int> with random elements. This method was from parallelMergesort.
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
		
		//looks for all 0 in i and store their index sequentially in the current permutation array
		count = 0;
		bitPointer = 128;
		for (int j = 0; j < 8; j++) {
			if ((i&bitPointer) == 0) {
				permutationTable[i][count++] = j;
			}
			bitPointer = bitPointer >> 1;
		}

		//looks for all 1 in i and store their index sequentially in the current permutation array
		bitPointer = 128;
		for (int j = 0; j < 8; j++) {
			if ((i & bitPointer) == bitPointer) {
				permutationTable[i][count++] = j;
			}
			bitPointer = bitPointer >> 1;
		}
	}
}
