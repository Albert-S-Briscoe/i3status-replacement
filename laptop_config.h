#ifndef CONFIG_H
#define CONFIG_H

// Number of batteries to show, max 2, 0 to disable.
#define BATTERIES 2

// Define to show battery percentage over a battery icon with colored level. Comment out to just show percent and state.
#define BATTERYBAR

// Define to shorten the battery icon by about 1/4.
//#define SHORTBATTERY

// Print less extra spaces if defined, only changes ram and storage at the moment. Comment out to keep spaces.
#define SMALLSCREEN

// Summarize ram if defined, full "Used: X | Free: X | Avail.: X" if undefined.
#define RAMSUMMARY

// Define to enbale checking swap (and change color thresholds to seem more urgent if ram is almost full). This should not be defined while RAMSUMMARY is undefined, but all other configurations are valid.
#define CHECKSWAP


// Network interface settings:

// Network interface to display ip of. Comment out to disable.
#define NETINTERFACE "wlan0"

// 2nd network interface. Comment to disable
#define NETINTERFACE2 "wlan1"

// Define to prefix ip address(es) with network interface name(s).
#define SHOWINTERFACE


// Experimental:

// Receive click events from i3bar
//#define CLICKEVENTS

#endif
