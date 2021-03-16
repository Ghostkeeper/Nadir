/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2021 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef NADIR_BENCHMARKER
#define NADIR_BENCHMARKER

#include <any> //To allow the prepared test input data to have any data type.
#include <chrono> //To measure execution time.
#include <fstream> //To write the benchmark data to file.
#include <functional> //To register functions to be tested.
#include <stdlib.h> //For exit() in case an error occurs.
#include <string> //To identify options.
#include <tuple> //To work with parameters of mixed types.
#include <type_traits> //To specialise behaviour for some parameter types.
#include <vector> //To store the options and benchmark data.

namespace nadir {

/*!
 * This class allows the user of the Nadir library to create a benchmarking
 * application with relative ease.
 * \tparam Param A list of parameters. Any number of parameters is allowed. All
 * parameters must be serialisable to a file stream. Enum parameters must have a
 * member called ``COUNT`` and be integral type without specialised values, so
 * that the benchmarker can determine the possible options of the enum type.
 */
template<typename... Param>
class Benchmarker {
public:
	/*!
	 * The name of the problem we are benchmarking options for.
	 *
	 * This will be serialised in the file, so that the application may later
	 * retrieve multiple different benchmarks in one application.
	 */
	std::string problem_name;

	/*!
	 * How often to repeat each experiment.
	 *
	 * More repeats make the measurements more accurate, but also take more time
	 * to execute.
	 *
	 * Aside from these, every experiment will also be ran once extra in order
	 * to warm up the processor and seed the branch prediction. This extra run
	 * is not benchmarked, but required to get reliable test results. If your
	 * experiment has any side effects, you can expect those side effects to
	 * occur once too many.
	 */
	size_t repeats = 10;

	/*!
	 * Construct a new benchmark for a certain problem which might have multiple
	 * algorithms to solve.
	 * \param problem_name A unique identifier for the problem to which there
	 * are multiple solutions to choose from.
	 */
	Benchmarker(const std::string& problem_name) : problem_name(problem_name) {
		fill_default_ranges<0, Param...>();
		output_file = std::ofstream("bench_" + problem_name + ".hpp");
		if(!output_file) {
			exit(1); //Can't open file for writing.
			return;
		}
	}

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
	 * \param setup A function to execute for setting up some data for the
	 * algorithm under scrutiny to work on. For instance you could generate an
	 * input data set of the correct size. This can return any one object. The
	 * returned object is passed as the first argument of the experiment.
	 * \param experiment An experiment to run to benchmark the algorithm. This
	 * can be a lambda function that runs the experiment. The experiment will be
	 * given the output of the setup function as first parameter, and the set of
	 * variation parameters as the rest of the parameters. This function will be
	 * benchmarked, so any unpacking of the parameters should be as lean as
	 * possible.
	 */
	void add_option(const std::string& identifier, std::function<std::any(Param...)> setup, std::function<void(std::any, Param...)> experiment) {
		options.emplace_back(identifier, setup, experiment);
	}

	/*!
	 * Change which parameter values the benchmarker will test with.
	 *
	 * Each parameter gets executed with a predefined default set of parameter
	 * values. However if you want to customise this you can provide a custom
	 * range in the form of a vector of values.
	 * \tparam ParamNr The index of the parameter to change, in the same order
	 * as how the template arguments of the ``Benchmarker`` class are provided.
	 * \tparam ThisParam Automatically filled in parameter type to change the
	 * range for. You shouldn't need to provide this due to template argument
	 * deduction.
	 * \param Range the new range of parameter values to test with.
	 */
	template<size_t ParamNr, typename ThisParam>
	void set_param_range(const std::vector<ThisParam>& range) {
		num_measurements /= std::get<ParamNr>(param_ranges).size();
		std::get<ParamNr>(param_ranges) = range;
		num_measurements *= std::get<ParamNr>(param_ranges).size();
	}

	/*!
	 * Run the benchmarks and save the results in the specified file name.
	 */
	void run() {
		output_file << "//Benchmark file generated by Nadir benchmarker.\n";
		output_file << "namespace nadir {\n\n";

		//Declare the table of measurements of the correct size.
		output_file << "constexpr std::array<std::tuple<std::string, "; //Mapping option identifier to an array of measurements. Each measurement is a tuple of all parameters and a duration.
		std::apply([this](auto&&... ranges) {
			((this->print_parameter_type(ranges)), ...);
		}, param_ranges);
		output_file << "double>, " << num_measurements * options.size() << "> " << problem_name << " = {\n"; //Measurements are in seconds, floating-point.

		for(const std::tuple<std::string, std::function<std::any(Param...)>, std::function<void(std::any, Param...)>>& option : options) {
			const std::string& identifier = std::get<0>(option);
			const std::function<std::any(Param...)> setup = std::get<1>(option);
			const std::function<void(std::any, Param...)> experiment = std::get<2>(option);

			std::tuple<Param...> values;
			experiment_combinations<0, Param...>(identifier, setup, experiment, values, std::index_sequence_for<Param...>{});
		}

		output_file << "};\n"; //measurements end.
		output_file << "\n}"; //Namespace end.
		output_file.close();
	}

protected:
	/*!
	 * File stream to write the output measurement data into.
	 */
	std::ofstream output_file;

