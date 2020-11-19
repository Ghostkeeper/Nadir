/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2020 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */
#include <iostream>

#include <random>
#include <nadir/nadir.hpp>
#include "sort.hpp"

/*!
 * Helper function to create input test data for the sort functions.
 * \param problem_size How many items to put in the input problem.
 */
std::any test_data(const size_t problem_size, const example::SortDirection direction) {
	//Create a random problem, but with a fixed seed so every experiment is the same and fair.
	std::mt19937 rng(1337);
	std::vector<int> test;
	test.reserve(problem_size);
	for(size_t i = 0; i < problem_size; ++i) {
		test.push_back(rng());
	}
	return test;
}

int main() {
	//Create benchmarks with 1 parameter: The size of the data set to sort.
	nadir::Benchmarker<size_t, example::SortDirection> benchmarker;
	benchmarker.add_option("sort_n2", test_data, [](std::any input, const size_t problem_size, const example::SortDirection direction){
		std::cout << "sort_n2(" << problem_size << ", " << direction << ")" << std::endl;
		example::sort_n2(std::any_cast<std::vector<int>>(input), direction);
	});
	benchmarker.add_option("sort_nlogn", test_data, [](std::any input, const size_t problem_size, const example::SortDirection direction){
		std::cout << "sort_nlogn(" << problem_size << ", " << direction << ")" << std::endl;
		example::sort_nlogn(std::any_cast<std::vector<int>>(input), direction);
	});

	benchmarker.repeats = 5;
	benchmarker.run();
	return 0;
}