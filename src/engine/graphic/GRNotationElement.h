#ifndef GRNotationElement_H
#define GRNotationElement_H

/*
  GUIDO Library
  Copyright (C) 2002  Holger Hoos, Juergen Kilian, Kai Renz
  Copyright (C) 2003, 2004 	Grame

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr

*/

#include <cassert>
#include <iostream>

#include "GObject.h"
#include "defines.h"	// for TYPE_DURATION
#include "GRVisitable.h"
#include "GRVisitor.h"

class ARMusicalObject;
class GRAccidental;
class GRBar;
class GREvent;
class GRFingering;
class GRGlue;
class GRHarmony;
class GRNote;
class GRRest;
class GRSingleNote;
class GRStaff;
class GRSystemSlice;
class GRTempo;
class GRText;
class GRTrill;
class NEPointerList;

#ifdef SMUFL
#define DEFAULTNUMERICSPACE	6
#else
#define DEFAULTNUMERICSPACE	0
#endif


const float kMinNoteSize = 0.001f;	// minimum size of an element, the element is not drawn when the size is smaller

/** \brief parent class for all notation elements.
*/
class GRNotationElement : public GObject, public GRVisitable
{
public:
					GRNotationElement();
	virtual		 	~GRNotationElement();

	static int		ComparePos( const GRNotationElement * g1, const GRNotationElement * g2 );

			GRStaff * getGRStaff() const				{ return mGrStaff; }
	virtual void	setGRStaff( GRStaff * in )			{ mGrStaff = in; }

			GRSystemSlice * getGRSystemSlice() const;
	virtual int		getStaffNumber() const;

	virtual void	tellPosition( GObject * caller, const NVPoint & point);
		
	virtual float	getLeftSpace() const 				{ return mLeftSpace; }
	virtual float	getRightSpace() const 				{ return mRightSpace; }

	virtual void	setLeftSpace( float lspace ) 		{ mLeftSpace = lspace; }
	virtual void	setRightSpace( float rspace ) 		{ mRightSpace = rspace; }

	virtual void	setHPosition( float nx );
	virtual void	setPosition( const NVPoint & newPosition);

	virtual void	setNeedsSpring( int inFlag )		{ mNeedsSpring = inFlag; }
	virtual int		getNeedsSpring() const				{ return mNeedsSpring; }
	virtual int		getSpringID() const 				{ return  mSpringID; }
	virtual void	tellSpringID( int inSpringID ) 		{ mSpringID = inSpringID; }

			NEPointerList * getAssociations()			{ return mAssociated; }
			const NEPointerList * getAssociations()	const { return mAssociated; }

	virtual void removeAssociation( const NEPointerList & nl );
	virtual void removeAssociation( GRNotationElement * el );
			void FreeAssociatedList();

	virtual NVPoint getTieEnd() const					{ return mPosition; } // do not rely on that
	virtual NVPoint getTieStart() const					{ return mPosition; }
	virtual NVRect  getAssociatedBoundingBox() const;

			void addToBoundingBox( GRNotationElement * in );


	
	// - Drawing	
	virtual void OnDraw( VGDevice & hdc ) const;
			void OnDrawSymbol(VGDevice & hdc, unsigned int inSymbol, float inOffsetX = 0,
								float inOffsetY = 0, float inFontSize = 0) const; // , float inScaleX = 0) const;
			void OnDrawText( VGDevice & hdc, NVPoint pos, const char * cp, int inCharCount = -1 ) const;
			void OnDrawText( VGDevice & hdc, const char * cp, int inCharCount = -1 ) const;
			void DrawSymbol( VGDevice & hdc, unsigned int inSymbol, float x = 0, float y = 0, float size = 0) const;
			void DrawNumericSymbols( VGDevice & hdc, const char* symbols, float x, float y, float size, float spacing=DEFAULTNUMERICSPACE) const;
			void Show( bool status )		{ mShow = status; }
			bool GetShow() const			{ return mShow; }

