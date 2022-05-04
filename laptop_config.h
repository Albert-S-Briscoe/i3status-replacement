#ifndef CONFIG_H
#define CONFIG_H

// number of batteries to show, max 2, 0 to disable.
#define BATTERIES 2

// define to show battery percentage over a battery icon with colored level. comment out to just show percent and state.
#define BATTERYBAR 1

// define to shorten the battery icon by about 1/4.
//#define SHORTBATTERY 1

// print less extra spaces if defined, only changes ram and storage at the moment. comment out to keep spaces.
#define SMALLSCREEN 1

// summarize ram if defined, full "Used: X | Free: X | Avail.: X" if undefined.
#define RAMSUMMARY 1

// define to enbale checking swap (and change color thresholds to seem more urgent if ram is almost full). This should not be defined while RAMSUMMARY is undefined, but all other configurations are valid.
#define CHECKSWAP 1


// Network interface settings:

// network interface to display ip of. Comment out to disable.
#define NETINTERFACE "wlan0"

// 2nd network interface. Comment to disable
#define NETINTERFACE2 "wlan1"

// define to include network interface name(s) with ip address(es).
#define SHOWINTERFACE 1


// Experimental:

// receive click events from i3bar
#define CLICKEVENTS 1

#endif
