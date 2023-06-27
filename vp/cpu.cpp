#ifndef CPU_C
#define CPU_C
#include "cpu.hpp"

Cpu::Cpu(sc_module_name name, char* input_video, char* input_titl):sc_module(name), input_video(input_video), input_titl(input_titl)
{
	SC_THREAD(sof);
	in_port0.bind(sig0);
	in_port1.bind(sig1);
	sig0 = sc_dt::SC_LOGIC_0;
	sig1 = sc_dt::SC_LOGIC_0;
	command = 0;

	cout << "Cpu constucted" << endl;
}

void Cpu::sof()
{	
	unsigned char *buf;
  	unsigned int LEN_MATRIX;
	unsigned int LEN_FRAME;

	// Učitaj video datoteku
    VideoCapture cap(input_video);

    // Provjeri da li se video uspješno otvorio
    if (!cap.isOpened()) 
        cout << "Error opening video file" << endl;

    ReadSrt Srb(input_titl);
    vector<int> startTime = Srb.getStartTimes();
    vector<int> endTime = Srb.getEndTimes();
    vector<string> text1 = Srb.getText1();
    vector<string> text2 = Srb.getText2();

    int max_index = startTime.size();

    // Get the frame rate
    int fps = cap.get(CAP_PROP_FPS);
    
    int delay = 1000 / fps;

    int current_subtitle_index = 0;

    int frame_count = 0;
    
    bool pom = true;
    
    bool pause = false;

	int width_frame = int(cap.get(CAP_PROP_FRAME_WIDTH));
    int height_frame = int(cap.get(CAP_PROP_FRAME_HEIGHT));
    int dimension;
    int bram_row;

    vector<vector<vector<sc_dt::sc_uint<1>>>> letterMatrix;

    if(width_frame + height_frame > 2750)
    {
    	dimension = 4;
		letterMatrix = loadMatrix("../data/font_database1920.txt");
		LEN_MATRIX = D4_LETTER_MATRIX;
		LEN_FRAME = D4_FRAME_SIZE;
		bram_row = D4_BRAM;
    }
    else if(width_frame + height_frame > 2250)
    {
    	dimension = 3;
    	letterMatrix = loadMatrix("../data/font_database1600.txt");
    	LEN_MATRIX = D3_LETTER_MATRIX;
    	LEN_FRAME = D3_FRAME_SIZE;
    	bram_row = D3_BRAM;
    }
    else if(width_frame + height_frame > 1750)
    {
    	dimension = 2;
    	letterMatrix = loadMatrix("../data/font_database1280.txt");
    	LEN_MATRIX = D2_LETTER_MATRIX;
    	LEN_FRAME = D2_FRAME_SIZE;
    	bram_row = D2_BRAM;
    }
    else if(width_frame + height_frame > 1250)
    {
    	dimension = 1;
    	letterMatrix = loadMatrix("../data/font_database960.txt");
    	LEN_MATRIX = D1_LETTER_MATRIX;
    	LEN_FRAME = D1_FRAME_SIZE;
    	bram_row = D1_BRAM;
    }
    else
    {
    	dimension = 0;
    	letterMatrix = loadMatrix("../data/font_database640.txt");
    	LEN_MATRIX = D0_LETTER_MATRIX;
    	LEN_FRAME = D0_FRAME_SIZE;
    	bram_row = D0_BRAM;
    }

    vector<sc_dt::sc_uint<8>> letterData;
    vector<sc_dt::sc_uint<1>> transformedArray = transformMatrixArray(letterMatrix, letterData);
    
    letterData.push_back(dimension);

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
//***********************************************************************************
	ddr.clear();
	ddr = letterData;

	cout << "cpu: offset, pre slanja mem, letterData: " << offset << endl;
	cout << "cpu: time, pre slanja mem, letterData: " << sc_time_stamp() << endl;

	buf=(unsigned char*)&ddr[0];
	//offset = SC_ZERO_TIME;
	pl.set_address(0);
	pl.set_data_length(SIZE_LETTER_DATA);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i1->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);

	#ifdef QUANTUM
	qk.set_and_sync(offset);
	#endif

	cout << "cpu: offset, posle slanja mem: " << offset << endl;
	cout << "cpu: time, posle slanja mem: " << sc_time_stamp() << endl;		
	
	ddr.clear();

	//offset = SC_ZERO_TIME;

	//slanje iz mem preko dma u ip podatke
	pl.set_address(0x81000000);
	pl.set_data_length(SIZE_LETTER_DATA);
	pl.set_command(TLM_WRITE_COMMAND);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	cout << "cpu: offset, posle slanja iz mem u ip: " << offset << endl;
	cout << "cpu: time, posle slanja iz mem u ip: " << sc_time_stamp() << endl;

	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	//offset = SC_ZERO_TIME;

	cout << "cpu: offset, posle slanja iz mem u ip: " << offset << endl;
	cout << "cpu: time, posle slanja iz mem u ip: " << sc_time_stamp() << endl;
	
	command = 1;
	buf = (unsigned char*)&command;

	pl.set_address(0x80000001);
	pl.set_data_length(1);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	//offset = SC_ZERO_TIME;

	cout << "cpu: offset, posle skladistenja u ip: " << offset << endl;
	cout << "cpu: time, posle skladistenja u ip: " << sc_time_stamp() << endl;

