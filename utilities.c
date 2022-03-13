#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void itoa(int i, char *a, int len){

	//check for the case where the number is 0
	if (i==0 && len>=2){
		a[1]=0x00;
		a[0]=0x30;
		return;
	}
	//check if the number is 0 but the buffer is too small
	else if (i==0 && len<2){
		fprintf(stderr, "error in function itoa(): ");
		fprintf(stderr, "target array too small to fit result\n");
		exit(1);
	}
		

	int temp=i;
	int digit;
	int num_digits=log10((double) i)+1;

	//check if overflow would occur. +1 for null character
	if (num_digits+1 > len){
		fprintf(stderr, "error in function itoa(): ");
		fprintf(stderr, "target array too small to fit result\n");
		exit(1);
	}

	a[num_digits]=0x0; //set last index to null character
	int idx=num_digits-1;


	while (idx >= 0){
		digit=temp%10;
		temp/=10;
		a[idx]=digit+0x30; //add 0x30 to convert to ascii
		idx--;
	}
	return;
}

