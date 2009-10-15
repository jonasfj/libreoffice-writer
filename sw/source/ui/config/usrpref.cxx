/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: usrpref.cxx,v $
 * $Revision: 1.39 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <tools/stream.hxx>
#include <unotools/syslocale.hxx>

#include "swtypes.hxx"
#include "hintids.hxx"
#include "uitool.hxx"
#include "usrpref.hxx"
#include "crstate.hxx"
#include <linguistic/lngprops.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <unotools/localedatawrapper.hxx>

#include <unomid.h>

using namespace utl;
using namespace rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


void SwMasterUsrPref::SetUsrPref(const SwViewOption &rCopy)
{
    *((SwViewOption*)this) = rCopy;
}

SwMasterUsrPref::SwMasterUsrPref(BOOL bWeb) :
    eFldUpdateFlags(AUTOUPD_OFF),
    nLinkUpdateMode(0),
    bIsHScrollMetricSet(sal_False),
    bIsVScrollMetricSet(sal_False),
    nDefTab( MM50 * 4 ),
    bIsSquaredPageMode(sal_False),
    aContentConfig(bWeb, *this),
    aLayoutConfig(bWeb, *this),
    aGridConfig(bWeb, *this),
    aCursorConfig(*this),
    pWebColorConfig(bWeb ? new SwWebColorConfig(*this) : 0)
{
    MeasurementSystem eSystem = SvtSysLocale().GetLocaleData().getMeasurementSystemEnum();
    eUserMetric = MEASURE_METRIC == eSystem ? FUNIT_CM : FUNIT_INCH;
    eHScrollMetric = eUserMetric;
    eVScrollMetric = eUserMetric;

    aContentConfig.Load();
    aLayoutConfig.Load();
    aGridConfig.Load();
    aCursorConfig.Load();
    if(pWebColorConfig)
        pWebColorConfig->Load();
}
/* -----------------------------13.02.01 09:48--------------------------------

 ---------------------------------------------------------------------------*/
