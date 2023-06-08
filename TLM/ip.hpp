#ifndef IP_H
#define IP_H

#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "types.hpp"
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

	protected:
		void b_transport0(pl_t&, sc_time&);
		void b_transport1(pl_t&, sc_time&);
		void proc();

		vector<sc_dt::sc_uint<8>> base;
		int command;
		cv::Mat vectorToMat(const vector<sc_dt::sc_uint<8>>& , int, int);

};




#endif // IP_H