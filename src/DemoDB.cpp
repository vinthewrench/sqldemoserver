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
	_isSetup = false;
	_properties.clear();
 }

DemoDB::~DemoDB(){
	
	if(_sdb)
	{
		sqlite3_close(_sdb);
		_sdb = NULL;
		_isSetup = false;
	}
}

// MARK: - Database

bool DemoDB::initDatabase(string filePath){
		
	if(filePath.size() == 0)
		filePath = "demo.db";

	LOGT_DEBUG("OPEN database: %s", filePath.c_str());

	//  Open database
	if(sqlite3_open(filePath.c_str(), &_sdb) != SQLITE_OK){
		LOGT_ERROR("sqlite3_open FAILED: %s", filePath.c_str(), sqlite3_errmsg(_sdb	) );
		return false;
	}
	
	// make sure primary tables are there.
	string sql1 = "CREATE TABLE IF NOT EXISTS DEMO_DATA("  \
						"NAME TEXT PRIMARY KEY," \
						"VALUE )";

	char *zErrMsg = 0;
	if(sqlite3_exec(_sdb,sql1.c_str(),NULL, 0, &zErrMsg  ) != SQLITE_OK){
		LOGT_ERROR("sqlite3_exec FAILED: %s\n\t%s", sql1.c_str(), sqlite3_errmsg(_sdb	) );
		sqlite3_free(zErrMsg);
		return false;
	}

	_isSetup = true;
 
	if(!restoreProperties()){
		LOGT_ERROR("restoreValuesFromDB FAILED");
		return false;
	}

	return true;
}


bool DemoDB::restoreProperties(){
	
	bool statusOk = false;
	
	_properties.clear();
	
	if(_isSetup){
		
		std::lock_guard<std::mutex> lock(_mutex);
		
		string sql = string("SELECT NAME, VALUE FROM DEMO_DATA;");
		sqlite3_stmt* stmt = NULL;
		sqlite3_prepare_v2(_sdb, sql.c_str(), -1,  &stmt, NULL);
		
		while ( (sqlite3_step(stmt)) == SQLITE_ROW) {
			string  key = string( (char*) sqlite3_column_text(stmt, 0));
			string  value = string( (char*) sqlite3_column_text(stmt, 1));
			
//			printf("%10s : %s\n", key.c_str(),value.c_str());
			
			_properties[key]  = value;
		}
		sqlite3_finalize(stmt);
		
		statusOk = true;
 	}
 
	return statusOk;
}


bool DemoDB::saveProperties() {
	bool statusOk = false;
	
	if(_properties.size() == 0)
		return true;
	
	
	std::lock_guard<std::mutex> lock(_mutex);
	
	string sql = string("REPLACE INTO DEMO_DATA (NAME, VALUE) VALUES ");
	
	
	for (auto& [name, value] : _properties) {
		sql.append( "('" +  name  + "','" + value  + "' ),");
	}
	
	// remove last comma and add semicolon
	sql.pop_back();
	sql.append(";");
	
	//	printf("%s\n", sql.c_str());
	
	char *zErrMsg = 0;
	if(sqlite3_exec(_sdb,sql.c_str(),NULL, 0, &zErrMsg  ) != SQLITE_OK){
		LOGT_ERROR("sqlite3_exec FAILED: %s\n\t%s", sql.c_str(), sqlite3_errmsg(_sdb	) );
		sqlite3_free(zErrMsg);
	}
	else {
		statusOk = true;
	}
	
	return statusOk;
}


// MARK: - properties
bool DemoDB::setProperty(string key, string value){
	_properties[key] = value;
 
	return true;
}

bool DemoDB::removeProperty(string key){
	
	if(_properties.count(key)){
		_properties.erase(key);
		saveProperties();
		return true;
	}
	return false;
}

bool DemoDB::setPropertyIfNone(string key, string value){
	
	if(_properties.count(key) == 0){
		_properties[key] = value;
		return true;
	}
	return false;
}

map<string ,string> DemoDB::getProperties(){
	
	return _properties;
}

bool DemoDB::getProperty(string key, string *value){
	
	if(_properties.count(key)){
		if(value)
			*value = _properties[key];
		return true;
	}
	return false;
}

bool  DemoDB::getUint16Property(string key, uint16_t * valOut){
	
	string str;
	if(getProperty(string(key), &str)){
		char* p;
		long val = strtoul(str.c_str(), &p, 0);
		if(*p == 0 && val <= UINT16_MAX){
			if(valOut)
				*valOut = (uint16_t) val;
			return true;
		}
	}
	return false;
}

bool  DemoDB::getFloatProperty(string key, float * valOut){
	
	string str;
	if(getProperty(string(key), &str)){
		char* p;
		float val = strtof(str.c_str(), &p);
		if(*p == 0){
			if(valOut)
				*valOut = (float) val;
			return true;
		}
	}
	return false;
}
 
bool  DemoDB::getBoolProperty(string key, bool * valOut){
	
	string str;
	if(getProperty(string(key), &str) ){
		char* p;
		
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		
		long val = strtoul(str.c_str(), &p, 0);
		if(*p == 0 && (val == 0 || val == 1)){
			if(valOut) *valOut = (bool)val;
			return true;
			
		}else if(str == "true"){
			if(valOut) *valOut = true;
			return true;
		}
		else if(str == "false"){
			if(valOut)	*valOut = false;
			return true;
		}
	}
	return false;
}

