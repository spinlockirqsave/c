#include <stdio.h>
#include <stdlib.h>

int
solution(int A[], int N)
{
	long long int	sum, sum_max, diff;
	int p, q, p_max, q_max;

	if (N < 1 || N > 100000)
		exit(EXIT_FAILURE);
	sum = sum_max = 2 * A[0];
	p = q = p_max = q_max = 0;
	while (q < N)
	{
		sum = A[q] + A[p] + q - p;
		if (sum > sum_max)
		{
			sum_max = sum;
			q_max = q;
		}
		if (q > 0)
		{
			diff = A[q-1] - A[p] - ((q-1) - p);
			if (diff > 0)
			{
				sum_max = sum + diff;
				p = q-1;
				p_max = p;
			}
		}
		sum = 2 * A[q];
		if (sum > sum_max)
		{
			p = q;
			sum_max = sum;
		}	
		++q;
	}
	printf("sum_max [%lld], p_max [%d], q_max [%d]\n",
			sum_max, p_max, q_max);
	return sum_max;
}

int
main()
{
	int res;
	int A[10] = { 20, 20, 23, 0, 0, 0, 1, 16, 0, 15 };
	res = solution(A, 10);
    	printf("res [%d]\n", res);
    	return 0;
}
