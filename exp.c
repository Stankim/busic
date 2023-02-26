#include<stdio.h>
int factorial(int number){
	
	if(number <= 1) return 1;
	else
	 return  number * factorial(number-1);
	 /*int fact=1;
	 for(;number>1;number--){
	 	fact *= number;
	 }
	 return fact;*/
}

int main(){

double expo = 0;
double step = 1;
for(int i=0; ;i++){
	
	step = 1.0/(double)factorial(i);
	if(step<0) break;
	expo += step;

}
printf("Eulers constant e = %f\n", expo);

}
