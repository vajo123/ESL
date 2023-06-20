#include <systemc>
#include "cpu.hpp"
#include "memory.hpp"
#include "intcon.hpp"
#include "dma.hpp"
#include "ip.hpp"

int sc_main(int argc, char* argv[])
{

	char* input_video = argv[1];
    char* input_titl = argv[2];

    //cout << input_titl << endl;
    //cout << input_video << endl;

    Cpu cpu("Cpu", input_video, input_titl);

    Mem memory("memory");

    InterCon intcon("intcon");

    DMA dma("dma");

    Ip ip("ip");

    ip.out_port0(cpu.in_port0);
    cpu.s_cp_i1.bind(memory.s_mem_t1);
    cpu.s_cp_i0.bind(intcon.s_ic_t);
    intcon.s_ic_i1.bind(dma.s_dma_t);
    intcon.s_ic_i0.bind(ip.s_ip_t0);
    dma.s_dma_i0.bind(memory.s_mem_t0);
    dma.s_dma_i1.bind(ip.s_ip_t1);
    
    #ifdef QUANTUM
    tlm_global_quantum::instance().set(sc_time(10, SC_NS));
    #endif

    cout<<"Starting simulation..."<<endl;

    sc_start(10,SC_SEC);

    cout << "Simulation finished at " << sc_time_stamp() <<endl;
	
    return 0;
}


