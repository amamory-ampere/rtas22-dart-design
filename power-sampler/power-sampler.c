/*
original source code from:
https://www.xilinx.com/developer/articles/accurate-design-power-measurement.html
including several modifications, such as:
 - change the default values for sampling frequency and integrations
 - reduce printing and saving to increase the sampling frequency. data is saved into memory
 - power calculation changed to: abs(in1-in2) * curr1
 - output format change to CSV be used w gnuplot

compile w:
$ gcc power-sampler.c -o power-sampler

todo:
improve the sampling frequency by buffering all data into memory and write it into the file just in the end, when CTRL+C is hit

*/
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

//These are specific to ZCU102
#define VCCPSINTFP 0
#define VCCINTLP 1
#define VCCPSAUX 2
#define VCCPSPLL 3
#define MGTRAVCC 4
#define MGTRAVTT 5
#define VCCPSDDR 6
#define VCCOPS 7
#define VCCOPS3 8
#define VCCPSDDRPLL 9
#define VCCINT  10
#define VCCBRAM 11
#define VCCAUX 12
#define VCC1V2 13
#define VCC3V3 14
#define VADJ_FMC 15 // this is not accounted. why ?!
#define MGTAVCC 16
#define MGTAVTT 17

const char railname_arr[50][12] = {
		"VCCPSINTFP",
		"VCCINTLP",
		"VCCPSAUX",
		"VCCPSPLL",
		"MGTRAVCC",
		"MGTRAVTT",
		"VCCPSDDR",
		"VCCOPS",
		"VCCOPS3",
		"VCCPSDDRPLL",
		"VCCINT",
		"VCCBRAM",
		"VCCAUX",
		"VCC1V2",
		"VCC3V3",
		"VADJ_FMC",
		"MGTAVCC",
		"MGTAVTT"
};


typedef struct ina {
	char current_path[50];
	char voltage1_path[50];
    char voltage2_path[50];
	char name[12];
	int current;
	int voltage;
	int last;
} ina;

// global buffer to save all collected data in memory to increase the samplig frequency
int g_n_sensors = 0;
int g_n_samples = 0;
int * g_buffer=NULL;
#define MAX_BUFFER_LINES 10000
char target_file[50];
ina inas[30];

int cmp_ina(const void *a, const void *b) {
	ina *temp1 = (ina*)a;
	ina *temp2 = (ina*)b;
	int len1 = strlen(temp1->current_path);
	int len2 = strlen(temp2->current_path);

	if(len1==len2){
		return strcmp(temp1->current_path, temp2->current_path);
	} else if(len1>len2){
		return 1;
	} else {
		return -1;
	}

}

void populate_ina_array() {
	DIR *d;
	struct dirent *dir;

	char buffer[100];
	char fname_buff[100];

	FILE *fptr;

	d = opendir("/sys/class/hwmon/");
	int counter = 0;

	while ((dir = readdir(d)) != NULL) {
		if (strncmp(".", dir->d_name, 1) == 0) {
			continue;
		}
		//printf("tree: %s\n", dir->d_name);
		strcpy(fname_buff, "/sys/class/hwmon/");
		strcat(fname_buff, dir->d_name);
		strcat(fname_buff, "/name");

		//printf("name: %s\n", fname_buff);

		fptr = fopen(fname_buff, "r");
		fread(&buffer, 10, 1, fptr);
		//printf("device type: %s", buffer);

		if (strncmp(buffer, "ina", 3) == 0) {
			fname_buff[strlen(fname_buff)-5] = 0;

			strcpy(inas[counter].current_path,fname_buff);
			strcat(inas[counter].current_path,"/curr1_input");

			strcpy(inas[counter].voltage1_path,fname_buff);
			strcat(inas[counter].voltage1_path,"/in1_input");

			strcpy(inas[counter].voltage2_path,fname_buff);
			strcat(inas[counter].voltage2_path,"/in2_input");

//			printf("found: %s\n", inas[counter].ina_dir);
			inas[counter].last = 0;
			counter++;
		}

	}
    g_n_sensors = counter;

	qsort(inas, counter, sizeof(ina), cmp_ina);
	if (counter > 0)
		inas[counter-1].last = 1;

	counter = 0;
	while(1) {
		snprintf(inas[counter].name, sizeof(inas[counter].name), "%s",railname_arr[counter]);
		if(inas[counter].last == 1)
			return;

		counter++;
	}
	closedir(d);

}

void list_inas () {
	int counter = 0;
	while(1) {
		printf("Found INA%03d at dir: %s\n", counter, inas[counter].current_path);
		if(inas[counter].last == 1)
			break;

		counter++;
	}
	return;
}

