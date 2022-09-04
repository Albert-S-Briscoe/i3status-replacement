#include <time.h>			// get_time
#include <sys/sysinfo.h>	// get_mem_info
#include <sys/statfs.h>		// get_fs

#include "getinfo.h"


// get time and display it in a few ways
void get_time(char* output, size_t output_size) {
	char time_str[100]; // " | UNIX xxxxxxxxxx |  UTC | " +8+27 is 63
	char utc_time_str[10]; // "xx:xx:xx" is 8
	char local_time_str[40]; // "xxx xxxx/xx/xx  xx:xx:xx xx" is 27
	char* error;
	struct tm *tmp;
	time_t unixtime = time(NULL);

	tmp = localtime(&unixtime);
	if (tmp == NULL) {
		error = "Internal error: localtime";
		goto error;
	}
	if (strftime(local_time_str, sizeof(local_time_str), "%Z %Y/%m/%d %l:%M:%S %p", tmp) == 0) {
		error = "Internal error: strftime";
		goto error;
	}

	tmp = gmtime(&unixtime);
	if (tmp == NULL) {
		error = "Internal error: gmtime";
		goto error;
	}
	if (strftime(utc_time_str, sizeof(utc_time_str), "%T", tmp) == 0) {
		error = "Internal error: strftime";
		goto error;
	}

	sprintf(time_str, " | UNIX %ld | %s UTC | %s", unixtime, utc_time_str, local_time_str);

	white_text(time_str, output, output_size);
	return;
error:
	error_text(error, output, output_size);
}

// get used ram and swap, change colors based on ram usage
void get_mem_info(char* output, size_t output_size) {
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

		color_text(line, color, output, output_size);
		return;
	}
#endif
#endif

	if (memperc < 10)
		color = "#ffffff"; // white if less than 10%
	else if (memperc < 90)
		color = "#00ff00"; // green if less than 90% and greater than 10%
#ifndef CHECKSWAP
	else if (memperc > 95)
		color = "#ff0000"; // red if greater then 95%
#endif
	else
		color = "#ffff00"; // orange if greater than 90%


#ifdef RAMSUMMARY
	sprintf(line, "RAM %.1f%% (%s)", memperc, format_memory((double)usedram));
	color_text(line, color, output, output_size);
#else
	strcpy(used_str, format_memory((double)usedram));
	strcpy(free_str, format_memory((double)freeram));
	strcpy(avail_str, format_memory((double)available));
	sprintf(line, "<span color=\\\"%1$s\\\">Used: %2$s<\\/span> | "
	              "<span color=\\\"%1$s\\\">Free: %3$s<\\/span> | "
	              "<span color=\\\"%1$s\\\">Avail.: %4$s<\\/span>", color, used_str, free_str, avail_str);
	pango_text(line, output, output_size);
#endif
}

// Needs color
void get_fs(char *fs, char* output, size_t output_size) {
	struct statfs fs_info;
	double total;
	double free;
	char output_str[100];
	char name_str[100];
	char total_str[10];

	statfs(fs, &fs_info);

	total = (double)fs_info.f_bsize * fs_info.f_blocks / 1024;
	free =  (double)fs_info.f_bsize * fs_info.f_bavail / 1024;

	// format_memory can't be called twice by the same function
	strcpy(total_str, format_memory(total));
	json_escape(fs, name_str);
	sprintf(output_str, "%s %s\\/%s | ", name_str, format_memory(free), total_str);

	white_text(output_str, output, output_size);
}

#if BATTERIES > 0
void get_batteries(char output[][BAT_STR_SIZE], size_t output_size) {
	FILE *file_var;
	char file_path[100];

	int charge_full;
	int charge_level;
	char state;
	float percent;

	char output_str[100];
	char *color;

#ifdef BATTERYBAR
	int charged_chars;
	char tmp_str1[20] = "";
	char tmp_str2[20] = "";
#endif

	for (int i = 0; i < BATTERIES; i++) {
		// get info from sysfs
		// update charge_full less often?
		sprintf(file_path, "/sys/class/power_supply/BAT%d/charge_full", i + BATTERY_START);
		file_var = fopen(file_path, "r");
		if (file_var == NULL)
			goto no_bat;
		if (fscanf(file_var, "%d", &charge_full) != 1)
			goto no_bat;
		fclose(file_var);

		sprintf(file_path, "/sys/class/power_supply/BAT%d/charge_now", i + BATTERY_START);
		file_var = fopen(file_path, "r");
		if (file_var == NULL)
			goto no_bat;
		if (fscanf(file_var, "%d", &charge_level) != 1)
			goto no_bat;
		fclose(file_var);

		sprintf(file_path, "/sys/class/power_supply/BAT%d/status", i + BATTERY_START);
		file_var = fopen(file_path, "r");
		if (file_var == NULL)
			goto no_bat;
		state = fgetc(file_var);
		fclose(file_var);

		percent = ((float)charge_level / (float)charge_full) * 100;

		if (state == 'U') {
			if (percent < 2)
				state = 'E';
			else if (percent > 98)
				state = 'F';
		}

#ifndef BATTERYBAR
		if (state == 'E') {
			color_text("Empty", "#bfbfbf", output[i], output_size);
			continue;
		}
		if (state == 'F') {
			white_text("Full", output[i], output_size);
			continue;
		}
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
		charged_chars = (percent / BATDIVISOR);
		if (percent > 2)
			charged_chars += 1;

		if (state == 'E')
			strcpy(tmp_str1, "Empty      ");
		else if (state == 'F')
			strcpy(tmp_str1, "Full       ");
		else
			sprintf(tmp_str1, "%c %.1f%%     ", state, percent);			// add percentage

		tmp_str1[BATSTRLEN] = '\0';
		strcpy(tmp_str2, tmp_str1 + charged_chars);	// copy specified length of blank space to other string
		tmp_str1[charged_chars] = '\0';				// shorten first string to match where 2nd starts

		sprintf(output_str, "<span bgcolor=\\\"%s7f\\\">%s<\\/span>%s", color, tmp_str1, tmp_str2);
		pango_text(output_str, output[i], output_size);

		// add extra objects
		output[i][strlen(output[i])-1] = '\0'; // remove the } from the json so we can add more objects
		sprintf(output_str, ",\"border\":\"#bfbfbf\",\"name\":\"Battery\"},\n");
		strcat(output[i], output_str);
		strcat(output[i], WHITE_TEXT(") | "));
#else
		sprintf(output_str, "<span color=\\\"%s\\\">%c %.1f%%<\\/span> | ", color, state, percent);
		pango_text(output_str, output[i], output_size);

		// add extra objects
		output[i][strlen(output[i])-1] = '\0'; // remove the } from the json so we can add more objects
		sprintf(output_str, ",\"name\":\"Battery\"}");
		strcat(output[i], output_str);
#endif
		continue;

	no_bat:
		pango_text("<span color=\\\"#7f7f7f\\\">N<\\/span> | ", output[i], output_size);
		continue;
	}
}
#endif