	/*!
	 * The list of options for the strategy pattern to choose from.
	 *
	 * The benchmark will be ran for each of these options in order to get
	 * comparative results.
	 * Each option consists of an identifier (string) and an experiment to run.
	 */
	std::vector<std::tuple<std::string, std::function<std::any(Param...)>, std::function<void(std::any, Param...)>>> options;

	/*!
	 * For each parameter, a list of values to test.
	 *
	 * A test will be ran for each combination of parameters. This is
	 * exponential in the number of parameters, so be conservative in the amount
	 * of parameters but also in the amount of values in each range!
	 */
	std::tuple<std::vector<Param>...> param_ranges;

	/*!
	 * The total number of measurements to take for each option.
	 */
	size_t num_measurements = 1;

	/*!
	 * Fill the parameter ranges with defaults for each type of parameter.
	 *
	 * This is a template-recursive function. Although it calls itself
	 * recursively, a compiler should be able to unroll the recursion. Each
	 * recursive call picks off one parameter ``ThisParam``. This then calls a
	 * template specialisation that fills a default range for the type of the
	 * parameter. The remaining default ranges are then filled with a recursive
	 * call with the remaining parameter types.
	 * \tparam I A counter that tracks which element of the param_ranges tuple
	 * we need to fill.
	 * \tparam ThisParam The parameter type currently being filled.
	 * \tparam RemainingParams The remaining set of parameters to fill in
	 * recursive calls.
	 */
	template<size_t I = 0, typename ThisParam, typename... RemainingParams>
	typename std::enable_if<I < sizeof...(Param), void>::type fill_default_ranges() {
		std::vector<ThisParam>& range = std::get<I>(param_ranges);
		fill_default_range(range);
		fill_default_ranges<I + 1, RemainingParams...>();
	}

	/*!
	 * The base case of the recursion of fill_default_ranges. This version is
	 * called if there are no more parameter ranges to fill.
	 * \tparam I A counter that tracks which element of the param_ranges tuple
	 * we need to fill. At this point it will be equal to the size of the tuple.
	 * \tparam RemainingParams The remaining parameter types we still need to
	 * fill. At this point, this should be empty.
	 */
	template<size_t I = 0, typename... RemainingParams>
	typename std::enable_if<I == sizeof...(Param), void>::type fill_default_ranges() {
		//End of iteration over tuple. Base case of the recursion.
	}

	/*!
	 * Default range of parameters to test for ``size_t`` parameters.
	 * \param range A range of size_t parameters to experiment. This vector will
	 * be modified in-place.
	 */
	void fill_default_range(std::vector<size_t>& range) {
		range.clear();
		range.insert(range.end(), {0, 1, 5, 10, 25, 50, 100, 500, 1000, 5000, 10000, 50000, 100000});
		num_measurements *= range.size();
	}

	/*!
	 * Default range of parameters to test for enumerable parameters.
	 *
	 * To be able to determine all of the enumerable parameters, the enum MUST
	 * be integral and MUST have a field called ``COUNT`` at the end.
	 *
	 * This is an example of a correctly defined enum:
	 *
	 * ``enum Direction { East, North, West, South, COUNT };``
	 *
	 * This is an example of an incorrectly defined enum, since it's missing the
	 * ``COUNT`` member. It will give a compilation error:
	 *
	 * ``enum Direction { East, North, West, South };``
	 *
	 * This is an example of an incorrectly defined enum, since one of the
	 * members has a value. It will not give a compilation error, but the member
	 * with a value will be skipped:
	 *
	 * ``enum Direction { East, North = 125, West, South, COUNT };``
	 * \tparam EnumType This function can generate a range for any enum.
	 * \param range A range of enum parameters to experiment. This fector will
	 * be modified in-place.
	 */
	template<typename EnumType>
	typename std::enable_if<std::is_enum<EnumType>::value, void>::type fill_default_range(std::vector<EnumType>& range) {
		range.clear();
		range.reserve(EnumType::COUNT); //Enums MUST have a field called COUNT at the end, so that we can determine how many options the enum provides.
		for(unsigned int i = 0; i < EnumType::COUNT; ++i) {
			range.push_back(EnumType(i));
		}
		num_measurements *= range.size();
	}

	/*!
	 * Print the type of parameter used in this parameter range to the output
	 * file.
	 * \param range A range of parameter values to test with, of a certain type.
	 */
	template<typename ParamType>
	void print_parameter_type(std::vector<ParamType>& range) {
		if constexpr(std::is_same_v<ParamType, size_t>) {
			output_file << "size_t, ";
		} else if constexpr(std::is_enum<ParamType>::value) {
			output_file << "unsigned short, ";
		} else {
			exit(2); //Parameter type is not supported. Should never happen because this gives a compilation warning due to not having a default range. Is this function updated too?
		}
	}

