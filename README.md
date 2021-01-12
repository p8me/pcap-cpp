# A minimum cpp wrapper for pcap for live Rx and Tx
This is a minimum pcap wrapper that creates a live Tx or Rx object for a network interface.
For a more comprehensive tool check out [PcapPlusPlus][PcapPlusPlus].

## Dependecies
1. [libpcap][tcpdump]

## Create an interface (linux) [optional]

For some applications, an interface may be created prior to creating `Chipset::Rx` or `Chipset::Tx` instances, then the name of that interface shall be passed to `Chipset::Rx` / `Chipset::Tx` constructors.

Here is an example of creating a new interface on a wifi card on a linux machine with [`iw` tool][iw]:

```shell
>> iw PHY_NAME interface add INTERFACE_NAME type monitor flags fcsfail control
>> ifconfig INTERFACE_NAME up
```
* `PHY_NAME` is usually something like `phy2` (you can type `iw` to get a list of avaiable phys and declared interfaces on each `phy`).

* `INTERFACE_NAME` is arbitrary and is the name that should be passed to `Chipset::Rx` / `Chipset::Tx` constructors.

* In many cases the interface needs to be in `monitor` mode to allow arbitrary Tx or receive frames that are not addressed to the device.

*  `fscfail` and and `control` flags are optional, you can learn more about the flags in [iw manual][iw].

* In some cases depending on the wifi card, other interfaces on the same card should be down. To shut down and interface (`wlan1`) for example:
    ```shell
    >> ifconfig wlan1 down
    ```


[iw]: https://wireless.wiki.kernel.org/en/users/documentation/iw


## Sample code

```cpp
#include <unistd.h>
#include "chipset_rx.h"
#include "chipset_tx.h"

// all debug messages are printed to std::cout
bool verbose = true;
// Warnings are always printed to std::cout, errors to std::cerr. See out.h.

void rx_callback(uint64_t timeStamp, std::string packet){
    /* take care of the incoming packet */
    std::cout << timeStamp;
}

int main(int argc, char **argv)
{
    Chipset::Rx rx("rxInterfaceName", rx_callback);

    // Set filter (optional)
    std::string filter ("ether host 00:de:ad:be:ef:00");
    rx.setFilter(filter);

    // Open interface for capturing
    rx.open();

    // Run for 10 seconds
    usleep(10000);
    
    // Stop capture
    rx.stop();
    
    return 0;
}
```

## Related
* [tshark](https://www.wireshark.org/docs/man-pages/tshark.html)
* [PcapPlusPlus].
* [tcpdump]

[PcapPlusPlus]: https://pcapplusplus.github.io/

[tcpdump]: https://www.tcpdump.org/