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
json_object *get_mem_info() {
	struct sysinfo info;
	long long int usedram; // the calculation for this is working with ram in bytes, and the unsigned int limit is about 4gib. 8eib should be enough for a while.
	long long int available;
	long long int freeram;
	float memperc;
	char line[250];
	char *color;
#ifndef MEMSUMMARY
	char used_str[50];
	char free_str[50];
	char avail_str[50];
#endif
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
	strcpy(used_str, format_memory((double)usedram));
	strcpy(free_str, format_memory((double)freeram));
	strcpy(avail_str, format_memory((double)available));
	sprintf(line, "<span color=\"%1$s\">Used: %2$s</span> | <span color=\"%1$s\">Free: %3$s</span> | <span color=\"%1$s\">Avail.: %4$s</span>", color, used_str, free_str, avail_str);
	return pango_text(line);
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
	
#ifdef BATTERYBAR
	int atzero = 1;
	char tmp_str1[20] = "";
	char tmp_str2[20] = "";
#endif
	
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
	
	if (state == 'U') {
		if (percent < 2)
			state = 'E';
		if (percent > 98)
			state = 'F';
	}
	
#ifdef BATTERYBAR
	if (percent < 2)
		atzero = 0;

#ifdef SHORTBATTERY //F 100.0%
	strcpy(tmp_str1, "        ");							// 8 zeros
#else
	strcpy(tmp_str1, "           ");							// 11 zeros
#endif
	
	if (state == 'E')
		strcpy(tmp_str2, "Empty");
	else if (state == 'F')
		strcpy(tmp_str2, "Full");
	else
		sprintf(tmp_str2, "%c %.1f%%", state, percent);			// add percentage
	strncpy(tmp_str1, tmp_str2, strlen(tmp_str2));				// add percentage
	
#ifdef SHORTBATTERY
	strcpy(tmp_str2, tmp_str1 + (int)(percent / 14) + atzero);	// copy specified length of blank space to other string
	tmp_str1[(int)(percent / 14) + atzero] = '\0';				// shorten first string to match where 2nd starts
#else
	strcpy(tmp_str2, tmp_str1 + (int)(percent / 10) + atzero);	// copy specified length of blank space to other string
	tmp_str1[(int)(percent / 10) + atzero] = '\0';				// shorten first string to match where 2nd starts
#endif
	
#else
	if (state == 'E')
		return color_text("Empty", "#bfbfbf");
	if (state == 'F')
		return white_text("Full");
#endif
	
	if (percent < 20)
		color = "#ff0000"; // red if below 20%
	else if (percent < 40)
		color = "#ffff00"; // orange if below 40 and above 20
	else if (percent < 75)
		color = "#00ff00"; // green if above 40 and below 75
	else
		color = "#ffffff"; // white if above 75
	
#ifdef BATTERYBAR
	sprintf(output_str, "<span bgcolor=\"%s7f\">%s</span>%s", color, tmp_str1, tmp_str2);
	
	json_object *output = pango_text(output_str);
	json_object_object_add(output, "border", json_object_new_string("#bfbfbf"));
#else
	sprintf(output_str, "%c %.1f%%", state, percent);
	
	json_object *output = color_text(output_str, color);
#endif
	
	json_object_object_add(output, "name", json_object_new_string("Battery"));
	return output;
}
#endif
