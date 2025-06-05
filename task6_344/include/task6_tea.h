/* 
PG3401 Exam: Task 6 - Candidate number: 344 
task6_tea.h 
*/
#ifndef __TASK6_TEA_H__
#define __TASK6_TEA_H__

/* Decrypt one 64-bit block (v) into w using 128-bit key k */
void tea_decipher(unsigned int v[2], unsigned int w[2], const unsigned int k[4]);

#endif /* __TASK6_TEA_H__ */
