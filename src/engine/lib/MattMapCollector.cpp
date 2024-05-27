
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "MattMapCollector.h"

using std::pair;



static bool mCompare( const pair<TimeSegment, FloatRect>& a, const pair<TimeSegment, FloatRect>& b)
{
    return a.first < b.first;
}
    
static bool mTimeAndBoxLeftmostCompare( const pair<TimeSegment, FloatRect>& a, const pair<TimeSegment, FloatRect>& b)
{
    if (a.first < b.first) return true;
    return a.first.startEqual(b.first) && (a.second.left < b.second.left);
}

static bool mTimeAndBoxRightmostCompare( const pair<TimeSegment, FloatRect>& a, const pair<TimeSegment, FloatRect>& b)
{
    if (a.first < b.first) return true;
	if (a.first.startEqual(b.first)) {
		if (a.second.left > b.second.left) return true;
	}
	return false;
}



void MattMapCollector::Graph2TimeMap(const FloatRect& box, const TimeSegment& dates, const GuidoElementInfos& infos) {
	printf("MatMapCollector::Graph2TimeMap--------------------------------\n");
	if (mFilter && !(*mFilter)(infos))	return;			// element is filtered out
	if ( dates.empty() )				return;			// empty time segments are filtered out
	if ( !box.IsValid() )				return;			// empty graphic segments are filtered out
	//if ( infos.type == kGraceNote)      return;       // grace notes are {CURRENTLY NOT} filtered out
	if ( infos.type == kEmpty)			return;         // empty events are filtered out
	t2gMap->push_back(make_pair(dates, box));
	// Also save the elmentInfo struct
	mInfos.push_back(infos);
}

void MattMapCollector::process (int page, float w, float h, Time2GraphicMap* outmap) {
	printf("MattMapCollector::process (page %d)-----------------------------------\n", page);
	t2gMap = outmap;
	if (!t2gMap) return;
	GuidoGetMap( mGRHandler, page, w, h, mSelector, *this );
}

char* MattMapCollector::getMappingsString() {
	printf("MattMapCollector::getMappingsString\n");
	
	if (!t2gMap) return "No map to work off of!";
	if (t2gMap->size() == 0) return "No mappings found within t2gMap!";
	
	std::ostringstream oss;
	oss << "\n----------------\nMap Laid Out:\n";
	for (Time2GraphicMap::const_iterator i = t2gMap->begin(); i != t2gMap->end(); i++) {
		oss << ":: " << i->first << " -> " << i->second << " ::" << std::endl;
	}
	oss << "----------------\n";
	
	std::string permStr = std::string(oss.str());
	char* permVal = (char*)malloc(permStr.size());
	memcpy(permVal, permStr.c_str(), permStr.size());
	
	return permVal;
}
