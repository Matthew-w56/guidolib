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

#include "ARKey.h"
#include "GRKey.h"
#include "GRAccidental.h"
#include "GRStaff.h"
#include "GRDefine.h"

using namespace std;

static const int quint[] = { NOTE_F, NOTE_C, NOTE_G, NOTE_D, NOTE_A, NOTE_E, NOTE_H };

GRKey::GRKey( GRStaff * inStaff, const ARKey * key, int p_natural, bool ownsAR )
		: GRARCompositeNotationElement(key, ownsAR), GRTag(key)
{
	// natural is set, it the key needs to be naturlized, that is, this
	// is the time, when another key is specified.
	mNatural = p_natural;
	mGrStaff = inStaff;
	mNumKeys = key->getKeyNumber();
	getKeyArray(mkarray);
	getOctArray(mOctarray);
//	mElements.setOwnership(1); // the elements are really owned by the key
	if (mGrStaff != NULL)
	{
		mTagSize = mGrStaff->getSizeRatio();
		mCurLSPACE = mGrStaff->getStaffLSPACE();
		createAccidentals();
	}
	mNeedsSpring = 1;
	sconst = SCONST_KEY;

	// rod-spring-modell
	setRelativeTimePosition (key->getRelativeTimePosition());
    mIsInHeader = key->isInHeader();
}

GRKey::~GRKey()
{
}

// This is a static function ...
int GRKey::getNonFreeKeyArray(int pnumkeys, float *KeyArray)
{
	for (int i=0;i<NUMNOTES;i++)
		KeyArray[i] = 0;
	if (pnumkeys > 0)
		for (int i=0;i<pnumkeys;i++)
		{
			KeyArray[ quint[i%7] - NOTE_C ] += 1;
		}
	else
		for (int i=0;i<(pnumkeys*-1);i++)
		{
			KeyArray[ quint[6 - i%7] - NOTE_C] -= 1;
		}

	return pnumkeys;
}

// -----------------------------------------------------------------------------
void GRKey::accept (GRVisitor& visitor)
{
	visitor.visitStart (this);
	visitor.visitEnd (this);
}

/** \brief Determines the accidentals ...
*/
int GRKey::getKeyArray(float * KeyArray)
{
	const ARKey * mykey = static_cast<const ARKey *>(getAbstractRepresentation());
	if (mykey->freeKey())
		mykey->getFreeKeyArray(KeyArray);
	else
		getNonFreeKeyArray(mNumKeys,KeyArray);
	return mNumKeys;
}


void GRKey::getOctArray(int * OctArray)
{
	const ARKey * mykey = /*dynamic*/static_cast<const ARKey *>(getAbstractRepresentation());
	mykey->getOctArray(OctArray);
}


void GRKey::setHPosition( GCoord nx)
{
	setPosition( NVPoint( nx,mPosition.y ));
}

/**  Update the the x-coordinates of the accidentals,
	without altering their y distributions on the staff.
*/

void GRKey::setPosition( const NVPoint & inPos )
{	
	// y-position is not really settable ...
	NVPoint newPos( inPos.x, getPosition().y );

	// GRARCompositeNotationElement::setPosition( newPos );
	mPosition = newPos;

	// long length = leftSpace + rightSpace;
	// old: mBoundingBox.right - mBoundingBox.left;
	// tmppoint.x -= length/2;

	// - A little distance between the elements ...
	const float xMargin = mCurLSPACE * float(0.1); // arbitrary

	GuidoPos pos = First();
	while (pos)
	{
		GRNotationElement * e = GetNext(pos);
		newPos.x += e->getLeftSpace() + xMargin;
		e->setHPosition( newPos.x );
		newPos.x += e->getRightSpace();
	}
	updateBoundingBox();
}

//____________________________________________________________________________________
/** Create and automatically place the accidentals correctly
*/
void GRKey::createAccidentals()
{
	if (mGrStaff->getStaffState()->getInstrKeyNumber() == mNumKeys && mNumKeys != 0) 	
		return;
	
	int mynumkeys = mNumKeys;
	float mymkarray [ NUMNOTES ];
	float * keyarr = mkarray;
	int instrkeynumber = mGrStaff->getStaffState()->getInstrKeyNumber();
	if (instrkeynumber != 0)
	{
		mynumkeys =  mNumKeys - instrkeynumber;
		GRKey::getNonFreeKeyArray(mynumkeys,mymkarray);
		keyarr = mymkarray;
	}

	NVPoint pos;
	pos.x = mPosition.x;
	// paint the accidentals ...
	// there are 7 possible accidentals altogether (regardless of key)
	for( int i = 0; i < 7; ++i )
	{
		// test for each accidental
		int j;
		// numkeys<0 means flats
		if( mynumkeys < 0 )	j = 6-i;
		else				j = i;

		// quint is an array that looks like half the Circel of Fifths
	    // beginning with F then going to C,G,D, A, E end ending at H/B 
	    //                      C
		//                 F          G
        //            B(flat)             D
	    //          Es                       A
	    //            As                  E
		//                Des          B/H
		//                    Ges/Fis
			  
	    //
		// if numkeys is positive, we need sharps and quint[j] just
	    // moves along the position, where sharps are set (fis,cis,gis etc.)
	    // if numkeys is negative we need flats beginning at quint[6-j]=B
	    // (B,Es,As,Des,Ges)
		if( keyarr[ quint[j] - NOTE_C ] != 0)
		{
			GRAccidental * acc;
			if (mNatural)
				acc = new GRAccidental( NULL, 0L, -10, mTagSize, mCurLSPACE );
			else
				acc = new GRAccidental( NULL,0L, keyarr[ quint[j] - NOTE_C ], mTagSize, mCurLSPACE );
			acc->setGRStaff( getGRStaff());
            if (mColRef)
                acc->setColRef(mColRef);
			pos.x += mCurLSPACE / 10 + acc->getLeftSpace();
			
			int mypitch = quint[j];
			if (instrkeynumber > 0)
				mypitch = quint[ (j + instrkeynumber ) % 7 ];
			else if (instrkeynumber < 0 )
				mypitch = quint[ (j + 21 + instrkeynumber ) % 7 ];
			
			pos.y = mGrStaff->getKeyPosition(mypitch, mynumkeys) - (mCurLSPACE * 3.5f * mOctarray[ quint[j] - NOTE_C ]);

			///pos.y = grstaff->getKeyPosition(1);
			acc->setPosition( pos );
			pos.x += acc->getRightSpace();
			AddTail( acc );
		}
	}
	updateBoundingBox();
}

