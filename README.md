# WOL-shutdown-forward
Two programs written in c, to shutdown on received WOL packet or to forward it on local broadcast address.

1. WOL_forwarder.c - accepts WOL packet and broadcasts it to local network.
2. WOL_shutdown.c - accepts WOL packet and powers off computer. Start it by calling path/to/program/WOL_shutdown network_adapter_name. 

To start it at startup, create a systemd service.

