/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2020 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef NADIR_BENCHMARKER
#define NADIR_BENCHMARKER

#include <functional> //To register functions to be tested.
#include <string> //To identify options.
#include <vector> //To store the options and benchmark data.

namespace nadir {

/*!
 * This class allows the user of the Nadir library to create a benchmarking
 * application with relative ease.
 * \tparam Param A list of parameters. Any number of parameters is allowed. All
 * parameters must be serialisable to a file stream.
 */
template<typename... Param>
class Benchmarker {
public:
	/*!
	 * Add an algorithm for the optimiser to choose from.
	 *
	 * Add an algorithm that could be optimal for some set of parameters or on
	 * some combination of hardware. The algorithm will be one of the possible
	 * outputs of Nadir.
	 * \param identifier A unique string you want to recognise this option by.
	 * The output of Nadir will be one of these identifiers so that your code
	 * can know which algorithm to run. It will also be used to store the
	 * benchmark data in the generated data header.
	 * \param experiment An experiment to run to benchmark the algorithm. This
	 * can be a lambda function. The experiment must take the appropriate set of
	 * parameters.
	 */
	void add_option(const std::string identifier, std::function<void(Param...)> experiment) {
		options.push_back(experiment);
	}

}

#endif