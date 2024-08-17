
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "MattWrapper.h"
#include "GUIDOEngine.h"
#include "GUIDOScoreMap.h"
#include "MattMapCollector.h"
#include "SVGSystem.h"
#include "SVGDevice.h"
#include "GUIDOInternal.h"
#include "GRMusic.h"
#include "guido2.h"
#include "ARMusic.h"
#include "Guido2Midi.h"

using guido::GuidoMapCollector;
using std::vector;

static char* textOutput;

/**
 *  Initializes Guido with the preparation to draw SVGs.  The
 *  width and height given define the size of the SVG generated.
 *  
 *  Returns an error code.  0 == no problems
 */
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

int mattWrapper_parseString(const char* data, void* ar_ptr) {
	ARHandler* handler = static_cast<ARHandler*>(ar_ptr);
	return GuidoParseString(data, handler);
}

int mattWrapper_AR2GR(void* ar_ptr, void* gr_ptr) {
	ARHandler* arHandler = static_cast<ARHandler*>(ar_ptr);
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	return GuidoAR2GR(*arHandler, nullptr, grHandler);
}

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
	int len = int(permStr.size());
	textOutput = (char*)malloc(len + 2);
	
	memcpy(textOutput, permStr.c_str(), permStr.size());
	textOutput[len] = 0;
	textOutput[len+1] = 0;
	
	return textOutput;
}

/**
 *  Returns an array (vector) of map items.  Map items map a position in the
 *  SVG to a time in the score, measured in measure lengths.  Print this map
 *  with mattWrapper_printMapItems(ptr);
 */
void* mattWrapper_getMap(void* gr_ptr, int page, int selectorIn, int* outmapSize) {
	
	// Do some initial casting
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	GuidoElementSelector selector = static_cast<GuidoElementSelector>(selectorIn);
	
	// Empty vector to hold map output
	vector<ExtMapElement>* outmap = new vector<ExtMapElement>();
	
	// Actuall run map get
	int err = GuidoGetExtendedSVGMap(*grHandler, page, selector, *outmap);
	
	// Print any errors that occurred
	if (err != guidoNoErr) {
		printf("Got error in map get!  Error %d!\n", err);
		std::cout.flush();
	}
	
	// Output size to the outmapSize parameter variable
	*outmapSize = (int)outmap->size();
	
	// Return opaque reference to map list
	return &(outmap->at(0));
}

// --------------------------[ GetMap Variants ]-----------------------------
/**
 *  Returns a map containing the systems in the score.
 *  A system is a set of staves.
 */
void* mattWrapper_getSystemMap(void* gr_ptr, int page, int* outmapSize) {
  return mattWrapper_getMap(gr_ptr, page, 1, outmapSize);  // kGuidoSystem
}
/**
 *  Returns a map containing the extents of the staffs (lines) in the score
 */
void* mattWrapper_getStaffMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 3, outmapSize);  // kGuidoStaff
}
/**
 *  Returns a map containing the bar lines in the score
 */
void* mattWrapper_getBarMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 4, outmapSize);  // kGuidoBar
}
/**
 *  Returns a map containing all elements returned from either the
 *  Bar map, or the Event map.
 */
void* mattWrapper_getBarAndEventMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 5, outmapSize);  // kGuidoBarAndEvent
}
/**
 *  Returns a map containing:
 *   - Notes
 *   - 
 */
void* mattWrapper_getEventMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 6, outmapSize);  // kGuidoEvent
}
/**
 *  Returns a map containing the clefs in the score
 */
void* mattWrapper_getClefMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 7, outmapSize);  // kClefSel
}
/**
 *  Returns a map containing the meters in the score
 */
void* mattWrapper_getMeterMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 8, outmapSize);  // kMeterSel
}
/// TODO: Document this.
void* mattWrapper_getTagMap(void* gr_ptr, int page, int* outmapSize) {
	return mattWrapper_getMap(gr_ptr, page, 9, outmapSize);  // kTagSel
}

// ------------------------[ End GetMap Variants ]---------------------------

ExtMapElement mattWrapper_getElementAtPosIgnoreType(void* map, int mapSize, int x, int y, int type, bool* successOut) {
	*successOut = false;
	ExtMapElement* mapEls = static_cast<ExtMapElement*>(map);
	for (int i = 0; i < mapSize; i++) {
		ExtMapElement element = mapEls[i];
		// If the bounds contain the click point
		if (element.rect_top <= y && element.rect_bottom >= y 
				&& element.rect_left <= x && element.rect_right >= x
				&& element.type != type) {  // 3 is the code for empty
			// Return the element found
			*successOut = true;
			return element;
		}
	}
	// Return a placeholder element (successOut left blank)
	return ExtMapElement();
}

