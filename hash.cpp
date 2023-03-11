#include <math.h>
#include <stdlib.h>

static const int hash_range = 1000000;
#define hash_size  100000
static int hash[hash_size] = { 0 };
#define arr_len(arr) (int)(sizeof(arr)/sizeof(*arr))

size_t gen_hash_key(size_t n) 
{
    size_t key = (n + 4096 - (1 << 12) + 9111 - (1 << 10)) % hash_size;
    return key;
}
