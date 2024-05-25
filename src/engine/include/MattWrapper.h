
// My wrapper for C

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Structs and others here



// Functions starting here

// Constructors
int mattWrapper_constructor();
int mattWrapper_svgConstructor();

// Handler generators and free-ers
void* mattWrapper_getNewARHandler();
void mattWrapper_freeARHandler(void* ar_ptr);
void* mattWrapper_getNewGRHandler();
void mattWrapper_freeGRHandler(void* gr_ptr);
void mattWrapper_free(void* ptr);

// File to SVG path
int mattWrapper_parseFile(const char* path, void* ar_ptr);
int mattWrapper_AR2GR(void* ar_ptr, void* gr_ptr);
const char* mattWrapper_GR2SVG(void* gr_ptr, int page);

// Score Map Stuff
void* mattWrapper_getScoreMap(void* gr_ptr, int page, float width, float height, 
				int selectorIn);
char* mattWrapper_getMapCollectorTextual(void* mapCollectorPtr);

#ifdef __cplusplus
}
#endif

