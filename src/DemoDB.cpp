//
//  DemoDB.cpp
//  demoserver
//
//  Created by Vincent Moscaritolo on 2/18/22.
//

#include "DemoDB.hpp"
#include "LogMgr.hpp"

DemoDB *DemoDB::sharedInstance = NULL;


DemoDB::DemoDB(){
	
	_sdb = NULL;
 }

DemoDB::~DemoDB(){
	
	if(_sdb)
	{
		sqlite3_close(_sdb);
		_sdb = NULL;
	}
}

bool DemoDB::initDatabase(string filePath){
		
	if(filePath.size() == 0)
		filePath = "demo.db";

	LOGT_DEBUG("OPEN database: %s", filePath.c_str());

	//  Open database
	if(sqlite3_open(filePath.c_str(), &_sdb) != SQLITE_OK){
		LOGT_ERROR("sqlite3_open FAILED: %s", filePath.c_str(), sqlite3_errmsg(_sdb	) );
		return false;
	}
	
	return true;
}


//
// MARK: - Value Setters

void 	 DemoDB::setValBool(bool val){
	
	_val_bool = val;
}

void 	 DemoDB::setValInt(int val){

	_val_int = val;

}

void 	 DemoDB::setValString(string val){
	
	_val_str = val;

}

