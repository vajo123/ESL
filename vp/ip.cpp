#ifndef IP_C
#define IP_C
#include "ip.hpp"

Ip::Ip(sc_module_name name):sc_module(name)																									
{
	SC_THREAD(proc);
	s_ip_t0.register_b_transport(this, &Ip::b_transport0);
	s_ip_t1.register_b_transport(this, &Ip::b_transport1);

	command = 0;
	len_text = 0;
	tmp_sig = sc_dt::SC_LOGIC_0;

	base.reserve(1333*2000*3);
	letterData.reserve(SIZE_LETTER_DATA);
	letterMatrix.reserve(D4_LETTER_MATRIX);
	cout << "IP created" << endl;
}

void Ip::b_transport0(pl_t& pl, sc_time& offset)
{
	tlm_command cmd    = pl.get_command();
	sc_dt::uint64 adr  = pl.get_address();
	const unsigned char *buf = pl.get_data_ptr();
	unsigned int len  = pl.get_data_length();
	len_text = len;

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
                          base[adr+i]=((sc_dt::sc_uint<8>*)buf)[adr+i];
                  }
                  pl.set_response_status(TLM_OK_RESPONSE);
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

	sc_dt::sc_uint<8> fifo_read;
	sc_time offset=SC_ZERO_TIME;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif

	while(1)
	{
		while(command == 0)
		{
			//cout << "ip: cekam komandu " << sc_time_stamp() <<endl;
			#ifdef QUANTUM
	        qk.inc(sc_time(10, SC_NS));
	        offset = qk.get_local_time();
	        qk.set_and_sync(offset);
	        #else
	        offset += sc_time(10, SC_NS);
	        #endif

		}

		switch(command)
		{
			case 1:
				
				for (int i = 0; i < SIZE_LETTER_DATA; i++) {
	        letterData.push_back(base[i]);
			  }
    			
    		base.clear();
				
				#ifdef QUANTUM
        qk.inc(sc_time(10, SC_NS));
        offset = qk.get_local_time();
        qk.set_and_sync(offset);
        #else
        offset += sc_time(10, SC_NS);
        #endif

			break;

			case 2:
				unsigned int LEN_MATRIX;
				if(letterData[213] == 0)
					LEN_MATRIX = D0_LETTER_MATRIX;
				else if(letterData[213] == 1)
					LEN_MATRIX = D1_LETTER_MATRIX;
				else if(letterData[213] == 2)
					LEN_MATRIX = D2_LETTER_MATRIX;
				else if(letterData[213] == 3)
					LEN_MATRIX = D3_LETTER_MATRIX;
				else
					LEN_MATRIX = D4_LETTER_MATRIX;
					
				if(LEN_MATRIX != D4_LETTER_MATRIX)
					letterMatrix.resize(LEN_MATRIX,0);

				for (int i = 0; i < LEN_MATRIX; i++)
				{
					letterMatrix[i] = (sc_dt::sc_uint<1>)base[i];
				}

				base.clear();

				#ifdef QUANTUM
        qk.inc(sc_time(10, SC_NS));
        offset = qk.get_local_time();
        qk.set_and_sync(offset);
        #else
        offset += sc_time(10, SC_NS);
        #endif

			break;

			case 3: 
				
				text1.clear();
				for (int i = 0; i < len_text; i++) {
		    	if (base[i] == 255) 
		        break;
			    text1.push_back(base[i]);
				}
				text2.clear();
				for (int i = text1.size() + 1; i < len_text; i++) {
					text2.push_back(base[i]);			
				}

    		base.clear(); 
				
				#ifdef QUANTUM
        qk.inc(sc_time(10, SC_NS));
        offset = qk.get_local_time();
        qk.set_and_sync(offset);
        #else
        offset += sc_time(10, SC_NS);
        #endif

			break;

			case 4:

				tmp_sig = sc_dt::SC_LOGIC_0;
    		out_port0->write(tmp_sig);
    		int frameWidth;
    		int frameHeight;

    		array<int, 106> possition;

    		if(letterData[213] == 0){
					possition = D0_possition;
					frameWidth = D0_WIDTH;
					frameHeight = D0_HEIGHT;
    		}
				else if(letterData[213] == 1){
					possition = D1_possition;
					frameWidth = D1_WIDTH;
					frameHeight = D1_HEIGHT;
				}
				else if(letterData[213] == 2){
					possition = D2_possition;
					frameWidth = D2_WIDTH;
					frameHeight = D2_HEIGHT;
				}
				else if(letterData[213] == 3){
					possition = D3_possition;
					frameWidth = D3_WIDTH;
					frameHeight = D3_HEIGHT;
				}
				else{
					possition = D4_possition;
					frameWidth = D4_WIDTH;
					frameHeight = D4_HEIGHT;
				}

  			int y = letterData[212];
				int spacing = letterData[213] + 1;
				int currX;
				int currY;

				vector<vector<sc_dt::sc_uint<8>>> rows;
				rows = splitText(text1, text2, frameWidth);
				int numRow = rows.size();
				
				for (int z = 0; z < numRow; z++)
				{
					int pom = getStringWidth(rows[numRow - 1 - z], spacing);		
					currX = (frameWidth - pom)/2;
					currY = y/3 + z * (1.3 * y);
					
					for (int k = 0; k < rows[numRow - 1 - z].size(); k++)
					{	
						char c = rows[numRow - 1 - z][k];   	
	        	unsigned char uc = static_cast<unsigned char>(c);
						int ascii = static_cast<int>(uc);
						int flag = 0;
						int startPos;
						sc_dt::sc_uint<8> letterWidth;
						sc_dt::sc_uint<8> letterHeight;  


			    	if(ascii == 71 || ascii == 74 || ascii == 80 || ascii == 81 || ascii == 89)
	        		flag = 1;

	        	if (ascii >= 106) {
	        	    cout << "Greška: Nedostajuća matrica za slovo " << c << endl;
	        	    ascii = 31;
	        	}

	        	startPos = possition[ascii];
	        	letterWidth = letterData[ascii*2];
	        	letterHeight = letterData[ascii*2+1];

	 					for (int i = 0; i < letterHeight; i++) {
	    				for (int j = 0; j < letterWidth; j++) {
	    					int rowIndex = letterHeight - 1 - i;
	    					if(letterMatrix[i * letterWidth + j + startPos] == 1){
	    						int idx = ((frameHeight - 1 - currY + (flag * letterHeight / 4) - rowIndex) * frameWidth + (currX + j)) * 3;
			            base[idx] = 255;    // Plava komponenta piksela
			            base[idx + 1] = 255;  // Zelena komponenta piksela
			            base[idx + 2] = 255;  // Crvena komponenta piksela
	    					}
	    				}
	    			}
	    			currX += letterWidth + spacing;
					}
				}

				#ifdef QUANTUM
				qk.inc(sc_time(20, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(20, SC_NS);
				#endif
				tmp_sig = sc_dt::SC_LOGIC_1;
				out_port0->write(tmp_sig);
				
			break;
		}

		command = 0;
	}	

}

int Ip::getStringWidth(vector<sc_dt::sc_uint<8>> st, int space)
{
	int width = 0;
  
	for (int i = 0; i < st.size(); i++)
	{
		int ascii = static_cast<int>(st[i]);

		width += letterData[ascii*2] + space;		
	}
	return width;
}


vector<vector<sc_dt::sc_uint<8>>> Ip::splitText(const vector<sc_dt::sc_uint<8>>& text1, const vector<sc_dt::sc_uint<8>>& text2, int photoWidth)
{
	int space = letterData[0];
	int rowLen1 = 0, rowLen2 = 0;
	int razmak = letterData[213] + 1;

	vector<vector<sc_dt::sc_uint<8>>> result;
  vector<sc_dt::sc_uint<8>> currentRow1, currentRow2;
  int lenR1, lenR2;
  int i = 0;

  while (i < text1.size()) {
    int j = i;
    while (j < text1.size() && text1[j] != 0) {
        j++;
    }

    vector<sc_dt::sc_uint<8>> rijec1(text1.begin() + i, text1.begin() + j);
    lenR1 = getStringWidth(rijec1, razmak);
		
    if (currentRow1.empty()) {
        currentRow1 = rijec1;
        rowLen1 = lenR1 ;
    } else {
        if (rowLen1 + lenR1 + space + razmak > photoWidth) {
            result.push_back(currentRow1);
            currentRow1 = rijec1;
            rowLen1 = lenR1;
        } else {
            currentRow1.insert(currentRow1.end(), rijec1.begin(), rijec1.end());
            rowLen1 += lenR1;
        }
    }

    if (j < text1.size()) {
        currentRow1.push_back(text1[j]);
        rowLen1 += 2*razmak + space;
    }
    i = j + 1;
  }

  if (!currentRow1.empty()) {
      result.push_back(currentRow1);
  }

  int k = 0;
  while (k < text2.size()) {
    int j = k;
    while (j < text2.size() && text2[j] != 0) {
        j++;
    }

    vector<sc_dt::sc_uint<8>> rijec2(text2.begin() + k, text2.begin() + j);
    lenR2 = getStringWidth(rijec2, razmak);

    if (currentRow2.empty()) {
        currentRow2 = rijec2;
        rowLen2 = lenR2 ;
    } else {
        if (rowLen2 + lenR2 + space + razmak > photoWidth) {
            result.push_back(currentRow2);
            currentRow2 = rijec2;
            rowLen2 = lenR2;
        } else {
            currentRow2.insert(currentRow2.end(), rijec2.begin(), rijec2.end());
            rowLen2 += lenR2;
        }
    }

    if (j < text2.size()) {
        currentRow2.push_back(text2[j]);
        rowLen2 += 2*razmak + space;
    }
    k = j + 1;
  }

  if (!currentRow2.empty()) {
      result.push_back(currentRow2);
  }

  return result;

}


#endif // IP_C