
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "MattWrapper.h"
#include "GUIDOEngine.h"
#include "GUIDOScoreMap.h"
#include "MattMapCollector.h"
using guido::GuidoMapCollector;
using std::vector;
//using guido::GuidoMapCollector::Filter;

int mattWrapper_constructor() {
	struct GuidoInitDesc initdesc = GuidoInitDesc();
	return GuidoInit(&initdesc);
}

int mattWrapper_svgConstructor() {
	return GuidoInitWithIndependentSVG();
}

void* mattWrapper_getNewARHandler() {
	return new ARHandler();
}

void mattWrapper_freeARHandler(void* ar_ptr) {
	ARHandler* handler = static_cast<ARHandler*>(ar_ptr);
	GuidoFreeAR(*handler);
}

void* mattWrapper_getNewGRHandler() {
	return new GRHandler();
}

void mattWrapper_freeGRHandler(void* gr_ptr) {
	GRHandler* handler = static_cast<GRHandler*>(gr_ptr);
	GuidoFreeGR(*handler);
}

int mattWrapper_parseFile(const char* path, void* ar_ptr) {
	ARHandler* handler = static_cast<ARHandler*>(ar_ptr);
	return GuidoParseFile(path, handler);
}

int mattWrapper_AR2GR(void* ar_ptr, void* gr_ptr) {
	ARHandler* arHandler = static_cast<ARHandler*>(ar_ptr);
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	return GuidoAR2GR(*arHandler, nullptr, grHandler);
}

/**
 *  Must free the char* given!
*/
const char* mattWrapper_GR2SVG(void* gr_ptr, int page) {
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	std::ostringstream oss;
	int result = GuidoGR2SVG(
		*grHandler,
		page,
		oss,
		true,
		"", 2
	);
	
	std::string permStr = std::string(oss.str());
	char* permVal = (char*)malloc(permStr.size());
	memcpy(permVal, permStr.c_str(), permStr.size());
	
	return permVal;
}

void* mattWrapper_getScoreMap(void* gr_ptr, int page, float width, float height, 
				int selectorIn) {
	
	// Do some initial casting
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	GuidoElementSelector selector = static_cast<GuidoElementSelector>(selectorIn);
	
	// Create map collector
	MattMapCollector* myCollector = new MattMapCollector(
		*grHandler,
		selector
	);
	vector<MapElement> outmap;
	int err = GuidoGetSVGMap(*grHandler, page, selector, outmap);
	if (err != guidoNoErr) {
		printf("Got error in map get!  Error $d!\n", err);
	} else {
		std::cout << "( [left, right] [top, bottom] ) maps to ( [start, end] )" << std::endl;
	
		for (size_t i = 0; i < outmap.size(); i++)
		{
			FloatRect r = outmap[i].first;
			TimeSegment time = outmap[i].second.time();
			std::cout << "( [" << int(r.left) << "," << int(r.right) << "] [" << int(r.top) << "," << int(r.bottom) << "] ) "
			<< " ( [" << time.first << ", " << time.second << "] )" << std::endl;
		}
	}
	
	return (void*)myCollector;
}

/**
 *  Must free the char* given!
*/
char* mattWrapper_getMapCollectorTextual(void* mapCollectorPtr) {
	MattMapCollector* collector = static_cast<MattMapCollector*>(mapCollectorPtr);
	
	return collector->getMappingsString();
	
	/*
	std::string permStr = std::string(oss.str());
	char* permVal = (char*)malloc(permStr.size());
	memcpy(permVal, permStr.c_str(), permStr.size());
	
	return permVal;
	*/
}

void mattWrapper_free(void* ptr) {
	if (ptr) {
		delete ptr;
	}
}