//**************************************************************************************

	ddr.clear();
	for (const auto& value : transformedArray) {
    	sc_dt::sc_uint<8> convertedValue = static_cast<sc_dt::sc_uint<8>>(value); // Konverzija vrednosti iz sc_uint<1> u sc_uint<8>
    	ddr.push_back(convertedValue); // Dodavanje konvertovane vrednosti u vektor 'vecUint8'
	}

	cout << "cpu: offset, pre slanja mem, letterData: " << offset << endl;
	cout << "cpu: time, pre slanja mem, letterData: " << sc_time_stamp() << endl;

	buf=(unsigned char*)&ddr[0];
	//offset = SC_ZERO_TIME;
	pl.set_address(0);
	pl.set_data_length(LEN_MATRIX);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i1->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);

	#ifdef QUANTUM
	qk.set_and_sync(offset);
	#endif

	cout << "cpu: offset, posle slanja mem: " << offset << endl;
	cout << "cpu: time, posle slanja mem: " << sc_time_stamp() << endl;		
	
	ddr.clear();

	//offset = SC_ZERO_TIME;

	//slanje iz mem preko dma u ip podatke
	pl.set_address(0x81000000);
	pl.set_data_length(LEN_MATRIX);
	pl.set_command(TLM_WRITE_COMMAND);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	cout << "cpu: offset, posle slanja iz mem u ip: " << offset << endl;
	cout << "cpu: time, posle slanja iz mem u ip: " << sc_time_stamp() << endl;

	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	//offset = SC_ZERO_TIME;

	cout << "cpu: offset, posle slanja iz mem u ip: " << offset << endl;
	cout << "cpu: time, posle slanja iz mem u ip: " << sc_time_stamp() << endl;
	
	command = 2;
	buf = (unsigned char*)&command;

	pl.set_address(0x80000001);
	pl.set_data_length(1);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	//offset = SC_ZERO_TIME;

	cout << "cpu: offset, posle skladistenja u ip: " << offset << endl;
	cout << "cpu: time, posle skladistenja u ip: " << sc_time_stamp() << endl;

