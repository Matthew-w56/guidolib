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

#include <iostream>
using namespace std;

#include "GUIDOEngine.h"
#include "GUIDOInternal.h"

#include "ARDoubleBar.h"
#include "ARFinishBar.h"
#include "ARMusic.h"
#include "ARMusicalVoice.h"
#include "ARRepeatBegin.h"
#include "ARRepeatEnd.h"

#include "GRBar.h"
#include "GRDoubleBar.h"
#include "GRFinishBar.h"
#include "GRGlue.h"
#include "GRMusic.h"
#include "GRPossibleBreakState.h"
#include "GRRepeatBegin.h"
#include "GRRepeatEnd.h"
#include "GRStaffManager.h"
#include "GRSystem.h"
#include "GRSystemSlice.h"

#include "VGDevice.h"
#include "kf_ivect.h"
#include "TCollisions.h"

#include "TagParameterFloat.h"

//#define TRACE
#ifdef TRACE
#define traceMethod(method)		cout << (void*)this << " GRSystemSlice::" << method << endl
#else
#define traceMethod(method)	
#endif
#define trace1Method(method)		cout << (void*)this << " GRSystemSlice::" << method << endl


/** \brief When a system-slice is build, the staff-manager is loosing its grip on 
	the current staves, which make up the slice. 
*/
GRSystemSlice::GRSystemSlice(GRStaffManager * stfmgr, const TYPE_TIMEPOSITION & tp)
	: GREvent(NULL,stfmgr->getGRMusic()->getARMusic(), tp, DURATION_0)
{
	mStaffs = new StaffVector(1); // owns elements
	mGrSystem = 0;
	mPossibleBreakState = 0;

	mStartSpringID = -1;
	mEndSpringID = -1;

	mStartGlue = 0;
	mEndGlue = 0;

	mBeginForceFunc = stfmgr->getCurrentBegSFF();
	mForceFunction = 0;
	mHasSystemBars = false;
	mFirstEvXPosition = 0.f;
}

GRSystemSlice::~GRSystemSlice()
{
	delete mForceFunction;	mForceFunction = 0;
	delete mStartGlue;		mStartGlue = 0;
	delete mEndGlue;		mEndGlue = 0;
	delete mStaffs;			mStaffs = 0;
	delete mPossibleBreakState; mPossibleBreakState = 0;
}


// ----------------------------------------------------------------------------
void GRSystemSlice::print(std::ostream& os) const
{
	for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i )
	{
		GRStaff * staff = mStaffs->Get(i);
		if (staff)
			os << "Staff " << i << endl << staff;
	}
}
std::ostream& operator<< (std::ostream& os, const GRSystemSlice& slice)		{ slice.print(os); return os; }
std::ostream& operator<< (std::ostream& os, const GRSystemSlice* slice)		{ slice->print(os); return os; }

// --------------------------------------------------------------------------
void GRSystemSlice::checkCollisions (TCollisions& state)
{
	state.clearElements();
	for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i )
	{
		GRStaff * staff = mStaffs->Get(i);
		// staff could be null (probably due to voices with \staff tags that redirect the content to anothers staff
		if (staff) {
			state.setStaff (i);
			staff->checkCollisions (state);
		}
	}
}

// --------------------------------------------------------------------------
const GRBar* GRSystemSlice::getBar () const
{
	GRBar* bar = 0;
	for( int i = mStaffs->GetMinimum(); (i <= mStaffs->GetMaximum()) && !bar; ++i )
	{
		GRStaff * staff = mStaffs->Get(i);
		if (staff)
			bar = staff->getLastBar();
	}
	return bar;
}

void GRSystemSlice::addSystemTag(GRNotationElement * mytag)
{
	AddTail(mytag);
}

void GRSystemSlice::addStaff( GRStaff * newStaff, int num)
{
	assert(newStaff);
	mStaffs->Set(num, newStaff);
}

GRBar * GRSystemSlice::getBarAt (const TYPE_TIMEPOSITION& t) const
{
	GRBar * bar = 0;
	const NEPointerList& list = GetCompositeElements();
	GuidoPos pos = list.GetHeadPosition();
	while (pos && !bar) {
		GRNotationElement * elt = list.GetNext(pos);
		if (elt->getRelativeTimePosition() == t)
			bar = dynamic_cast<GRBar*> (elt);
	}
	return bar;
}

