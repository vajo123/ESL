#ifndef IP_H
#define IP_H

#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <sstream>
#include "types.hpp"
#include "address.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

using namespace std;
using namespace sc_core;


SC_MODULE(Ip)
{
	public:
		SC_HAS_PROCESS(Ip);
		Ip(sc_module_name name);
		tlm_utils::simple_target_socket<Ip> s_ip_t0;
		tlm_utils::simple_target_socket<Ip> s_ip_t1;

		sc_out<sc_dt::sc_logic> out_port0;

	protected:
		void b_transport0(pl_t&, sc_time&);
		void b_transport1(pl_t&, sc_time&);
		void proc();

		int getStringWidth(vector<sc_dt::sc_uint<8>>, int);

		vector<sc_dt::sc_uint<8>> base;
		vector<sc_dt::sc_uint<8>> letterData;
		vector<sc_dt::sc_uint<8>> letterMatrix;
		vector<sc_dt::sc_uint<8>> text1;
		vector<sc_dt::sc_uint<8>> text2;
		sc_dt::sc_logic tmp_sig;
		
		int command;
		int len_text;
		vector<vector<sc_dt::sc_uint<8>>> splitText(const vector<sc_dt::sc_uint<8>>&, const vector<sc_dt::sc_uint<8>>&, int);

};




#endif // IP_H