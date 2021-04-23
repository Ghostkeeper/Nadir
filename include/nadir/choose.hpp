/*
 * Strategy pattern optimiser with compile-time linear regression.
 * Copyright (C) 2021 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef NADIR_CHOOSE
#define NADIR_CHOOSE

#include <string_view>
#include <tuple>
#include <type_traits>

namespace nadir {

template<
	std::array Benchmarks,
	typename... Param
>
std::string choose(const Param&... parameters) {
	return choose<Benchmarks.size(), Param..., Benchmarks>(parameters...);
}

template<
	size_t N,
	typename... Param,
	std::array<std::tuple<std::string_view, Param..., double>, N> Benchmarks
>
std::string choose(const Param&... parameters) {
	return "TODO: Implement";
}

}

#endif