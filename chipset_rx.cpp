// chipset_rx.cpp
// Created by p8me in Nov 2020.

#include "chipset_rx.h"
#include <unistd.h>

#define IS_OPEN_WRT_OS (access("/etc/openwrt_release", F_OK) != -1)

namespace Chipset
{
    static void rx_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
    {
        auto rx = reinterpret_cast<Rx *>(args);

        if (!rx->isOpen())
            FATAL_OUT_EXIT("Impossible has happened: !rx->isOpen() in rx_handler");

        if (rx->_stopRequested)
        {
            pcap_breakloop(rx->pcap());
            rx->_stopRequested = false;
            return;
        }

        uint64_t ts = ((uint64_t)header->ts.tv_sec * 1e6 + header->ts.tv_usec);
        std::string frame(reinterpret_cast<char const *>(packet), header->len);

        rx->_callback(ts, frame);
    }

    static void *receptionThread(void *arg)
    {
        auto rx = reinterpret_cast<Rx *>(arg);
        pcap_loop(rx->pcap(), 0, rx_handler, reinterpret_cast<u_char *>(rx));
        return NULL;
    }

    chipsetResult Rx::open()
    {
        chipsetResult chipsetRes;
        static char errbuf[PCAP_ERRBUF_SIZE];

        if (this->isOpen())
            CHIPSET_ERROR("Tried to reOpen pcap on " << this->ifaceName());

        chipsetRes = this->Pcap::open();

        if (chipsetRes.failed())
            return chipsetRes;

        if (!IS_OPEN_WRT_OS)
        {
            int can_set_mon = pcap_can_set_rfmon(pcap());
            if (can_set_mon == 0)
                CHIPSET_ERROR("Device does not support monitor mode")
            else if (can_set_mon < 0)
                CHECK_CHIPSET_ERROR(can_set_mon, "pcap_can_set_rfmon error");

            int monitor_mode = 1;
            CHECK_CHIPSET_ERROR(pcap_set_rfmon(pcap(), monitor_mode), "pcap_set_rfmon");
        }

        CHECK_CHIPSET_ERROR(pcap_activate(pcap()), "pcap_activate");

        if (!IS_OPEN_WRT_OS)
            CHECK_CHIPSET_ERROR(pcap_set_datalink(pcap(), DLT_IEEE802_11_RADIO), "pcap_set_datalink");

        if (pcap_setnonblock(pcap(), 1, errbuf) == PCAP_ERROR)
            CHIPSET_ERROR("pcap_setnonblock returned an error:\n\t" << errbuf);

        int pcap_res = pcap_getnonblock(pcap(), errbuf);
        if (pcap_res < 0)
            CHIPSET_ERROR("pcap_getnonblock returned an error:\n\t" << errbuf);

        if (pcap_res != 1)
            CHIPSET_ERROR("pcap_setnonblock failed to set the pcap tp nonblocking mode.");

        DEBUG_OUT("\tSuccessful!");
        this->_isOpen = true;
        return chipsetRes;
    }

    chipsetResult Rx::start()
    {
        chipsetResult chipsetRes;
        if (!this->isOpen())
        {
            chipsetRes = open();
            if (chipsetRes.failed())
                return chipsetRes;
        }
        if (pthread_create(&this->_receptionThreadObj, NULL, receptionThread, this))
            CHIPSET_ERROR("Failed to create receptionThread with pthread");

        return chipsetRes;
    }

    chipsetResult Pcap::setFilter(const std::string &filter)
    {
        chipsetResult chipsetRes;

        if (!this->isOpen())
        {
            chipsetRes = open();
            if (chipsetRes.failed())
                return chipsetRes;
        }
        const char *filter_c_str = filter.c_str();
        if (filter_c_str && filter_c_str[0])
        {
            struct bpf_program f;
            CHECK_CHIPSET_ERROR(pcap_compile(pcap(), &f, filter_c_str, 1, 0), "pcap_compile");
            CHECK_CHIPSET_ERROR(pcap_setfilter(pcap(), &f), "pcap_setfilter");
            DEBUG_OUT("Setting pcap filter for " << _ifaceName << " to " << filter);
            this->_filter = filter;
        }
        return chipsetRes;
    }

} // namespace Chipset
