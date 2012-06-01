QuestZero (q0)

All code Copyright 2012 by David Weikersdorfer, Technische Universitaet Muenchen

Author: David Weikersdorfer
Email: david.weikersdorfer@danvil.de

last edited: 06/01/2012


~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~


Q0 is a template black box optimization and tracking toolbox.

This library is under heavy development and contains bugs!

USE AT YOUR ONE RISK!

Black box in this context describes functions where no derivatives
are available. Most of theses algorithms are also meant to search for
global maxima and not only local maxima.

The library provides a general handling of the state space for
algorithms through a small set of functions.

Supportes Spaces:
* Cartesian
* SO(2) (two-dimensional rotation)
* SO(3) (three-dimensional rotation)

There is also limited support for constraints:
* Interval/box constraint on cartesian space
* Interval constraint for SO(2)
* Cone/Twist constraint for SO(3)

There are implementations for several commonly used black box
optimization and tracking algorithms.

Optimization Algorithms:
* Mont Carlo Random Sampling
* Nelder-Mead Simplex Method
* Particle Swarm Optimization (PSO)
* Differential Evolution

Tracking Algorithms:
* Condensation Particle Filter
* Annealed Particle Filter
* Hierarchical Particle Filter


~~~~~ Required dependencies ~~~~~

loki template library
sudo apt-get install libloki-dev

Eigen (version 3) linear algebra library
http://eigen.tuxfamily.org


~~~~~ Supported compilers ~~~~

GCC 4.6.3


~~~~~ Build instructions ~~~~

This builds the examples and tests. The main library is headers only.

mkdir build
cd build
ccmake ..
make
