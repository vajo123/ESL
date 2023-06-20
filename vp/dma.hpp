#ifndef DMA_H
#define DMA_H

#include <systemc>
#include "types.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
#include <string>
#include <iostream>

using namespace std;
using namespace sc_core;


SC_MODULE(DMA)
{
	public:
			SC_HAS_PROCESS(DMA);
			DMA(sc_module_name name);
			tlm_utils::simple_target_socket<DMA> s_dma_t;
			tlm_utils::simple_initiator_socket<DMA> s_dma_i0;
			tlm_utils::simple_initiator_socket<DMA> s_dma_i1;

	protected:
			void b_transport(pl_t&, sc_time&);	

			vector<sc_dt::sc_uint<8>> pom_mem;
			sc_dt::sc_logic startSend;
			sc_dt::sc_logic startRead;

			tlm_command cmd;
			sc_dt::uint64 adr;
			unsigned int length;
			unsigned char *buf;
			unsigned int begin;

};




#endif // DMA_H 