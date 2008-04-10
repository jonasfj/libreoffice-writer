/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dflyobj.hxx,v $
 * $Revision: 1.10 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _DFLYOBJ_HXX
#define	_DFLYOBJ_HXX
#include <svx/svdovirt.hxx>

class SwFlyFrm;
class SwFrmFmt;
class SdrObjMacroHitRec;

const UINT32 SWGInventor =	UINT32('S')*0x00000001+
                            UINT32('W')*0x00000100+
                            UINT32('G')*0x00010000;

const UINT16 SwFlyDrawObjIdentifier = 0x0001;
const UINT16 SwDrawFirst			= 0x0001;

//---------------------------------------
//SwFlyDrawObj, Die DrawObjekte fuer Flys.

class SwFlyDrawObj : public SdrObject
{
    virtual sdr::properties::BaseProperties* CreateObjectSpecificProperties();

public:
    TYPEINFO();

    SwFlyDrawObj();
    ~SwFlyDrawObj();

    virtual sal_Bool DoPaintObject(XOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const;

    //Damit eine Instanz dieser Klasse beim laden erzeugt werden kann
    //(per Factory).
    virtual UINT32 GetObjInventor()		const;
    virtual UINT16 GetObjIdentifier()	const;
    virtual UINT16 GetObjVersion()		const;
};

//---------------------------------------
//SwVirtFlyDrawObj, die virtuellen Objekte fuer Flys.
//Flys werden immer mit virtuellen Objekten angezeigt. Nur so koennen sie
//ggf. mehrfach angezeigt werden (Kopf-/Fusszeilen).

class SwVirtFlyDrawObj : public SdrVirtObj
{
    SwFlyFrm *pFlyFrm;

public:
    TYPEINFO();

    SwVirtFlyDrawObj(SdrObject& rNew, SwFlyFrm* pFly);
    ~SwVirtFlyDrawObj();

    //Ueberladene Methoden der Basisklasse SdrVirtObj
    virtual SdrObject* CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const;
    virtual sal_Bool DoPaintObject(XOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const;
    virtual void  	 TakeObjInfo( SdrObjTransformInfoRec& rInfo ) const;

    //Wir nehemen die Groessenbehandlung vollstaendig selbst in die Hand.
    virtual const Rectangle& GetCurrentBoundRect() const;
    virtual		  void		 RecalcBoundRect();
    virtual		  void		 RecalcSnapRect();
    virtual const Rectangle& GetSnapRect()	const;
    virtual		  void		 SetSnapRect(const Rectangle& rRect);
    virtual		  void		 NbcSetSnapRect(const Rectangle& rRect);
    virtual const Rectangle& GetLogicRect() const;
    virtual		  void		 SetLogicRect(const Rectangle& rRect);
    virtual		  void		 NbcSetLogicRect(const Rectangle& rRect);
    virtual ::basegfx::B2DPolyPolygon TakeXorPoly(sal_Bool bDetail) const;
    virtual		  void		 NbcMove  (const Size& rSiz);
    virtual		  void		 NbcResize(const Point& rRef, const Fraction& xFact,
                                       const Fraction& yFact);
    virtual		  void		 Move  (const Size& rSiz);
    virtual		  void		 Resize(const Point& rRef, const Fraction& xFact,
                                    const Fraction& yFact);

    const SwFrmFmt *GetFmt() const;
          SwFrmFmt *GetFmt();

    // Get Methoden fuer die Fly Verpointerung
          SwFlyFrm* GetFlyFrm()			{ return pFlyFrm; }
    const SwFlyFrm* GetFlyFrm() const	{ return pFlyFrm; }

    void SetRect() const;

    // ist eine URL an einer Grafik gesetzt, dann ist das ein Makro-Object
    virtual FASTBOOL HasMacro() const;
    virtual SdrObject* CheckMacroHit       (const SdrObjMacroHitRec& rRec) const;
    virtual Pointer    GetMacroPointer     (const SdrObjMacroHitRec& rRec) const;
};


#endif