ExtMapElement mattWrapper_getElementAtPos(void* map, int mapSize, int x, int y, bool* successOut) {
	return mattWrapper_getElementAtPosIgnoreType(map, mapSize, x, y, -7, successOut);
}

double getDivOf(int num, int den) { return ((double)num) / ((double)den); }

ExtMapElement mattWrapper_getElementAtVoiceTime(void* map, int mapSize, double time, int voice, int typeToIgnore, int midiPitch, bool* successOut) {
	*successOut = false;
	ExtMapElement* mapEls = static_cast<ExtMapElement*>(map);
	for (int i = 0; i < mapSize; i++) {
		ExtMapElement element = mapEls[i];
		if (element.dur_start_den == 0 || element.dur_end_den == 0) continue;
		double elStartTime = getDivOf(element.dur_start_num, element.dur_start_den);
		// If the duration of the element is at the given duration
		if (elStartTime == time && element.voiceNum == voice && element.type != typeToIgnore
			&& (midiPitch == -1 || midiPitch == element.midiPitch)) {
			*successOut = true;
			return element;
		}
	}
	printf("Could not find an element at that time! (%f)\n", time);
	std::cout.flush();
	return ExtMapElement();
}

ExtMapElement mattWrapper_getElementAtStaffTime(void* map, int mapSize, double time, int staff, int typeToIgnore, int midiPitch, bool* successOut) {
	*successOut = false;
	ExtMapElement* mapEls = static_cast<ExtMapElement*>(map);
	for (int i = 0; i < mapSize; i++) {
		ExtMapElement element = mapEls[i];
		if (element.dur_start_den == 0 || element.dur_end_den == 0) continue;
		double elStartTime = getDivOf(element.dur_start_num, element.dur_start_den);
		// If the duration of the element is at the given duration
		if (elStartTime == time && element.staffNum == staff && element.type != typeToIgnore
			&& (midiPitch == -1 || midiPitch == element.midiPitch)) {
			*successOut = true;
			return element;
		}
	}
	printf("Could not find an element at that time! (%f)\n", time);
	std::cout.flush();
	return ExtMapElement();
}

ExtMapElement mattWrapper_getElementAtIndex(void* map, int mapSize, int index, bool* successOut) {
	*successOut = false;
	if (index >= mapSize || index < 0) return ExtMapElement();
	ExtMapElement* mapEls = static_cast<ExtMapElement*>(map);
	ExtMapElement el =  mapEls[index];
	*successOut = true;
	return el;
}

/**
 *  Prints the contents of the given map to std::out
 */
void mattWrapper_printMapItems(void* map, int mapSize) {
	ExtMapElement* castedArr = static_cast<ExtMapElement*>(map);
	
	printf("Elements of Guido Map:\n");
	// printf("( [left, right] [top, bottom] ) maps to ( [start, end] ) with [pitch] and [type]");
	for (size_t i = 0; i < mapSize; i++)
	{
		ExtMapElement currEl = castedArr[i];
		printf("Staff %d  Voice %d  Pitch %d  Type %d  Duration %d/%d-%d/%d  Hitbox: [(%d, %d), (%d, %d)]\n",
			currEl.staffNum,
			currEl.voiceNum,
			currEl.midiPitch,
			currEl.type,
			currEl.dur_start_num,
			currEl.dur_start_den,
			currEl.dur_end_num,
			currEl.dur_end_den,
			currEl.rect_left,
			currEl.rect_top,
			currEl.rect_right,
			currEl.rect_bottom
		);
	}
	printf("\nDone with map elements!  Total Length: %d\n", mapSize);
	std::cout.flush();
}

// ------------------------[ End Map Methods ]------------------------------

// I don't think this works as-is..  Fober said we have to look for ARTitle obj.
const char* mattWrapper_getScoreName(void* gr_ptr) {
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	const char* tempString = (*grHandler)->grmusic->getName().c_str();
	
	std::string permStr = std::string(tempString);
	int len = int(permStr.size());
	textOutput = (char*)malloc(len + 2);
	
	memcpy(textOutput, permStr.c_str(), permStr.size());
	textOutput[len] = 0;
	textOutput[len+1] = 0;
	
	return textOutput;
}

void mattWrapper_free(void* ptr) {
	if (ptr) {
		delete ptr;
	}
}

int mattWrapper_getScorePageCount(void* gr_ptr) {
	GRHandler* grHandler = static_cast<GRHandler*>(gr_ptr);
	return (*grHandler)->grmusic->getNumPages();
}