void GRSystemSlice::addRepeatBegin( GRRepeatBegin * mybar,const ARBar::TRanges& ranges, GRStaff * grstaff)
{
	const ARRepeatBegin * ar = mybar->getARRepeatBegin();
	assert(ar);
//	ar->setRanges(ranges);
	GRRepeatBegin * newrepeat = new GRRepeatBegin(ar, grstaff);
	newrepeat->setTagType(GRTag::SYSTEMTAG);
	mHasSystemBars = true;
	mybar->addAssociation(newrepeat);
	AddTail(newrepeat);
}

void GRSystemSlice::addRepeatEnd ( GRRepeatEnd * mybar,const ARBar::TRanges& ranges, GRStaff * grstaff)
{
	const ARRepeatEnd * ar = mybar->getARRepeatEnd();
	assert(ar);
//	ar->setRanges(ranges);
	GRRepeatEnd * newrepeat = new GRRepeatEnd(ar, grstaff, mybar->getRelativeTimePosition(), grstaff->getProportionnalRender() );
	newrepeat->setTagType(GRTag::SYSTEMTAG);
	mHasSystemBars = true;
	mybar->addAssociation(newrepeat);
	AddTail(newrepeat);
}

void GRSystemSlice::addBar(GRBar * mybar, const ARBar::TRanges& ranges, GRStaff * grstaff )
{
	// We have to build a new Barline ...
	// the barline needs to know that it belongs to a system. 
	// when the distance of staffs is being set, these tags must be updated (for length) 
	const ARBar * arbar = mybar->getARBar();
	assert(arbar);
//	arbar->setRanges(ranges);
	GRBar * newbar = new GRBar(arbar, NULL, grstaff, mybar->getRelativeTimePosition(), grstaff->getProportionnalRender());
	mHasSystemBars = true;

	// I must attach the newbar with the other spring (or at least
	// be told when something happens ...)
	mybar->addAssociation(newbar);

	// this needs to be rethought later .
	// Just for testing :
	// newbar->setPosFrom(2.5*grstaff->getStaffLSPACE());
	// newbar->setPosTo(20*grstaff->getStaffLSPACE());
	AddTail(newbar);
}

void GRSystemSlice::addFinishBar(GRFinishBar * mybar,const ARBar::TRanges& ranges, GRStaff * grstaff)
{
	const ARFinishBar * arbar = mybar->getARFinishBar();
	assert(arbar);
//	arbar->setRanges(ranges);
	GRFinishBar * newbar = new GRFinishBar(arbar, NULL, grstaff, mybar->getRelativeTimePosition(), grstaff->getProportionnalRender());
	mHasSystemBars = true;
	mybar->addAssociation(newbar);
	AddTail(newbar);
}

void GRSystemSlice::addDoubleBar(GRDoubleBar * mybar,const ARBar::TRanges& ranges, GRStaff * grstaff)
{
	const ARDoubleBar * arbar = mybar->getARDoubleBar();
	assert(arbar);
//	arbar->setRanges(ranges);
	GRDoubleBar * newbar = new GRDoubleBar(arbar, NULL, grstaff, mybar->getRelativeTimePosition(), grstaff->getProportionnalRender());
	mHasSystemBars = true;
	mybar->addAssociation(newbar);
	AddTail(newbar);
}

void GRSystemSlice::addPossibleBreakState(GRPossibleBreakState * in)
{
	mPossibleBreakState = in;
}

// ----------------------------------------------------------------------------
/** \brief Retrieves the mapping
*/
void GRSystemSlice::GetMap( GuidoElementSelector sel, MapCollector& f, MapInfos& infos ) const
{
	if (sel == kGuidoSystemSlice) {
		if (mFirstEvXPosition) {
			NVRect map = mMapping;
			map.right = mFirstEvXPosition;
			SendMap (map, f, getRelativeTimePosition(), TYPE_DURATION(0,1), kSystemSlice, infos);
			map.left = mFirstEvXPosition;
			map.right = mMapping.right;
			SendMap (map, f, getRelativeTimePosition(), getDuration(), kSystemSlice, infos);
		}
		else SendMap (f, getRelativeTimePosition(), getDuration(), kSystemSlice, infos);
	}
	else for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i ) {
		GRStaff * staff = mStaffs->Get(i);
		if (staff) staff->GetMap (sel, f, infos);
	}
}

