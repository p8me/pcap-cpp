// chipset_rx.h
// Created by p8me in Nov 2020.

#ifndef __CHIPSET_RX_H__
#define __CHIPSET_RX_H__

#include <functional>
#include <string>
#include <pcap.h>
#include <pthread.h>
#include <unistd.h>

#include "chipset_pcap.h"

/*
Example use-case:
1. instantiate Rx()
2. open()
3. setFilter()
4. start()
5. [optional] changeFilter
*/

namespace Chipset
{
    class Rx : public Chipset::Pcap
    {
    public:
        Rx(
            const std::string &ifaceName = "rx",
            const std::function<void(uint64_t, std::string)> cb = [](uint64_t, std::string) {})
            : Chipset::Pcap(ifaceName),
              _callback(cb),
              _stopRequested(false){};

        chipsetResult open() override;
        chipsetResult setFilter(const std::string &filter);
        chipsetResult start();
        bool stop(int timeoutMs) // returns true on timeout
        {
            _stopRequested = true;
            while (_stopRequested && --timeoutMs >= 0)
                usleep(1000); // 1ms
            return timeoutMs < 0;
        }

        //! Following are made public to be accessible to the static function "rx_handler"
        const std::function<void(uint64_t, std::string)> _callback;
        bool _stopRequested;

    private:
        pthread_t _receptionThreadObj;
    };
} // namespace Chipset

#endif // __CHIPSET_RX_H__
