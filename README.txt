QuestZero (q0) - global optimization on manifolds

by David Weikersdorfer (weikersd@in.tum.de)


~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~

Q0 is a C++ template library for global optimization on non-Euclidean manifolds. An important examples is SO(3), the space of rotations in 3D space.

Q0 is a fast, customizable and extendable C++ template library. Currently some of the most important optimization algorithms like particle swarm optimization, annealing and differential evolution are implemented. Available state spaces are currently the Cartesian space, SO(2) and SO(3). Custom state spaces can be easily added by implementing a small set of operations.

This library is not a local optimization library! However, some special local optimization algorithms (i.e. Nelder-Mead) are included. Q0 theoretically works with an arbitrary objective function. Functions do not need to have derivatives or be smooth. However for ill-behaving functions some algorithms may have difficulties finding the global optimum.

Q0 has mainly been used throughout my PhD thesis for human motion tracking using a high-dimensional skeleton model.

Available Space Components:
* Cartesian (fixed or dynamic dimension, i.e. position of a rigid object in 3D)
* SO(2) (rotation in 2D space, i.e. a hinge joint)
* SO(3) (rotation in 3D space, i.e. the orientation of a rigid object)

Available constraints on spaces (experimental):
* Interval/box constraint for Cartesian space
* Interval constraint for SO(2)

Optimization Algorithms:
* Mont Carlo Random Sampling
* Particle Swarm Optimization (PSO)
* Differential Evolution (under construction)
* Nelder-Mead Simplex Method (local optimizer)

Tracking Algorithms (under construction):
* Condensation Particle Filter
* Annealed Particle Filter
* Hierarchical Particle Filter


~~~~~ Required dependencies ~~~~~

loki template library
Required version: 0.1.7 or newer
http://loki-lib.sourceforge.net/

Boost C++ Libraries
Required version: 1.46.1 or newer
http://www.boost.org/

Eigen C++ template library for linear algebra
Required version: 3.0 or newer
http://eigen.tuxfamily.org


~~~~~ Supported compilers ~~~~

GCC 4.6.3


~~~~~ Build instructions ~~~~

This builds examples and tests. The main library uses only headers.

mkdir build
cd build
ccmake ..
make

~~~~~ Usage ~~~~~

Have a look at the examples.
