
#include "GUIDOScoreMap.h"
#include "GuidoMapCollector.h"
using guido::GuidoMapCollector;
typedef GuidoMapCollector::Filter Filter;


#ifdef __cplusplus
extern "C" {
#endif


class MattMapCollector: public MapCollector {
	private:
		CGRHandler mGRHandler;
		GuidoElementSelector mSelector;
		const Filter* mFilter;
		Time2GraphicMap* t2gMap;
		std::vector<GuidoElementInfos> mInfos;
	
	public:
		
		MattMapCollector(CGRHandler grHandler, GuidoElementSelector selectorIn, Filter* filterIn = 0)
			: 	mGRHandler(grHandler),
				mSelector(selectorIn),
				mFilter(filterIn),
				t2gMap(0),
				mInfos() {  }
		
		virtual void Graph2TimeMap( const FloatRect& box, const TimeSegment& dates,  const GuidoElementInfos& infos );
		/*Good*/void process (int page, float w, float h, Time2GraphicMap* outmap);
		void add (const TimeSegment& ts, const FloatRect& r)	{ t2gMap->push_back(make_pair(ts, r)); }
		char* getMappingsString();
};


#ifdef __cplusplus
}
#endif
