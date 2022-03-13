#include<stdio.h>
#include<string.h>
int main()
{
	int n;
	scanf("%d",&n);

	int p = n;
	int k = 0;
	while (p > 0) {
		k *= 10;
		k += p % 10;
		p /= 10;
	}

	if(n == k){
		printf("Y");
	}else{
		printf("N");
	}
	return 0;
}
