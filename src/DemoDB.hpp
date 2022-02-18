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

	bool 		getValBool() {return _val_bool;};
	void 		setValBool(bool val);
	
	int  		getValInt() {return _val_int;};
	void 		setValInt(int val);
		
	string 	getValString() {return _val_str;};
	void 		setValString(string val);
	
private:
	
	static DemoDB *sharedInstance;
 
	sqlite3 	*_sdb;
	mutable 	std::mutex _mutex;

	
	// values
	bool 		_val_bool;
	int  		_val_int;
	string 	_val_str ;

};

#endif /* DemoDB_hpp */
