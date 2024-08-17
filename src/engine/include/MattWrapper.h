#ifndef MATTWRAPPER_H
#define MATTWRAPPER_H

// My wrapper for C

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// -----------------[ Structs and others here ]-----------------

struct gemeName {
	int rect_top,
		rect_bottom,
		rect_left,
		rect_right;
	int dur_start_num,
		dur_start_den,
		dur_end_num,
		dur_end_den;
	int staffNum,
		voiceNum;
	int midiPitch;
	// Ffigen will complain - We still need this.
	// To build, just remove, run ffigen, and replace.
	int dots = 0;
	// Replace prev line with: int dots;
	int accidental;
	int type;
	void* element;
};

typedef struct gemeName ExtMapElement;

// -----------------[ Functions starting here ]-----------------

// Constructors
int mattWrapper_svgConstructor();

// Handler generators and free-ers
void* mattWrapper_getNewARHandler();
void  mattWrapper_freeARHandler(void* ar_ptr);
void* mattWrapper_getNewGRHandler();
void  mattWrapper_freeGRHandler(void* gr_ptr);
void  mattWrapper_free(void* ptr);

// File to SVG path
int mattWrapper_parseFile(const char* path, void* ar_ptr);
int mattWrapper_parseString(const char* data, void* ar_ptr);
int mattWrapper_AR2GR(void* ar_ptr, void* gr_ptr);
const char* mattWrapper_GR2SVG(void* gr_ptr, int page);

// Map Stuff
void* mattWrapper_getMap(void* gr_ptr, int page, int selectorIn, int* outmapSize);
void* mattWrapper_getSystemMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getStaffMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getBarMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getBarAndEventMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getEventMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getClefMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getMeterMap(void* gr_ptr, int page, int* outmapSize);
void* mattWrapper_getTagMap(void* gr_ptr, int page, int* outmapSize);
void  mattWrapper_printMapItems(void* map, int mapSize);
// Map getters
ExtMapElement mattWrapper_getElementAtPos(void* map, int mapSize, int x, int y, bool* successOut);
ExtMapElement mattWrapper_getElementAtPosIgnoreType(void* map, int mapSize, int x, int y, int type, bool* successOut);
ExtMapElement mattWrapper_getElementAtVoiceTime(void* map, int mapSize, double time, int voice, int typeToIgnore, int midiPitch, bool* successOut);
ExtMapElement mattWrapper_getElementAtStaffTime(void* map, int mapSize, double time, int staff, int typeToIgnore, int midiPitch, bool* successOut);
ExtMapElement mattWrapper_getElementAtIndex(void* map, int mapSize, int index, bool* successOut);

// Score info and misc.
int mattWrapper_getScorePageCount(void* gr_ptr);
const char* mattWrapper_getScoreName(void* gr_ptr);

#ifdef __cplusplus
}
#endif


#endif