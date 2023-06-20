#ifndef CPU_H
#define CPU_H

#include <systemc>
#include "../spec/readsrt.hpp"
#include "types.hpp"
#include "address.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;
using namespace sc_core;


SC_MODULE(Cpu){

	public:
		SC_HAS_PROCESS(Cpu);
		Cpu(sc_module_name name, char* input_video, char* input_titl);
		tlm_utils::simple_initiator_socket<Cpu> s_cp_i0;
		tlm_utils::simple_initiator_socket<Cpu> s_cp_i1;

		sc_out<sc_dt::sc_logic> in_port0;
		sc_signal<sc_dt::sc_logic> sig0;
		

	protected:
		void sof();
		vector<sc_dt::sc_uint<8>> ddr;
		char* input_video;
		char* input_titl;

		int command;
		sc_dt::sc_logic tmp_sig;

		void matToVector(const cv::Mat& );
		cv::Mat vectorToMat(const vector<sc_dt::sc_uint<8>>& , int, int);
		vector<vector<vector<sc_dt::sc_uint<1>>>> loadMatrix(const string& );
		vector<sc_dt::sc_uint<1>> transformMatrixArray(const vector<vector<vector<sc_dt::sc_uint<1>>>>& , vector<sc_dt::sc_uint<8>>& );
		void stringToVector(const string&, vector<sc_dt::sc_uint<8>>&);

};

#endif //CPU_H
