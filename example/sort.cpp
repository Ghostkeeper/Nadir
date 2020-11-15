/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2020 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SORT
#define SORT

#include <algorithm> //For std::sort.
#include "sort.hpp"

namespace example {

std::vector<int> sort_nlogn(const std::vector<int>& input, const SortDirection direction) {
	std::vector<int> result = input; //Make a copy as to not modify the input.
	std::sort(result.begin(), result.end());
	if(direction == SortDirection::Backward) {
		std::reverse(result.begin(), result.end());
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