void GRKey::recalcVerticalPosition()
{
	// remove the key-elements ...
	RemoveAllSubElements();
	createAccidentals();
}

void GRKey::updateBoundingBox()
{
	mLeftSpace = (+mCurLSPACE);
	mRightSpace = 0;
  	mBoundingBox.left = 0;
  	mBoundingBox.top = (-mCurLSPACE * 0.5f);
  	mBoundingBox.right = 0;
  	mBoundingBox.bottom = (mCurLSPACE * 0.5f);

	// for all elements
	GRNotationElement * e;
	GuidoPos pos = First();

	// - Todo: rewrite following code:

	while (pos)
	{
		e = GetNext(pos);
		
		const NVPoint & mypos = e->getPosition();
		if(( mypos.x - mPosition.x) + e->getRightSpace() > mRightSpace)
			mRightSpace = mypos.x - mPosition.x + e->getRightSpace();

		if(( mypos.x - mPosition.x) - e->getLeftSpace() < -mLeftSpace)
			mLeftSpace = -(mypos.x-mPosition.x - e->getLeftSpace());

		NVRect b (e->getBoundingBox());
		NVPoint p (e->getPosition());
	
		if (p.x + b.right > mPosition.x + mBoundingBox.right)
			mBoundingBox.right = p.x + b.right - mPosition.x;

		if (p.x + b.left < mPosition.x + mBoundingBox.left)
			mBoundingBox.left = p.x + b.left - mPosition.x;
	
		if (p.y + b.top < mPosition.y + mBoundingBox.top)
			mBoundingBox.top = p.y + b.top - mPosition.y;
		
		if (p.y + b.bottom > mPosition.y + mBoundingBox.bottom)
			mBoundingBox.bottom = p.y + b.bottom - mPosition.y;
	 }

	if (mBoundingBox.right > 0)
	{
		mBoundingBox.left -= GCoord(mCurLSPACE / 5);
		mBoundingBox.right += GCoord(mCurLSPACE * 0.5f);
	}
	if (mNatural)
		mRightSpace -= mCurLSPACE * 0.5f;
	else if (mRightSpace > 0)
		mRightSpace += mCurLSPACE * 0.5f;
	
}

void GRKey::OnDraw( VGDevice & hdc) const
{
	if (getError()) return;
	if(!mDraw || !mShow)
		return;
	DrawSubElements( hdc );
}

void GRKey::setGRStaff( GRStaff * inStaff )
{
	if( mGrStaff == 0 )
	{
		// if this is the first time!?, then, we need
		// to create the elements ...
		mGrStaff = inStaff;
		createAccidentals();
	}
	else
	{
		GRARCompositeNotationElement::setGRStaff( inStaff );
	}
}

bool GRKey::operator==(const GRKey & key) const
{
	return (mNumKeys == key.mNumKeys);
}

bool GRKey::operator==(const GRTag &tag) const
{
	const GRKey * key = dynamic_cast<const GRKey *>(&tag);
	if (key)
		return this->operator ==(*key);

	return false;
}

void GRKey::SendExtendedMap (const NVRect& map, ExtendedMapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const
{
	FloatRect r (map.left, map.top, map.right, map.bottom);
	r.Shift( infos.fPos.x, infos.fPos.y);
	r.Scale( infos.fScale.x, infos.fScale.y);

	GuidoDate from	= { date.getNumerator(), date.getDenominator() };
	TYPE_TIMEPOSITION end = date + dur;
	GuidoDate to	= { end.getNumerator(), end.getDenominator() };
	TimeSegment dates (from, to);			// current rolled segment
	GuidoElementInfos inf;
	inf.type = type;
	inf.staffNum = getStaffNumber();
	if (inf.staffNum < 0) inf.staffNum = 0;

	const ARMusicalObject * ar = getAbstractRepresentation();
	inf.voiceNum = ar ? ar->getVoiceNum() : 0;

    inf.midiPitch = this->mNumKeys;

	f.Graph2TimeMap (r, dates, inf, (void*)this);
}

void GRKey::SendExtendedMap (ExtendedMapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const
{
	NVRect calculatedBox;
	calculatedBox.top = mPosition.y;
	calculatedBox.left = mPosition.x;
	calculatedBox.bottom = mPosition.y + getBoundingBox().Height();
	calculatedBox.right = mPosition.x + getBoundingBox().Width() - 5;
	SendExtendedMap (calculatedBox, f, date, dur, type, infos);
}

void GRKey::GetExtendedMap( GuidoElementSelector sel, ExtendedMapCollector& f, MapInfos& infos) const {
	if (sel == kTagSel) {
		SendExtendedMap(f, getRelativeTimePosition(), getDuration(), kKey, infos);
	}
}
