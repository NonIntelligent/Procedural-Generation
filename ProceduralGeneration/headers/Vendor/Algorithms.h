#pragma once
#include <string>
#include <vector>
#include <fstream>


#define vectorString std::vector<std::string>

// https://stackoverflow.com/questions/9642292/function-to-check-if-string-contains-a-number
// Does the string contain a digit?
bool has_a_digit(std::string &s);

// Quicksort partition algorithm (works for string or numbers)
template <typename T>
int partition(T* arr, int start, int end);

// https://www.geeksforgeeks.org/quick-sort/
// https://www.geeksforgeeks.org/quicksort-tail-call-optimization-reducing-worst-case-space-log-n/
// Quicksort algorithm to sort a list of strings
// Tail-cull optimisation was used to reduce space complexity
// Worst case space is now O(Log n) as opposed to O(n)
void quickSort(int* arr, int start, int end);
void quickSort(float* arr, int start, int end);

