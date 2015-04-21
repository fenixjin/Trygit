#include <stdio.h>
#include <math.h>

int main(void)
{
	double r, h, max, min;
	int i = 1;
	while (i)
	{
		printf("r = ");
		scanf("%lf", &r);
		printf("h = ");
		scanf("%lf", &h);
		max = 3.14 * r * r * h;
		min = max/3;
		printf("the volume is between %lf and %lf\n continue?");
		scanf("%d", &i);
	}
	return;
}