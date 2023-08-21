#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vec.h"

void randomCompleteCap(vec_int_t *cap, int dim)
{
    int spaceSize = 1 << dim;
    vec_int_t availablePts;
    vec_init(&availablePts);
    for (int i = 0; i < spaceSize; i++)
        vec_push(&availablePts, i);
    vec_push(cap, 0);

    // Standard basis
    for (int i = 0; i < dim; i++)
    {
        vec_push(cap, (int)1 << i);
        vec_remove(&availablePts, (int)1 << i);
    }

    int length = cap->length;

    for (int i = 0; i < length - 2; i++)
    {
        for (int j = i + 1; j < length - 1; j++)
        {
            for (int k = j + 1; k < length; k++)
            {
                int sum = cap->data[i] ^ cap->data[j] ^ cap->data[k];
                vec_remove(&availablePts, sum);
            }
        }
    }
    // Run until there are no points left. aka: complete the cap
    while (availablePts.length > 0)
    {
        // Random integer between 0 and length - 1
        int r = rand() % availablePts.length;
        // The available point to add to the cap
        int to_add = availablePts.data[r];
        length = cap->length;

        // Run all possible pairwise sums and add to_add to it.
        // This gets the new excludes for when to_add is added to the cap.
        for (int i = 0; i < length - 1; i++)
        {
            for (int j = i + 1; j < length; j++)
            {
                int sum = to_add ^ cap->data[i] ^ cap->data[j];
                vec_remove(&availablePts, sum);
            }
        }
        int idx = 0;
        vec_find(cap, to_add, idx);
        if (idx == -1)
        {
            // to_add is currently not in the cap, so it's not a duplicate
            vec_push(cap, to_add);
        }
        vec_remove(&availablePts, to_add);
    }
    vec_deinit(&availablePts);
}

int getMaxExcludeValue(vec_int_t *cap, int dim)
{
    int spaceSize = 1 << dim;
    vec_int_t excludeCount; // Index is point, value is exclude multiplicity
    vec_init(&excludeCount);
    for (int i = 0; i < spaceSize; i++)
        vec_push(&excludeCount, 0);

    int length = cap->length;
    for (int i = 0; i < length - 2; i++)
    {
        for (int j = i + 1; j < length - 1; j++)
        {
            for (int k = j + 1; k < length; k++)
            {
                // excluded point
                int sum = cap->data[i] ^ cap->data[j] ^ cap->data[k];
                // value of k (the number of triples summing to this point)
                int mult = excludeCount.data[sum];
                // increase the value by pushing, swapping, popping
                vec_push(&excludeCount, mult + 1);
                vec_swap(&excludeCount, sum, excludeCount.length - 1);
                vec_pop(&excludeCount);
            }
        }
    }
    int max = 0;
    for (int i = 0; i < excludeCount.length; i++)
    {
        max = excludeCount.data[i] > max ? excludeCount.data[i] : max;
    }
    vec_deinit(&excludeCount);
    return max;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <dim> <k>\n", argv[0]);
        return 1; // Exit with an error code
    }

    int dim = atoi(argv[1]);
    int k = atoi(argv[2]);

    if (dim <= 0 || k <= 0)
    {
        printf("Error: dim and k must be positive integers.\n");
        return 1;
    }

    // Seed random
    srand(time(NULL));
    // Keep trying until found
    while (1)
    {
        vec_int_t random_cap;
        vec_init(&random_cap);
        randomCompleteCap(&random_cap, dim);
        int max_k = getMaxExcludeValue(&random_cap, dim);
        if (max_k == k)
        {
            // Found the k we were searching for
            int i;
            int val;
            vec_foreach(&random_cap, val, i)
            {
                printf("%d,", val);
            }
            return 0;
        }
        vec_deinit(&random_cap);
    }
    return 0;
}