SwMasterUsrPref::~SwMasterUsrPref()
{
    delete pWebColorConfig;
}
/*-- 28.09.00 09:55:32---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence<OUString> SwContentViewConfig::GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "Display/GraphicObject",   					// 	0
        "Display/Table",							//  1
        "Display/DrawingControl",  					//  2
        "Display/FieldCode",	   					//  3
        "Display/Note",								//  4
        "Display/PreventTips",                      //  5
        "NonprintingCharacter/MetaCharacters",     //   6
        "NonprintingCharacter/ParagraphEnd",        //  7
        "NonprintingCharacter/OptionalHyphen",      //  8
        "NonprintingCharacter/Space",               //  9
        "NonprintingCharacter/Break",               // 10
        "NonprintingCharacter/ProtectedSpace",      // 11
            "NonprintingCharacter/Tab",             // 12 //not in Writer/Web
            "NonprintingCharacter/HiddenText",      // 13
            "NonprintingCharacter/HiddenParagraph", // 14
            "NonprintingCharacter/HiddenCharacter",      // 15
            "Update/Link",                          // 16
            "Update/Field",                         // 17
            "Update/Chart"                          // 18


    };
    const int nCount = bWeb ? 12 : 19;
    Sequence<OUString> aNames(nCount);
    OUString* pNames = aNames.getArray();
    for(int i = 0; i < nCount; i++)
    {
        pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}
/*-- 28.09.00 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwContentViewConfig::SwContentViewConfig(BOOL bIsWeb, SwMasterUsrPref& rPar) :
    ConfigItem(bIsWeb ? C2U("Office.WriterWeb/Content") :  C2U("Office.Writer/Content")),
    rParent(rPar),
    bWeb(bIsWeb)
{
    Load();
    EnableNotification( GetPropertyNames() );
}
/*-- 28.09.00 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwContentViewConfig::~SwContentViewConfig()
{
}
/*-- 09.02.07 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwContentViewConfig::Notify( const Sequence< OUString > & /*rPropertyNames*/ )
{
    Load();
}
/*-- 28.09.00 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwContentViewConfig::Commit()
{
    Sequence<OUString> aNames = GetPropertyNames();

    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        sal_Bool bVal = FALSE;
        switch(nProp)
        {
            case  0: bVal = rParent.IsGraphic();	break;// "Display/GraphicObject",
            case  1: bVal = rParent.IsTable();	break;// "Display/Table",
            case  2: bVal = rParent.IsDraw();    	break;// "Display/DrawingControl",
            case  3: bVal = rParent.IsFldName(); 	break;// "Display/FieldCode",
            case  4: bVal = rParent.IsPostIts(); 	break;// "Display/Note",
            case  5: bVal = rParent.IsPreventTips(); break; // "Display/PreventTips"
            case  6: bVal = rParent.IsViewMetaChars(); break; //"NonprintingCharacter/MetaCharacters"
            case  7: bVal = rParent.IsParagraph(sal_True); break;// "NonprintingCharacter/ParagraphEnd",
            case  8: bVal = rParent.IsSoftHyph(); break;// "NonprintingCharacter/OptionalHyphen",
            case  9: bVal = rParent.IsBlank(sal_True);  break;// "NonprintingCharacter/Space",
            case 10: bVal = rParent.IsLineBreak(sal_True);break;// "NonprintingCharacter/Break",
            case 11: bVal = rParent.IsHardBlank(); break;// "NonprintingCharacter/ProtectedSpace",
            case 12: bVal = rParent.IsTab(sal_True);        break;// "NonprintingCharacter/Tab",
            case 13: bVal = rParent.IsShowHiddenField(); break;// "NonprintingCharacter/Fields: HiddenText",
            case 14: bVal = rParent.IsShowHiddenPara(); break;// "NonprintingCharacter/Fields: HiddenParagraph",
            case 15: bVal = rParent.IsShowHiddenChar(sal_True);    break;// "NonprintingCharacter/HiddenCharacter",
            case 16: pValues[nProp] <<= rParent.GetUpdateLinkMode();    break;// "Update/Link",
            case 17: bVal = rParent.IsUpdateFields(); break;// "Update/Field",
            case 18: bVal = rParent.IsUpdateCharts(); break;// "Update/Chart"
        }
        if(nProp != 16)
            pValues[nProp].setValue(&bVal, ::getBooleanCppuType());
    }
    PutProperties(aNames, aValues);
}
/*-- 28.09.00 09:55:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwContentViewConfig::Load()
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                sal_Bool bSet = nProp != 16 ? *(sal_Bool*)pValues[nProp].getValue() : sal_False;
                switch(nProp)
                {
                    case  0: rParent.SetGraphic(bSet);	break;// "Display/GraphicObject",
                    case  1: rParent.SetTable(bSet);	break;// "Display/Table",
                    case  2: rParent.SetDraw(bSet);    	break;// "Display/DrawingControl",
                    case  3: rParent.SetFldName(bSet); 	break;// "Display/FieldCode",
                    case  4: rParent.SetPostIts(bSet); 	break;// "Display/Note",
                    case  5: rParent.SetPreventTips(bSet);  break;// "Display/PreventTips",
                    case  6: rParent.SetViewMetaChars(bSet); break; //"NonprintingCharacter/MetaCharacters"
                    case  7: rParent.SetParagraph(bSet); break;// "NonprintingCharacter/ParagraphEnd",
                    case  8: rParent.SetSoftHyph(bSet); break;// "NonprintingCharacter/OptionalHyphen",
                    case  9: rParent.SetBlank(bSet);    break;// "NonprintingCharacter/Space",
                    case 10: rParent.SetLineBreak(bSet);break;// "NonprintingCharacter/Break",
                    case 11: rParent.SetHardBlank(bSet); break;// "NonprintingCharacter/ProtectedSpace",
                    case 12: rParent.SetTab(bSet);      break;// "NonprintingCharacter/Tab",
                    case 13: rParent.SetShowHiddenField(bSet);   break;// "NonprintingCharacter/Fields: HiddenText",
                    case 14: rParent.SetShowHiddenPara(bSet); break;// "NonprintingCharacter/Fields: HiddenParagraph",
                    case 15: rParent.SetShowHiddenChar(bSet); break;// "NonprintingCharacter/HiddenCharacter",
                    case 16:
                    {
                        sal_Int32 nSet = 0;
                        pValues[nProp] >>= nSet;
                        rParent.SetUpdateLinkMode(nSet, TRUE);
                    }
                    break;// "Update/Link",
                    case 17: rParent.SetUpdateFields(bSet, TRUE); break;// "Update/Field",
                    case 18: rParent.SetUpdateCharts(bSet, TRUE); break;// "Update/Chart"
                }
            }
        }
    }
}
/*-- 28.09.00 09:55:34---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence<OUString> SwLayoutViewConfig::GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "Line/Guide",                       // 0
        "Line/SimpleControlPoint",          // 1
        "Line/LargeControlPoint",           // 2
        "Window/HorizontalScroll",          // 3
        "Window/VerticalScroll",            // 4
        "Window/ShowRulers",                // 5
        "Window/HorizontalRuler",           // 6
        "Window/VerticalRuler",             // 7
        "Window/HorizontalRulerUnit",       // 8
        "Window/VerticalRulerUnit",         // 9
        "Window/SmoothScroll",              //10
        "Zoom/Value",                       //11
        "Zoom/Type",                        //12
        "Other/MeasureUnit",                //13
        "Other/TabStop",                    //14
        "Window/IsVerticalRulerRight",      //15
        "ViewLayout/Columns",               //16
        "ViewLayout/BookMode",              //17
        "Other/IsSquaredPageMode"           //18
    };
    const int nCount = bWeb ? 14 : 19;
    Sequence<OUString> aNames(nCount);
    OUString* pNames = aNames.getArray();
    for(int i = 0; i < nCount; i++)
    {
        pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}
/*-- 28.09.00 09:55:34---------------------------------------------------

  -----------------------------------------------------------------------*/
