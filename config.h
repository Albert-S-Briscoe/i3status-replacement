#ifndef CONFIG_H
#define CONFIG_H

// number of batteries to show, max 2, 0 to disable.
#define BATTERIES 0

// print less extra spaces if defined, only changes ram and storage at the moment.
//#define SMALLSCREEN 1

// summarize ram if defined, full "Used: X | Free: X | Avail.: X" feilds if undefined.
//#define RAMSUMMARY 1

// define to enbale checking swap (and change color thresholds to seem more urgent if ram is almost full). This should not be defined while RAMSUMMARY is undefined, but all other configurations are valid.
//#define CHECKSWAP 1


// Network interface settings:

// network interface to display ip of. Comment out to disable.
#define NETINTERFACE "enp8s0"

// 2nd network interface. Comment to disable
//#define NETINTERFACE2 "wlan1"

// define to include network interface name(s) with ip address(es).
//#define SHOWINTERFACE 1


#endif
