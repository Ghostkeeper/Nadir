/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2020 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SORT
#define SORT

#include <iostream> //DEBUG!
#include <chrono>
#include <thread>
#include "sort.hpp"

namespace example {

std::vector<int> sort_nlogn(const std::vector<int>& input, const SortDirection direction) {
	//We can't use std::sort since that is just way too well optimised such that even my simple insertion sort won't beat it.
	//So roll our own recursive mergesort here. Quick and dirty. With a lot of data copying, on purpose.
	if(input.size() <= 1) { //Recursion ends.
		return input; //Makes a copy!
	}
	size_t split_index = input.size() / 2;
	std::vector<int> left(input.begin(), input.begin() + split_index);
	std::vector<int> right(input.begin() + split_index, input.end());
	left = sort_nlogn(left, direction);
	right = sort_nlogn(right, direction);
	return sort_merge(left, right, direction);
}

std::vector<int> sort_merge(const std::vector<int>& left, const std::vector<int>& right, const SortDirection direction) {
	std::vector<int> result;
	result.reserve(left.size() + right.size());
	const int transform = (direction == SortDirection::Forward) ? 1 : -1; //Multiply all numbers by this to reverse the comparison.

	size_t left_index = 0;
	size_t right_index = 0;
	while(left_index < left.size() && right_index < right.size()) {
		if(left[left_index] * transform < right[right_index] * transform) {
			result.push_back(left[left_index]);
			++left_index;
		} else {
			result.push_back(right[right_index]);
			++right_index;
		}
	}
	//Now either of the vectors is empty. Append the remaining one completely to the result.
	for(; left_index < left.size(); ++left_index) {
		result.push_back(left[left_index]);
	}
	for(; right_index < right.size(); ++right_index) {
		result.push_back(right[right_index]);
	}
	return result;
}

std::vector<int> sort_n2(const std::vector<int>& input, const SortDirection direction) {
	std::vector<int> result;
	result.reserve(input.size());

	//We'll implement a simple insertion sort here.
	for(const int& number : input) {
		std::vector<int>::iterator insert_place;
		for(insert_place = result.begin(); insert_place != result.end(); ++insert_place) {
			if(direction == SortDirection::Forward && number < *insert_place) {
				break; //Place it here, just before the first number that is greater.
			} else if(direction == SortDirection::Backward && number > *insert_place) {
				break; //When sorting backward, place it just before the first number that is smaller.
			}
		} //If we didn't find any number that was greater, insert_place ends up at result.end() so we'll insert it at the end.
		result.insert(insert_place, number);
	}
	return result;
}

}

#endif