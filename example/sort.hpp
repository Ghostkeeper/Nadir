/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2020 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef EXAMPLE_SORT
#define EXAMPLE_SORT

#include <vector> //Input and output of the sorting algorithms.

namespace example {

enum SortDirection { Forward, Backward, COUNT };

/*!
 * This is a sorting function that is expected to be more efficient with large
 * data sets, while it may be less efficient with small data sets.
 * \param input A list of numbers that needs to be sorted.
 * \return A sorted list of numbers.
 */
std::vector<int> sort_nlogn(const std::vector<int>& input, const SortDirection direction);

/*!
 * This is a sorting function that is expected to be more efficient with small
 * data sets, while it may not scale as well with large data sets.
 * \param input A list of numbers that needs to be sorted.
 * \return A sorted list of numbers.
 */
std::vector<int> sort_n2(const std::vector<int>& input, const SortDirection direction);

}

#endif