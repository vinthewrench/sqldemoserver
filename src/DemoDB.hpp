//
//  DemoDB.hpp
//  demoserver
//
//  Created by Vincent Moscaritolo on 2/18/22.
//

#ifndef DemoDB_hpp
#define DemoDB_hpp

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <mutex>
#include <random>
#include <set>
#include <vector>
#include <tuple>
#include <functional>
#include <map>
#include <string>

#include <sqlite3.h>

using namespace std;


class DemoDB {

public:

	static DemoDB *shared() {
		if(!sharedInstance){
			sharedInstance = new DemoDB;
		}
		return sharedInstance;
	}

	DemoDB();
  ~DemoDB();
	
	bool initDatabase(string filePath = "");

 
	// MARK: - properties
	bool saveProperties() ;
	bool restoreProperties();
 
	bool setProperty(string key, string value);
	bool getProperty(string key, string *value);
	bool setPropertyIfNone(string key, string value);

	bool getUint16Property(string key, uint16_t * value);
	bool getFloatProperty(string key, float * valOut);
	bool getBoolProperty(string key, bool * valOut);

	bool removeProperty(string key);
	map<string ,string> getProperties();
	
private:
 
 
	bool 		_isSetup;

	static DemoDB *sharedInstance;
 
	sqlite3 	*_sdb;
	mutable 	std::mutex 			_mutex;
 	map<string,string> 			_properties;
 
	 
};

#endif /* DemoDB_hpp */
