// chipset_tx.h
// Created by p8me in Nov 2020.

#ifndef __CHIPSET_TX_H__
#define __CHIPSET_TX_H__

#include <string>

#include "chipset_pcap.h"

namespace Chipset
{
	class Tx : public Chipset::Pcap
	{
	public:
		Tx(const std::string &ifaceName = "tx") : Chipset::Pcap(ifaceName) {}
		chipsetResult open() override;
		chipsetResult emitFrame(const std::string &frame);
	};
} // namespace Chipset

#endif // __CHIPSET_TX_H__