	template<size_t I = 0>
	typename std::enable_if<I < sizeof...(Param), void>::type print_parameters(const std::tuple<Param...>& parameters) {
		output_file << std::get<I>(parameters) << ", ";
		print_parameters<I + 1>(parameters);
	}

	template<size_t I = 0>
	typename std::enable_if<I == sizeof...(Param), void>::type print_parameters(const std::tuple<Param...>& parameters) {
		//End of iteration over tuple. Base case of the recursion.
	}

	/*!
	 * Run an experiment with all combinations of parameter values.
	 *
	 * This will cause a certain experiment to run once for each possible
	 * permultation of all parameter values, and benchmark the execution time.
	 * This is a template-recursive function that will take one range of
	 * parameters at a time from the pre-filled tuple ``param_ranges``. It loops
	 * over each range of parameters, going into recursion on the remaining
	 * ranges of parameters. The current parameter value is filled in in the
	 * ``param_values`` argument. The actual experiment is ran in the base case
	 * of the recursion when an item has been chosen from each of the parameter
	 * ranges.
	 * \tparam I A counter of where in the tuple of parameter ranges we are
	 * currently iterating.
	 * \tparam ThisParam The current parameter type we are selecting a parameter
	 * value for.
	 * \tparam RemainingParams The types of parameters that we haven't yet
	 * selected a parameter value for.
	 * \tparam Is A constexpr list of indices to convert the ``param_values``
	 * tuple to a list of arguments for the experiment.
	 * \param identifier The identifier for the option we're running the
	 * experiment for. This is necessary to write the results to the output
	 * file.
	 * \param setup The setup function for the experiment to pre-generate some
	 * data that is not under test.
	 * \param experiment The function to benchmark for the actual experiment.
	 * \param param_values A tuple to fill with parameter values. When calling
	 * this function, you can leave the values uninitialised.
	 */
	template<size_t I = 0, typename ThisParam, typename... RemainingParams, size_t... Is>
	typename std::enable_if<I < sizeof...(Param), void>::type experiment_combinations(const std::string& identifier, const std::function<std::any(Param...)> setup, const std::function<void(std::any, Param...)> experiment, std::tuple<Param...>& param_values, std::index_sequence<Is...>) {
		const std::vector<ThisParam>& range = std::get<I>(param_ranges);
		for(const ThisParam& parameter : range) { //Test each value from the range of values for this parameter.
			std::get<I>(param_values) = parameter;
			experiment_combinations<I + 1, RemainingParams...>(identifier, setup, experiment, param_values, std::index_sequence_for<Param...>{});
		}
	}

	/*!
	 * The base case for the template recursion that runs experiments on
	 * combinations of parameter values.
	 *
	 * In the base case, all of the parameter values have been filled in for a
	 * single experiment, and so the actual experiment can be ran.
	 * \tparam I A counter of where in the tuple of parameter ranges we are
	 * currently iterating. At this point this should be equal to the number of
	 * parameters.
	 * \tparam RemainingParams The parameter types for which we haven't yet
	 * selected a value. This should be empty at this point.
	 * \tparam Is A constexpr list of indices to convert the ``param_values``
	 * tuple to a list of arguments for the experiment.
	 * \param identifier The identifier for the option we're running the
	 * experiment for. This is necessary to write the results to the output
	 * file.
	 * \param setup The setup function for the experiment to pre-generate some
	 * data that is not under test.
	 * \param experiment The function to benchmark for the actual experiment.
	 * \param param_values The parameter values to run the experiment with.
	 */
	template<size_t I = 0, typename... RemainingParams, size_t... Is>
	typename std::enable_if<I == sizeof...(Param), void>::type experiment_combinations(const std::string& identifier, const std::function<std::any(Param...)> setup, const std::function<void(std::any, Param...)> experiment, std::tuple<Param...>& param_values, std::index_sequence<Is...>) {
		std::any input_data = setup(std::get<Is>(param_values)...);
		experiment(input_data, std::get<Is>(param_values)...); //Warm up the processor, so to say. Helps to pre-allocate memory as well as to seed the weights of branch prediction.
		std::chrono::time_point start = std::chrono::high_resolution_clock::now();
		for(size_t repeat = 0; repeat < repeats; ++repeat) {
			experiment(input_data, std::get<Is>(param_values)...);
		}
		std::chrono::time_point end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> duration = (end - start) / repeats;
		output_file << "\t{";
		output_file << "\"" << identifier << "\", ";
		print_parameters<0>(param_values);
		output_file << duration.count();
		output_file << "},\n";
		//Prints: \t{"identifier", "p0", "p1", ... , "pN", "<duration>"},\n
	}
};

}

#endif