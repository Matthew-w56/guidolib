#ifndef GRRepeatEnd_H
#define GRRepeatEnd_H

/*
  GUIDO Library
  Copyright (C) 2002  Holger Hoos, Juergen Kilian, Kai Renz
  Copyright (C) 2002-2017 Grame

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr

*/

#include "GRBar.h"
#include "GRDefine.h"
#include "GUIDOScoreMap.h"

class ARRepeatEndRangeEnd;
class ARRepeatEnd;


/** \brief The left facing repeat bar sign.
*/
// (JB) It's actually a range tag, so it must derive from Position Tag.
//class GRRepeatEnd : public GRPTagARNotationElement // was GRTagARNotationElement
//class GRRepeatEnd : public GRTagARNotationElement
class GRRepeatEnd : public GRBar
{
	public:

				 GRRepeatEnd(const ARRepeatEnd * ar, GRStaff * inStaff, const TYPE_TIMEPOSITION & inTimePos , float proportionnalRender);
		virtual	~GRRepeatEnd() {}

		void setPosFrom( float posy );
		void setPosTo( float posy );
	
		const ARRepeatEnd*		 getARRepeatEnd();
		virtual unsigned int getTextAlign() const;
		virtual void	updateBoundingBox();
		virtual void	setHPosition( float nx);
		virtual void	GetMap( GuidoElementSelector sel, MapCollector& f, MapInfos& infos ) const;
		virtual void 	GetExtendedMap( GuidoElementSelector sel, ExtendedMapCollector& f, MapInfos& infos ) const;
		virtual void	OnDraw( VGDevice & hdc ) const;

		virtual const NVPoint & getReferencePosition() const { return refpos; }

	protected:
        static NVPoint refpos;
        float  fBaseThickness;
        float  fSize;
        float  fStaffThickness;
        float  fLSpace;

	private:
				float	getXOffset() const;
				void	DrawDots( VGDevice & hdc ) const;
				void	InitRepeatEnd();
};

#endif
