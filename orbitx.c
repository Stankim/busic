#include<stdio.h>
#include<math.h>
#include<stdlib.h>

int main(int args, char **argsarr){
	if(args<1)
	exit(0);
	for(int i=1;i<args;i++){
		int number= atoi(argsarr[i]);
		printf("#%d\n",number);
		int count = 0;
		for(int j=2;j<pow(2,number)/2;j++){
			if(j%2 == 0) continue;

			int divident = j;
			int ispowerof2 = 1;
			do{
			if(divident%2 ==1)
			ispowerof2 = 0;
			divident = divident/2;
			}while(divident !=1);
			if(ispowerof2) continue;

			if(number%2 == 0){
				int ishalfcycled = 1;
					divident = j;
					int lower_divident = j;
					for(int l = 0;l<number/2;l++)
					{
						lower_divident/=2;
					}
					for(int l = 0;l<number/2;l++){
						if(divident % 2 != lower_divident % 2)
						{
							ishalfcycled = 0;
							break;
						}
					divident/=2;
					lower_divident/=2;

				}

				if(ishalfcycled) continue;
			}
			
			int adjacent_positives = 0;
			int adjacent_negatives = 0;
			int previous_adjacent_positives =0;
			int previous_adjacent_negatives = 0;
			int has_ascending_positivity = 0;
			int has_positive_tie=0;
			int prev_binary=0;
			divident = j;
			for(int p=0;p<number;p++){
			int current_binary = divident%2;
			
			if(current_binary  && has_positive_tie){
				//printf("\nat p = %d tied now with %d %d\n",p,
				 //adjacent_negatives,previous_adjacent_negatives);
				
				if(adjacent_negatives && previous_adjacent_negatives)
				{
					if(adjacent_negatives < previous_adjacent_negatives){
						has_ascending_positivity=1;
						 break;
					}
				}
			}
				
			if(current_binary){
				adjacent_positives++;
				if(adjacent_negatives){
					previous_adjacent_negatives = adjacent_negatives;
					adjacent_negatives = 0;
				}
			}
			else{
				adjacent_negatives++;
				if(adjacent_positives){								
					if(previous_adjacent_positives){
					
					if(adjacent_positives > previous_adjacent_positives){
						has_ascending_positivity = 1;
						break;
						}

					else if(adjacent_positives == 	 
					 	previous_adjacent_positives){
						has_positive_tie = 1;
						//printf("\n positive tie at %d with %d and %d\n", p, adjacent_positives, previous_adjacent_positives);
						
					}		
				}		
					previous_adjacent_positives = adjacent_positives;
					adjacent_positives = 0;
					
				}
				
			}
			
			if(p+1 == number && has_positive_tie){
				//printf("\nat p = %d tied now with %d %d\n",p,
				 //adjacent_negatives,previous_adjacent_negatives);
				
				if(adjacent_negatives && previous_adjacent_negatives)
				{
					if(adjacent_negatives < previous_adjacent_negatives){
						has_ascending_positivity=1;
						 break;
					}
				}
			}
			
			divident = divident/2;  
			}
			if(has_ascending_positivity) continue;
			
			
			printf("%d\t%d\t ",count++, j);
			divident = j;
			for(int p=0;p<number;p++){
			printf("%d", divident%2);
			divident = divident/2;  
			}
			printf("\n");			
		}
		
	}
}
