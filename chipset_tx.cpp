// chipset_tx.cpp
// Created by p8me in Nov 2020.

#include "chipset_tx.h"
#include <pcap.h>

namespace Chipset
{
	chipsetResult Tx::open()
	{
		chipsetResult chipsetRes = this->Pcap::open();

		if (chipsetRes.failed())
			return chipsetRes;

		CHECK_CHIPSET_ERROR(pcap_activate(pcap()), "pcap_activate");

		DEBUG_OUT("\tSuccessful!");
		this->_isOpen = true;
		return chipsetRes;
	}

	chipsetResult Tx::emitFrame(const std::string &frame)
	{
		chipsetResult chipsetRes;
		if (!this->isOpen())
		{
			chipsetRes = open();
			if (chipsetRes.failed())
				return chipsetRes;
		}

		const unsigned char *frameBytes = reinterpret_cast<const unsigned char *>(frame.c_str());
		CHECK_CHIPSET_ERROR(pcap_sendpacket(this->pcap(), frameBytes, frame.length()), "pcap_sendpacket");
		return chipsetRes;
	}

} // namespace Chipset
