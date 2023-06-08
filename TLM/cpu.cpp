#ifndef CPU_C
#define CPU_C

#include "cpu.hpp"

Cpu::Cpu(sc_module_name name, char* input_video, char* input_titl):sc_module(name), input_video(input_video), input_titl(input_titl)
{
	SC_THREAD(sof);
	command = 0;
	cout << "Cpu constucted" << endl;

}

void Cpu::sof()
{	

	cv::Mat slika = cv::imread(input_video);
	unsigned char *buf;


	pl_t pl;
	sc_time offset=SC_ZERO_TIME;

	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
	
	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif 

	//convert Mat to vector<sc_uint<8>> and added to ddr
	matToVector(slika);
	
	//Transfer image to Memory
	buf=(unsigned char*)&ddr[0];

	pl.set_address(0);
	pl.set_data_length(slika.cols * slika.rows * 3);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i1->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);

	#ifdef QUANTUM
	qk.set_and_sync(offset);
	#endif

	#ifdef PRINTS
	 
	#endif
	cout<< "Ofset: " << offset << endl; 
	cout << "podaci su ubaceni u memoriju u "<<sc_time_stamp()<<endl;
	
	ddr.clear();

	pl.set_address(0x81000000);
	pl.set_data_length(slika.cols * slika.rows * 3);
	pl.set_command(TLM_WRITE_COMMAND);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	cout<< "Ofset: " << offset << endl;
	cout << "Nakon dma " << sc_time_stamp() << endl;	
	
	command = 1;
	buf = (unsigned char*)&command;

	pl.set_address(0x80000000);
	pl.set_data_length(1);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);

	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	cout <<offset <<endl;
	cout << "Nakon intcon " << sc_time_stamp() << endl;

	
}


void Cpu::matToVector(const cv::Mat& mat)
{
	if(ddr.size() != 0) ddr.clear();

	// Provjera da li je Mat objekt trokanalna slika
    if (mat.channels() != 3) {
        // Greška - očekuje se trokanalna slika
        // Možete dodati tretman greške ili vratiti prazan vektor
    }

    // Kopiranje podataka iz Mat objekta u vektor
    if (mat.isContinuous()) {
        // Ako je Mat objekt kontinuiran, možemo jednostavno kopirati sve podatke u vektor
        const uchar* ptr = mat.ptr<uchar>(0);
        ddr.assign(ptr, ptr + mat.total() * mat.channels());
    } else {
        // Ako Mat objekt nije kontinuiran, moramo kopirati red po red
        for (int i = 0; i < mat.rows; i++) {
            const uchar* rowPtr = mat.ptr<uchar>(i);
            for (int j = 0; j < mat.cols; j++) {
                ddr.push_back(rowPtr[j * 3]);  // Plava komponenta piksela
                ddr.push_back(rowPtr[j * 3 + 1]);  // Zelena komponenta piksela
                ddr.push_back(rowPtr[j * 3 + 2]);  // Crvena komponenta piksela
            }
        }
    }    
}


cv::Mat Cpu::vectorToMat(const vector<sc_dt::sc_uint<8>>& data, int width, int height)
{
	// Provera veličine vektora
    if (data.size() != width * height * 3) {
        // Greška - veličina vektora se ne podudara sa očekivanom veličinom Mat objekta
        // Možete dodati tretman greške ili vratiti prazan Mat objekat
        return cv::Mat();
    }

    // Kreiranje Mat objekta
    cv::Mat mat(height, width, CV_8UC3);

    // Kopiranje podataka iz vektora u Mat objekat
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = (i * width + j) * 3;
            mat.at<cv::Vec3b>(i, j) = cv::Vec3b(data[idx], data[idx + 1], data[idx + 2]);
        }
    }

    return mat;
}










#endif //CPU_C