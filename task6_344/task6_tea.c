/* 
PG3401 Exam: Task 6 - Candidate number: 344
task6_tea.c

The Tiny Encryption Algorithm (TEA) by David Wheeler and Roger Needham of the Cambridge Computer Labratory
Original code credited to David Wheeler and Roger Needham. 
*/

#include "task6_tea.h"

/* ANSI-C TEA decipher */
void tea_decipher(unsigned int *const v,unsigned int *const w,
const unsigned int *const k)
{
   register unsigned int y=v[0], z=v[1], sum=0xC6EF3720,
				delta=0x9E3779B9, a=k[0], b=k[1], c=k[2],
				d=k[3], n=32;

   /* sum = delta<<5, in general sum = delta * n */

	/* Included paranteses for clarity. Precence is (a ^ b ^ c) */
	while(n-->0) {
		z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
		sum -= delta;
	}
	   
	   w[0]=y; w[1]=z;
}
