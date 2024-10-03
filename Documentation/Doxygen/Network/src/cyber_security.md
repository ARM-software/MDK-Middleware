# Cyber Security {#cyber_security}

Every device that is connected to a network is a potential target of a cyber attack. This also counts for devices that are
not connected to public networks - a private network might get hacked and all devices attached to it compromised. Thus, the
industry has a strong drive towards safety and security. Dedicated vulnerability tests enable you to check your equipment
throughout the development stage.

## Achilles Test Platform

The
**[Achilles Test Platform](https://www.ge.com/digital/sites/default/files/download_assets/achilles-test-platform-from-ge-digital-datasheet.pdf)**
by **[GE Digital](https://www.ge.com/digital/)** provides a stress test that simulates cyber attacks via a TCP/IP network connection.
GE Digital offers also a certification service for end-equipment.

The **Network Component** is verified with the Achilles Test Software (ATS) and passes Level 1 tests. 
Thus, Arm Cortex-M based microcontrollers using the **Network Component**, should pass the Achilles Level 1 certification.

If during your own tests the ATS has recorded an anomaly, usually the Storm Rate Limit for the device under test is too high
and should be reduced. In general, it is enough to reduce the overall Global Storm Rate limit. For a device based on an Arm
Cortex-M3 running at 120 MHz and an EMAC controller with no checksum offload capabilities, an initial rate limit of 2000
packets per second is a good choice.
 
The Network component itself does not have packet rate limit. The rate limit is variable and among others depends also on the
test packet size and content. When the packet loss is negligible for the tests, the global storm rate may be increased. If
the ATS detects anomalies, it is necessary to reduce the global storm rate.

### Setting up the Achilles Test Software

In order to be able to run the ATS, you need to setup your device in a certain way. Please follow the steps below (make sure
that all IP address ranges are adapted to your own internal test network):

-# Environment/Test Network 1
   - Achilles IP Address: 192.168.219.200/24 (should be in same network as the DUT)
   - Peer IP Address: 192.168.219.50 (IP address of the device)
   - Auto detect MAC address
   - Select Link Mode: 100BASE-T

-# Environment/Monitors
   - ICMP Monitor (DUT #1) (do not enable ARP Monitor)

-# Environment/Details of Target Device
   - Enter Target Device Name (this name is printed in the test report)

-# Discovery/Open Ports and Multicast Addresses
   - Scan for Open TCP Port Numbers: 0 - 65535
   - Manually Add UDP Port Number: 137 (none if UDP not used; in this case, also disable UDP tests in the Test Setup)
   - Manually Add Multicast IP Address: 224.0.0.1 (none if IGMP not used)

-# Test Setup/Test Suite
   - open predefined: Level 1 Test Suite (File -> Restore Test Suite -> Level 1)
   - Show Test Suite Settings
   - Global Storm Rate Limit: Limit (packets/s) 2000 (the limit depends on CPU speed and EMAC hardware features)

-# Test execution/Execute All

### Storm Rate Limit determination

To determine the correct Storm Rate Limit, check the ICMP 1 control panel during the test. The red line indicates a packet
loss. If the diagram crosses the 10% warning level, the test fails. This is a sign that the Storm Rate Limit is too high and
needs to be reduced.

Refer to **Achilles Level 1 Certification Requirements for Embedded Devices** for more information.