//*********************************************************************************************
	// od ovog mesta se salje slika

	Mat slika;
	bool send_text = true;
	while(cap.read(slika))
	{

		// Pokušaj dohvatiti trenutno vrijeme u video datoteci     
        int current_time = cap.get(CAP_PROP_POS_MSEC);
		
		//convert Mat to vector<sc_uint<8>> and added to ddr
		matToVector(slika);
		
		//Transfer image to Memory
		buf=(unsigned char*)&ddr[0];

		//offset = SC_ZERO_TIME;
		pl.set_address(0);
		pl.set_data_length(LEN_FRAME);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.set_and_sync(offset);
		#endif

		//cout << "cpu: offset, posle slanja mem: " << offset << endl;
		//cout << "cpu: time, posle slanja mem: " << sc_time_stamp() << endl;		
		
		ddr.clear();

		//offset = SC_ZERO_TIME;

	//**********************************************************************
		//slanje stringa u mem
		if (current_subtitle_index < max_index && current_time >= startTime[current_subtitle_index]) {
			if(send_text){
				cout << "SLANJEEE STRINGA " << endl;
				send_text = false;

				string st1 = text1[current_subtitle_index];
				string st2 = text2[current_subtitle_index];

			  	stringToVector(st1,ddr);
			  	ddr.push_back(255);
			  	stringToVector(st2,ddr);
			  	
				unsigned int strLen = ddr.size();
			  	
			  	//Transfer string to Memory
				buf=(unsigned char*)&ddr[0];

				//offset = SC_ZERO_TIME;
				pl.set_address(LEN_FRAME);
				pl.set_data_length(strLen);
				pl.set_command(TLM_WRITE_COMMAND);
				pl.set_data_ptr(buf);
				s_cp_i1->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);

				#ifdef QUANTUM
				qk.set_and_sync(offset);
				#endif

				//cout << "cpu: offset, posle slanja mem: " << offset << endl;
				//cout << "cpu: time, posle slanja mem: " << sc_time_stamp() << endl;		
				
				ddr.clear();

				//slanje iz mem preko dma u ip podatke
				pl.set_address(0x81000000 + LEN_FRAME);
				pl.set_data_length(strLen);
				pl.set_command(TLM_WRITE_COMMAND);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);
				qk.set_and_sync(offset);

				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif

				//offset = SC_ZERO_TIME;

				//cout << "cpu: offset, posle slanja iz mem u ip: " << offset << endl;
				//cout << "cpu: time, posle slanja iz mem u ip: " << sc_time_stamp() << endl;

				buf = (unsigned char*)&strLen;

				pl.set_address(0x80000010);
				pl.set_data_length(1);
				pl.set_command(TLM_WRITE_COMMAND);
				pl.set_data_ptr(buf);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);
				qk.set_and_sync(offset);

				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif


				command = 3;
				buf = (unsigned char*)&command;

				pl.set_address(0x80000001);
				pl.set_data_length(1);
				pl.set_command(TLM_WRITE_COMMAND);
				pl.set_data_ptr(buf);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);
				qk.set_and_sync(offset);

				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif

				//offset = SC_ZERO_TIME;

				//cout << "cpu: offset, posle skladistenja u ip: " << offset << endl;	
				//cout << "cpu: time, posle skladistenja u ip: " << sc_time_stamp() << endl;
			}


			// ************************************************************
			cout << "\tOBRADA SLIKE " << endl;
			//slanje iz mem preko dma u ip podatke

			int adress_row;
			int tmp = 1;
			do
			{	
				adress_row = height_frame - bram_row * tmp;
				if(adress_row < 0)
					adress_row = 0;

				pl.set_address(0x81000000 + adress_row * width_frame * 3);
				pl.set_data_length(bram_row * width_frame * 3);
				pl.set_command(TLM_WRITE_COMMAND);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);
				qk.set_and_sync(offset);

				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif

				int pomocna = height_frame - adress_row;
				buf = (unsigned char*)&pomocna;

				pl.set_address(0x80000100);
				pl.set_data_length(1);
				pl.set_command(TLM_WRITE_COMMAND);
				pl.set_data_ptr(buf);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);
				qk.set_and_sync(offset);

				command = 4;
				buf = (unsigned char*)&command;

				pl.set_address(0x80000001);
				pl.set_data_length(1);
				pl.set_command(TLM_WRITE_COMMAND);
				pl.set_data_ptr(buf);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);
				qk.set_and_sync(offset);
				//cout << "cpu: primio "<<sc_time_stamp()<<endl;

				do{
					#ifdef QUANTUM
					qk.inc(sc_time(CLK_PERIOD, SC_NS));
					offset = qk.get_local_time();
					qk.set_and_sync(offset);
					#endif
					tmp_sig0=sig0.read();
				} while(tmp_sig0 == sc_dt::SC_LOGIC_0);

				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif

				pl.set_address(0x81000000 + adress_row * width_frame * 3);
				pl.set_data_length(bram_row * width_frame * 3);
				pl.set_command(TLM_READ_COMMAND);
				s_cp_i0->b_transport(pl, offset);
				assert(pl.get_response_status() == TLM_OK_RESPONSE);	
				qk.set_and_sync(offset);

				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif
				cout <<sc_time_stamp()<<" "<<"Poslao sliku iz " << tmp << " delova"<< endl;


				tmp_sig1=sig1.read();
				tmp++;

			}while(tmp_sig1 == sc_dt::SC_LOGIC_0 || adress_row == 0);
		
			if(current_time >= endTime[current_subtitle_index]){
            	current_subtitle_index += 1;
            	send_text = true;
            }
		}
		//cout << "cpu: time, pre citanja iz mem: " << sc_time_stamp() << endl;

		pl.set_address(0);
		pl.set_data_length(LEN_FRAME);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		//cout << "cpu: time, posle citanja iz mem: " << sc_time_stamp() << endl;
		
		vector<sc_dt::sc_uint<8>> frame_uint8;
		buf = pl.get_data_ptr();

		for(unsigned int i=0; i<pl.get_data_length(); i++)
		{
			frame_uint8.push_back(((sc_dt::sc_uint<8>*)buf)[i]);
		}

		cv:Mat frame_finish;
		frame_finish = vectorToMat(frame_uint8, slika.cols, slika.rows);

		char c = cv::waitKey(delay);
		if (c == 27)  // ESC tipka
        	pom = false;
        else if ( c == ' ' && pause)
			pause = false;
		else if ( c == ' ' && !pause)
			pause = true;

		if(pom){
        	// Show video frame
        	cv::imshow("Video", frame_finish);    		
    		
    		while (pause){    			
    			c = cv::waitKey(delay);
    			if (c == ' ')
    				pause = false;
    			else if (c == 27){
    				pause = false;
    				pom = false;
    			}	
    		}
		}
		else
			break;

		frame_count += 1;
	}

	destroyAllWindows();

	cap.release();
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
    }
    else {
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

vector<vector<vector<sc_dt::sc_uint<1>>>> Cpu::loadMatrix(const string& fileName) {
    ifstream file(fileName);
    vector<vector<vector<sc_dt::sc_uint<1>>>> matrixArray;
    vector<vector<sc_dt::sc_uint<1>>> currentMatrix;

    if (file) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) {
                if (!currentMatrix.empty()) {
                    matrixArray.push_back(currentMatrix);
                    currentMatrix.clear();
                }
            }
            else {
                vector<sc_dt::sc_uint<1>> row;
                istringstream lineStream(line);
                int number;
                while (lineStream >> number) 
                    row.push_back(static_cast<sc_dt::sc_uint<1>>(number));

                currentMatrix.push_back(row);
            }
        }

        // Add the last matrix if not added due to end of file
        if (!currentMatrix.empty()) {
            matrixArray.push_back(currentMatrix);
        }

        file.close();
    }
    else {
    	cerr << "Failed to open file: " << fileName << endl;
    }

    return matrixArray;
}

