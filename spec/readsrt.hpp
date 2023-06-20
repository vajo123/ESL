#ifndef READSRT_HPP_INCLUDED
#define READSRT_HPP_INCLUDED

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
 
using namespace std;

class ReadSrt{
	private:
		vector<int> startTimes;
		vector<int> endTimes;
		vector<string> text1;
		vector<string> text2;
	public:
		ReadSrt(const string&);
		
		vector<int> getStartTimes() const;
		
		vector<int> getEndTimes() const;
		
		vector<string> getText1() const;
		
		vector<string> getText2() const;
		
				
};

#endif // READSRT_HPP_INCLUDED
