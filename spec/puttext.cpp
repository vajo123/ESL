#include "puttext.hpp"

int readMaxHeight(int d)
{
	if(d == 4)
    		return 39;
    	else if(d == 3)
    		return 35;
    	else if(d == 2)
   	 	return 31;
    	else if(d == 1)
    		return 27;
    	else
    		return 23;
}


int readAscii(int n, int d)
{	
	int ascii;
        if( n == 161)
          ascii = 96;
        else if( n == 160)
          ascii = 97;
        else if ( n == 145)
          ascii = 98;
        else if( n == 144)
          ascii = 99;
        else if ( n == 141)
          ascii = 100; 
        else if( n == 140)
          ascii = 101;
        else if ( n == 135)
          ascii = 102;
        else if( n == 134)
          ascii = 103;
        else if ( n == 190)
          ascii = 104; 
        else if( n == 189)
          ascii = 105;
        else 
          ascii = n - 32;
          
        return ascii + (106 * d);
}


sizeWord getSizeWord(const string& text, const vector<vector<vector<int>>>& nizMatrica, int dim)
{
	int sirina = 0;
    	int maxVisina = 0;
    	sizeWord object;
    	
    	for (int i = 0; i < text.length(); i++){
        	char c = text[i];
		unsigned char uc = static_cast<unsigned char>(c);
		int ascii = static_cast<int>(uc);
		
		if( ascii > 195){
		    continue;
		}
		ascii = readAscii(ascii, dim);
		
		if (ascii >= nizMatrica.size()) {
		    continue;
		    cout << "Greška: Nedostajuća matrica za slovo " << c << endl;
		}
		const vector<vector<int>>& slovo = nizMatrica[ascii];
		int visina = slovo.size();
		int slovoSirina = slovo[0].size();

		sirina += slovoSirina + dim + 1;
		maxVisina = max(maxVisina, visina);
	}
	
	object.width = sirina;
	object.maxHeigh = maxVisina;
	
	return object;	
}


vector<string> splitText(const string& text1, const string& text2, int sirinaSlike, const vector<vector<vector<int>>>& nizMatrica, int dim) {

    const vector<vector<int>>& slovo = nizMatrica[0 + (dim * 106)];
    int space = slovo[0].size();
    int redLen1 = 0, redLen2 = 0;
    
    vector<string> rezultat;
    stringstream ss1(text1);
    stringstream ss2(text2);
    string rijec1, rijec2;
    string trenutniRed1, trenutniRed2;
    sizeWord lenR1, lenR2;
    

    while (getline(ss1, rijec1, ' ')) {
        lenR1 = getSizeWord(rijec1, nizMatrica, dim);

        if (trenutniRed1.empty()) {
            trenutniRed1 = rijec1;
            redLen1 = lenR1.width;
        } else {
            if (redLen1 + lenR1.width + space + dim + 1 > sirinaSlike) {
                rezultat.push_back(trenutniRed1);
                trenutniRed1 = rijec1;
                redLen1 = lenR1.width;
            } else {
                trenutniRed1 += " " + rijec1;
                redLen1 += space + dim + 1 + lenR1.width;
            }
        }
    }

    if (!trenutniRed1.empty()) {
        rezultat.push_back(trenutniRed1);
    }

    while (getline(ss2, rijec2, ' ')) {
        lenR2 = getSizeWord(rijec2, nizMatrica, dim);

        if (trenutniRed2.empty()) {
            trenutniRed2 = rijec2;
            redLen2 = lenR2.width;
        } else {
            if (redLen2 + lenR2.width + space + dim + 1 > sirinaSlike) {
                rezultat.push_back(trenutniRed2);
                trenutniRed2 = rijec2;
                redLen2 = lenR2.width;
            } else {
                trenutniRed2 += " " + rijec2;
                redLen2 += space + dim + 1 + lenR2.width;
            }
        }
    }

    if (!trenutniRed2.empty()) {
        rezultat.push_back(trenutniRed2);
    }

    return rezultat;
}


void writeText(Mat& slika, const string& text1, const string& text2,const vector<vector<vector<int>>>& nizMatrica, int dim) {
    int y = readMaxHeight(dim);
    int razmak = dim + 1;    
    int currX;
    int currY;
    
    vector<string> redovi;
    redovi = splitText(text1, text2, slika.cols, nizMatrica, dim); 
    int brojredova = redovi.size();

    for(int z = 0; z < brojredova; z++)
    {
    	sizeWord velicinaReda = getSizeWord(redovi[brojredova - 1 - z], nizMatrica, dim);        
    	currX = (slika.cols - velicinaReda.width)/2;
    	currY = y/3 + z * (1.3 * y);
    	
    	if(currY > slika.rows)
    	{
    		cout <<"Presli smo opseg slike " << endl; 
    		return;
    	}
    
    	for (int k = 0; k < redovi[brojredova - 1 - z].length(); k++){
        	char c = redovi[brojredova - 1 - z][k];   	
        	unsigned char uc = static_cast<unsigned char>(c);
		int ascii = static_cast<int>(uc);
	
		if( ascii > 195){
		    continue;
		}
		ascii = readAscii(ascii, dim);

        	int flag = 0;
        
        	if(ascii == 71 + (106 * dim) || ascii == 74 + (106 * dim) || ascii == 80 + (106 * dim) || ascii == 81 + (106 * dim) || ascii == 89 + (106 * dim))
        		flag = 1;
                
       
        	if (ascii >= nizMatrica.size()) {
        	    cout << "Greška: Nedostajuća matrica za slovo " << c << endl;
        	    ascii = 31;
        	}
        
        	const vector<vector<int>>& slovo = nizMatrica[ascii];
        	int visina = slovo.size();
        	int slovoSirina = slovo[0].size();
			
        	for (int i = 0; i < visina; i++) {
        	    for (int j = 0; j < slovoSirina; j++) {
        	        if (slovo[visina - 1 - i][j] == 1) {
        	            int idx = ((slika.rows - 1 - currY + (flag * visina / 4) - i) * slika.cols + (currX + j)) * 3;
        	            slika.data[idx] = 255;    // Plava komponenta piksela
        	            slika.data[idx + 1] = 255;  // Zelena komponenta piksela
        	            slika.data[idx + 2] = 255;  // Crvena komponenta piksela
        	        }
        	    }
        	}
	
        	currX += slovoSirina + razmak;
    	}	
    }	
}


vector<vector<vector<int>>> loadMatrix(const string& nazivDatoteke) {
    ifstream datoteka(nazivDatoteke);
    vector<vector<vector<int>>> nizMatrica;
    vector<vector<int>> trenutnaMatrica;

    if (datoteka) {
        string linija;
        while (getline(datoteka, linija)) {
            if (linija.empty()) {
                if (!trenutnaMatrica.empty()) {
                    nizMatrica.push_back(trenutnaMatrica);
                    trenutnaMatrica.clear();
                }
            } else {
                vector<int> red;
                istringstream linijaStream(linija);
                int broj;
                while (linijaStream >> broj) {
                    red.push_back(broj);
                }
                trenutnaMatrica.push_back(red);
            }
        }

        // Dodajemo poslednju matricu ako nije dodata zbog kraja datoteke
        if (!trenutnaMatrica.empty()) {
            nizMatrica.push_back(trenutnaMatrica);
        }

        datoteka.close();
    } else {
        cerr << "Neuspjelo otvaranje datoteke: " << nazivDatoteke << endl;
    }

    return nizMatrica;
}