void GRSystemSlice::GetExtendedMap( GuidoElementSelector sel, ExtendedMapCollector& f, MapInfos& infos ) const
{
	if (sel == kGuidoSystemSlice) {
		if (mFirstEvXPosition) {
			NVRect map = mMapping;
			map.right = mFirstEvXPosition;
			SendExtendedMap (map, f, getRelativeTimePosition(), TYPE_DURATION(0,1), kSystemSlice, infos);
			map.left = mFirstEvXPosition;
			map.right = mMapping.right;
			SendExtendedMap (map, f, getRelativeTimePosition(), getDuration(), kSystemSlice, infos);
		}
		else SendExtendedMap (f, getRelativeTimePosition(), getDuration(), kSystemSlice, infos);
	}
	else for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i ) {
		GRStaff * staff = mStaffs->Get(i);
		if (staff) staff->GetExtendedMap (sel, f, infos);
	}
}


/** \brief Actually draws the SystemSlice.
*/
void GRSystemSlice::OnDraw( VGDevice & hdc ) const
{
	traceMethod("OnDraw");
	for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i )
	{
		GRStaff * theStaff = mStaffs->Get(i);
		if (theStaff)
		{
			gCurStaff = theStaff;
			theStaff->generatePositions();
			theStaff->OnDraw(hdc);
		}
	}
	if (gBoundingBoxesMap & kSystemsSliceBB)
		DrawBoundingBox( hdc, kSystemSliceBBColor);
}

void GRSystemSlice::setHPosition( GCoord nx )
{
	GREvent::setHPosition(nx);
}

void GRSystemSlice::tellPosition(GObject * caller, const NVPoint & newPosition)
{
	traceMethod("tellPosition");
	int i;
	GRGlue * callerGlue = dynamic_cast<GRGlue *>(caller);
	if (caller && callerGlue == mStartGlue)
	{
		// then we have the beginning ....
		for( i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); i++)
		{
			GRStaff * stf = mStaffs->Get(i);
			if (stf) stf->setHPosition(newPosition.x);
		}
	}
	else if (caller && callerGlue == mEndGlue)
	{
		// else we have the end .... the end caller has a spring ....
		float x = 0;
		if (mGrSystem)
		{
			GRSpring * spr = mGrSystem->getSpring(mEndGlue->getSpringID());
			if (spr) x = spr->getExtent();
		}

		for( i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); i++)
		{
			GRStaff * staff = mStaffs->Get(i);
			if (staff)
				staff->setEndPosition( (GCoord)(newPosition.x + x ));
		}
	}
}

/** Returns the number of given staff object. Returns -1 on failure.
*/
int GRSystemSlice::getStaffNumber(const GRStaff * staff) const
{
	const int mini = mStaffs->GetMinimum();
	const int maxi = mStaffs->GetMaximum();
	
	for( int i = mini; i <= maxi; ++i )
	{
		GRStaff * stf = mStaffs->Get(i);
		if (stf == staff)
			return i;
	}
	return -1;
}

