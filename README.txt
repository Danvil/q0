QuestZero (q0)

All code Copyright 2012 by David Weikersdorfer, Technische Universitaet Muenchen

Author: David Weikersdorfer
Email: weikersd@in.tum.de

last edited: 06/03/2012


~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~

Q0 is a template global optimization and tracking toolbox.

Q0 is a fast and customizable C++ template library to find the global optimium of a static or time-dependent function defined over a general class of state spaces. The library can handle arbitrary combinations of cartesian vectors and spatial rotations.

This library is not a local optimization library! Some special local optimization algorithms (i.e. Nelder-Mead) are included. Q0 works with any function defined over the state space. Functions do not need to have derivatives or be smooth.

Q0 has mainly been used throughout my PhD thesis for human motion tracking with a high-dimensional skeleton model. However it is currently in a state of re-structuring so not all elements might work as expected.

USE AT YOUR OWN RISK!

Available Space Components:
* Cartesian (fixed or dynamic dimension)
* SO(2) (two-dimensional rotation)
* SO(3) (three-dimensional rotation)

Available constraints on spaces (experimental):
* Interval/box constraint on cartesian space
* Interval constraint for SO(2)
* Cone/Twist constraint for SO(3)

Optimization Algorithms:
* Mont Carlo Random Sampling
* Nelder-Mead Simplex Method (local optimizer)
* Particle Swarm Optimization (PSO)
* Differential Evolution (under construction)

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

This builds examples and tests. The main library is headers only.

mkdir build
cd build
ccmake ..
make

~~~~~ Usage ~~~~~

Have a look at the examples. For example optimization/Polynom_PSO.cpp is easy to understand.
