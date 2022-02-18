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


 
constexpr string_view NOUN_TEST		 		= 	"test";
constexpr string_view JSON_ARG_VAL_INT		= 	"val.int";
constexpr string_view JSON_ARG_VAL_BOOL	= 	"val.bool";
constexpr string_view JSON_ARG_VAL_STR		= 	"val.str";


static bool Test_NounHandler_PATCH(ServerCmdQueue* cmdQueue,
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

	bool isValidURL = false;
	auto demoDB = DemoDB::shared();

	int i;
	string str;
	bool b;
	

	if(v1.getIntFromJSON(JSON_ARG_VAL_INT, body, i)){
		demoDB->setValInt(i);
		isValidURL = true;
	}
	
	if(v1.getBoolFromJSON(JSON_ARG_VAL_BOOL, body, b)){
		demoDB->setValBool(b);
		isValidURL = true;
	}

	if(v1.getStringFromJSON(JSON_ARG_VAL_STR, body, str)){
		demoDB->setValString(str);
		isValidURL = true;
	}
	
	if(isValidURL) {
		makeStatusJSON(reply,STATUS_OK);
		(completion) (reply, STATUS_OK);
	}

	return isValidURL;
}

static bool Test_NounHandler_GET(ServerCmdQueue* cmdQueue,
											REST_URL url,
											TCPClientInfo cInfo,
											ServerCmdQueue::cmdCallback_t completion) {
 using namespace rest;
	auto path = url.path();
	auto queries = url.queries();
	auto headers = url.headers();

	json reply;
  
	bool isValidURL = false;
	auto demoDB = DemoDB::shared();

	// get all values
	if (path.size() == 1){
		reply[string(JSON_ARG_VAL_INT)] 	= demoDB->getValInt();
		reply[string(JSON_ARG_VAL_BOOL)] = demoDB->getValBool();
		reply[string(JSON_ARG_VAL_STR)] 	= demoDB->getValString();
		isValidURL = true;
	}
	else if (path.size() == 2){
		string key = path.at(1);
		if(key == "int"){
			reply[string(JSON_ARG_VAL_INT)] =  demoDB->getValInt();
			isValidURL = true;
			
		}else if(key == "bool"){
			reply[string(JSON_ARG_VAL_BOOL)] = demoDB->getValBool();
			isValidURL = true;

		} else if(key == "str"){
			reply[string(JSON_ARG_VAL_STR)] = demoDB->getValString();
			isValidURL = true;
 		}
 	}
	
	if(isValidURL) {
		makeStatusJSON(reply,STATUS_OK);
		(completion) (reply, STATUS_OK);
 	}
 	

	return isValidURL;
}


static void Test_NounHandler(ServerCmdQueue* cmdQueue,
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
		  isValidURL = Test_NounHandler_GET(cmdQueue,url,cInfo, completion);
		  break;

	  case HTTP_PATCH:
 			isValidURL = Test_NounHandler_PATCH(cmdQueue,url,cInfo, completion);
 			break;

//		case HTTP_PUT:
//		case HTTP_POST:
//		case HTTP_DELETE:
 
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

	cmdQueue->registerNoun(NOUN_TEST, 	Test_NounHandler);

}

