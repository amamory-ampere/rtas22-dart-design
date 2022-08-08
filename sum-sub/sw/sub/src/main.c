#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "fred_lib.h"

typedef uint64_t data_t;

// make sure these constants match with the hw design
#define  ARRAY_SIZE (1024)
#define  BLOCK_SIZE_BYTE (ARRAY_SIZE * sizeof(data_t))

#define REPETITIONS 20

data_t *A_in, *B_in, *C_out;

const int hw_id = 101;

void init_vect(data_t * base, data_t value)
{
	for (unsigned int i = 0; i < ARRAY_SIZE; ++i) {
		base[i] = value;
	}
}

void print_vect(data_t *base, unsigned int size)
{
	printf("[ ");
	for (unsigned int i = 0; i < size; ++i) {
			printf("%llu ", base[i]);
	}
	printf("] \n");
}

uint32_t check_output(data_t *base, unsigned int size, data_t expected_value)
{
	for (unsigned int i = 0; i < size; ++i) {
		if (base[i] != expected_value)
			return 0;
	}
	return 1;
}

// using the wall-clock time micros() here is a bad error: a task may be scheduled along with others
// here we need to enforce a given execution time on the CPU, so we're using THREAD_CPUTIME_ID
void Count_Time(unsigned long us){
    struct timespec ts_beg, ts_end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts_beg);
    unsigned long elapsed_us;
    do {
      clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts_end);
      elapsed_us = (ts_end.tv_sec - ts_beg.tv_sec) * 1000000 + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1000;
    } while (elapsed_us < us);
}

int main (int argc, char **argv)
{
	printf(" starting vector sub \n");

	struct fred_data *fred;
	struct fred_hw_task *hw_ip;
	int retval;
	int error_code = 0;
    
	retval = fred_init(&fred);
	if (retval) {
		printf("fred_init failed for hw-task %u\n", hw_id);
		error_code = 1;
	}
	
	// Bind with HW-memcpy having hw-id 100 - sum
	retval = fred_bind(fred, &hw_ip, hw_id);
	if (retval) {
		printf("fred_bind failed for hw-task %u\n", hw_id);
		error_code = 1;
	}

	C_out = fred_map_buff(fred, hw_ip, 0);
	if (!C_out) {
		printf("fred_map_buff failed on buff 0 for C_out\n");
		error_code = 1;
	}
	A_in  = fred_map_buff(fred, hw_ip, 1);
	if (!A_in) {
		printf("fred_map_buff failed on buff 1 for A_in\n");
		error_code = 1;
	}
	B_in  = fred_map_buff(fred, hw_ip, 2);
	if (!B_in) {
		printf("fred_map_buff failed on buff 2 for B_in\n");
		error_code = 1;
	}



	// set the memcpy parameters (dest, source)
	unsigned scalar_in_A=1, scalar_in_B=0;
for (int i=0;i<REPETITIONS;++i){
	init_vect(A_in, scalar_in_A);
	init_vect(B_in, scalar_in_B);

	// Call fred IP
	retval = fred_accel(fred, hw_ip);
	if (retval) {
		printf("fred_accel failed for hw-task %u\n", hw_id);
		error_code = 1;
	}	

	//validate
	if (check_output(C_out, ARRAY_SIZE,scalar_in_A-scalar_in_B) != 1){
		printf("SUB Mismatch!\n");
		error_code = 1;
	}else{
		printf("SUB Match!\n");
	}
/*
	printf("Content of A[0:9]:\n");
	print_vect(A_in, 10);
	printf("Content of B[0:9]:\n");
	print_vect(B_in, 10);
	printf("Content of C[0:9]:\n");
	print_vect(C_out, 10);
*/
	// waste some time doing computation
	Count_Time(1000);



	scalar_in_A+=2;
	scalar_in_B++;


}

	//cleanup and finish
	fred_free(fred);
	printf("Fred finished\n");

	return(error_code);
}
