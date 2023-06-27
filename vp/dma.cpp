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

    unsigned int remaining = length;
    unsigned int tmp = 0;
	switch (cmd)
	{
		case TLM_WRITE_COMMAND:
			cout << "dma: cita podatke iz mem" << endl;
			
			pl.set_command(TLM_READ_COMMAND);
			pl.set_address(begin);
			s_dma_i0->b_transport(pl, offset);
			assert(pl.get_response_status() == TLM_OK_RESPONSE);

			buf = pl.get_data_ptr();
			pom_mem.clear();
			
			for(unsigned int i=0; i<length; i++)
			{
				pom_mem.push_back(((sc_dt::sc_uint<8>*)buf)[i]);
			}
			
			cout << "dma: salje podatke u ip" << endl;

			buf=(unsigned char*)&pom_mem[0];
			
			// Slanje podataka u blokovima od 10
            while (remaining > 0)
            {
                unsigned int send_length = min(10u, remaining);
                pl.set_address(tmp);
                pl.set_command(TLM_WRITE_COMMAND);
                pl.set_data_ptr(buf);
                pl.set_data_length(send_length);
                s_dma_i1->b_transport(pl, offset);
                assert(pl.get_response_status() == TLM_OK_RESPONSE);

                tmp += send_length;
                remaining -= send_length;
            }

			//sent to hw 
			startSend = sc_dt::SC_LOGIC_1;		 	

			pl.set_response_status( TLM_OK_RESPONSE );
			offset += sc_time(20, SC_NS);
			break;

		case TLM_READ_COMMAND:
			cout << "dma: cita podatke iz ip" << endl;

			pom_mem.clear();

			while (remaining > 0)
            {
                unsigned int read_length = min(10u, remaining);
                pl.set_address(0 + tmp);
                pl.set_command(TLM_READ_COMMAND);
                pl.set_data_length(read_length);
                s_dma_i1->b_transport(pl, offset);
                assert(pl.get_response_status() == TLM_OK_RESPONSE);

                buf = pl.get_data_ptr();
                for (unsigned int i = 0; i < read_length; i++) {
        			pom_mem.push_back(((sc_dt::sc_uint<8>*)buf)[i]);
   				}

                tmp += read_length;
                remaining -= read_length;
            }
	
			cout << "dma: salje podatke u mem" << endl;

			buf=(unsigned char*)&pom_mem[0];
			pl.set_address(begin);
			pl.set_data_length(length);
			pl.set_command(TLM_WRITE_COMMAND);
			pl.set_data_ptr(buf);
			s_dma_i0->b_transport(pl, offset);
			assert(pl.get_response_status() == TLM_OK_RESPONSE);

			startRead = sc_dt::SC_LOGIC_1;
			offset += sc_time(20, SC_NS);
			break;

		default:
			pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}
}


#endif // DMA_C