We have crack 250 public keys on NIST-P256 curve in 100 seconds.
The private keys are of the form `A*P(2**32)` where `A` is a 32 bit number
and `P(x)` is 7th-degree polynomial with all coefficients either 0 or 1.

The naive approach is to go through all 256 possible `P(x)` and try to solve
the resulting 32 bit discrete logarithm using the well known giant-step/baby-step
algorithm in `sqrt(2**32) = 2**16` steps. However this is suboptimal:
you will make `256 * 2**16 = 2**24` giant steps in total and only `2**16` baby steps.
As baby steps can be done before connecting to the challenge server they are almost
free, the only cost is memory and slower hash table lookups.

My solution precomputes `2**25` baby steps and stores each as a 16-byte entry in
a linearly probed hash table. Bits of EC coordinates are random enough on their own,
so they are used as hash table indices directly, without a hash. After the table is
precomputed it forks into multiple processes to take multiple logarithms in parallel.
With these optimizations it takes only 40 seconds to crack all 250 keys.

Also I use `Apache Milagro Crypto Library <https://milagro.incubator.apache.org/docs/amcl-overview.html>`_
which is roughly an order of magnitude faster than the `fastecdhe` python module used in
the challenge script.
