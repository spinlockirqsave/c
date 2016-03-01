/*
 * @file    main.c
 * @author  Piotr Gregor <piotrek.gregor gmail com>
 * @brief   Finds the longest segment of 0s or 1s
 *          given that max 1 element may be reversed,
 *          e.g.:
 *          { 0, 1, 1 }             return 3
 *          { 0, 1, 0 }             return 3
 *          { 1, 0, 1, 1 }          return 4
 *          { 1, 0, 0, 1, 1, 1, 1 } return 5
*/


#include <stdio.h>


enum state {
    EQUAL = 0,
    DIFF_SEEN_CONTINUE = 1
};

/*
 * X   X   Y  Z
 *         i
 */

int solution(int *A, int A_length) {
    int i, ref, zero_diff, first_diff, res_tmp;

    if (A_length < 0) return -1;

    int result = 1;

    int count = 1;
    zero_diff = first_diff = -1;
    ref = A[0];
    for (i = 1; i < A_length; i++)
    {
        if (ref == A[i])
        {
            count++;
            if (i == (A_length - 1) && zero_diff != -1 && first_diff == -1)
                count++;
        } else {
            if (i < A_length - 1)
            {
                /* A[i] is different element */
                if (first_diff == -1)
                {
                    /* continue this segment ? */
                    if (A[i + 1] == A[i - 1])
                    {
                        /* continue */
                        first_diff = i;
                        ref = A[i - 1];
                        count++;
                    } else {
                        /* commit */
                        if (count + 1 > result)
                            result = count + 1;
                        /* RESET, change segment's reference */
                        count = 1;
                        zero_diff = i;
                        ref = A[i];
                    }
                } else {
                    /* inside continued already segment */
                    /* commit */
                    if (count > result)
                        result = count;
                    /* continue this segment ? */
                    if (A[i + 1] == A[i - 1])
                    {
                        /* reset-move segment */
                        ref = A[i - 1];
                        count = count - first_diff;
                        first_diff = i;
                    } else {
                        /* commit */
                        if (count + 1 > result)
                            result = count + 1;
                        /* RESET, change segment's reference */
                        ref = A[i - 1];
                        count = 1;
                        first_diff = -1;
                        zero_diff = i;
                    }
                }
            } else {
                /* last element */
                if (first_diff == -1)
                    count++;
            }
        }
    }

    if (count > result)
        result = count;

    return result;
}

int
main(void)
{
    int res;
    int A[] = { 1, 1, 1, 1, 1, 1 };
    int B[] = { 0, 1, 1, 1, 1, 1 };
    int C[] = { 1, 1, 1, 1, 1, 0 };
    int D[] = { 0, 0, 1, 1, 1, 1 };
    int E[] = { 0, 1, 1, 1, 1, 0 };
    int F[] = { 0, 0, 1, 1, 0, 1 };
    int G[] = { 1, 1, 0, 1, 1, 0, 1, 1 };
    int H[] = { 1, 1, 0, 1, 1, 0, 1, 1, 1 };
    int I[] = { 1, 1, 0, 1, 1, 0, 1, 1, 1, 0 };
    int J[] = { 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 };
    int K[] = { 1, 1, 1, 0, 0, 0 };
    int L[] = { 1, 1, 1, 0, 0, 0, 0 };
    int M[] = { 1, 1, 0, 1, 1, 1, 1 };
    res = solution(A, 6);
    printf("A res [%d]    expectation [%d]\n", res, 6);
    res = solution(B, 6);
    printf("B res [%d]    expectation [%d]\n", res, 6);
    res = solution(C, 6);
    printf("C res [%d]    expectation [%d]\n", res, 6);
    res = solution(D, 6);
    printf("D res [%d]    expectation [%d]\n", res, 5);
    res = solution(E, 6);
    printf("E res [%d]    expectation [%d]\n", res, 5);
    res = solution(F, 6);
    printf("F res [%d]    expectation [%d]\n", res, 4);
    res = solution(G, 8);
    printf("G res [%d]    expectation [%d]\n", res, 5);
    res = solution(H, 9);
    printf("H res [%d]    expectation [%d]\n", res, 6);
    res = solution(I, 10);
    printf("I res [%d]    expectation [%d]\n", res, 6);
    res = solution(J, 10);
    printf("J res [%d]    expectation [%d]\n", res, 7);
    res = solution(K, 6);
    printf("K res [%d]    expectation [%d]\n", res, 4);
    res = solution(L, 7);
    printf("L res [%d]    expectation [%d]\n", res, 5);
    res = solution(M, 7);
    printf("M res [%d]    expectation [%d]\n", res, 7);
    return 0;
}