	virtual void SendMap (const NVRect& r, MapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const;
	virtual void SendExtendedMap (const NVRect& r, ExtendedMapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const;
	virtual void SendMap (MapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const;
	virtual void SendExtendedMap (ExtendedMapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const;
	virtual unsigned int getTextAlign() const;
	
	// 
	virtual const ARMusicalObject * getAbstractRepresentation() const		{ return 0; }
	virtual const TYPE_DURATION & getDuration() const						{ return mDurationOfGR; }
	
	virtual void setDuration(const TYPE_TIMEPOSITION & dur )					{ mDurationOfGR = dur; }
	virtual void setRelativeTimePosition(const TYPE_TIMEPOSITION & tp )			{ mRelativeTimePositionOfGR = tp; }
	virtual void setRelativeEndTimePosition(const TYPE_TIMEPOSITION & tp )		{ mDurationOfGR = tp - mRelativeTimePositionOfGR; }
	virtual const TYPE_TIMEPOSITION & getRelativeTimePosition() const			{ return mRelativeTimePositionOfGR; }
	
	virtual void setDrawOnOff(bool onoff)	{ mDraw = onoff;}
	virtual bool getDrawOnOff()				{ return mDraw;}
	
	virtual TYPE_TIMEPOSITION  getRelativeEndTimePosition() const;
	
	virtual void recalcVerticalPosition();
	virtual void addAssociation( GRNotationElement * grnot );
	virtual void accept (GRVisitor& visitor);

	// - Text, streams
    virtual void print(std::ostream& os) const;
	inline const NEPointerList* associated() const { return mAssociated; }

    /**** Functions to avoid dynamic_cast ****/
    /*****************************************/
    
    virtual void setIsInHeader(bool state)	{ mIsInHeader = state; }
    virtual bool isInHeader() const			{ return false; }
    virtual bool isEmpty() const			{ return false; }
    virtual bool isLyrics() const			{ return false; }
    virtual bool isChordComma() const		{ return false; }
    virtual bool checkCollisionWith() const	{ return false; }

    virtual const GRText*		isText() const			{ return 0; }
    virtual const GRSingleNote* isSingleNote() const	{ return 0; }
    virtual 	  GRSingleNote* isSingleNote() 			{ return 0; }
    virtual const GRNote *		isGRNote() const		{ return 0; }
    virtual const GRRest *		isRest() const			{ return 0; }
    virtual const GREvent *		isGREvent() const		{ return 0; }
    virtual 	  GREvent *		isGREvent() 			{ return 0; }
    virtual const GRGlue *		isGRGlue() const		{ return 0; }
    virtual const GRTempo *		isGRTempo() const		{ return 0; }
    virtual const GRTrill *		isGRTrill() const		{ return 0; }
    virtual const GRBar *		isGRBar() const			{ return 0; }
    virtual const GRDynamics *	isGRDynamic() const		{ return 0; }
    virtual 	  GRFingering *	isGRFingering() 		{ return 0; }
    virtual 	  GRSlur *		isGRSlur() 				{ return 0; }
    virtual const GRHarmony *	isGRHarmony() const 	{ return 0; }

protected:
	
	void		DrawExtents( VGDevice & hdc, const VGColor & inColor ) const; // Debug

	unsigned int mSymbol;
	GRStaff * 	mGrStaff;
	int  		mNeedsSpring;	// Can be -1, 0 or 1
	int  		mSpringID;
	float 		mLeftSpace;		// Can't we deal only with bounding boxes ?
	float 		mRightSpace;
	bool		mDraw;		 // a flag to show or hide an element (default is true) connected to staff on/off
	bool		mShow;		 // a flag to show or hide an element (default is true) used by GuidoShowElement
    bool        mIsInHeader; // For proportional rendering

	TYPE_DURATION mDurationOfGR;
	NEPointerList * mAssociated;

private:
	TYPE_TIMEPOSITION mRelativeTimePositionOfGR;
};

std::ostream& operator<< (std::ostream& os, const GRNotationElement& e);
std::ostream& operator<< (std::ostream& os, const GRNotationElement* e);


/** \brief This comparison routine compares to notationelements
	 by y-position. This way the association-list of chords is sorted.
*/
/*
 previous implementation made use of template declared as below
 template <class T> int compnotposy( const T * el1, const T * el2 );
 since gcc 4.0, template has been removed and replaced by the 2 specialized
 functions declared below and implemented in cpp file.
*/
int compnotposy( const GRNotationElement * el1, const GRNotationElement * el2 );
int compaccposy( const GRAccidental * el1, const GRAccidental * el2 );

#endif
