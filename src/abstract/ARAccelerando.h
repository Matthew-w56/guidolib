#ifndef ARAccelerando_H
#define ARAccelerando_H

/*
  GUIDO Library
  Copyright (C) 2002  Holger Hoos, Juergen Kilian, Kai Renz
  Copyright (C) 2002-2013 Grame

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr

*/


#include "ARMTParameter.h"
#include "ARPositionTag.h"
#include "TagParameterString.h"

class TagParameterString;

/** \brief The Accelerando position tag
*/
class ARAccelerando :  public ARMTParameter,  public ARPositionTag
{		
public:			

	virtual bool MatchEndTag(const char * s);


	ARAccelerando() : ARMTParameter()
	{
		rangesetting = ONLY;
		s1 = 0;
		s2 = 0;
	}

	virtual ~ARAccelerando();

	virtual void setTagParameterList(TagParameterList & tpl);

	// routines to access TagParameters ..
	virtual void print() const;
	
	virtual void PrintName(std::ostream & os) const;
	virtual void PrintParameters(std::ostream & os) const;
	TagParameterString * getTempo(){return s1;}
	TagParameterString * getAbsTempo(){return s2;}	
	const char* getFont() const	{ return font ? font->getValue() : 0; }
	void		setFont(const TagParameterString * ft);
	const char*	getFAttrib() const { return fattrib ? fattrib->getValue() : 0; }
	void		setFAttrib(const TagParameterString * fa);

protected:

	// TagParameters ..
	TagParameterString * s1;
	TagParameterString * s2;
	TagParameterString * font;
	TagParameterString * fattrib;
	// TagParameterFloat  *fsize;
	// TagParameterInt	  *fnumber;


	// this is important for allowed
	// TagParameters ....
	static ListOfTPLs ltpls;
};

#endif
