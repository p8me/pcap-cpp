// chipset_pcap.cpp
// Created by p8me in Nov 2020.

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <regex>
#include <pstreams/pstream.h>

#include "chipset_pcap.h"
#include "out.h"

namespace Chipset
{
    int pcapRet;

    chipsetResult get_pcap_warn_err(pcap_t *pcap, int ret, const char *f_name)
    {
        chipsetResult chipsetRes;
        static char *additional_err;
        std::stringstream errStr;

        if (ret != 0)
            additional_err = pcap_geterr(pcap);
        if (ret < 0)
        {
            errStr << "\t" << f_name << " error [ret:" << ret << "]";
            if (strlen(additional_err) > 0)
                errStr << "\t\t" << additional_err;
            CHIPSET_ERROR(errStr.str());
        }
        if (ret > 0)
        {
            WARNING_OUT("\t" << f_name << " warning [ret:" << ret << "]");
            if (strlen(additional_err) > 0)
                WARNING_OUT("\t\t" << additional_err);
        }
        return chipsetRes;
    }

    static std::string systemRead(std::string cmd)
    {
        redi::ipstream is(cmd);
        std::string line;
        std::ostringstream result;
        while (std::getline(is, line))
            result << line << std::endl;
        return result.str();
    }

    chipsetResult Pcap::setChannel(int channel)
    {
        chipsetResult chipsetRes;
        std::stringstream cmdss;
        DEBUG_OUT("Setting " << _ifaceName << " to channel " << channel);

        /* Sometimes after running iw set channel the channel is not changed.
		As a workaround we can try first setting the channel
        to channel 7 and then the target channel. Yep! */
        // cmdss << "iw " << _ifaceName << " set channel " << 7;
        // std::system(cmdss.str().c_str());

        cmdss.str("");
        cmdss << "iw " << _ifaceName << " set channel " << channel;
        std::system(cmdss.str().c_str());

        std::smatch match;
        std::regex expression("channel (\\d+)");

        // For verification
        cmdss.str("");
        cmdss << "iw " << _ifaceName << " info";
        std::string str = systemRead(cmdss.str());
        std::regex_search(str, match, expression);
        int actual_channel = atoi(match[1].str().c_str());

        if (actual_channel != channel)
            CHIPSET_ERROR("Failed to set " << _ifaceName << " to " << channel);

        DEBUG_OUT("\tDone!\n");
        return chipsetRes;
    }

    chipsetResult Pcap::open()
    {
        chipsetResult chipsetRes;
        static char errbuf[PCAP_ERRBUF_SIZE];
        int immediate = 1, promiscuous = 1,
            pcp_timeout = 1; // should be > 0 for real-time response

        DEBUG_OUT("Setting up pcap on " << _ifaceName);

        _pcap = pcap_create(_ifaceName.c_str(), errbuf);

        if (!_pcap)
            CHIPSET_ERROR("pcap_create failed!");

        //CHECK_CHIPSET_ERROR(pcap_set_buffer_size(_pcap, 5000), "pcap_set_buffer_size"); // Causes kernel crash on debian linux
        CHECK_CHIPSET_ERROR(pcap_set_snaplen(_pcap, 65536), "pcap_set_snaplen");
        CHECK_CHIPSET_ERROR(pcap_set_promisc(_pcap, promiscuous), "pcap_set_promisc");
        CHECK_CHIPSET_ERROR(pcap_set_timeout(_pcap, pcp_timeout), "pcap_set_timeout");
        CHECK_CHIPSET_ERROR(pcap_set_immediate_mode(_pcap, immediate), "pcap_set_immediate_mode");

        return chipsetRes;
    }

} // namespace Chipset
