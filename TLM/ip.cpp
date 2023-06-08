#ifndef IP_C
#define IP_C
#include "ip.hpp"

Ip::Ip(sc_module_name name):sc_module(name)																									
{
	SC_THREAD(proc);
	s_ip_t0.register_b_transport(this, &Ip::b_transport0);
	s_ip_t1.register_b_transport(this, &Ip::b_transport1);
	command = 0;

	base.reserve(1333*2000*3);
	cout << "IP created" << endl;
}

void Ip::b_transport0(pl_t& pl, sc_time& offset)
{
	tlm_command cmd    = pl.get_command();
	sc_dt::uint64 adr  = pl.get_address();
	const unsigned char *buf = pl.get_data_ptr();
	unsigned int len   = pl.get_data_length();
	switch(cmd)
	{
		case TLM_WRITE_COMMAND:
			command = int(*buf);
			pl.set_response_status(TLM_OK_RESPONSE);
			break;
		case TLM_READ_COMMAND:
			break;
		default:
			pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}
	offset += sc_time(10, SC_NS);
}

void Ip::b_transport1(pl_t& pl, sc_time& offset)
{
	tlm_command cmd    = pl.get_command();
    sc_dt::uint64 adr  = pl.get_address();
    unsigned char *buf = pl.get_data_ptr();
    unsigned int len   = pl.get_data_length();
    

    switch(cmd)
    {
            case TLM_WRITE_COMMAND:
                    //if(adr+len != ram.size())
                    //        ram.resize(ram.size() - (ram.size()-adr)+len,0);

                    for(unsigned int i=0; i<len; i++)
                    {       
                            base[adr+i]=((sc_dt::sc_uint<8>*)buf)[i];
                    }
                    pl.set_response_status(TLM_OK_RESPONSE);
                    cout << "primio pod, ip" << endl;
                    break;

            case TLM_READ_COMMAND:
                    buf = (unsigned char*)&base[adr];
                    pl.set_data_ptr(buf);
                    pl.set_response_status(TLM_OK_RESPONSE);
                    break;
            default:
                    pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
    }
    
    offset += sc_time(10, SC_NS);
}

void Ip::proc()
{
	sc_time offset=SC_ZERO_TIME;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif

	while(1)
	{
		while(command == 0)
		{
			#ifdef QUANTUM
	        qk.inc(sc_time(10, SC_NS));
	        offset = qk.get_local_time();
	        qk.set_and_sync(offset);
	        #else
	        offset += sc_time(10, SC_NS);
	        #endif
		}

		if(command == 1)
		{
			cout << "Usao u komandu jedan" << endl;

			//cv::Mat pic = cv::imread("test.jpg");
			
			cv::Mat pc1;
			pc1 = vectorToMat(base,1333,2000);

			//if(pic == pc1)
				//cout << "Dobro je " << endl;
			cv::imshow("Slika s ispisanim slovom", pc1);
    		
    		cv::waitKey(0);

		}

		command = 0;
	}	

}


cv::Mat Ip::vectorToMat(const vector<sc_dt::sc_uint<8>>& data, int width, int height)
{
	// Provera veličine vektora
    //if (data.size() != width * height * 3) {
        // Greška - veličina vektora se ne podudara sa očekivanom veličinom Mat objekta
        // Možete dodati tretman greške ili vratiti prazan Mat objekat
        //return cv::Mat();
    //}

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

#endif // IP_C