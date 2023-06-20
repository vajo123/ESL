#include "readsrt.hpp"
#include "puttext.hpp"
#include <SFML/Audio.hpp>

int main(int argc, char** argv) {

     if(argc != 3){
     	cout << "Navedite putanju do videa i titla" << endl;
     	return -1;
     }
     
    // Učitaj video datoteku
    VideoCapture cap(argv[1]);

    // Provjeri da li se video uspješno otvorio
    if (!cap.isOpened()) {
        cerr << "Error opening video file" << endl;
        return -1;
    }

    ReadSrt Srb(argv[2]);
    
    vector<int> startTime = Srb.getStartTimes();
    vector<int> endTime = Srb.getEndTimes();
    vector<string> text1 = Srb.getText1();
    vector<string> text2 = Srb.getText2();
    
    
    vector<vector<vector<int>>> nizMatrica = loadMatrix("../data/font_database.txt");
    
    int max_index = startTime.size();
    
    // Get the video dimensions
    int width = int(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = int(cap.get(CAP_PROP_FRAME_HEIGHT));
    int dimension;
    
    if(width + height > 2750)
    	dimension = 4;
    else if(width + height > 2250)
    	dimension = 3;
    else if(width + height > 1750)
    	dimension = 2;
    else if(width + height > 1250)
    	dimension = 1;
    else
    	dimension = 0;
    
    
    // Get the frame rate
    int fps = cap.get(CAP_PROP_FPS);
    
    int delay = 1000 / fps;
    
    // Inicijaliziraj varijable za praćenje podnaslova
    int current_subtitle_index = 0;
     
    int frame_count = 0;
    
    bool pom = true;
    
    bool pause = false;
    
    string video_filename = argv[1];
    string audio_filename = "audio.wav";
    string cmd = "ffmpeg -i " + video_filename + " -vn -acodec pcm_s16le -ar 44100 -ac 2 " + audio_filename;
    system(cmd.c_str());
    
    // Uključi reprodukciju zvuka
    sf::Sound sound;
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("audio.wav")) {
    	std::cout << "Error loading audio file!" << std::endl;
    	return -1;
    }
    
    sound.setBuffer(buffer);
   
   // Petlja koja prolazi kroz sve okvire u video datoteci
    Mat frame;
    while (cap.read(frame)) {
        // Pokušaj dohvatiti trenutno vrijeme u video datoteci     
        int current_time = cap.get(CAP_PROP_POS_MSEC);
   
    	sound.setPlayingOffset(sf::milliseconds(current_time));	
    	
    	if (current_subtitle_index < max_index &&
            current_time >= startTime[current_subtitle_index]) {
       		
       		writeText(frame, text1[current_subtitle_index], text2[current_subtitle_index], nizMatrica, dimension);
       		
       		if(current_time >= endTime[current_subtitle_index]){
                	current_subtitle_index += 1;
                }
        
        }
        
                
        char c = waitKey(delay);	
        if (c == 27)  // ESC tipka
        	pom = false;
        else if ( c == ' ' && pause)
		pause = false;
	else if ( c == ' ' && !pause)
		pause = true;
	
               
        if(pom){
        	// Show video frame
        	imshow("Video", frame);
        	
		// Pokreni reprodukciju zvuka
    		sound.play();    		
    		
    		while (pause){
    			sound.stop();    			
    			c = waitKey(delay);
    			if (c == ' ')
    				pause = false;
    			else if (c == 27){
    				pause = false;
    				pom = false;
    			}
    			
    		}
        	
        
	}
	else{
		break;
        }
        // Write the frame to the new video file
        frame_count += 1;

    }
    
    destroyAllWindows();
    sound.stop();

    // Oslobodi resurse
    cap.release();
    

    // Brisanje privremenog audio fajla
    cmd = "rm audio.wav ";
    system(cmd.c_str());
    
    
    return 0;
}
    
	
       			
    	
