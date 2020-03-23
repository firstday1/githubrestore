#include <stdio.h>
double fact(int k);
int main()
{
	double sum;
	int k;
	while(scanf("%d",&k)==1)
	{
		int i;
		sum=0;
		for(i=1;i<=k;i++)
		{
			sum=sum+1.0/fact(i);
		}
		printf("sum=%.5lf\n",sum);
	}
	return 0;
}
double fact(int k)
{
	double sum=1;
	int i;
	for(i=1;i<=k;i++)
	{
		sum=sum*i;
	}
	return (sum);
}