SwLayoutViewConfig::SwLayoutViewConfig(BOOL bIsWeb, SwMasterUsrPref& rPar) :
    ConfigItem(bIsWeb ? C2U("Office.WriterWeb/Layout") :  C2U("Office.Writer/Layout"),
        CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
    rParent(rPar),
    bWeb(bIsWeb)
{
}
/*-- 28.09.00 09:55:35---------------------------------------------------

  -----------------------------------------------------------------------*/
SwLayoutViewConfig::~SwLayoutViewConfig()
{
}
/*-- 28.09.00 09:55:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwLayoutViewConfig::Commit()
{
    Sequence<OUString> aNames = GetPropertyNames();

    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        sal_Bool bSet;
        switch(nProp)
        {
            case  0: bSet = rParent.IsCrossHair(); break;// "Line/Guide",
            case  1: bSet = rParent.IsSolidMarkHdl(); break;// "Line/SimpleControlPoint",
            case  2: bSet = rParent.IsBigMarkHdl(); break;// "Line/LargeControlPoint",
            case  3: bSet = rParent.IsViewHScrollBar(); break;// "Window/HorizontalScroll",
            case  4: bSet = rParent.IsViewVScrollBar(); break;// "Window/VerticalScroll",
            case  5: bSet = rParent.IsViewAnyRuler(); break; // "Window/ShowRulers"
            // #i14593# use IsView*Ruler(TRUE) instead of IsView*Ruler()
            // this preserves the single ruler states even if "Window/ShowRulers" is off
            case  6: bSet = rParent.IsViewHRuler(TRUE); break;// "Window/HorizontalRuler",
            case  7: bSet = rParent.IsViewVRuler(TRUE); break;// "Window/VerticalRuler",
            case  8:
                if(rParent.bIsHScrollMetricSet)
                    pValues[nProp] <<= (sal_Int32)rParent.eHScrollMetric; // "Window/HorizontalRulerUnit"
            break;
            case  9:
                if(rParent.bIsVScrollMetricSet)
                    pValues[nProp] <<= (sal_Int32)rParent.eVScrollMetric; // "Window/VerticalRulerUnit"
            break;
            case 10: bSet = rParent.IsSmoothScroll(); break;// "Window/SmoothScroll",
            case 11: pValues[nProp] <<= (sal_Int32)rParent.GetZoom(); break;// "Zoom/Value",
            case 12: pValues[nProp] <<= (sal_Int32)rParent.GetZoomType(); break;// "Zoom/Type",
            case 13: pValues[nProp] <<= (sal_Int32)rParent.GetMetric(); break;// "Other/MeasureUnit",
            case 14: pValues[nProp] <<= static_cast<sal_Int32>(TWIP_TO_MM100(rParent.GetDefTab())); break;// "Other/TabStop",
            case 15: bSet = rParent.IsVRulerRight(); break;// "Window/IsVerticalRulerRight",
            case 16: pValues[nProp] <<= (sal_Int32)rParent.GetViewLayoutColumns(); break;// "ViewLayout/Columns",
            case 17: bSet = rParent.IsViewLayoutBookMode(); break;// "ViewLayout/BookMode",
            case 18: bSet = rParent.IsSquaredPageMode(); break;// "Other/IsSquaredPageMode",
        }
        if(nProp < 8 || nProp == 10 || nProp == 15 || nProp == 17 || nProp == 18 )
            pValues[nProp].setValue(&bSet, ::getBooleanCppuType());
    }
    PutProperties(aNames, aValues);
}
/*-- 28.09.00 09:55:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwLayoutViewConfig::Load()
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                sal_Bool bSet = nProp < 8 || nProp == 10 || nProp == 17 || nProp == 18 ? *(sal_Bool*)pValues[nProp].getValue() : sal_False;
                switch(nProp)
                {
                    case  0: rParent.SetCrossHair(bSet); break;// "Line/Guide",
                    case  1: rParent.SetSolidMarkHdl(bSet); break;// "Line/SimpleControlPoint",
                    case  2: rParent.SetBigMarkHdl(bSet); break;// "Line/LargeControlPoint",
                    case  3: rParent.SetViewHScrollBar(bSet); break;// "Window/HorizontalScroll",
                    case  4: rParent.SetViewVScrollBar(bSet); break;// "Window/VerticalScroll",
                    case  5: rParent.SetViewAnyRuler(bSet);break; // "Window/ShowRulers"
                    case  6: rParent.SetViewHRuler(bSet); break;// "Window/HorizontalRuler",
                    case  7: rParent.SetViewVRuler(bSet); break;// "Window/VerticalRuler",
                    case  8:
                    {
                        rParent.bIsHScrollMetricSet = sal_True;
                        sal_Int32 nUnit = 0;
                        pValues[nProp] >>= nUnit;
                        rParent.eHScrollMetric = ((FieldUnit)nUnit);  // "Window/HorizontalRulerUnit"
                    }
                    break;
                    case  9:
                    {
                        rParent.bIsVScrollMetricSet = sal_True;
                        sal_Int32 nUnit = 0;
                        pValues[nProp] >>= nUnit;
                        rParent.eVScrollMetric = ((FieldUnit)nUnit); // "Window/VerticalRulerUnit"
                    }
                    break;
                    case 10: rParent.SetSmoothScroll(bSet); break;// "Window/SmoothScroll",
                    case 11:
                    {
                        sal_Int32 nVal = 0;
                        pValues[nProp] >>= nVal;
                        rParent.SetZoom( static_cast< USHORT >(nVal) );
                    }
                    break;// "Zoom/Value",
                    case 12:
                    {
                        sal_Int32 nVal = 0;
                        pValues[nProp] >>= nVal;
                        rParent.SetZoomType( static_cast< SvxZoomType >(nVal) );
                    }
                    break;// "Zoom/Type",
                    case 13:
                    {
                        sal_Int32 nUnit = 0;
                        pValues[nProp] >>= nUnit;
                        rParent.SetMetric((FieldUnit)nUnit, TRUE);
                    }
                    break;// "Other/MeasureUnit",
                    case 14:
                    {
                        sal_Int32 nTab = 0;
                        pValues[nProp] >>= nTab;
                        rParent.SetDefTab(MM100_TO_TWIP(nTab), TRUE);
                    }
                    break;// "Other/TabStop",
                    case 15: rParent.SetVRulerRight(bSet); break;// "Window/IsVerticalRulerRight",
                    case 16:
                    {
                        sal_Int32 nColumns = 0;
                        pValues[nProp] >>= nColumns;
                        rParent.SetViewLayoutColumns( static_cast<USHORT>(nColumns) );
                    }
                    break;// "ViewLayout/Columns",
                    case 17: rParent.SetViewLayoutBookMode(bSet); break;// "ViewLayout/BookMode",
                    case 18: rParent.SetDefaultPageMode(bSet,TRUE); break;// "Other/IsSquaredPageMode",
                }
            }
        }
    }
}

void SwLayoutViewConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}

/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
Sequence<OUString> SwGridConfig::GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "Option/SnapToGrid",			// 0
        "Option/VisibleGrid",			// 1
        "Option/Synchronize",			// 2
        "Resolution/XAxis",				// 3
        "Resolution/YAxis",				// 4
        "Subdivision/XAxis",			// 5
        "Subdivision/YAxis"				// 6
    };
    const int nCount = 7;
    Sequence<OUString> aNames(nCount);
    OUString* pNames = aNames.getArray();
    for(int i = 0; i < nCount; i++)
    {
        pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
SwGridConfig::SwGridConfig(BOOL bIsWeb, SwMasterUsrPref& rPar) :
    ConfigItem(bIsWeb ? C2U("Office.WriterWeb/Grid") :  C2U("Office.Writer/Grid"),
        CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
    rParent(rPar),
    bWeb(bIsWeb)
{
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
SwGridConfig::~SwGridConfig()
{
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
void SwGridConfig::Commit()
{
    Sequence<OUString> aNames = GetPropertyNames();

    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        sal_Bool bSet;
        switch(nProp)
        {
            case  0: bSet = rParent.IsSnap(); break;// 		"Option/SnapToGrid",
            case  1: bSet = rParent.IsGridVisible(); break;//"Option/VisibleGrid",
            case  2: bSet = rParent.IsSynchronize(); break;//  "Option/Synchronize",
            case  3: pValues[nProp] <<= (sal_Int32)TWIP_TO_MM100(rParent.GetSnapSize().Width()); break;//      "Resolution/XAxis",
            case  4: pValues[nProp] <<= (sal_Int32)TWIP_TO_MM100(rParent.GetSnapSize().Height()); break;//      "Resolution/YAxis",
            case  5: pValues[nProp] <<= (sal_Int16)rParent.GetDivisionX(); break;//   "Subdivision/XAxis",
            case  6: pValues[nProp] <<= (sal_Int16)rParent.GetDivisionY(); break;//   "Subdivision/YAxis"
        }
        if(nProp < 3)
              pValues[nProp].setValue(&bSet, ::getBooleanCppuType());
    }
    PutProperties(aNames, aValues);
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
void SwGridConfig::Load()
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        Size aSnap(rParent.GetSnapSize());
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                sal_Bool bSet = nProp < 3 ? *(sal_Bool*)pValues[nProp].getValue() : sal_False;
                sal_Int32 nSet = 0;
                if(nProp >= 3)
                    pValues[nProp] >>= nSet;
                switch(nProp)
                {
                    case  0: rParent.SetSnap(bSet); break;// 		"Option/SnapToGrid",
                    case  1: rParent.SetGridVisible(bSet); break;//"Option/VisibleGrid",
                    case  2: rParent.SetSynchronize(bSet); break;//  "Option/Synchronize",
                    case  3: aSnap.Width() = MM100_TO_TWIP(nSet); break;//      "Resolution/XAxis",
                    case  4: aSnap.Height() = MM100_TO_TWIP(nSet); break;//      "Resolution/YAxis",
                    case  5: rParent.SetDivisionX((short)nSet); break;//   "Subdivision/XAxis",
                    case  6: rParent.SetDivisionY((short)nSet); break;//   "Subdivision/YAxis"
                }
            }
        }
        rParent.SetSnapSize(aSnap);
    }
}

void SwGridConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}

/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
Sequence<OUString> SwCursorConfig::GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "DirectCursor/UseDirectCursor",	// 0
        "DirectCursor/Insert",			// 1
        "Option/ProtectedArea"          // 2
    };
    const int nCount = 3;
    Sequence<OUString> aNames(nCount);
    OUString* pNames = aNames.getArray();
    for(int i = 0; i < nCount; i++)
        pNames[i] = C2U(aPropNames[i]);
    return aNames;
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
SwCursorConfig::SwCursorConfig(SwMasterUsrPref& rPar) :
    ConfigItem(C2U("Office.Writer/Cursor"),
        CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
    rParent(rPar)
{
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
SwCursorConfig::~SwCursorConfig()
{
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
void SwCursorConfig::Commit()
{
    Sequence<OUString> aNames = GetPropertyNames();

    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        sal_Bool bSet;
        switch(nProp)
        {
            case  0: bSet = rParent.IsShadowCursor(); 		break;//  "DirectCursor/UseDirectCursor",
            case  1: pValues[nProp] <<= (sal_Int32)rParent.GetShdwCrsrFillMode(); 	break;//  "DirectCursor/Insert",
            case  2: bSet = rParent.IsCursorInProtectedArea(); break;// "Option/ProtectedArea"
        }
        if(nProp != 1 )
              pValues[nProp].setValue(&bSet, ::getBooleanCppuType());
    }
    PutProperties(aNames, aValues);
}
/* -----------------------------19.01.01 13:07--------------------------------

 ---------------------------------------------------------------------------*/
void SwCursorConfig::Load()
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {

        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                sal_Bool bSet = sal_False;
                sal_Int32 nSet = 0;
                if(nProp != 1 )
                    bSet = *(sal_Bool*)pValues[nProp].getValue();
                else
                    pValues[nProp] >>= nSet;
                switch(nProp)
                {
                    case  0: rParent.SetShadowCursor(bSet); 		break;//  "DirectCursor/UseDirectCursor",
                    case  1: rParent.SetShdwCrsrFillMode((BYTE)nSet); break;//  "DirectCursor/Insert",
                    case  2: rParent.SetCursorInProtectedArea(bSet); break;// "Option/ProtectedArea"
                }
            }
        }

    }
}

void SwCursorConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}

/*-- 28.09.00 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwWebColorConfig::SwWebColorConfig(SwMasterUsrPref& rPar) :
    ConfigItem(C2U("Office.WriterWeb/Background"),
        CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
    rParent(rPar),
    aPropNames(1)
{
    aPropNames.getArray()[0] = C2U("Color");
}
/*-- 28.09.00 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwWebColorConfig::~SwWebColorConfig()
{
}
/*-- 28.09.00 09:55:33---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwWebColorConfig::Commit()
{
    Sequence<Any> aValues(aPropNames.getLength());
    Any* pValues = aValues.getArray();
    for(int nProp = 0; nProp < aPropNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case  0: pValues[nProp] <<= (sal_Int32)rParent.GetRetoucheColor().GetColor();	break;// "Color",
        }
    }
    PutProperties(aPropNames, aValues);
}

void SwWebColorConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}

/*-- 28.09.00 09:55:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwWebColorConfig::Load()
{
    Sequence<Any> aValues = GetProperties(aPropNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aPropNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aPropNames.getLength())
    {
        for(int nProp = 0; nProp < aPropNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case  0:
                        sal_Int32 nSet = 0;
                        pValues[nProp] >>= nSet; rParent.SetRetoucheColor(nSet);
                    break;// "Color",
                }
            }
        }
    }
}


