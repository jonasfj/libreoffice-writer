/*************************************************************************
 *
 *  $RCSfile: unoevent.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: dvo $ $Date: 2001-01-02 14:29:24 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

// HINTIDs must be on top; it is required for the macitem.hxx header
#ifndef _HINTIDS_HXX
#include "hintids.hxx"
#endif

#ifndef _UNOEVENT_HXX
#include "unoevent.hxx"
#endif

#ifndef _UNOFRAME_HXX
#include "unoframe.hxx"
#endif

#ifndef _UNOSTYLE_HXX
#include "unostyle.hxx"
#endif

#ifndef _SWEVENT_HXX
#include "swevent.hxx"
#endif

#ifndef _DOCSTYLE_HXX
#include "docstyle.hxx"
#endif

#ifndef _SFX_HRC
#include <sfx2/sfx.hrc>
#endif

#ifndef _FMTINFMT_HXX
#include "fmtinfmt.hxx"
#endif

#ifndef _SFXMACITEM_HXX
#include <svtools/macitem.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

using ::com::sun::star::container::NoSuchElementException;
using ::com::sun::star::container::XNameReplace;
using ::com::sun::star::lang::IllegalArgumentException;
using ::com::sun::star::lang::WrappedTargetException;
using ::com::sun::star::lang::XServiceInfo;
using ::com::sun::star::beans::PropertyValue;
using ::cppu::WeakImplHelper2;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;


const sal_Char sAPI_ServiceName[] = "com.sun.star.container.XNameReplace";
const sal_Char sAPI_SwFrameEventDescriptor[] = "SwFrameEventDescriptor";
const sal_Char sAPI_SwFrameStyleEventDescriptor[] = 
                                    "SwFrameStyleEventDescriptor";
const sal_Char sAPI_SwDetachedEventDescriptor[] = "SwDetachedEventDescriptor";
const sal_Char sAPI_SwHyperlinkEventDescriptor[] = 
                                    "SwHyperlinkEventDescriptor";

//
// tables of all known events handled by this class
//

// careful: keep aKnownEventIDs and aKnownEventNames in sync!

const USHORT aKnownEventIDs[] =
{
    SW_EVENT_OBJECT_SELECT,
    SW_EVENT_START_INS_GLOSSARY,
    SW_EVENT_END_INS_GLOSSARY,
    SW_EVENT_MAIL_MERGE,
    SW_EVENT_FRM_KEYINPUT_ALPHA,
    SW_EVENT_FRM_KEYINPUT_NOALPHA,
    SW_EVENT_FRM_RESIZE,
    SW_EVENT_FRM_MOVE,
    SW_EVENT_PAGE_COUNT,
    SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
     SFX_EVENT_OPENDOC,
     SFX_EVENT_CLOSEDOC,
     SFX_EVENT_STARTAPP,
     SFX_EVENT_CLOSEAPP,
     SFX_EVENT_CREATEDOC,
     SFX_EVENT_SAVEDOC,
     SFX_EVENT_SAVEASDOC,
     SFX_EVENT_ACTIVATEDOC,
     SFX_EVENT_DEACTIVATEDOC,
     SFX_EVENT_PRINTDOC,
     SFX_EVENT_ONERROR,
     SFX_EVENT_LOADFINISHED,
     SFX_EVENT_SAVEFINISHED,
     SFX_EVENT_MODIFYCHANGED,
     SFX_EVENT_PREPARECLOSEDOC,
     SFX_EVENT_NEWMESSAGE,
     SFX_EVENT_TOGGLEFULLSCREENMODE,
     SFX_EVENT_SAVEDOCDONE,
     SFX_EVENT_SAVEASDOCDONE,

// ??? graphics load faulty, terminated, success
    0
};

const sal_Char* aKnownEventNames[] =
{
    "OnSelect",
    "OnInsertStart",
    "OnInsertDone",
    "OnMailMerge",
    "OnAlphaCharInput",
    "OnNonAlphaCharInput",
    "OnResize",
    "OnMove",
    "PageCountChange",
    "OnMouseOver",
    "OnClick",
    "OnMouseOut",
    "OnLoadError",
    "OnLoadCancel",
    "OnLoadDone",
    "OnLoad",
    "OnUnload",
    "OnStartApp",
    "OnCloseApp",
    "OnNew",
    "OnSave",
    "OnSaveAs",
    "OnFocus",
    "OnUnfocus",
    "OnPrint",
    "OnError",
    "OnLoadFinished",
    "OnSaveFinished",
    "OnModifyChanged",
    "OnPrepareUnload",
    "OnNewMail",
    "OnToggleFullscreen",
    "OnSaveDone",
    "OnSaveAsDone",

    NULL
};


//
// tables of allowed events for specific objects
//

const USHORT aGraphicEvents[] =
{ 
    SW_EVENT_OBJECT_SELECT,
    SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
// graphics load (faulty|terminated|successful)
    0
};

const USHORT aFrameEvents[] =
{ 
    SW_EVENT_OBJECT_SELECT,
    SW_EVENT_FRM_KEYINPUT_ALPHA,
    SW_EVENT_FRM_KEYINPUT_NOALPHA,
    SW_EVENT_FRM_RESIZE,
    SW_EVENT_FRM_MOVE,
    SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
    0
};

const USHORT aOLEEvents[] =
{ 
    SW_EVENT_OBJECT_SELECT,
    SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
    0
};

const USHORT aHyperlinkEvents[] =
{ 
    SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
    0
};

const USHORT aAutotextEvents[] =
{ 
    SW_EVENT_START_INS_GLOSSARY,
    SW_EVENT_END_INS_GLOSSARY,
    0
};

const USHORT aFrameStyleEvents[] =
{ 
    SW_EVENT_OBJECT_SELECT,
    SW_EVENT_FRM_KEYINPUT_ALPHA,
    SW_EVENT_FRM_KEYINPUT_NOALPHA,
    SW_EVENT_FRM_RESIZE,
    SW_EVENT_FRM_MOVE,
    SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
// graphics load (faulty|terminated|successful)
    0
};

const USHORT aDocumentEvents[] =
{ 
     SFX_EVENT_STARTAPP,
     SFX_EVENT_CLOSEAPP,
     SFX_EVENT_CREATEDOC,
     SFX_EVENT_OPENDOC,
     SFX_EVENT_SAVEDOC,
     SFX_EVENT_SAVEASDOC,
// ??? cf. SAVEDOCDONE, SAVEASDOCDONE
     SFX_EVENT_SAVEFINISHED,
     SFX_EVENT_CLOSEDOC,
     SFX_EVENT_ACTIVATEDOC,
     SFX_EVENT_DEACTIVATEDOC,
     SFX_EVENT_ONERROR,
     SFX_EVENT_NEWMESSAGE,
     SFX_EVENT_PRINTDOC,
    SW_EVENT_MAIL_MERGE,
    SW_EVENT_PAGE_COUNT,
// 	SFX_EVENT_LOADFINISHED,
// 	SFX_EVENT_MODIFYCHANGED,
// 	SFX_EVENT_PREPARECLOSEDOC,
// 	SFX_EVENT_TOGGLEFULLSCREENMODE,
// 	SFX_EVENT_SAVEDOCDONE,
// 	SFX_EVENT_SAVEASDOCDONE,

    0
};

const USHORT aTest[] = 
{
     SFX_EVENT_MOUSEOVER_OBJECT,
    SFX_EVENT_MOUSECLICK_OBJECT,
    SFX_EVENT_MOUSEOUT_OBJECT,
    0
};




//
// SwEventDescriptor
//

SwBaseEventDescriptor::SwBaseEventDescriptor(
    const USHORT* aSupportedEvents) :
        sServiceName(RTL_CONSTASCII_USTRINGPARAM(sAPI_ServiceName)),
        aSupportedMacroItemIDs(aSupportedEvents),
        sEventType(RTL_CONSTASCII_USTRINGPARAM("EventType")),
        sMacroName(RTL_CONSTASCII_USTRINGPARAM("MacroName")),
        sLibrary(RTL_CONSTASCII_USTRINGPARAM("Library")),
        sStarBasic(RTL_CONSTASCII_USTRINGPARAM("StarBasic")),
        sJavaScript(RTL_CONSTASCII_USTRINGPARAM("JavaScript")),
        sScript(RTL_CONSTASCII_USTRINGPARAM("Script")),
        sNone(RTL_CONSTASCII_USTRINGPARAM("None")),
        sEmpty(),
        aEmptyMacro(sEmpty, sEmpty)
{
    DBG_ASSERT(aSupportedEvents != NULL, "Need a list of supported events!");
}


SwBaseEventDescriptor::~SwBaseEventDescriptor()
{
}

void SwBaseEventDescriptor::replaceByName( 
    const OUString& rName,
    const Any& rElement )
    throw(
        IllegalArgumentException, 
        NoSuchElementException, 
        WrappedTargetException, 
        RuntimeException)
{
    USHORT nMacroID = getMacroID(rName);

    // error checking
    if (0 == nMacroID)
        throw new NoSuchElementException();
    if (rElement.getValueType() != getElementType())
        throw new IllegalArgumentException();

    // get sequence
    Sequence<PropertyValue> aSequence;
    rElement >>= aSequence;

    // perform replace (in subclass)
    replaceByName(nMacroID, getMacroFromAny(rElement));
}

Any SwBaseEventDescriptor::getByName( 
    const OUString& rName ) 
    throw(NoSuchElementException)
{
    USHORT nMacroID = getMacroID(rName);

    // error checking
    if (0 == nMacroID)
        throw new NoSuchElementException();

    // perform get (in subclass)
    return getAnyFromMacro(getByName(nMacroID));
}

Sequence<OUString> SwBaseEventDescriptor::getElementNames() throw()
{
    // this implementation is somewhat slower than it needs to be, but
    // I don't think it's worth the effort to speed it up...

    // 1) count the supported macro IDs
    sal_Int16 nCount = 0;
    for( ; aSupportedMacroItemIDs[nCount] != 0; nCount++) ;

    // 2) create and fill sequence
    Sequence<OUString> aSequence(nCount);
    for( sal_Int16 i = 0; i < nCount; i++)
    {
        aSequence[i] = mapEventIDToName( aSupportedMacroItemIDs[i] );
    }

    return aSequence;
}

sal_Bool SwBaseEventDescriptor::hasByName( 
    const OUString& rName ) 
    throw()
{
    USHORT nMacroID = getMacroID(rName);
    return (nMacroID != 0);
}

Type SwBaseEventDescriptor::getElementType() throw()
{
    return ::getCppuType((Sequence<PropertyValue> *)0);
}

sal_Bool SwBaseEventDescriptor::hasElements() throw()
{
    // check if the first element of aSupportedMacroItemIDs is already
    // the delimiter
    return aSupportedMacroItemIDs[0] == 0;
}

sal_Bool SwBaseEventDescriptor::supportsService(const OUString& rServiceName) 
    throw( )
{
    return sServiceName.equals(rServiceName);
}

Sequence<OUString> SwBaseEventDescriptor::getSupportedServiceNames(void) 
    throw()
{
    Sequence<OUString> aSequence(1);
    aSequence[0] = sServiceName;

    return aSequence;
}

USHORT SwBaseEventDescriptor::mapNameToEventID(const OUString& rName) const
{
    // iterate over known event names
    for(sal_Int16 i = 0; aKnownEventNames[i] != NULL; i++)
    {
        if (0 == rName.compareToAscii(aKnownEventNames[i]))
        {
            return aKnownEventIDs[i];
        }
    }

    // not found -> return zero
    return 0;
}

OUString SwBaseEventDescriptor::mapEventIDToName(USHORT nPoolID) const
{
    // iterate over known event IDs
    for(sal_Int16 i = 0; aKnownEventIDs[i] != 0; i++)
    {
        if (nPoolID == aKnownEventIDs[i])
        {
            return OUString::createFromAscii(aKnownEventNames[i]);
        }
    }

    // not found -> return empty string
    OUString sEmpty;
    return sEmpty;
}

USHORT SwBaseEventDescriptor::getMacroID(const OUString& rName) const
{
    USHORT nID = mapNameToEventID(rName);
    if (nID != 0)
    {
        for (sal_Int16 i=0; aSupportedMacroItemIDs[i] != 0; i++)
        {
            if (aSupportedMacroItemIDs[i] == nID)
            {
                // found! return nID
                return nID;
            }
        }
    }

    // not found -> return 0
    return 0;
}

Any SwBaseEventDescriptor::getAnyFromMacro(const SvxMacro& rMacro)
{
    Any aRetValue;
    sal_Bool bRetValueOK = sal_False;	// do we have a ret value?

    if (rMacro.HasMacro())
    {
        switch (rMacro.GetScriptType())
        {
            case STARBASIC:
            {
                // create sequence
                Sequence<PropertyValue> aSequence(3);
                Any aTmp;

                // create type
                PropertyValue aTypeValue;
                aTypeValue.Name = sEventType;
                aTmp <<= sStarBasic;
                aTypeValue.Value = aTmp;
                aSequence[0] = aTypeValue;

                // macro name
                PropertyValue aNameValue;
                aNameValue.Name = sMacroName;
                OUString sNameTmp(rMacro.GetMacName());
                aTmp <<= sNameTmp;
                aNameValue.Value = aTmp;
                aSequence[1] = aNameValue;

                // library name
                PropertyValue aLibValue;
                aLibValue.Name = sLibrary;
                OUString sLibTmp(rMacro.GetLibName());
                aTmp <<= sLibTmp;
                aLibValue.Value = aTmp;
                aSequence[2] = aLibValue;

                aRetValue <<= aSequence;
                bRetValueOK = sal_True;
                break;
            }

            case JAVASCRIPT:
            case EXTENDED_STYPE:
            default:
                DBG_ERROR("not implemented");
        }
    }
    // else: bRetValueOK not set

    // if we don't have a return value, make an empty one
    if (! bRetValueOK)
    {
        // create "None" macro
        Sequence<PropertyValue> aSequence(1);

        PropertyValue aKindValue;
        aKindValue.Name = sEventType;
        Any aTmp;
        aTmp <<= sNone;
        aKindValue.Value = aTmp;
        aSequence[0] = aKindValue;

        aRetValue <<= aSequence;
        bRetValueOK = sal_True;
    }

    return aRetValue;
}


const SvxMacro SwBaseEventDescriptor::getMacroFromAny(
    const Any& rAny)
        throw ( IllegalArgumentException )
{
    // get sequence
    Sequence<PropertyValue> aSequence;
    rAny >>= aSequence;

    // process ...
    sal_Bool bTypeOK = sal_False;
    sal_Bool bNone = sal_False;		// true if EventType=="None"
    enum ScriptType eType;
    OUString sScriptVal;
    OUString sMacroVal;
    OUString sLibVal;
    sal_Int32 nCount = aSequence.getLength();
    for (sal_Int32 i = 0; i < nCount; i++)
    {
        PropertyValue& aValue = aSequence[i];
        if (aValue.Name.equals(sEventType))
        {
            OUString sTmp;
            aValue.Value >>= sTmp;
            if (sTmp.equals(sStarBasic))
            {
                eType = STARBASIC;
                bTypeOK = sal_True;
            }
            else if (sTmp.equals(sJavaScript))
            {
                eType = JAVASCRIPT;
                bTypeOK = sal_True;
            }
            else if (sTmp.equals(sNone))
            {
                bNone = sal_True;
                bTypeOK = sal_True;
            }
            // else: unknown script type
        }
        else if (aValue.Name.equals(sMacroName))
        {
            aValue.Value >>= sMacroVal;
        }
        else if (aValue.Name.equals(sLibrary))
        {
            aValue.Value >>= sLibVal;
        }
        else if (aValue.Name.equals(sScript))
        {
            aValue.Value >>= sScriptVal;
        }
        // else: unknown PropertyValue -> ignore
    }

    if (bTypeOK)
    {
        if (bNone)
        {
            // create empty macro
            OUString sEmpty;
            SvxMacro aMacro(sEmpty, sEmpty, STARBASIC);
            return aMacro;
        }
        else
        {
            if (eType == STARBASIC)
            {
                // create macro and return
                SvxMacro aMacro(sMacroVal, sLibVal, eType);
                return aMacro;
            }
            else
            {
                // we can't process type: abort
                // TODO: JavaScript macros
                throw new IllegalArgumentException();
            }
        }
    }
    else
    {
        // no valid type: abort
        throw new IllegalArgumentException();
    }
}




//
// SwEventDescriptor
//


SwEventDescriptor::SwEventDescriptor(
    XInterface& rParent,
    const USHORT* aSupportedEvents) :
        SwBaseEventDescriptor(aSupportedEvents),
        xParentRef(&rParent)
{
}


SwEventDescriptor::~SwEventDescriptor()
{
    // automatically release xParentRef !
}

void SwEventDescriptor::replaceByName( 
    const USHORT nEvent,
    const SvxMacro& rMacro)
        throw(
            IllegalArgumentException, 
            NoSuchElementException, 
            WrappedTargetException, 
            RuntimeException)
{
    SvxMacroItem aItem(RES_FRMMACRO);
    aItem.SetMacroTable(getMacroItem().GetMacroTable());
    aItem.SetMacro(nEvent, rMacro);
    setMacroItem(aItem);
}

const SvxMacro& SwEventDescriptor::getByName( 
    const USHORT nEvent )
        throw(
            NoSuchElementException, 
            WrappedTargetException, 
            RuntimeException)
{
    const SvxMacroItem& rItem = getMacroItem();
    if (rItem.HasMacro(nEvent))
    {
        return rItem.GetMacro(nEvent);
    }
    else
    {
        return aEmptyMacro;
    }
}




//
// SwDetachedEventDescriptor
//

SwDetachedEventDescriptor::SwDetachedEventDescriptor(
    const USHORT* aMacroItems) : 
    SwBaseEventDescriptor(aMacroItems),
    sImplName(RTL_CONSTASCII_USTRINGPARAM(sAPI_SwDetachedEventDescriptor))
{
    // allocate aMacros
    sal_Int16 nCount = getIndex(0);
    aMacros = new SvxMacro*[nCount];

    // ... and initialize
    for(sal_Int16 i = 0; i < nCount; i++)
    {
        aMacros[i] = NULL;
    }
}

SwDetachedEventDescriptor::~SwDetachedEventDescriptor()
{
    // delete contents of aMacros
    sal_Int16 nCount = getIndex(0);
    for(sal_Int16 i = 0; i < nCount; i++)
    {
        if (NULL != aMacros[i])
            delete aMacros[i];
    }
    
    delete aMacros;
}

sal_Int16 SwDetachedEventDescriptor::getIndex(const USHORT nID)
{
    // iterate over supported events
    sal_Int16 nIndex = 0;
    while ( (aSupportedMacroItemIDs[nIndex] != nID) && 
            (aSupportedMacroItemIDs[nIndex] != 0)      )
    {
        nIndex++;
    }
    return (aSupportedMacroItemIDs[nIndex] == nID) ? nIndex : -1;
}


OUString SwDetachedEventDescriptor::getImplementationName() 
    throw( ::com::sun::star::uno::RuntimeException )
{
    return sImplName;
}


void SwDetachedEventDescriptor::replaceByName( 
    const USHORT nEvent,
    const SvxMacro& rMacro)
    throw(
        IllegalArgumentException, 
        NoSuchElementException, 
        WrappedTargetException, 
        RuntimeException)
{
    sal_Int16 nIndex = getIndex(nEvent);
    if (-1 == nIndex)
        throw new IllegalArgumentException();

    aMacros[nIndex] = new SvxMacro(rMacro.GetMacName(), rMacro.GetLibName(),
                                   rMacro.GetScriptType() );
}


const SvxMacro& SwDetachedEventDescriptor::getByName( 
    const USHORT nEvent )
    throw(
        NoSuchElementException, 
        WrappedTargetException, 
        RuntimeException)
{
    sal_Int16 nIndex = getIndex(nEvent);
    if (-1 == nIndex)
        throw new IllegalArgumentException();

    return (NULL == aMacros[nIndex]) ? aEmptyMacro : (*aMacros[nIndex]);
}

const sal_Bool SwDetachedEventDescriptor::hasByName( 
    const USHORT nEvent )		/// item ID of event
        throw(IllegalArgumentException)
{
    sal_Int16 nIndex = getIndex(nEvent);
    if (-1 == nIndex)
        throw new IllegalArgumentException();

    return (NULL == aMacros[nIndex]) ? sal_False : aMacros[nIndex]->HasMacro();
}


//
// SwHyperlinkEventDescriptor
//

SwHyperlinkEventDescriptor::SwHyperlinkEventDescriptor() :
    SwDetachedEventDescriptor(aHyperlinkEvents),
    sImplName(RTL_CONSTASCII_USTRINGPARAM(sAPI_SwHyperlinkEventDescriptor))
{
}

SwHyperlinkEventDescriptor::~SwHyperlinkEventDescriptor()
{
}

OUString SwHyperlinkEventDescriptor::getImplementationName(void) 
    throw( RuntimeException )
{
    return sImplName;
}

void SwHyperlinkEventDescriptor::copyMacrosFromINetFmt(
    const SwFmtINetFmt& aFmt)
{
    for(sal_Int16 i = 0; aSupportedMacroItemIDs[i] != NULL; i++)
    {
        USHORT nEvent = aSupportedMacroItemIDs[i];
        const SvxMacro* aMacro = aFmt.GetMacro(nEvent);
        if (NULL != aMacro)
            replaceByName(nEvent, *aMacro);
    }
}

void SwHyperlinkEventDescriptor::copyMacrosIntoINetFmt(
    SwFmtINetFmt& aFmt)
{
    for(sal_Int16 i = 0; aSupportedMacroItemIDs[i] != NULL; i++)
    {
        USHORT nEvent = aSupportedMacroItemIDs[i];
        if (hasByName(nEvent))
            aFmt.SetMacro(nEvent, getByName(nEvent));
    }
}


void SwHyperlinkEventDescriptor::copyMacrosFromNameReplace(
    ::com::sun::star::uno::Reference<
        ::com::sun::star::container::XNameReplace> & xReplace)
{
    // iterate over all names (all names that *we* support)
    Sequence<OUString> aNames = getElementNames();
    sal_Int32 nCount = aNames.getLength();
    for(sal_Int32 i = 0; i < nCount; i++)
    {
        // copy element for that name
        const OUString& rName = aNames[i];
        if (xReplace->hasByName(rName))
        {
            SwBaseEventDescriptor::replaceByName(rName, 
                                                 xReplace->getByName(rName));
        }
    }
}


//
// SwFrameEventDescriptor
//

// use double cast in superclass constructor to avoid ambigous cast
SwFrameEventDescriptor::SwFrameEventDescriptor( 
    SwXTextFrame& rFrameRef ) :
        SwEventDescriptor((text::XTextFrame&)rFrameRef, aFrameEvents),
        sSwFrameEventDescriptor(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_SwFrameEventDescriptor)),
        rFrame(rFrameRef)
{
}

SwFrameEventDescriptor::SwFrameEventDescriptor(
    SwXTextGraphicObject& rGraphicRef ) :
        SwEventDescriptor((text::XTextContent&)rGraphicRef, aGraphicEvents),
        rFrame((SwXFrame&)rGraphicRef)
{
}

SwFrameEventDescriptor::SwFrameEventDescriptor( 
    SwXTextEmbeddedObject& rObjectRef ) :
        SwEventDescriptor((text::XTextContent&)rObjectRef, aOLEEvents),
        rFrame((SwXFrame&)rObjectRef)
{
}

SwFrameEventDescriptor::~SwFrameEventDescriptor()
{
}

void SwFrameEventDescriptor::setMacroItem(const SvxMacroItem& rItem)
{
    rFrame.GetFrmFmt()->SetAttr(rItem);
}

const SvxMacroItem& SwFrameEventDescriptor::getMacroItem()
{
    return (const SvxMacroItem&)rFrame.GetFrmFmt()->GetAttr(RES_FRMMACRO);
}

OUString SwFrameEventDescriptor::getImplementationName() 
    throw( RuntimeException )
{
    return sSwFrameEventDescriptor;
}


//
// SwFrameStyleEventDescriptor
//

SwFrameStyleEventDescriptor::SwFrameStyleEventDescriptor( 
    SwXFrameStyle& rStyleRef ) :
        SwEventDescriptor((document::XEventSupplier&)rStyleRef, 
                          aFrameStyleEvents),
        sSwFrameStyleEventDescriptor(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_SwFrameStyleEventDescriptor)),
        rStyle(rStyleRef)
{
}

SwFrameStyleEventDescriptor::~SwFrameStyleEventDescriptor()
{
}

void SwFrameStyleEventDescriptor::setMacroItem(const SvxMacroItem& rItem)
{
    // As I was told, for some entirely unobvious reason getting an
    // item from a style has to look as follows:
    SfxStyleSheetBasePool* pBasePool = rStyle.GetBasePool();
    if (pBasePool)
    {
        SfxStyleSheetBase* pBase = pBasePool->Find(rStyle.GetStyleName());
        if (pBase)
        {
            SwDocStyleSheet aStyle(*(SwDocStyleSheet*)pBase);
            SfxItemSet& rStyleSet = aStyle.GetItemSet();
            SfxItemSet aSet(*rStyleSet.GetPool(), RES_FRMMACRO, RES_FRMMACRO);
            aSet.Put(rItem);
            aStyle.SetItemSet(aSet);
        }
    }
}

static const SvxMacroItem aEmptyMacroItem(RES_FRMMACRO);

const SvxMacroItem& SwFrameStyleEventDescriptor::getMacroItem()
{
    // As I was told, for some entirely unobvious reason getting an
    // item from a style has to look as follows:
    SfxStyleSheetBasePool* pBasePool = rStyle.GetBasePool();
    if (pBasePool)
    {
        SfxStyleSheetBase* pBase = pBasePool->Find(rStyle.GetStyleName());
        if (pBase)
        {
            SwDocStyleSheet aStyle(*(SwDocStyleSheet*)pBase);
            return (const SvxMacroItem&)aStyle.GetItemSet().Get(RES_FRMMACRO);
        }
        else
            return aEmptyMacroItem;
    }
    else
        return aEmptyMacroItem;
}

OUString SwFrameStyleEventDescriptor::getImplementationName() 
    throw( RuntimeException )
{
    return sSwFrameStyleEventDescriptor;
}
