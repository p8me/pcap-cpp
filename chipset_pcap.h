// chipset_pcap.h
// Created by p8me in Nov 2020.

#ifndef __CHIPSET_PCAP_H__
#define __CHIPSET_PCAP_H__

#include <pcap.h>
#include <string>
#include <sstream>
#include "out.h"

#define CHIPSET_ERROR(...)         \
    {                              \
        ERROR_OUT(__VA_ARGS__);    \
        chipsetRes << __VA_ARGS__; \
        return chipsetRes;         \
    }

/* chipsetRes must be defined and should be used a function that returns chipsetResult */
#define CHECK_CHIPSET_ERROR(ret, f_name)                         \
    {                                                            \
        pcapRet = ret;                                           \
        chipsetRes = get_pcap_warn_err(pcap(), pcapRet, f_name); \
        if (chipsetRes.failed())                                 \
            return chipsetRes;                                   \
    }

namespace Chipset
{
    extern int pcapRet;

    class chipsetResult
    {
    public:
        chipsetResult() : _failed(false),
                          _errorMessage("") {}
        template <typename T>
        chipsetResult &operator<<(T &&value)
        {
            _failed = true;
            _errorMessage << value;
            return *this;
        }
        std::string getErrorMessage() { return _errorMessage.str(); } // don't return by reference. std::ostringstream::str is temporary
        const bool failed() { return _failed; }

    private:
        bool _failed;
        std::ostringstream _errorMessage;
    };

    chipsetResult get_pcap_warn_err(pcap_t *pcap, int ret, const char *f_name);

    class Pcap
    {
    public:
        Pcap(const std::string &ifaceName) : _isOpen(false),
                                             _pcap(NULL),
                                             _ifaceName(ifaceName),
                                             _filter(""){};
        virtual ~Pcap()
        {
            pcap_close(_pcap);
            _isOpen = false;
            _pcap = NULL;
        }
        chipsetResult setChannel(int channel);
        chipsetResult setFilter(const std::string &filter);

        // Accessors
        const std::string &ifaceName() { return _ifaceName; }
        const std::string &filter() { return _filter; }
        bool isOpen() { return _isOpen; }
        pcap_t *pcap()
        {
            if (_isOpen)
                return _pcap;
            FATAL_OUT_EXIT("Please first create a pcap object with Pcap::open()");
        }

    protected:
        virtual chipsetResult open();
        bool _isOpen; // ?? a getter? Tx and Rx need to both read and write to it. Is this an ugly solution?

    private:
        pcap_t *_pcap;
        std::string _ifaceName;
        std::string _filter;
    };

} // namespace Chipset

#endif // __CHIPSET_PCAP_H__
