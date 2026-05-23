# chris_dataplane
programmable userspace network dataplane


Designed for packet processing. 

It implements a custom L2/L3/transport pipeline on top of TUN/TAP interfaces, including Ethernet, ARP, IPv4 routing, ICMP, UDP, and a minimal TCP stack. 

The system features a hook‑based processing architecture, dynamic plugin loading, a rule engine, and a CLI‑driven control plane, enabling it to operate as a configurable router, switch, firewall, or experimental networking platform.
