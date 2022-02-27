#ifndef CONFIG_H
#define CONFIG_H

// number of batteries to show, 0 for none
#define BATTERIES 2

// summarize ram if defined, full "Used", "Free", and "Avail." feilds if not defined.
#define RAMSUMMARY 1

// print less spaces if defined, only changes ram and storage space at the moment.
#define SMALLSCREEN 1

// define to enbale checking swap (and change color thresholds to seem more urgent if ram is almost full), not fully compatible with a undefined RAMSUMMARY.
#define CHECKSWAP 1

// network interface to use. undefined if none
#define NETINTERFACE "wlan0"

#endif
