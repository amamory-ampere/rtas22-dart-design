/*
source code from:
https://www.xilinx.com/developer/articles/accurate-design-power-measurement.html

compile w:
$ gcc power-sampler.c -o power-sampler
*/
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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

void populate_ina_array(ina *inas) {
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

void list_inas (ina *inas) {
	int counter = 0;
	while(1) {
		printf("Found INA%03d at dir: %s\n", counter, inas[counter].current_path);
		if(inas[counter].last == 1)
			break;

		counter++;
	}
	return;
}

void run_bm (char target_file[50], int sleep_per, unsigned iterations, int verbose, ina *inas) {
	FILE *sav_ptr;
	FILE *ina_ptr;

	sav_ptr = fopen(target_file, "w");

	char buffer[20];
    unsigned v1,v2;
	float plpower = 0;
	float pspower = 0;
	float mgtpower = 0;

	int counter = 0;
	while(1) {
		if (verbose == 1) {
			fprintf(sav_ptr, "%s (mV),%s (mA),", inas[counter].name, inas[counter].name);
		}
		if(inas[counter].last == 1)
			break;

		counter++;
	}
    // print the last columns
    fprintf(sav_ptr, "PS Power(W), PL Power(W), MGT Power(W), Total Power(W)\n");

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

			if(verbose==1) {
				//printf("Voltage # %d = %d \n", counter, atoi(buffer));
				fprintf(sav_ptr, "%d,", inas[counter].voltage);
			}

			ina_ptr = fopen(inas[counter].current_path, "r");

			fscanf(ina_ptr,"%[^\n]", buffer);

			inas[counter].current = atoi(buffer);
			if(verbose==1) {
				//printf("Current # %d = %d \n", counter, atoi(buffer));
				fprintf(sav_ptr, "%s,", buffer);
			}

			if(inas[counter].last) {

				pspower = (float) (inas[VCCPSINTFP].voltage*inas[VCCPSINTFP].current+
						inas[VCCINTLP].voltage*inas[VCCINTLP].current+
						inas[VCCPSAUX].voltage*inas[VCCPSAUX].current+
						inas[VCCPSPLL].voltage*inas[VCCPSPLL].current+
						inas[VCCPSDDR].voltage*inas[VCCPSDDR].current+
						//inas[VCCOPS].voltage*inas[VCCOPS].current+
						//inas[VCCOPS3].voltage*inas[VCCOPS3].current+
						inas[VCCPSDDRPLL].voltage*inas[VCCPSDDRPLL].current)/1000.0;

				plpower = (float) (inas[VCCINT].voltage*inas[VCCINT].current+
						inas[VCCBRAM].voltage*inas[VCCBRAM].current+
						inas[VCCAUX].voltage*inas[VCCAUX].current+
						inas[VCC1V2].voltage*inas[VCC1V2].current+
						inas[VCC3V3].voltage*inas[VCC3V3].current)/1000.0;

				mgtpower = (float) (inas[MGTRAVCC].voltage*inas[MGTRAVCC].current+
						inas[MGTRAVTT].voltage*inas[MGTRAVTT].current+
						inas[MGTAVCC].voltage*inas[MGTAVCC].current+
						inas[MGTAVTT].voltage*inas[MGTAVTT].current+
						inas[VCC3V3].voltage*inas[VCC3V3].current)/1000.0;

				fprintf(sav_ptr, "%g,%g,%g,%g\n", pspower, plpower, mgtpower, mgtpower+plpower+pspower);
				fclose(ina_ptr);
				break;
			}
			fclose(ina_ptr);
			counter++;
		}

	// running as fast as possible ?!?!?
	//sleep(sleep_per);
	}
	fclose(sav_ptr);
}

int main(int argc, char *argv[]) {

	ina inas[30];
	populate_ina_array(inas);

	int opt;
	int sleep_per = 1;
    	// run until the end of times ... or a CTRL+C
	unsigned iterations = UINT32_MAX;
	int verbose = 0;
	char target_file[50] = "./out.txt";

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
	run_bm(target_file, sleep_per, iterations, verbose, inas);

	return 0;
}
