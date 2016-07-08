#include <math.h>
#include "is_prime.h"

static const int seed_primes[] = {
    2,   3,  5,    7,  11,  13,  17,  19,  23,  29, 
   31,  37,  41,  43,  47,  53,  59,  61,  67,  71, 
   73,  79,  83,  89,  97, 101, 103, 107, 109, 113, 
  127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
  179, 181, 191, 193, 197, 199
};
static const int NUM_SEED_PRIMES = sizeof(seed_primes) / sizeof(int);

int is_prime(int n)
{
        for (int i = 0; i < NUM_SEED_PRIMES; i++) {
                int seed_prime = seed_primes[i];
                if (seed_prime == n) {
                        return 1;
                }
                if (n % seed_prime == 0) {
                        return 0;
                }
        }

        int n_sqrt = (int) sqrt(n);

        for (int f = seed_primes[NUM_SEED_PRIMES - 1] + 2; f <= n_sqrt; f += 2) {
                if (n % f == 0) {
                        return 0;
                }
        }

        return 1;
}

