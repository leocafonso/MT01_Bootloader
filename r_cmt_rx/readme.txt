PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_cmt_rx
========

Document Number 
---------------
Sample Code

Version
-------
v1.40

Overview
--------
This module creates a timer tick using a CMT channel based on a frequency input by the user.

Features
--------
* Create periodic or one-shot timer easily by passing in desired frequency/period
* User is alerted through callback function
* CMT channels are allocated dynamically.

Supported MCUs
--------------
* RX621, RX62N Group
* RX62T Group
* RX630 Group
* RX631, RX63N Group 
* RX210 Group
* RX111 Group

Boards Tested On
----------------
* RSKRX62T
* RDKRX62N
* RSKRX630
* RSKRX63N
* RDKRX63N
* RSKRX210
* RSKRX111

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* CMT

Required Packages
-----------------
* None

How to add to your project
--------------------------
* Add src\r_cmt_rx.c to your project.
* Add an include path to the 'r_cmt_rx' directory. 
* Add an include path to the 'r_cmt_rx\src' directory.
* Copy r_cmt_rx_config_reference.h from 'ref' directory to your desired location and rename to r_cmt_rx_config.h.
* Configure middleware through r_cmt_rx_config.h.

Toolchain(s) Used
-----------------
* Renesas RX v1.02

File Structure
--------------
r_cmt_rx
|   readme.txt
|   r_cmt_rx_if.h
|
+---demo
|       r_cmt_demo_main.c
|
+---doc
|       r_cmt_rx.pdf
|
+---ref
|       r_cmt_rx_config_reference.h
|
\---src
        r_cmt_rx.c
                