/** \brief Sets the vertical offset of the participating staves.

	This is called before the systemslice is added to a list of finished slices. 
	In this call, the staffs are placed vertically so that they do not overlap.
	One idea for overlapping is a sequential list comparison of steps ....:
\verbatim
	----.  x1  .-----
	    |      |
	-------. x2| 
	       | x3.-----
	       |      |
	-------. x4  .----
	  |      x5  |
	--.          .----
\endverbatim

	will add together by comparing the minimum distance required between the inidival step-points
	We always take the top one as a reference.
	The same holds for vertical positioning Will hopefully be implemented some time .... 
	Right now, Finish just takes the bounding boxes and offsets the staff accordingly.
	I should take care of present \\staffFormat tags (distance set ....) ... think about that ?
*/
void GRSystemSlice::Finish()
{
	traceMethod("Finish");
	bool first = true;
	GRStaff * prev = 0;
	NVPoint nextposition;

	GRStaff * theStaff = 0;
	for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i)
	{
		theStaff = mStaffs->Get(i);
		if (theStaff)
		{
			theStaff->boundingBoxPreview();
			NVRect myrect (theStaff->getBoundingBox());
			if (mBeginForceFunc) {
				NVRect myrect2;
				myrect2.top = mBeginForceFunc->getTop(i);
				myrect2.bottom = mBeginForceFunc->getBottom(i);
				if (myrect2.top < myrect.top)			myrect.top = myrect2.top;
				if (myrect2.bottom > myrect.bottom)		myrect.bottom = myrect2.bottom;
				// else if (i != mStaffs->GetMaximum())
				//	myrect.bottom += theStaff->getStaffLSPACE();
			}
			//else if (i != mStaffs->GetMaximum())		myrect.bottom += theStaff->getStaffLSPACE();
			if (i != mStaffs->GetMaximum())
			  myrect.bottom += (GCoord)theStaff->getStaffLSPACE();

			// the question is, wether the top margin ends at the staff or at the top element?
			if (prev && !prev->getStaffState()->distanceset)
                nextposition.y -= myrect.top;

            // fixed stave spacing
            if (theStaff->getStaffState()->getStaffDistance() != NULL) {
                float staffDistance = theStaff->getStaffState()->getStaffDistance()->getValue(theStaff->getStaffLSPACE());
                if (staffDistance > 0.0 && !first)
                    nextposition.y = prev->getPosition().y + prev->getDredgeSize() + staffDistance;
            }

            float staffYOffset = theStaff->getStaffState()->getYOffset(); // Y offset doesn't work for first staff
            nextposition.y += staffYOffset;
			theStaff->setPosition(nextposition);

			if (first) {
                mBoundingBox.top = theStaff->getBoundingBox().top;
				first = false;
			}

#if 1
			GCoord min = myrect.bottom < 6*LSPACE ? 6*LSPACE : myrect.bottom + LSPACE;
//			GCoord min = (myrect.bottom + curLSPACE) < 7*curLSPACE ? 7*curLSPACE : myrect.bottom + curLSPACE;
#else
			GCoord min = myrect.bottom;
#endif
			if (theStaff->getStaffState()->distanceset)
				min = theStaff->getDredgeSize() + theStaff->getStaffState()->distance;
			
			nextposition.y += min;
			prev = theStaff;
		}
	}
	mBoundingBox.bottom = nextposition.y;
}

// ----------------------------------------------------------------------------
/** \brief Called by the GRSystem FinishSystem.

	Propagates the finish procedure to staves
*/
void GRSystemSlice::FinishSlice()
{
	traceMethod("FinishSlice");
    NVRect r, tmp;
 	TYPE_DURATION duration;
	const float nopos = 999999999.f;
	float firstevxpos = nopos;
	if (mStaffs) {
        for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); ++i) {
            GRStaff * staff = mStaffs->Get(i);
            if (staff) {
				staff->FinishStaff();
				float x = staff->FirstNoteORRestXPos();
				if (x < firstevxpos) firstevxpos = x;
                tmp = staff->getBoundingBox();
                NVPoint p = getPosition();
                p.y = 0;
                tmp += staff->getPosition() - p;
                r.Merge (tmp);
				TYPE_DURATION duration = staff->getDuration();
				if (duration > getDuration())
					setDuration (duration);
			}
		}
    }
	if (firstevxpos != nopos) mFirstEvXPosition = firstevxpos;
    mBoundingBox.Merge (r);
	mMapping = mBoundingBox;				// set the mapping box
	mMapping += mPosition + getOffset()	;	// and adjust position	
}

/*
	updateBoundingBox is actually never called
	the bounding box computation is made by FinishSlice (see above)
*/
void GRSystemSlice::updateBoundingBox()
{
	traceMethod("updateBoundingBox");

	mBoundingBox.Set( 0, 0, 0, 0 );
    if (mStaffs) {
		NVRect r, tmp;
		for( int i = mStaffs->GetMinimum(); i <= mStaffs->GetMaximum(); i++) {
            GRStaff * staff = mStaffs->Get(i);
            if (staff) {
                tmp = staff->getBoundingBox();
                r.Merge (tmp);
            }
        }
		mBoundingBox.Merge (r);
    }
}