vector<sc_dt::sc_uint<1>> Cpu::transformMatrixArray(const vector<vector<vector<sc_dt::sc_uint<1>>>>& matrixArray, vector<sc_dt::sc_uint<8>>& letterData) {
    vector<sc_dt::sc_uint<1>> transformedArray;

    int maxHeight = 0; // Najveća visina slova

    for (const auto& matrix : matrixArray) {
        //int startPos = transformedArray.size(); // Početna pozicija trenutne matrice u transformisanom nizu

        // Dodajemo elemente matrice slova
        for (const auto& row : matrix) {
            transformedArray.insert(transformedArray.end(), row.begin(), row.end());
        }

        sc_dt::sc_uint<8> width = matrix[0].size(); // Širina matrice
        sc_dt::sc_uint<8> height = matrix.size(); // Visina matrice

       // letterData.push_back(startPos); // Dodajemo početnu poziciju
        letterData.push_back(width); // Dodajemo širinu
        letterData.push_back(height); // Dodajemo visinu

        // Ažuriramo najveću visinu slova
        if (height > maxHeight) {
            maxHeight = height;
        }
    }

    letterData.push_back(maxHeight); // Dodajemo najveću visinu slova

    return transformedArray;
}

void Cpu::stringToVector(const string& str, vector<sc_dt::sc_uint<8>>& asciiVec){
	for (int i = 0; i < str.length(); i++)
	{
		char c = str[i];   	
        unsigned char uc = static_cast<unsigned char>(c);
		int ascii = static_cast<int>(uc);

		if( ascii > 195)
		    continue;

		if( ascii == 161)
	      ascii = 96;
	    else if( ascii == 160)
	      ascii = 97;
	    else if ( ascii == 145)
	      ascii = 98;
	    else if( ascii == 144)
	      ascii = 99;
	    else if ( ascii == 141)
	      ascii = 100; 
	    else if( ascii == 140)
	      ascii = 101;
	    else if ( ascii == 135)
	      ascii = 102;
	    else if( ascii == 134)
	      ascii = 103;
	    else if ( ascii == 190)
	      ascii = 104; 
	    else if( ascii == 189)
	      ascii = 105;
	    else 
	      ascii = ascii - 32;

		asciiVec.push_back(static_cast<sc_dt::sc_uint<8>>(ascii));
	}
}

#endif //CPU_C
