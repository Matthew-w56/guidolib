
#include <iostream>
#include <sstream>
#include <string>

#include "MattWrapper.h"
#include "GUIDOEngine.h"
#include "GUIDOScoreMap.h"
#include "MattMapCollector.h"
using guido::GuidoMapCollector;
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

void doPrintMap(Time2GraphicMap map, int result, char* name) {
	// Example of using the map
	printf("Mapping for map: %s\n", name);
	printf("Mapping op result: %d\n", result);
	printf("Mapping #1 for map: %d\n", map.begin());
	for (Time2GraphicMap::const_iterator m = map.begin(); m != map.end(); m++)
	{
		std::cout << "   [" << m->first.first << ", " << m->first.second << "]";
		std::cout << " " << m->second << std::endl; 
	}
	printf("Done with: %s\n\n", name);
}

void* mattWrapper_getScoreMap(void* gr_ptr, int page, float width, float height, 
				int selectorIn) {
	// Do some initial casting
	CGRHandler* grHandler = static_cast<CGRHandler*>(gr_ptr);
	GuidoElementSelector selector = static_cast<GuidoElementSelector>(selectorIn);
	
	// Create filter and map collector
	MattMapCollector* myCollector = new MattMapCollector(
		*grHandler,
		selector
	);
	
	// Run the actual getMap methods
	GuidoGetMap (
		*grHandler,
		page,
		width,
		height,
		selector,
		*myCollector
	);
	
	// TESTING CODE
	Time2GraphicMap staffMap = Time2GraphicMap();
	int staffMapResult = GuidoGetStaffMap(
		*grHandler,
		page,
		width,
		height,
		1,
		staffMap
	);
	Time2GraphicMap pageMap = Time2GraphicMap();
	int pageMapResult = GuidoGetPageMap(
		*grHandler,
		page,
		width,
		height,
		pageMap
	);
	Time2GraphicMap voiceMap = Time2GraphicMap();
	int voiceMapResult = GuidoGetVoiceMap(
		*grHandler,
		page,
		width,
		height,
		1,
		voiceMap
	);
	
	printf("\n");
	doPrintMap(staffMap, staffMapResult, "Staff Map");
	doPrintMap(pageMap, pageMapResult, "Page Map");
	doPrintMap(voiceMap, voiceMapResult, "Voice Map");
	
	printf("Did mapping for map, from gr pointer with %d pages\n", GuidoGetPageCount(*grHandler));
	printf("System Count: %d\n", GuidoGetSystemCount(*grHandler, 1));
	
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
