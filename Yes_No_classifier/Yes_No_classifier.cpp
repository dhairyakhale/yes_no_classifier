// Yes_No_classifier.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	FILE *fp = fopen("my_no.txt", "r");
	
	if (fp==NULL)
        printf("This file is not available");
    else
    {
	
		char mystring[150];		//string which inputs the line read by fgets()

		int i=0;				//index to be used throughout the program

		double zcr[2400]={0};	//zcr values of each set of 100 samples
		double xi[2]={0};		//xi(i-1) & xi used to calculate zcr
		double max=0;			//used to calculate maximum amplitude
		double dat=0;			//used to read value given in line
	
		int set=0;
		int count=0;			//used to find average dc-shift
		double dc_sum=0;		//used to find average dc-shift

		while(!feof(fp))		//loops until end of file
		{
			for(i=0;i<100;i++)	//for each 100 samples
			{
				fgets(mystring, 150, fp);
				if(feof(fp)) break;			//if file empty, break from while()

				dat=atoi(mystring);			//used to convert string read by fgets() to integer
											//I tried using fscanf() to avoid conversion, but wasn't working

				count++;					//counting number of samples
				dc_sum += dat;				//summing amplitudes for dc-shift

				xi[1]=dat;					//setting current sample as xi
											//at first iteration, x(i-1) aka xi[0] is 0

				if( (xi[0]>=0 && xi[1]<0) || (xi[0]<0 && xi[1]>=0) )	//ZCR calculation
					zcr[set]++;

				xi[0]=xi[1];				//setting current sample as x(i-1) before next iteration

				if(abs(dat)>max)			//calculating max amplitude
					max=abs(dat);
			}
			set++;							//incrementing set-of-100-samples number

			xi[0]=0;						//resetting x(i-1) and xi before going to next set
			xi[1]=0;
		}

		double dc_shift = dc_sum/count;

		rewind(fp);							//resetting file ptr to start

		double energy[2400]={0};			//array of energies of each set-of-100
		double e_sum=0;						//sum of squares of amplitudes, used to calculate energy

		set=0;
		while(!feof(fp))
		{
			for(i=0;i<100;i++)
			{
				fgets(mystring, 150, fp);
				if(feof(fp)) break;

				dat = atoi(mystring);

				dat -= dc_shift;			//subtracting dc-shift from each sample
				dat *= 5000/max;			//normalizing all amplitudes

				e_sum += (dat*dat);
			}

			energy[set] = e_sum/i;			//energy = avg of squares of amplitude
			
			e_sum=0;						//resetting energy sum for next iteration
			set++;
		}
		
		double threshold = 0;	//energy threshold = avg of first 25 sets

		for(i=0;i<25;i++)
			threshold += energy[i];

		threshold /= 25;

		int start=0,end=0;

		i=0;
		while(i<count)
		{
			if(energy[i]>threshold*5)		//word starts at above 500% of threshold (worked best for me)
			{
				start=i;					//mark start point of word
				break;
			}
			i++;
		}
		while(1)
		{
			if(energy[i]<threshold*5)		//word ends when energy is below 500% of threshold
			{
				end = --i;					//mark end of word. pre-decrement is done otherwise it'll mark one index later than desired
				break;
			}
			i++;
		}

		if(start==0 || end==0)				//if both start and end ptrs do not move
		{
			printf("No word found.");
			return 0;
		}

		printf("The word starts at: %d and ends at: %d\n",start,end);

		//now we'll calculate in the identified word

		int zcr_avg=0;						//avg zcr of last 40 sets of the word

		for(i=end;i>(end-40);i--)
			zcr_avg+=zcr[i];

		zcr_avg /= 40;

		if(zcr_avg>35)						//setting it as 35 worked the best for me
			printf("The word is YES.");
		else
			printf("The word is NO.");
	}

	system("PAUSE");						//pausing the console window

	return 0;
}