# csa-attack

## Purpose of this project 
This tool manipulates WiFi network beacon frames to perform a channel switch attack. It captures beacon frames of a specific WiFi Access Point (AP) and injects a Channel Switch Announcement (CSA) Information Element (IE) to force clients to move to a different channel.

## Prerequisites  
To use this tool, a wireless network environment is required, and the network interface must be set to monitor mode.  
For testing purposes, you need to set up a wireless network using an 802.11n wireless adapter. To enable monitor mode on the network interface, execute the following commands:  
```bash
# Check available wireless interfaces
iwconfig  

# Enable monitor mode
sudo ip link set <interface> down  
sudo iw dev <interface> set type monitor  
sudo ip link set <interface> up  
```

## How to build

```bash
make
```

## How to use the code
```bash
syntax : csa-attack <interface> <ap mac> [<station mac>]
sample : csa-attack mon0 01:23:45:67:89:AB 01:23:45:67:89:AB
```

## Result
![](./result_mobile.jpg)
![](./result_desktop.png)