void run_bm (int sleep_per, unsigned iterations, int verbose) {
	FILE *ina_ptr;
	char buffer[20];
    int v1,v2,current;
	int counter;

	for (unsigned j = 0; j < iterations; j++) {
		counter = 0;
		while(1) {
			ina_ptr = fopen(inas[counter].voltage1_path, "r");
			fscanf(ina_ptr,"%[^\n]", buffer);
			v1 = atoi(buffer);
			fclose(ina_ptr);

			ina_ptr = fopen(inas[counter].voltage2_path, "r");
			fscanf(ina_ptr,"%[^\n]", buffer);
			v2 = atoi(buffer);
			fclose(ina_ptr);
            inas[counter].voltage = labs(v1-v2);

			ina_ptr = fopen(inas[counter].current_path, "r");
			fscanf(ina_ptr,"%[^\n]", buffer);
			current = atoi(buffer);
            fclose(ina_ptr);

            assert(iterations<=MAX_BUFFER_LINES);
            g_buffer[iterations*g_n_sensors + counter] = labs(v1-v2) * current;

			if(inas[counter].last) {
				break;
			}
			counter++;
		}
        ++g_n_samples;
    	sleep(sleep_per);
	}
}

void exit_all(int sigid){
    printf("Exting\n");
    unsigned line;
    float pspower, plpower, mgtpower;
	FILE *sav_ptr;
	sav_ptr = fopen(target_file, "w");

    // save the CSV header
    int counter=0;
	while(1) {
    	fprintf(sav_ptr, "\"%s (uW)\",", inas[counter].name);
		if(inas[counter].last == 1)
			break;
		counter++;
	}
    // print the last columns
    fprintf(sav_ptr, "\"PS Power(W)\",\"PL Power(W)\",\"MGT Power(W)\",\"Total Power(W)\"\n");

	for (unsigned j = 0; j < g_n_samples; j++) {
        line = j*g_n_sensors;

        // print the sensor data
        counter=0;
		while(1) {
            fprintf(sav_ptr, "%d,", g_buffer[line + counter]);
			if(inas[counter].last) {
				break;
			}
			counter++;
		}

        // print the totals
        pspower = (float) (
            g_buffer[line + VCCPSINTFP]+
            g_buffer[line + VCCINTLP]+
            g_buffer[line + VCCPSAUX]+
            g_buffer[line + VCCPSPLL]+
            g_buffer[line + VCCPSDDR]+
            //inas[VCCOPS].voltage*inas[VCCOPS].current+
            //inas[VCCOPS3].voltage*inas[VCCOPS3].current+
            g_buffer[line + VCCPSDDRPLL])/1000000.0;

        plpower = (float) (
                g_buffer[line + VCCINT]+
                g_buffer[line + VCCBRAM]+
                g_buffer[line + VCCAUX]+
                g_buffer[line + VCC1V2]+
                g_buffer[line + VCC3V3])/1000000.0;

        mgtpower = (float) (
                g_buffer[line + MGTRAVCC]+
                g_buffer[line + MGTRAVTT]+
                g_buffer[line + MGTAVCC]+
                g_buffer[line + MGTAVTT]+
                g_buffer[line + VCC3V3])/1000000.0;

        fprintf(sav_ptr, "%g,%g,%g,%g\n", pspower, plpower, mgtpower, mgtpower+plpower+pspower);
    }

    fclose(sav_ptr);
    exit(0);
}

int main(int argc, char *argv[]) {

	populate_ina_array(inas);

    // the normal usecase for this app is to be interrupted by CTRL+C or kill, 
    // so this will dump all buffered data into file after the app is actually closed
    signal(SIGKILL,exit_all);
    signal(SIGINT,exit_all);
    signal(SIGTERM,exit_all);

    // the global buffer is
    g_buffer = (int*) malloc(sizeof(int)*g_n_sensors*MAX_BUFFER_LINES);

	int opt;
    // run as fast as possible
	int sleep_per = 0;
    	// run until the end of times ... or a CTRL+C
	unsigned iterations = UINT32_MAX;
	int verbose = 0;
    strcpy(target_file, "./out.txt");

	while ((opt = getopt(argc, argv, "t:o:vn:l")) != -1) {

		switch (opt) {

			case 't':
				printf("Running with sleep @ %d\n", atoi(optarg));
				sleep_per = atoi(optarg);
				break;
			case 'o':
				printf("File output to %s\n", optarg);
				strcpy(target_file, optarg);
				break;
			case 'v':
				printf("Verbose mode on\n");
				verbose = 1;
				break;
			case 'l':
				list_inas(inas);
				break;
			case 'n':
				printf("Testing %d iterations\n", atoi(optarg));
				iterations = atoi(optarg);
				break;
		}
	}
	run_bm(sleep_per, iterations, verbose);

	return 0;
}
