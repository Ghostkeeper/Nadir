/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2020 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <random>
#include <nadir/nadir.hpp>
#include "sort.hpp"

int main() {
	//Create benchmarks with 1 parameter: The size of the data set to sort.
	nadir::Benchmarker<size_t> benchmarker;
	benchmarker.add_option("sort_n2", [](size_t problem_size){
		//Create a random problem, but with a fixed seed so every experiment is the same and fair.
		std::mt19937 rng(1337);
		std::vector<int> input;
		input.reserve(problem_size);
		for(size_t i = 0; i < problem_size; ++i) {
			input.push_back(rng());
		}

		//Execute the code under test.
		//TODO: Ideally we only want to time this part.
		example::sort_n2(input);
	});
	benchmarker.add_option("sort_nlogn", [](size_t problem_size){
		std::mt19937 rng(1337);
		std::vector<int> input;
		input.reserve(problem_size);
		for(size_t i = 0; i < problem_size; ++i) {
			input.push_back(rng());
		}

		example::sort_nlogn(input);
	});

	benchmarker.run("sort_benchmarks.hpp");
	return 0;
}