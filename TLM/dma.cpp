#ifndef DMA_C
#define DMA_C
#include "dma.hpp"

DMA::DMA(sc_module_name name):sc_module(name)
{
	s_dma_t.register_b_transport(this, &DMA::b_transport);
	startRead = sc_dt::SC_LOGIC_0;
	startSend = sc_dt::SC_LOGIC_0;
}

void DMA::b_transport(pl_t& pl, sc_time& offset)
{
	cmd         = pl.get_command();
	adr         = pl.get_address();
	length      = pl.get_data_length();
	buf         = pl.get_data_ptr();
	begin       = adr-0x81000000;
	switch (cmd)
	{
		case TLM_WRITE_COMMAND:
			cout << "Usao u dma" << endl;
			pl.set_command(TLM_READ_COMMAND);
			pl.set_address(begin);
			s_dma_i0->b_transport(pl, offset);
			assert(pl.get_response_status() == TLM_OK_RESPONSE);
			cout << "Usao u dma, pristupam mem" << endl;
			buf = pl.get_data_ptr();
			pom_mem.clear();

			for(unsigned int i=0; i<length; i++)
			{
				pom_mem.push_back(((sc_dt::sc_uint<8>*)buf)[i]);
			}

			buf=(unsigned char*)&pom_mem[0];

			cout << "Usao u dma, saljem ip" << endl;
			
			pl.set_address(begin);
			pl.set_command(TLM_WRITE_COMMAND);
			pl.set_data_ptr(buf);
			s_dma_i1->b_transport(pl, offset);
			assert(pl.get_response_status() == TLM_OK_RESPONSE);
			
			//sent to hw 
			startSend = sc_dt::SC_LOGIC_1;		 	

			pl.set_response_status( TLM_OK_RESPONSE );
			offset += sc_time(20, SC_NS);
			break;

		case TLM_READ_COMMAND:
			//START READ FROM FIFO, OUTPUT IMAGE FROM CONV LAYER
			startRead = sc_dt::SC_LOGIC_1;
			offset += sc_time(20, SC_NS);
			break;

		default:
			pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}
}
#endif // DMA_C