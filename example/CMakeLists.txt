#Strategy pattern optimiser with compile-time linear regression.
#Copyright (C) 2020 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.16) #Oldest version it was tested with.

project(NadirExample DESCRIPTION "Example of how to use Nadir.")

#Find the files of the actual project.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set(project_sources
	sort.cpp
)

#To create benchmarks to optimise for, we build a benchmarker application that will write a file of measurements.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include") #Include Nadir. Normally you'd use CPack or a find module for this.
add_executable(benchmarker benchmarker.cpp ${project_sources})
set_property(TARGET benchmarker PROPERTY CXX_STANDARD 20)
