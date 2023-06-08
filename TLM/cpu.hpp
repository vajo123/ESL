#ifndef CPU_H
#define CPU_H

#include <systemc>
#include "readsrt.hpp"
#include "types.hpp"
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
		

	protected:
		void sof();
		vector<sc_dt::sc_uint<8>> ddr;
		char* input_video;
		char* input_titl;

		int command;

		void matToVector(const cv::Mat& );
		cv::Mat vectorToMat(const vector<sc_dt::sc_uint<8>>& , int, int);


};

#endif //CPU_H