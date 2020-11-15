#Strategy pattern optimiser with compile-time linear regression.
#Copyright (C) 2020 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.16) #Oldest version it was tested with.

#Project metadata.
set(NADIR_VERSION_MAJOR 0 CACHE STRING "Major release version. This must be incremented if there are changes that are not backwards compatible.")
set(NADIR_VERSION_MINOR 0 CACHE STRING "Minor release version. This must be incremented if there are changes that are not forwards compatible.")
set(NADIR_VERSION_PATCH 1 CACHE STRING "Patch release version. This must be incremented for every release where there are no changes to the API.")
project(Nadir VERSION ${NADIR_VERSION_MAJOR}.${NADIR_VERSION_MINOR}.${NADIR_VERSION_PATCH} DESCRIPTION "Strategy pattern optimiser with compile-time linear regression.")

#The main target.
add_library(nadir INTERFACE)
target_include_directories(nadir INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET nadir PROPERTY CXX_STANDARD 17)
