IHS: Improved Distributed Hypercube Sampling

Licensing: This code is distributed under the GNU LGPL license. 
Modified: 15 September 2003
Author: John Burkardt

IHS is a C++ library which carries out the improved distributed hypercube sampling (IHS) algorithm.

N Points in an M dimensional Latin hypercube are to be selected. Each of the M coordinate dimensions is discretized to the values 1 through N. The points are to be chosen in such a way that no two points have any coordinate value in common. This is a standard Latin hypercube requirement, and there are many solutions.

This algorithm differs in that it tries to pick a solution which has the property that the points are "spread out" as evenly as possible. It does this by determining an optimal even spacing, and using the duplication factor D to allow it to choose the best of the various options available to it (the more duplication, the better chance of optimization).

One drawback to this algorithm is that it requires an internal real distance array of dimension D * N * N. For a relatively moderate problem with N = 1000, this can exceed the easily accessible memory. Moreover, the program is inherently quadratic in N in execution time as well as memory; the computation of the I-th point in the set of N requires a consideration of the value of the coordinates used up by the previous points, and the distances from each of those points to the candidates for the next point.
