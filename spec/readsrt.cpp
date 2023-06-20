#include "readsrt.hpp"

ReadSrt::ReadSrt(const string& srtPath){

	
	ifstream srtFile(srtPath); // otvaramo srt fajl

	if (!srtFile.is_open()) // ako se fajl nije uspio otvoriti
        {
        	cerr << "Unable to open SRT file!" << endl;
        }
        
        else{
        
		string line;
		
		while (getline(srtFile, line)) // čitamo fajl liniju po liniju
		{
			istringstream ss(line);
			int index;

			if (ss >> index) // ako je pročitan broj (redni broj titla)
			{
			    string timeString;
			    getline(srtFile, timeString); // čitamo sljedeću liniju koja sadrži vremenske oznake

			    // izdvajamo početno i krajnje vrijeme pojavljivanja titla iz stringa vremenskih oznaka
			    string startTimeString = timeString.substr(0, 12);
			    string endTimeString = timeString.substr(17, 12);

			    // konvertujemo početno i krajnje vrijeme u int format
			    int startHours = stoi(startTimeString.substr(0, 2));
			    int startMinutes = stoi(startTimeString.substr(3, 2));
			    int startSeconds = stoi(startTimeString.substr(6, 2));
			    int startMilliseconds = stoi(startTimeString.substr(9, 3));
			    int startTime = (startHours * 3600 + startMinutes * 60 + startSeconds) * 1000 + startMilliseconds;
			    
			    int endHours = stoi(endTimeString.substr(0, 2));
			    int endMinutes = stoi(endTimeString.substr(3, 2));
			    int endSeconds = stoi(endTimeString.substr(6, 2));
			    int endMilliseconds = stoi(endTimeString.substr(9, 3));
			    int endTime = (endHours * 3600 + endMinutes * 60 + endSeconds) * 1000 + endMilliseconds;
			    
			    string subtitleText1;
		    	    string subtitleText2;
		            getline(srtFile, subtitleText1); // čitamo sljedeću liniju koja sadrži tekst titla
	 	            subtitleText1 = subtitleText1.substr(0, subtitleText1.length() - 1);                	
		            getline(srtFile, subtitleText2);
		            subtitleText2 = subtitleText2.substr(0, subtitleText2.length() - 1);
				
			    startTimes.push_back(startTime);
			    endTimes.push_back(endTime);
			    text1.push_back(subtitleText1);
			    text2.push_back(subtitleText2);  
			}
		}	  			
	}	
	srtFile.close(); // zatvaramo srt fajl			
}

vector<int> ReadSrt::getStartTimes() const{
	return startTimes;
}

vector<int> ReadSrt::getEndTimes() const{
	return endTimes;
}

vector<string> ReadSrt::getText1() const{
	return text1;
}

vector<string> ReadSrt::getText2() const{
	return text2;
}

