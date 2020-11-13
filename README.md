Nadir
====
The optimizer for strategy patterns that selects an implementations based on a linear regression using parameters of the user's situation.

What is Nadir?
----
Nadir is a library that uses the strategy design pattern of architecture. The aim is to select one of several algorithms that have the same outcome. These algorithms may have various costs associated with them. Nadir is able to choose the option with the lowest predicted cost for the particular situation.

To make this choice, Nadir allows you to first generate measurements to map out the search space with a separately-built benchmarking tool. This benchmarking tool will measure the (performance) cost of all options with various parameters. These measurements are stored in a C++ file that gets compiled along with your application. The parameters that are benchmarked include the parameters of your algorithm, such as the size of the input or the error margin of the output, as well as the hardware available in the computer such as CPU clock speed or memory latency. The results are stored in a big table of measurements that can be included with an application along with the runtime part of Nadir.

When compiling your application, Nadir will run a linear regression on this data to determine the optimal choice of algorithm at each possible combination of input parameters, taking into account the influence of hardware on each of the available options. This results in a look-up table of border cases that an application can quickly run through to find the most optimal solution for the current computational problem on the current hardware.

Nadir's name derives from the geometric term, meaning a direction pointing straight down. This name is a reference to the linear regression finding the lowest point on a curve as with gradient-descent type optimisations, as well as to the library called Apex for which it was originally developed.

Example use case
----
Imagine a scenario where your application has two implementations of an algorithm that computes a convex hull around a number of points. One implementation is a simple one that scales badly with the number of input points, but can be very effective with small problems. The second implementation is a highly parallel algorithm with great time complexity, but which has a greater overhead to get going because it needs to set up a thread pool and pre-compute a transformation on the input data.

You first create a simple benchmarking tool using a built-in function of Nadir by creating a single `main` function which simply calls Nadir's benchmarking function, providing the set of algorithms to benchmark and the range of parameters to benchmark for. This generates a file of C++ source code with the measurements. In your real application, when it comes to running one of the available algorithms, you call upon Nadir to make the optimal choice. During compile time, Nadir will have pre-computed a linear regression so that the runtime simply needs to look up the most optimal choice in a table. This look-up table will for instance store that inputs smaller than 7000 points need to use the simple algorithm, but any larger data sets need to use the more complex algorithm that scales better with large data sets. The runtime determines the choice that delivers the best performance. As a result, your algorithm will be performant for small data sets while still scaling well to large data sets.

The benchmark can also be ran on different systems. One could run the benchmarks on various types of machines, with different operating systems, different CPUs and RAM chips, different GPUs and so on. The runtime will select an algorithm based on the hardware available to the user, so that his performance is optimal tailored to his own hardware.

Features
----
Nadir strives to achieve the following goals:
* The overhead of making a choice during runtime - for the user - needs to be as low as possible.
* The runtime of Nadir is header-only, so that it can be used in a header-only library.
* Parameters can be either numeric or enumerable.
* The training data is a file that gets compiled along with the application, so that the training data can be embedded in the binary easily by any build system. The training data does not need to get stored in a separate file and parsed by the application during runtime. It is stored statically in the binary.
* The linear regression that optimises the algorithm choice is completely constexpr, so that the linear regression is computed during compilation time of your application.

A disadvantage of this approach is that it incurs a significant cost on the compilation time of your application. It will run the linear regression during compilation so that this doesn't need to run on the user's computer. This can be mitigated by using pre-compiled header files or by putting the code that calls on Nadir to choose an algorithm into a separate compilation unit.

Another disadvantage is that benchmarks get outdated if your algorithms change. It is advisable to re-run your benchmarks before every release to optimise performance of your application. In the future, the age of a measurement may be taken into account as a factor of how reliable the measurement is perceived to be and how much it affects the linear regression. This is currently not in scope.

Usage
----
Once the implementation has progressed, this paragraph will explain how to make a benchmark application and how to use the benchmarks to make the optimal choice of algorithms in your actual application.
