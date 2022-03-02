//
//  ServerNouns.cpp
//  demoServer
//
//  Created by Vincent Moscaritolo on 2/10/22.
//

#include <stdio.h>
#include "ServerCmdQueue.hpp"
#include "REST/RESTServerConnection.hpp"
#include "ServerCmdQueue.hpp"

#include "ServerNouns.hpp"
#include "ServerCmdValidators.hpp"
#include "DemoDB.hpp"

#include <sys/utsname.h>


 
constexpr string_view NOUN_PROP					= 	"props";
constexpr string_view JSON_ARG_PROPERTIES		= 	"properties";
 
static bool Properties_NounHandler_PATCH(ServerCmdQueue* cmdQueue,
											REST_URL url,
											TCPClientInfo cInfo,
											  ServerCmdQueue::cmdCallback_t completion) {
	
	using namespace rest;
	auto path = url.path();
	auto queries = url.queries();
	auto headers = url.headers();
	auto body 	= url.body();
	
	json reply;
	
	ServerCmdArgValidator v1;
	
	auto db = DemoDB::shared();
	
	bool didUpdate = false;
	
	for(auto it =  body.begin(); it != body.end(); ++it) {
		string key = Utils::trim(it.key());
		
		if(body[it.key()].is_string()){
			string value = Utils::trim(it.value());
			
			if(db->setProperty(key, value)){
				didUpdate = true;
			}
		}
		else if(body[it.key()].is_number()){
			string value =to_string(it.value());
			
			if(db->setProperty(key, value)){
				didUpdate = true;
			}
		}
		else if(body[it.key()].is_boolean()){
			string value =  it.value()?"1":"0";
			
			if(db->setProperty(key, value)){
				didUpdate = true;
			}
			
		} else if(body[it.key()].is_null()){
			// delete property
			if(db->removeProperty( key)){
				didUpdate = true;
			}
		}
	}
	if(didUpdate){
		db->saveProperties();
	}
	
	makeStatusJSON(reply,STATUS_OK);
	(completion) (reply, STATUS_OK);
	return true;
	
}

static bool Properties_NounHandler_GET(ServerCmdQueue* cmdQueue,
											REST_URL url,
											TCPClientInfo cInfo,
											ServerCmdQueue::cmdCallback_t completion) {
	using namespace rest;
	json reply;

	auto path = url.path();
	string noun;
	
	if(path.size() > 0) {
		noun = path.at(0);
	}
	
	auto db = DemoDB::shared();
 
	// CHECK sub paths
	map<string ,string> propList;
	
	if(path.size() == 1){
		propList = db->getProperties();
		
	}else if (path.size() == 2){
		
		string propName = path.at(1);
		string value;
		
		if(db->getProperty(propName, &value)){
			propList[propName] = value;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
 
	json propEntries;
	
	for(const auto& [key, value] : propList) {
		propEntries[key] = value;
	}

	reply[ string(JSON_ARG_PROPERTIES) ] = propEntries;
	
	makeStatusJSON(reply,STATUS_OK);
	(completion) (reply, STATUS_OK);
	return true;
}


static bool Properties_NounHandler_DELETE(ServerCmdQueue* cmdQueue,
											  REST_URL url,
											  TCPClientInfo cInfo,
														ServerCmdQueue::cmdCallback_t completion) {
	using namespace rest;
	json reply;
	
	auto path = url.path();
	string noun;
	
	if(path.size() > 0) {
		noun = path.at(0);
	}

	auto db = DemoDB::shared();
 
	if (path.size() == 2){
		string propName = path.at(1);
		
		if(!db->removeProperty(propName)){
				return false;
		}
		
	}
	else {
		return false;
	}
	
	// CHECK sub paths
	
	makeStatusJSON(reply,STATUS_NO_CONTENT);
	(completion) (reply, STATUS_NO_CONTENT);
	return true;
}



static void Properties_NounHandler(ServerCmdQueue* cmdQueue,
										 REST_URL url,
										 TCPClientInfo cInfo,
										 ServerCmdQueue::cmdCallback_t completion) {

  using namespace rest;
  json reply;
  
  auto path = url.path();
  auto queries = url.queries();
  auto headers = url.headers();
  string noun;
  
  bool isValidURL = false;
  
  if(path.size() > 0) {
	  noun = path.at(0);
  }

  switch(url.method()){
	  case HTTP_GET:
		  isValidURL = Properties_NounHandler_GET(cmdQueue,url,cInfo, completion);
		  break;

	  case HTTP_PATCH:
 			isValidURL = Properties_NounHandler_PATCH(cmdQueue,url,cInfo, completion);
 			break;

//		case HTTP_PUT:
//		case HTTP_POST:
	  case HTTP_DELETE:
		  isValidURL = Properties_NounHandler_DELETE(cmdQueue,url,cInfo, completion);
		  break;

	  default:
		  (completion) (reply, STATUS_INVALID_METHOD);
		  return;
  }
  
  if(!isValidURL) {
	  (completion) (reply, STATUS_NOT_FOUND);
  }
};


void registerServerNouns() {
	// create the server command processor
	auto cmdQueue = ServerCmdQueue::shared();

	cmdQueue->registerNoun(NOUN_PROP, 	Properties_NounHandler);

}

