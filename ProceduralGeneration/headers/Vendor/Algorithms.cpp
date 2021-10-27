#include "Algorithms.h"

#include <array>
#include <iostream>
#include <algorithm>

bool has_a_digit(std::string& s) {
	return std::any_of(s.begin(), s.end(), ::isdigit);
}

int getNumberFromLine(std::string& line) {
	return std::stoi(line);
}

template <typename T>
int partition(T* arr, int start, int end) {
	T pivot = arr[end]; // Pivot is at end(right side of partition)
	int i = start - 1;

	// Swap values until pivot is reached
	for(int j = start; j < end; j++) {
		if(arr[j] < pivot) {
			i++;
			// Swap the values
			T temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
		}
	}
	// Swap the values
	T temp = arr[end];
	arr[end] = arr[i + 1];
	arr[i + 1] = temp;

	return i + 1;
}

void quickSort(int* arr, int start, int end) {

	while(start < end) {
		int pivot = partition(arr, start, end);

		if(pivot - start < end - pivot) {
			quickSort(arr, start, pivot - 1);
			start = pivot + 1;
		}
		else {
			quickSort(arr, pivot + 1, end);
			end = pivot - 1;
		}
	}
}

void quickSort(float* arr, int start, int end) {

	while(start < end) {
		int pivot = partition(arr, start, end);

		if(pivot - start < end - pivot) {
			quickSort(arr, start, pivot - 1);
			start = pivot + 1;
		}
		else {
			quickSort(arr, pivot + 1, end);
			end = pivot - 1;
		}
	}
}