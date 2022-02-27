#include <time.h>			// get_time
#include <sys/sysinfo.h>	// get_mem_info
#include <sys/statfs.h>		// get_fs

#include "getinfo.h"


// get time and display it in a few ways
json_object *get_time() {
	char time_str[200];
	char utc_time_str[100];
	char local_time_str[100];
	struct tm *tmp;
	time_t unixtime = time(NULL);
	
	tmp = localtime(&unixtime);
	if (tmp == NULL)
		return error_text("localtime");
	if (strftime(local_time_str, sizeof(local_time_str), "%Z %Y/%m/%d %l:%M:%S %p", tmp) == 0)
		return error_text("strftime");
	
	tmp = gmtime(&unixtime);
	if (tmp == NULL)
		return error_text("gmtime");
	if (strftime(utc_time_str, sizeof(utc_time_str), "%T UTC", tmp) == 0)
		return error_text("strftime");
	
	sprintf(time_str, " | UNIX %d | %s | %s", unixtime, utc_time_str, local_time_str);
	
	return white_text(time_str);
}

// get used ram and swap, change colors based on ram usage
#ifdef RAMSUMMARY
json_object *get_mem_info() {
#else
void get_mem_info(json_object *output[3]) {
#endif
	struct sysinfo info;
	long long int usedram; // the calculation for this is working with ram in bytes, and the unsigned int limit is about 4gib. 8eib should be enough for a while.
	long long int available;
	long long int freeram;
	float memperc;
	char line[200];
	char *color;
#ifdef CHECKSWAP
	long long int usedswap;
	float swapfreeperc;
#endif
	
	if(sysinfo(&info))
		perror("sysinfo");
	
	// Anoyingly, sysinfo doesn't give the whole picture. I'm not convinced that free isn't just making stuff up, but that's beside the point.
	// Getting available ram from /proc/meminfo and subtracting that from the total is good enough for me.
	FILE *meminfo = fopen("/proc/meminfo", "r");
	while (fgets(line, 200, meminfo) != NULL) {
		if (strncmp(line, "MemAvailable:", 13) == 0)
			break;
	}
	sscanf(line, "MemAvailable: %Ld", &available);
	fclose(meminfo);
	
	freeram = (info.freeram * info.mem_unit) / 1024;
	usedram = ((info.totalram * info.mem_unit) / 1024) - available;
	memperc = (float)usedram / (float)((info.totalram * info.mem_unit) / 1024) * 100;
	
	fflush(stdout);
	
#ifdef CHECKSWAP
	swapfreeperc = ((float)info.freeswap / (float)info.totalswap ) * 100;
	
#ifdef RAMSUMMARY
	if (swapfreeperc < 95) {
		usedswap = (info.totalswap - info.freeswap) * info.mem_unit / 1024;
		
		// format_memory returns a static pointer, and it doesn't work correctly if sprintf calls it twice.
		char swap_str[100];
		strcpy(swap_str, format_memory((double)usedswap)); 
		
		sprintf(line, "RAM %.1f%% (%s), Swap: %s", memperc, format_memory((double)usedram), swap_str);
		
		// let swap status override color if swap is used
		if (swapfreeperc < 80)
			color = "#ff0000";
		else
			color = "#ff7f00";
		
		return color_text(line, color);
	}
#endif
#endif
	
	if (memperc < 10)
		color = "#ffffff"; // white if less than 10%
	else if (memperc < 90)
		color = "#00ff00"; // green if less than 90% and greater than 10%
#ifdef CHECKSWAP
	else
		color = "#ffff00"; // orange if greater than 90%
#else
	else if (memperc < 95)
		color = "#ffff00"; // orange if greater than 90% and less than 95
	else
		color = "#ff0000"; // red if greater then 95%
#endif
	
	
#ifdef RAMSUMMARY
	sprintf(line, "RAM %.1f%% (%s)", memperc, format_memory((double)usedram));
	return color_text(line, color);
#else
	sprintf(line, "Used: %s", format_memory((double)usedram));
	output[0] = color_text(line, color);
	sprintf(line, "Free: %s", format_memory((double)freeram));
	output[1] = color_text(line, color);
	sprintf(line, "Avail.: %s", format_memory((double)available));
	output[2] = color_text(line, color);
	return;
#endif
}

json_object *get_fs(char *fs) {
	struct statfs fs_info;
	double total;
	double free;
	char output_str[100];
	char total_str[100];
	
	statfs(fs, &fs_info);
	
	total = (double)fs_info.f_bsize * fs_info.f_blocks / 1024;
	free =  (double)fs_info.f_bsize * fs_info.f_bavail / 1024;
	
	// format_memory can't be called twice by the same function
	strcpy(total_str, format_memory(total));
	sprintf(output_str, "%s %s/%s", fs, format_memory(free), total_str);
	
	return white_text(output_str);
}

#if BATTERIES > 0
json_object *get_battery(char *battery) {
	FILE *file_var;
	char file_path[100];
	
	int charge_full;
	int charge_level;
	char state;
	float percent;
	
	char output_str[100];
	char *color;
	
	
	// get info from sysfs
	sprintf(file_path, "/sys/class/power_supply/%s/charge_full", battery);
	file_var = fopen(file_path, "r");
	if (file_var == NULL)
		return color_text("N", "#7f7f7f");
	fscanf(file_var, "%d", &charge_full);
	fclose(file_var);
	
	sprintf(file_path, "/sys/class/power_supply/%s/charge_now", battery);
	file_var = fopen(file_path, "r");
	if (file_var == NULL)
		return color_text("N", "#7f7f7f");
	fscanf(file_var, "%d", &charge_level);
	fclose(file_var);
	
	sprintf(file_path, "/sys/class/power_supply/%s/status", battery);
	file_var = fopen(file_path, "r");
	if (file_var == NULL)
		return color_text("N", "#7f7f7f");
	state = fgetc(file_var);
	fclose(file_var);
	
	percent = ((float)charge_level / (float)charge_full) * 100;
	
	// makes the "Unknown" status make more sense in certain cases and remove the percentage when full.
	switch (state) {
		case 'U':
			switch (battery[3]) {
				case '0': // main battery
					if (percent > 95)
						return white_text("Full");
					break;
				case '1': // slice battery
					if (percent < 1)
						return color_text("Empty", "#bfbfbf");
					break;
			}
			break;
		case 'F':
			return white_text("Full");
	}
	// woah, while looking at this section of the code I just had Deja Vu, and then Deja Vu of having Deja Vu
	
	if (percent < 20)
		color = "#ff0000"; // red if below 20%
	else if (percent < 40)
		color = "#ffff00"; // orange if below 40 and above 20
	else if (percent < 75)
		color = "#00ff00"; // green if above 40 and below 75
	else
		color = "#ffffff"; // white if above 75
	
	sprintf(output_str, "%c %.1f%%", state, percent);
	
	return color_text(output_str, color);
}
#endif

// experimental thingy showing battery level visually
#ifdef BATTERYBAR
void get_battery_bar(json_object *output[2], char *battery) {
	FILE *file_var;
	char file_path[100];
	
	int charge_full;
	int charge_level;
	float percent;
	
	char output_str1[20];
	char output_str2[20];
	char *color;
	
	
	// get info from sysfs
	sprintf(file_path, "/sys/class/power_supply/%s/charge_full", battery);
	file_var = fopen(file_path, "r");
	if (file_var == NULL) {
		output[0] = error_text("N");
		output[1] = white_text("");
		return;
	}
	fscanf(file_var, "%d", &charge_full);
	fclose(file_var);
	
	sprintf(file_path, "/sys/class/power_supply/%s/charge_now", battery);
	file_var = fopen(file_path, "r");
	if (file_var == NULL) {
		output[0] = error_text("N");
		output[1] = white_text("");
		return;
	}
	fscanf(file_var, "%d", &charge_level);
	fclose(file_var);
	
	percent = ((float)charge_level / (float)charge_full) * 100;
	
	for (int i = 0; i < (int)(percent / 10); i++) {
		strcat(output_str1, " ");
	}
	for (int i = 0; i < 10 - strlen(output_str1); i++) {
		strcat(output_str2, " ");
	}
	
	output[0] = white_text(" ");
	json_object_object_add(output[0], "background", json_object_new_string("#7f7f7f"));
	json_object_object_add(output[0], "border", json_object_new_string("#ffffff"));
	json_object_object_add(output[0], "border_right", json_object_new_int(0));
	
	output[1] = white_text(" ");
	json_object_object_add(output[1], "border", json_object_new_string("#ffffff"));
	json_object_object_add(output[1], "border_left", json_object_new_int(0));
	
	return;
}
#endif
