/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fldmgr.cxx,v $
 * $Revision: 1.53 $
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

#include <cmdid.h>
#include <hintids.hxx>
#include <svtools/stritem.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/text/XDefaultNumberingProvider.hpp>
#include <com/sun/star/text/XNumberingTypeInfo.hpp>
#include <com/sun/star/style/NumberingType.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/sdbc/XDataSource.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>
#include <com/sun/star/uri/XVndSunStarScriptUrl.hpp>
#include <comphelper/processfactory.hxx>
#include <svx/unolingu.hxx>
#include <unotools/localedatawrapper.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/linkmgr.hxx>
#include <sfx2/app.hxx>
#include <basic/basmgr.hxx>
#include <svx/langitem.hxx>
#include <svtools/macitem.hxx>
#include <basic/sbmod.hxx>
#include <fmtrfmrk.hxx>
#include <basic/sbmeth.hxx>
#include <basic/sbx.hxx>
#include <svtools/zforlist.hxx>
#include <svtools/zformat.hxx>
#include <vcl/mnemonic.hxx>
#include <view.hxx>
#include <wrtsh.hxx>		// Actives Fenster
#include <doc.hxx>		// Actives Fenster
#include <docsh.hxx>		// Actives Fenster
#include <swmodule.hxx>
#include <charatr.hxx>
#include <fmtinfmt.hxx>
#include <cellatr.hxx>
#include <dbmgr.hxx>
#include <shellres.hxx>
#include <fldbas.hxx>
#include <docufld.hxx>
#include <chpfld.hxx>
#include <ddefld.hxx>
#include <expfld.hxx>
#include <reffld.hxx>
#include <usrfld.hxx>
#include <dbfld.hxx>
#include <authfld.hxx>
#include <flddat.hxx>
#include <fldmgr.hxx>
#include <crsskip.hxx>
#include <flddropdown.hxx>
#include <fldui.hrc>
#include <tox.hxx>

using rtl::OUString;
using namespace com::sun::star::uno;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::text;
using namespace com::sun::star::style;
using namespace com::sun::star::sdbc;
using namespace ::com::sun::star;
using namespace nsSwDocInfoSubType;

/*--------------------------------------------------------------------
    Beschreibung: Gruppen der Felder
 --------------------------------------------------------------------*/
enum
{
    GRP_DOC_BEGIN	=  0,
    GRP_DOC_END   	=  GRP_DOC_BEGIN + 11,

    GRP_FKT_BEGIN 	=  GRP_DOC_END,
    GRP_FKT_END     =  GRP_FKT_BEGIN + 8,

    GRP_REF_BEGIN 	=  GRP_FKT_END,
    GRP_REF_END   	=  GRP_REF_BEGIN + 2,

    GRP_REG_BEGIN 	=  GRP_REF_END,
    GRP_REG_END     =  GRP_REG_BEGIN + 1,

    GRP_DB_BEGIN  	=  GRP_REG_END,
    GRP_DB_END    	=  GRP_DB_BEGIN  + 5,

    GRP_VAR_BEGIN 	=  GRP_DB_END,
    GRP_VAR_END   	=  GRP_VAR_BEGIN + 9
};

enum
{
    GRP_WEB_DOC_BEGIN	=  0,
    GRP_WEB_DOC_END   	=  GRP_WEB_DOC_BEGIN + 9,

    GRP_WEB_FKT_BEGIN 	=  GRP_WEB_DOC_END + 2,
    GRP_WEB_FKT_END   	=  GRP_WEB_FKT_BEGIN + 0,	// Die Gruppe ist leer!

    GRP_WEB_REF_BEGIN 	=  GRP_WEB_FKT_END + 6,		// Die Gruppe ist leer!
    GRP_WEB_REF_END   	=  GRP_WEB_REF_BEGIN + 0,

    GRP_WEB_REG_BEGIN 	=  GRP_WEB_REF_END + 2,
    GRP_WEB_REG_END     =  GRP_WEB_REG_BEGIN + 1,

    GRP_WEB_DB_BEGIN  	=  GRP_WEB_REG_END,			// Die Gruppe ist leer!
    GRP_WEB_DB_END    	=  GRP_WEB_DB_BEGIN  + 0,

    GRP_WEB_VAR_BEGIN 	=  GRP_WEB_DB_END + 5,
    GRP_WEB_VAR_END   	=  GRP_WEB_VAR_BEGIN + 1
};

/*--------------------------------------------------------------------
    Beschreibung: Formate in der richtigen Reihenfolge
 --------------------------------------------------------------------*/
static const USHORT __FAR_DATA aSetFmt[] =
{
// die Reihenfolge muss zu Beginn mit den ResourceIds fuer FMT_SETVAR_???
// uebereinstimmen
    0,
    0
};

static const USHORT __FAR_DATA aGetFmt[] =
{
// die Reihenfolge muss zu Beginn mit den ResourceIds fuer FMT_GETVAR_???
// uebereinstimmen
    0
};

static const USHORT __FAR_DATA aUsrFmt[] =
{
// die Reihenfolge muss zu Beginn mit den ResourceIds fuer FMT_SETVAR_???
// uebereinstimmen
    0,
    nsSwExtendedSubType::SUB_CMD
};

static const USHORT __FAR_DATA aDBFmt[] =
{
// die Reihenfolge muss zu Beginn mit den ResourceIds fuer FMT_DBFLD_???
// uebereinstimmen
    nsSwExtendedSubType::SUB_OWN_FMT
};

static const USHORT VF_COUNT		= sizeof(aGetFmt) / sizeof(USHORT);
static const USHORT VF_USR_COUNT	= sizeof(aUsrFmt) / sizeof(USHORT);
static const USHORT VF_DB_COUNT		= sizeof(aDBFmt)  / sizeof(USHORT);

/*--------------------------------------------------------------------
    Beschreibung: Feldtypen und Subtypes
 --------------------------------------------------------------------*/
struct SwFldPack
{
    USHORT  nTypeId;

    USHORT	nSubTypeStart;
    USHORT  nSubTypeEnd;

    ULONG	nFmtBegin;
    ULONG	nFmtEnd;
};

/*--------------------------------------------------------------------
    Beschreibung: Strings und Formate
 --------------------------------------------------------------------*/
static const SwFldPack __FAR_DATA aSwFlds[] =
{
    // Dokument
    { TYP_EXTUSERFLD,	 	FLD_EU_BEGIN,	 	FLD_EU_END,	    0,     				0 },
    { TYP_AUTHORFLD,	 	0,					0,				FMT_AUTHOR_BEGIN,	FMT_AUTHOR_END },
    { TYP_DATEFLD,			FLD_DATE_BEGIN,		FLD_DATE_END,	0,					0 },
    { TYP_TIMEFLD,			FLD_TIME_BEGIN,		FLD_TIME_END,	0,					0 },
    { TYP_PAGENUMBERFLD,	0,					0,				FMT_NUM_BEGIN,   	FMT_NUM_END-1 },
    { TYP_NEXTPAGEFLD,		0,					0,				FMT_NUM_BEGIN,		FMT_NUM_END },
    { TYP_PREVPAGEFLD,		0,					0,				FMT_NUM_BEGIN,		FMT_NUM_END },
    { TYP_FILENAMEFLD,		0,					0,				FMT_FF_BEGIN,    	FMT_FF_END },
    { TYP_DOCSTATFLD,	 	FLD_STAT_BEGIN,		FLD_STAT_END,	FMT_NUM_BEGIN,		FMT_NUM_END-1 },

    { TYP_CHAPTERFLD,	 	0,					0,				FMT_CHAPTER_BEGIN, 	FMT_CHAPTER_END },
    { TYP_TEMPLNAMEFLD,		0,					0,				FMT_FF_BEGIN,    	FMT_FF_END },

    // Funktion
    { TYP_CONDTXTFLD,	 	0,					0,				0,					0 },
    { TYP_DROPDOWN,       	0,                  0,              0,                  0 },
    { TYP_INPUTFLD,			FLD_INPUT_BEGIN,	FLD_INPUT_END,	0,   				0 },
    { TYP_MACROFLD,			0,					0,				0,					0 },
    { TYP_JUMPEDITFLD, 		0,					0,				FMT_MARK_BEGIN,		FMT_MARK_END },
    { TYP_COMBINED_CHARS,	0,					0,				0,					0 },
    { TYP_HIDDENTXTFLD,		0,					0,				0,					0 },
    { TYP_HIDDENPARAFLD, 	0,			   		0,				0,					0 },

    // Referenzen
    { TYP_SETREFFLD,	 	0,					0,				0,					0 },
    { TYP_GETREFFLD,	 	0,					0,				FMT_REF_BEGIN,		FMT_REF_END },

    // Ablage
    { TYP_DOCINFOFLD,	 	0,					0,				FMT_REG_BEGIN, 		FMT_REG_END },

    // Datenbank
    { TYP_DBFLD,		 	0,					0,				FMT_DBFLD_BEGIN,	FMT_DBFLD_END },
    { TYP_DBNEXTSETFLD,		0,					0,				0,					0 },
    { TYP_DBNUMSETFLD,		0,					0,				0,					0 },
    { TYP_DBSETNUMBERFLD, 	0,					0,				FMT_NUM_BEGIN,	    FMT_NUM_END-2 },
    { TYP_DBNAMEFLD, 	 	0,					0,				0,					0 },

    // Variablen
    { TYP_SETFLD,		 	0,					0,				FMT_SETVAR_BEGIN,   FMT_SETVAR_END },

    { TYP_GETFLD,		 	0,					0, 				FMT_GETVAR_BEGIN,	FMT_GETVAR_END },
    { TYP_DDEFLD,		 	0,					0,				FMT_DDE_BEGIN, 		FMT_DDE_END },
    { TYP_FORMELFLD,  		0,					0,				FMT_GETVAR_BEGIN,	FMT_GETVAR_END },
    { TYP_INPUTFLD,			FLD_INPUT_BEGIN,	FLD_INPUT_END,	0,   				0 },
    { TYP_SEQFLD,		 	0,					0,				FMT_NUM_BEGIN,		FMT_NUM_END-2 },
    { TYP_SETREFPAGEFLD,	FLD_PAGEREF_BEGIN,	FLD_PAGEREF_END,0,   				0 },
    { TYP_GETREFPAGEFLD,	0,					0,				FMT_NUM_BEGIN,   	FMT_NUM_END-1 },
    { TYP_USERFLD,			0,					0,				FMT_USERVAR_BEGIN,	FMT_USERVAR_END }
};

/*--------------------------------------------------------------------
    Beschreibung: Zugriff auf die Shell
 --------------------------------------------------------------------*/

static SwWrtShell* lcl_GetShell()
{
    SwView* pView;
    if ( 0 != (pView = ::GetActiveView()) )
        return pView->GetWrtShellPtr();
    DBG_ERROR("no current shell found!");
    return 0;
}

inline USHORT GetPackCount() {	return sizeof(aSwFlds) / sizeof(SwFldPack); }

/*--------------------------------------------------------------------
    Beschreibung: FieldManager regelt das Einfuegen und Updaten
                  von Feldern
 --------------------------------------------------------------------*/

SwFldMgr::SwFldMgr(SwWrtShell* pSh ) :
    pModule(0),
    pMacroItem(0),
    pWrtShell(pSh),
    bEvalExp(TRUE)
{
    // aktuelles Feld ermitteln falls vorhanden
    GetCurFld();
}


SwFldMgr::~SwFldMgr()
{
}

/*--------------------------------------------------------------------
    Beschreibung: RefMark ueber Namen organisieren
 --------------------------------------------------------------------*/

BOOL  SwFldMgr::CanInsertRefMark( const String& rStr )
{
    BOOL bRet = FALSE;
    SwWrtShell *pSh = pWrtShell ? pWrtShell : lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    if(pSh)
    {
        USHORT nCnt = pSh->GetCrsrCnt();

        // der letzte Crsr muss keine aufgespannte Selektion
        if( 1 < nCnt && !pSh->SwCrsrShell::HasSelection() )
            --nCnt;

        bRet =  2 > nCnt && 0 == pSh->GetRefMark( rStr );
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung: Zugriff ueber ResIds
 --------------------------------------------------------------------*/

void SwFldMgr::RemoveFldType(USHORT nResId, const String& rName )
{
    SwWrtShell * pSh = pWrtShell ? pWrtShell : lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    if( pSh )
        pSh->RemoveFldType(nResId, rName);
}

USHORT SwFldMgr::GetFldTypeCount(USHORT nResId) const
{
    SwWrtShell * pSh = pWrtShell ? pWrtShell : lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    return pSh ? pSh->GetFldTypeCount(nResId) : 0;
}


SwFieldType* SwFldMgr::GetFldType(USHORT nResId, USHORT nId) const
{
    SwWrtShell * pSh = pWrtShell ? pWrtShell : lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    return pSh ? pSh->GetFldType(nId, nResId) : 0;
}


SwFieldType* SwFldMgr::GetFldType(USHORT nResId, const String& rName) const
{
    SwWrtShell * pSh = pWrtShell ? pWrtShell : lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    return pSh ? pSh->GetFldType(nResId, rName) : 0;
}


/*--------------------------------------------------------------------
    Beschreibung: Aktuelles Feld ermitteln
 --------------------------------------------------------------------*/
SwField* SwFldMgr::GetCurFld()
{
    SwWrtShell *pSh = pWrtShell ? pWrtShell : ::lcl_GetShell();
    if ( pSh )
        pCurFld = pSh->GetCurFld();
    else
        pCurFld = NULL;

    // Strings und Format initialisieren
    //
    aCurPar1.Erase();
    aCurPar2.Erase();
    sCurFrame.Erase();
    nCurFmt = 0;

    if(!pCurFld)
        return 0;

    // Aktuelle Werte aufbereiten Parameter 1 und Parameter 2
    // als auch das Format ermitteln
    //
    const USHORT nTypeId = pCurFld->GetTypeId();

    nCurFmt  	= pCurFld->GetFormat();
    aCurPar1 	= pCurFld->GetPar1();
    aCurPar2 	= pCurFld->GetPar2();

    switch( nTypeId )
    {
        case TYP_PAGENUMBERFLD:
        case TYP_NEXTPAGEFLD:
        case TYP_PREVPAGEFLD:
        case TYP_GETREFPAGEFLD:
            if( nCurFmt == SVX_NUM_PAGEDESC )
                nCurFmt -= 2;
            break;
    }
    return pCurFld;
}

/*--------------------------------------------------------------------
    Beschreibung: Gruppen-Bereich liefern
 --------------------------------------------------------------------*/


const SwFldGroupRgn& SwFldMgr::GetGroupRange(BOOL bHtmlMode, USHORT nGrpId) const
{
static SwFldGroupRgn __READONLY_DATA aRanges[] =
{
    { /* Dokument	*/ 	GRP_DOC_BEGIN, 	GRP_DOC_END },
    { /* Funktionen	*/ 	GRP_FKT_BEGIN,  GRP_FKT_END },
    { /* Referenzen	*/ 	GRP_REF_BEGIN,  GRP_REF_END },
    { /* Ablage		*/ 	GRP_REG_BEGIN,  GRP_REG_END },
    { /* DB			*/	GRP_DB_BEGIN,	GRP_DB_END },
    { /* User		*/	GRP_VAR_BEGIN,	GRP_VAR_END }
};
static SwFldGroupRgn __READONLY_DATA aWebRanges[] =
{
    { /* Dokument	*/ 	GRP_WEB_DOC_BEGIN, 	GRP_WEB_DOC_END },
    { /* Funktionen	*/ 	GRP_WEB_FKT_BEGIN,  GRP_WEB_FKT_END },
    { /* Referenzen	*/ 	GRP_WEB_REF_BEGIN,  GRP_WEB_REF_END },
    { /* Ablage		*/ 	GRP_WEB_REG_BEGIN,  GRP_WEB_REG_END },
    { /* DB			*/	GRP_WEB_DB_BEGIN,	GRP_WEB_DB_END },
    { /* User		*/	GRP_WEB_VAR_BEGIN,	GRP_WEB_VAR_END }
};

    if (bHtmlMode)
        return aWebRanges[(USHORT)nGrpId];
    else
        return aRanges[(USHORT)nGrpId];
}

/*--------------------------------------------------------------------
    Beschreibung: GroupId bestimmen
 --------------------------------------------------------------------*/

USHORT SwFldMgr::GetGroup(BOOL bHtmlMode, USHORT nTypeId, USHORT nSubType) const
{
    if (nTypeId == TYP_SETINPFLD)
        nTypeId = TYP_SETFLD;

    if (nTypeId == TYP_INPUTFLD && (nSubType & INP_USR))
        nTypeId = TYP_USERFLD;

    if (nTypeId == TYP_FIXDATEFLD)
        nTypeId = TYP_DATEFLD;

    if (nTypeId == TYP_FIXTIMEFLD)
        nTypeId = TYP_TIMEFLD;

    for (USHORT i = GRP_DOC; i <= GRP_VAR; i++)
    {
        const SwFldGroupRgn& rRange = GetGroupRange(bHtmlMode, i);
        for (USHORT nPos = rRange.nStart; nPos < rRange.nEnd; nPos++)
        {
            if (aSwFlds[nPos].nTypeId == nTypeId)
                return i;
        }
    }
    return USHRT_MAX;
}

/*--------------------------------------------------------------------
    Beschreibung: Namen zur TypeId ermitteln
                  ZUGRIFF ueber TYP_....
 --------------------------------------------------------------------*/


USHORT SwFldMgr::GetTypeId(USHORT nPos)
{
    ASSERT(nPos < ::GetPackCount(), "unzulaessige Pos");
    return aSwFlds[ nPos ].nTypeId;
}


const String& SwFldMgr::GetTypeStr(USHORT nPos)
{
    ASSERT(nPos < ::GetPackCount(), "unzulaessige TypeId");

    USHORT nFldWh = aSwFlds[ nPos ].nTypeId;

    // Sonderbehandlung fuer Datum/Zeit Felder (ohne var/fix)
    if( TYP_DATEFLD == nFldWh )
    {
        static String g_aDate( SW_RES( STR_DATEFLD ) );
        return g_aDate;
    }
    if( TYP_TIMEFLD == nFldWh )
    {
        static String g_aTime( SW_RES( STR_TIMEFLD ) );
        return g_aTime;
    }

    return SwFieldType::GetTypeStr( nFldWh );
}

/*--------------------------------------------------------------------
    Beschreibung: Pos in der Liste bestimmen
 --------------------------------------------------------------------*/


USHORT SwFldMgr::GetPos(USHORT nTypeId)
{
    switch( nTypeId )
    {
        case TYP_FIXDATEFLD:		nTypeId = TYP_DATEFLD;		break;
        case TYP_FIXTIMEFLD:      	nTypeId = TYP_TIMEFLD;		break;
        case TYP_SETINPFLD:      	nTypeId = TYP_SETFLD;		break;
        case TYP_USRINPFLD:      	nTypeId = TYP_USERFLD;		break;
    }

    for(USHORT i = 0; i < GetPackCount(); i++)
        if(aSwFlds[i].nTypeId == nTypeId)
            return i;

    return USHRT_MAX;
}

/*--------------------------------------------------------------------
    Beschreibung: Subtypen eines Feldes lokalisieren
 --------------------------------------------------------------------*/

BOOL SwFldMgr::GetSubTypes(USHORT nTypeId, SvStringsDtor& rToFill)
{
    BOOL bRet = FALSE;
    SwWrtShell *pSh = pWrtShell ? pWrtShell : lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    if(pSh)
    {
        const USHORT nPos = GetPos(nTypeId);

        switch(nTypeId)
        {
            case TYP_SETREFFLD:
            case TYP_GETREFFLD:
            {
                // Referenzen sind keine Felder
                pSh->GetRefMarks( &rToFill );
                break;
            }
            case TYP_MACROFLD:
            {
                break;
            }
            case TYP_INPUTFLD:
            {   String* pNew = new SW_RESSTR(aSwFlds[nPos].nSubTypeStart);
                rToFill.Insert(pNew, rToFill.Count());
                // Weiter bei generischen Typen
            }
            case TYP_DDEFLD:
            case TYP_SEQFLD:
            case TYP_FORMELFLD:
            case TYP_GETFLD:
            case TYP_SETFLD:
            case TYP_USERFLD:
            {

                const USHORT nCount = pSh->GetFldTypeCount();
                for(USHORT i = 0; i < nCount; ++i)
                {
                    SwFieldType* pFldType = pSh->GetFldType( i );
                    const USHORT nWhich = pFldType->Which();

                    if((nTypeId == TYP_DDEFLD && pFldType->Which() == RES_DDEFLD) ||

                       (nTypeId == TYP_USERFLD && nWhich == RES_USERFLD) ||

                       (nTypeId == TYP_GETFLD && nWhich == RES_SETEXPFLD &&
                        !(((SwSetExpFieldType*)pFldType)->GetType() & nsSwGetSetExpType::GSE_SEQ)) ||

                       (nTypeId == TYP_SETFLD && nWhich == RES_SETEXPFLD &&
                        !(((SwSetExpFieldType*)pFldType)->GetType() & nsSwGetSetExpType::GSE_SEQ)) ||

                       (nTypeId == TYP_SEQFLD && nWhich == RES_SETEXPFLD  &&
                       (((SwSetExpFieldType*)pFldType)->GetType() & nsSwGetSetExpType::GSE_SEQ)) ||

                       ((nTypeId == TYP_INPUTFLD  || nTypeId == TYP_FORMELFLD) &&
                         (nWhich == RES_USERFLD ||
                          nWhich == RES_SETEXPFLD &&
                          !(((SwSetExpFieldType*)pFldType)->GetType() & nsSwGetSetExpType::GSE_SEQ)) ) )
                    {
                        String* pNew = new String(pFldType->GetName());
                        rToFill.Insert(pNew, rToFill.Count());
                    }
                }
                break;
            }
            case TYP_DBNEXTSETFLD:
            case TYP_DBNUMSETFLD:
            case TYP_DBNAMEFLD:
            case TYP_DBSETNUMBERFLD:
                break;

            default:
            {
                // statische SubTypes
                if(nPos != USHRT_MAX)
                {
                    USHORT nCount;
                    if (nTypeId == TYP_DOCINFOFLD)
                        nCount = DI_SUBTYPE_END - DI_SUBTYPE_BEGIN;
                    else
                        nCount = aSwFlds[nPos].nSubTypeEnd - aSwFlds[nPos].nSubTypeStart;

                    for(USHORT i = 0; i < nCount; ++i)
                    {
                        String* pNew;
                        if (nTypeId == TYP_DOCINFOFLD)
                        {
                            if ( i == DI_CUSTOM )
                                pNew = new String( String(SW_RES( STR_CUSTOM )) );
                            else
                                pNew = new String(*ViewShell::GetShellRes()->aDocInfoLst[i]);
                        }
                        else
                            pNew = new SW_RESSTR(aSwFlds[nPos].nSubTypeStart + i);

                        rToFill.Insert(pNew, rToFill.Count());
                    }
                }
            }
        }
        bRet = TRUE;
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung: Format ermitteln
                  ZUGRIFF ueber TYP_....
 --------------------------------------------------------------------*/


USHORT SwFldMgr::GetFormatCount(USHORT nTypeId, BOOL bIsText, BOOL bHtmlMode) const
{
    ASSERT(nTypeId < TYP_END, "unzulaessige TypeId");

    {
        const USHORT nPos = GetPos(nTypeId);

        if(nPos == USHRT_MAX || (bHtmlMode && nTypeId == TYP_SETFLD))
            return 0;

        ULONG nStart = aSwFlds[nPos].nFmtBegin;
        ULONG nEnd   = aSwFlds[nPos].nFmtEnd;

        if (bIsText && nEnd - nStart >= 2)
            return 2;

        if (nTypeId == TYP_FILENAMEFLD)
            nEnd -= 2;	// Kein Bereich oder Vorlage

        switch(nStart)
        {
            case FMT_GETVAR_BEGIN:
            case FMT_SETVAR_BEGIN:	return VF_COUNT;
            case FMT_USERVAR_BEGIN:	return VF_USR_COUNT;
            case FMT_DBFLD_BEGIN:	return VF_DB_COUNT;
            case FMT_NUM_BEGIN:
            {
                USHORT nCount = (USHORT)(nEnd - nStart);
                GetNumberingInfo();
                if(xNumberingInfo.is())
                {
                    Sequence<sal_Int16> aTypes = xNumberingInfo->getSupportedNumberingTypes();
                    const sal_Int16* pTypes = aTypes.getConstArray();
                    for(sal_Int32 nType = 0; nType < aTypes.getLength(); nType++)
                    {
                        sal_Int16 nCurrent = pTypes[nType];
                        //skip all values below or equal to CHARS_LOWER_LETTER_N
                        if(nCurrent > NumberingType::CHARS_LOWER_LETTER_N)
                        {
                            // #i28073# it's not necessarily a sorted sequence
                            ++nCount;
                        }
                    }
                }
                return nCount;
            }

        }
        return (USHORT)(nEnd - nStart);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	FormatString zu einem Typ ermitteln
 --------------------------------------------------------------------*/


String SwFldMgr::GetFormatStr(USHORT nTypeId, ULONG nFormatId) const
{
    String aRet;
    ASSERT(nTypeId < TYP_END, "unzulaessige TypeId");

    const USHORT nPos = GetPos(nTypeId);

    if(nPos == USHRT_MAX)
        return aRet;

    ULONG nStart;

    nStart = aSwFlds[nPos].nFmtBegin;

    if (TYP_AUTHORFLD == nTypeId|| TYP_FILENAMEFLD == nTypeId)
        nFormatId &= ~FF_FIXED;		// Fixed-Flag ausmaskieren

    if((nStart + nFormatId) < aSwFlds[nPos].nFmtEnd)
        aRet = SW_RESSTR((USHORT)(nStart + nFormatId));
    else if( FMT_NUM_BEGIN == nStart)
    {
        if(xNumberingInfo.is())
        {
            Sequence<sal_Int16> aTypes = xNumberingInfo->getSupportedNumberingTypes();
            const sal_Int16* pTypes = aTypes.getConstArray();
            sal_Int32 nOffset = aSwFlds[nPos].nFmtEnd - nStart;
            sal_Int32 nValidEntry = 0;
            for(sal_Int32 nType = 0; nType < aTypes.getLength(); nType++)
            {
                sal_Int16 nCurrent = pTypes[nType];
                if(nCurrent > NumberingType::CHARS_LOWER_LETTER_N)
                {
                    if(nValidEntry == ((sal_Int32)nFormatId) - nOffset)
                    {
                        aRet = xNumberingInfo->getNumberingIdentifier( pTypes[nType] );
                        break;
                    }
                    ++nValidEntry;
                }
            }
        }
    }

    return aRet;
}

/*--------------------------------------------------------------------
    Beschreibung:	FormatId aus Pseudo-ID ermitteln
 --------------------------------------------------------------------*/

USHORT SwFldMgr::GetFormatId(USHORT nTypeId, ULONG nFormatId) const
{
    USHORT nId = (USHORT)nFormatId;

    switch( nTypeId )
    {
    case TYP_DOCINFOFLD:
        switch( aSwFlds[ GetPos( nTypeId ) ].nFmtBegin + nFormatId )
        {
        case FMT_REG_AUTHOR:	nId = DI_SUB_AUTHOR;	break;
        case FMT_REG_TIME:		nId = DI_SUB_TIME;		break;
        case FMT_REG_DATE:		nId = DI_SUB_DATE;		break;
        }
        break;

    case TYP_PAGENUMBERFLD:
    case TYP_NEXTPAGEFLD:
    case TYP_PREVPAGEFLD:
    case TYP_DOCSTATFLD:
    case TYP_DBSETNUMBERFLD:
    case TYP_SEQFLD:
    case TYP_GETREFPAGEFLD:
    {
        USHORT nPos = GetPos( nTypeId );
        ULONG nBegin = aSwFlds[ nPos ].nFmtBegin;
        ULONG nEnd = aSwFlds[nPos].nFmtEnd;
        if((nBegin + nFormatId) < nEnd)
        {
            switch( nBegin + nFormatId )
            {
            case FMT_NUM_ABC:				nId = SVX_NUM_CHARS_UPPER_LETTER;	break;
            case FMT_NUM_SABC:				nId = SVX_NUM_CHARS_LOWER_LETTER;	break;
            case FMT_NUM_ROMAN:             nId = SVX_NUM_ROMAN_UPPER;			break;
            case FMT_NUM_SROMAN:			nId = SVX_NUM_ROMAN_LOWER;			break;
            case FMT_NUM_ARABIC:            nId = SVX_NUM_ARABIC;				break;
            case FMT_NUM_PAGEDESC: 			nId = SVX_NUM_PAGEDESC; 			break;
            case FMT_NUM_PAGESPECIAL:		nId = SVX_NUM_CHAR_SPECIAL;			break;
            case FMT_NUM_ABC_N:				nId = SVX_NUM_CHARS_UPPER_LETTER_N; break;
            case FMT_NUM_SABC_N:			nId = SVX_NUM_CHARS_LOWER_LETTER_N;	break;
            }
        }
        else if(xNumberingInfo.is())
        {
            Sequence<sal_Int16> aTypes = xNumberingInfo->getSupportedNumberingTypes();
            const sal_Int16* pTypes = aTypes.getConstArray();
            sal_Int32 nOffset = nEnd - nBegin;
            sal_Int32 nValidEntry = 0;
            for(sal_Int32 nType = 0; nType < aTypes.getLength(); nType++)
            {
                sal_Int16 nCurrent = pTypes[nType];
                if(nCurrent > NumberingType::CHARS_LOWER_LETTER_N)
                {
                    if(nValidEntry == ((sal_Int32)nFormatId) - nOffset)
                    {
                        nId = pTypes[nType];
                        break;
                    }
                    ++nValidEntry;
                }
            }
        }
    }
    break;
    case TYP_DDEFLD:
        switch ( aSwFlds[ GetPos( nTypeId ) ].nFmtBegin + nFormatId )
        {
        case FMT_DDE_NORMAL:    nId = sfx2::LINKUPDATE_ONCALL; break;
        case FMT_DDE_HOT:       nId = sfx2::LINKUPDATE_ALWAYS; break;
        }
        break;
    }

    return nId;

}

/*--------------------------------------------------------------------
    Beschreibung: Traveling
 --------------------------------------------------------------------*/


BOOL SwFldMgr::GoNextPrev( BOOL bNext, SwFieldType* pTyp )
{
    SwWrtShell* pSh = pWrtShell ? pWrtShell : ::lcl_GetShell();
    if(!pSh)
        return FALSE;

    if( !pTyp && pCurFld )
    {
        const USHORT nTypeId = pCurFld->GetTypeId();
        if( TYP_SETINPFLD == nTypeId || TYP_USRINPFLD == nTypeId )
            pTyp = pSh->GetFldType( 0, RES_INPUTFLD );
        else
            pTyp = pCurFld->GetTyp();
    }

    if (pTyp && pTyp->Which() == RES_DBFLD)
    {
        // Fuer Feldbefehl-bearbeiten (alle DB-Felder anspringen)
        return pSh->MoveFldType( 0, bNext, USHRT_MAX, RES_DBFLD );
    }

    return pTyp && pSh ? pSh->MoveFldType( pTyp, bNext ) : FALSE;
}

/*--------------------------------------------------------------------
    Beschreibung: Feldtypen einfuegen
 --------------------------------------------------------------------*/


void SwFldMgr::InsertFldType(SwFieldType& rType)
{
    SwWrtShell* pSh = pWrtShell ? pWrtShell : ::lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    if(pSh)
        pSh->InsertFldType(rType);
}

/*--------------------------------------------------------------------
    Beschreibung: Aktuelle TypeId ermitteln
 --------------------------------------------------------------------*/


USHORT SwFldMgr::GetCurTypeId() const
{
    return pCurFld ? pCurFld->GetTypeId() : USHRT_MAX;
}

/*--------------------------------------------------------------------
    Beschreibung: Ueber String  Feld einfuegen oder Update
 --------------------------------------------------------------------*/


BOOL SwFldMgr::InsertFld(  const SwInsertFld_Data& rData )
{
    SwField* pFld   = 0;
    BOOL bExp = FALSE;
    BOOL bTbl = FALSE;
    BOOL bPageVar = FALSE;
    ULONG nFormatId = rData.nFormatId;
    USHORT nSubType = rData.nSubType;
    sal_Unicode cSeparator = rData.cSeparator;
    SwWrtShell* pCurShell = rData.pSh;
    if(!pCurShell)
        pCurShell = pWrtShell ? pWrtShell : ::lcl_GetShell();
    DBG_ASSERT(pCurShell, "no SwWrtShell found");
    if(!pCurShell)
        return FALSE;

    switch(rData.nTypeId)
    {   // ACHTUNG dieses Feld wird ueber einen gesonderten Dialog eingefuegt
        case TYP_POSTITFLD:
        {
            SwPostItFieldType* pType = (SwPostItFieldType*)pCurShell->GetFldType(0, RES_POSTITFLD);
            pFld = new SwPostItField(pType, rData.sPar1, rData.sPar2, DateTime());
            break;
        }
        case TYP_SCRIPTFLD:
        {
            SwScriptFieldType* pType =
                (SwScriptFieldType*)pCurShell->GetFldType(0, RES_SCRIPTFLD);
            pFld = new SwScriptField(pType, rData.sPar1, rData.sPar2, (BOOL)nFormatId);
            break;
        }
        case TYP_COMBINED_CHARS:
        {
            SwCombinedCharFieldType* pType = (SwCombinedCharFieldType*)
                                pCurShell->GetFldType( 0, RES_COMBINED_CHARS );
            pFld = new SwCombinedCharField( pType, rData.sPar1 );
        }
        break;
        case TYP_AUTHORITY:
        {
            SwAuthorityFieldType* pType =
                (SwAuthorityFieldType*)pCurShell->GetFldType(0, RES_AUTHORITY);
            if(!pType)
            {
                pType =
                    (SwAuthorityFieldType*)pCurShell->InsertFldType(
                                    SwAuthorityFieldType(pCurShell->GetDoc()));
            }
            pFld = new SwAuthorityField(pType, rData.sPar1);
        }
        break;
        case TYP_DATEFLD:
        case TYP_TIMEFLD:
        {
            USHORT nSub = static_cast< USHORT >(rData.nTypeId == TYP_DATEFLD ? DATEFLD : TIMEFLD);
            nSub |= nSubType == DATE_VAR ? 0 : FIXEDFLD;

            SwDateTimeFieldType* pTyp =
                (SwDateTimeFieldType*)pCurShell->GetFldType(0, RES_DATETIMEFLD);
            pFld = new SwDateTimeField(pTyp, nSub, nFormatId);
            pFld->SetPar2(rData.sPar2);
            break;
        }
        case TYP_FILENAMEFLD:
        {
            SwFileNameFieldType* pTyp =
                (SwFileNameFieldType*)pCurShell->GetFldType(0, RES_FILENAMEFLD);
            pFld = new SwFileNameField(pTyp, nFormatId);
            break;
        }
        case TYP_TEMPLNAMEFLD:
        {
            SwTemplNameFieldType* pTyp =
                (SwTemplNameFieldType*)pCurShell->GetFldType(0, RES_TEMPLNAMEFLD);
            pFld = new SwTemplNameField(pTyp, nFormatId);
            break;
        }
        case TYP_CHAPTERFLD:
        {
            USHORT nByte = (USHORT)rData.sPar2.ToInt32();
            SwChapterFieldType* pTyp =
                (SwChapterFieldType*)pCurShell->GetFldType(0, RES_CHAPTERFLD);
            pFld = new SwChapterField(pTyp, nFormatId);
            nByte = Max(USHORT(1), nByte);
            nByte = Min(nByte, USHORT(MAXLEVEL));
            nByte -= 1;
            ((SwChapterField*)pFld)->SetLevel((BYTE)nByte);
            break;
        }
        case TYP_NEXTPAGEFLD:
        case TYP_PREVPAGEFLD:
        case TYP_PAGENUMBERFLD:
        {
            short nOff  = (short)rData.sPar2.ToInt32();

            if(rData.nTypeId == TYP_NEXTPAGEFLD)
            {
                if( SVX_NUM_CHAR_SPECIAL == nFormatId )
                    nOff = 1;
                else
                    nOff += 1;
                nSubType = PG_NEXT;
            }
            else if(rData.nTypeId == TYP_PREVPAGEFLD)
            {
                if( SVX_NUM_CHAR_SPECIAL == nFormatId )
                    nOff = -1;
                else
                    nOff -= 1;
                nSubType =  PG_PREV;
            }
            else
                nSubType = PG_RANDOM;

            SwPageNumberFieldType* pTyp =
                (SwPageNumberFieldType*)pCurShell->GetFldType(0, RES_PAGENUMBERFLD);
            pFld = new SwPageNumberField(pTyp, nSubType, nFormatId, nOff);

            if( SVX_NUM_CHAR_SPECIAL == nFormatId &&
                ( PG_PREV == nSubType || PG_NEXT == nSubType ) )
                ((SwPageNumberField*)pFld)->SetUserString( rData.sPar2 );
            break;
        }
        case TYP_DOCSTATFLD:
        {	SwDocStatFieldType* pTyp =
                (SwDocStatFieldType*)pCurShell->GetFldType(0, RES_DOCSTATFLD);
            pFld = new SwDocStatField(pTyp, nSubType, nFormatId);
            break;
        }
        case TYP_AUTHORFLD:
        {	SwAuthorFieldType* pTyp =
                (SwAuthorFieldType*)pCurShell->GetFldType(0, RES_AUTHORFLD);
            pFld = new SwAuthorField(pTyp, nFormatId);
            break;
        }
        case TYP_CONDTXTFLD:
        case TYP_HIDDENTXTFLD:
        {
            SwHiddenTxtFieldType* pTyp =
                (SwHiddenTxtFieldType*)pCurShell->GetFldType(0, RES_HIDDENTXTFLD);
            pFld = new SwHiddenTxtField(pTyp, TRUE, rData.sPar1, rData.sPar2, FALSE, rData.nTypeId);
            bExp = TRUE;
            break;
        }
        case TYP_HIDDENPARAFLD:
        {
            SwHiddenParaFieldType* pTyp =
                (SwHiddenParaFieldType*)pCurShell->GetFldType(0, RES_HIDDENPARAFLD);
            pFld = new SwHiddenParaField(pTyp, rData.sPar1);
            bExp = TRUE;
            break;
        }
        case TYP_SETREFFLD:
        {
            if( rData.sPar1.Len() > 0 && CanInsertRefMark( rData.sPar1 ) )
            {
                pCurShell->SetAttr( SwFmtRefMark( rData.sPar1 ) );
                return TRUE;
            }
            return FALSE;
        }
        case TYP_GETREFFLD:
        {
            SwGetRefFieldType* pTyp =
                (SwGetRefFieldType*)pCurShell->GetFldType(0, RES_GETREFFLD);
            USHORT nSeqNo = (USHORT)rData.sPar2.ToInt32();
            pFld = new SwGetRefField(pTyp, rData.sPar1, nSubType, nSeqNo, nFormatId);
            bExp = TRUE;
            break;
        }
        case TYP_DDEFLD:
        {
            //JP 28.08.95: DDE-Topics/-Items koennen Blanks in ihren
            //				Namen haben! Wird hier noch nicht beachtet.
            String sCmd( rData.sPar2 );
            USHORT nTmpPos = sCmd.SearchAndReplace( ' ', sfx2::cTokenSeperator );
            sCmd.SearchAndReplace( ' ', sfx2::cTokenSeperator, nTmpPos );

            SwDDEFieldType* pTyp = (SwDDEFieldType*)pCurShell->InsertFldType(
                    SwDDEFieldType( rData.sPar1, sCmd, (USHORT)nFormatId ));
            pFld = new SwDDEField( pTyp );
            break;
        }
        case TYP_MACROFLD:
        {
            SwMacroFieldType* pTyp =
                (SwMacroFieldType*)pCurShell->GetFldType(0, RES_MACROFLD);

            pFld = new SwMacroField(pTyp, rData.sPar1, rData.sPar2);

            break;
        }
        case TYP_INTERNETFLD:
        {
            SwFmtINetFmt aFmt( rData.sPar1, sCurFrame );
            if( pMacroItem )
                aFmt.SetMacroTbl( &pMacroItem->GetMacroTable() );
            return pCurShell->InsertURL( aFmt, rData.sPar2 );
        }
        case TYP_JUMPEDITFLD:
        {
            SwJumpEditFieldType* pTyp =
                (SwJumpEditFieldType*)pCurShell->GetFldType(0, RES_JUMPEDITFLD);

            pFld = new SwJumpEditField(pTyp, nFormatId, rData.sPar1, rData.sPar2 );
            break;
        }
        case TYP_DOCINFOFLD:
        {
            SwDocInfoFieldType* pTyp = (SwDocInfoFieldType*)pCurShell->GetFldType(
                                            0, RES_DOCINFOFLD );
            pFld = new SwDocInfoField(pTyp, nSubType, rData.sPar1, nFormatId);
            break;
        }
        case TYP_EXTUSERFLD:
        {
            SwExtUserFieldType* pTyp = (SwExtUserFieldType*)pCurShell->GetFldType(
                                            0, RES_EXTUSERFLD);
            pFld = new SwExtUserField(pTyp, nSubType, nFormatId);
            break;
        }
        case TYP_DBFLD:
        {
            SwDBData aDBData;
            String sPar1;

            if (rData.sPar1.Search(DB_DELIM) == STRING_NOTFOUND)
            {
                aDBData = pCurShell->GetDBData();
                sPar1 = rData.sPar1;
            }
            else
            {
                aDBData.sDataSource = rData.sPar1.GetToken(0, DB_DELIM);
                aDBData.sCommand = rData.sPar1.GetToken(1, DB_DELIM);
                aDBData.nCommandType = rData.sPar1.GetToken(2, DB_DELIM).ToInt32();
                sPar1 = rData.sPar1.GetToken(3, DB_DELIM);
            }

            if(aDBData.sDataSource.getLength() && pCurShell->GetDBData() != aDBData)
                pCurShell->ChgDBData(aDBData);

            SwDBFieldType* pTyp = (SwDBFieldType*)pCurShell->InsertFldType(
                    SwDBFieldType(pCurShell->GetDoc(), sPar1, aDBData) );
            pFld = new SwDBField(pTyp);
            pFld->SetSubType(nSubType);

            if( !(nSubType & nsSwExtendedSubType::SUB_OWN_FMT) ) // Datenbankformat ermitteln
            {
                Reference< XDataSource> xSource;
                rData.aDBDataSource >>= xSource;
                Reference<XConnection> xConnection;
                rData.aDBConnection >>= xConnection;
                Reference<XPropertySet> xColumn;
                rData.aDBColumn >>= xColumn;
                if(xColumn.is())
                {
                    nFormatId = pCurShell->GetNewDBMgr()->GetColumnFmt(xSource, xConnection, xColumn,
                                        pCurShell->GetNumberFormatter(), GetCurrLanguage() );
                }
                else
                    nFormatId = pCurShell->GetNewDBMgr()->GetColumnFmt(
                                        aDBData.sDataSource, aDBData.sCommand, sPar1,
                                        pCurShell->GetNumberFormatter(), GetCurrLanguage() );
            }
            pFld->ChangeFormat( nFormatId );

            bExp = TRUE;
            break;
        }
        case TYP_DBSETNUMBERFLD:
        case TYP_DBNUMSETFLD:
        case TYP_DBNEXTSETFLD:
        case TYP_DBNAMEFLD:
        {
            USHORT nPos, nTablePos, nCmdTypePos, nExpPos;
            String sPar1;
            SwDBData aDBData;

            // DBName aus rData.sPar1 extrahieren. Format: DBName.TableName.CommandType.ExpStrg
            if ((nTablePos = rData.sPar1.Search(DB_DELIM)) != STRING_NOTFOUND)
                aDBData.sDataSource = rData.sPar1.Copy(0, nTablePos++);
            if ((nCmdTypePos = rData.sPar1.Search(DB_DELIM, nTablePos)) != STRING_NOTFOUND)
            {
                aDBData.sCommand = rData.sPar1.Copy(nTablePos, nCmdTypePos++ - nTablePos);
            }
            if ((nExpPos = rData.sPar1.Search(DB_DELIM, nCmdTypePos)) != STRING_NOTFOUND)
            {
                aDBData.nCommandType = rData.sPar1.Copy(nCmdTypePos, nExpPos++ - nCmdTypePos).ToInt32();
            }
            if (nExpPos != STRING_NOTFOUND)
                nPos = nExpPos;
            else if (nTablePos != STRING_NOTFOUND)
                nPos = nTablePos;
            else
                nPos = 0;
            sPar1 = rData.sPar1.Copy(nPos);

            if (aDBData.sDataSource.getLength() && pCurShell->GetDBData() != aDBData)
                pCurShell->ChgDBData(aDBData);

            switch(rData.nTypeId)
            {
                case TYP_DBNAMEFLD:
                {
                    SwDBNameFieldType* pTyp =
                        (SwDBNameFieldType*)pCurShell->GetFldType(0, RES_DBNAMEFLD);
                    pFld = new SwDBNameField(pTyp, aDBData);

                    break;
                }
                case TYP_DBNEXTSETFLD:
                {
                    SwDBNextSetFieldType* pTyp = (SwDBNextSetFieldType*)pCurShell->GetFldType(
                                                    0, RES_DBNEXTSETFLD);
                    pFld = new SwDBNextSetField(pTyp, sPar1, rData.sPar2, aDBData);
                    bExp = TRUE;
                    break;
                }
                case TYP_DBNUMSETFLD:
                {
                    SwDBNumSetFieldType* pTyp = (SwDBNumSetFieldType*)pCurShell->GetFldType(
                                                    0, RES_DBNUMSETFLD);
                    pFld = new SwDBNumSetField( pTyp, sPar1, rData.sPar2, aDBData);
                    bExp = TRUE;
                    break;
                }
                case TYP_DBSETNUMBERFLD:
                {
                    SwDBSetNumberFieldType* pTyp = (SwDBSetNumberFieldType*)
                                                pCurShell->GetFldType(0, RES_DBSETNUMBERFLD);
                    pFld = new SwDBSetNumberField( pTyp, aDBData, nFormatId);
                    bExp = TRUE;
                    break;	
                }
            }
            break;
        }
        case TYP_USERFLD:
        {
            SwUserFieldType* pTyp =
                (SwUserFieldType*)pCurShell->GetFldType(RES_USERFLD, rData.sPar1);

            // nur wenn vorhanden
            if(!pTyp)
            {
                pTyp = (SwUserFieldType*)pCurShell->InsertFldType(
                        SwUserFieldType(pCurShell->GetDoc(), rData.sPar1));
            }
            if (pTyp->GetContent(nFormatId) != rData.sPar2)
                pTyp->SetContent(rData.sPar2, nFormatId);
            pFld = new SwUserField(pTyp, 0, nFormatId);
            if (pFld->GetSubType() != nSubType)
                pFld->SetSubType(nSubType);
            bTbl = TRUE;
            break;
        }
        case TYP_INPUTFLD:
        {
            if ((nSubType & 0x00ff) == INP_VAR)
            {
                SwSetExpFieldType* pTyp = (SwSetExpFieldType*)
                                    pCurShell->GetFldType(RES_SETEXPFLD, rData.sPar1);

                // kein Experssion Type mit dem Namen vorhanden -> anlegen
                if(pTyp)
                {
                    SwSetExpField* pExpFld =
                        new SwSetExpField(pTyp, aEmptyStr, nFormatId);

                    // Typ vom SwSetExpFieldType nicht veraendern:
                    USHORT nOldSubType = pExpFld->GetSubType();
                    pExpFld->SetSubType(nOldSubType | (nSubType & 0xff00));

                    pExpFld->SetPromptText(rData.sPar2);
                    pExpFld->SetInputFlag(TRUE) ;
                    bExp = TRUE;
                    pFld = pExpFld;
                }
                else
                    return FALSE;
            }
            else
            {
                SwInputFieldType* pTyp =
                    (SwInputFieldType*)pCurShell->GetFldType(0, RES_INPUTFLD);

                SwInputField* pInpFld =
                    new SwInputField(pTyp, rData.sPar1, rData.sPar2, nSubType|nsSwExtendedSubType::SUB_INVISIBLE, nFormatId);
                pFld = pInpFld;
            }

            // Dialog starten
            //
            pCurShell->StartInputFldDlg(pFld, FALSE, rData.pParent);
            break;
        }
        case TYP_SETFLD:
        {
            if (!rData.sPar2.Len())   // Leere Variablen sind nicht erlaubt
                return FALSE;

            SwSetExpFieldType* pTyp = (SwSetExpFieldType*)pCurShell->InsertFldType(
                    SwSetExpFieldType(pCurShell->GetDoc(), rData.sPar1) );

            SwSetExpField* pExpFld = new SwSetExpField( pTyp, rData.sPar2, nFormatId);
            pExpFld->SetSubType(nSubType);
            pExpFld->SetPar2(rData.sPar2);
            bExp = TRUE;
            pFld = pExpFld;
            break;
        }
        case TYP_SEQFLD:
        {
            SwSetExpFieldType* pTyp = (SwSetExpFieldType*)pCurShell->InsertFldType(
                    SwSetExpFieldType(pCurShell->GetDoc(), rData.sPar1, nsSwGetSetExpType::GSE_SEQ));

            BYTE nLevel = static_cast< BYTE >(nSubType & 0xff);

            pTyp->SetOutlineLvl(nLevel);
            if (nLevel != 0x7f && cSeparator == 0)
                cSeparator = '.';

            pTyp->SetDelimiter(cSeparator);
            SwSetExpField* pExpFld = new SwSetExpField(pTyp, rData.sPar2, nFormatId);
            bExp = TRUE;
            pFld = pExpFld;
            nSubType = nsSwGetSetExpType::GSE_SEQ;
            break;
        }
        case TYP_GETFLD:
        {
            // gibt es ein entprechendes SetField
            SwSetExpFieldType* pSetTyp = (SwSetExpFieldType*)
                                    pCurShell->GetFldType(RES_SETEXPFLD, rData.sPar1);

            if(pSetTyp)
            {
                SwGetExpFieldType* pTyp = (SwGetExpFieldType*)pCurShell->GetFldType(
                                                0, RES_GETEXPFLD);
                pFld = new SwGetExpField(pTyp, rData.sPar1, pSetTyp->GetType(), nFormatId);
                pFld->SetSubType(nSubType | pSetTyp->GetType());
                bExp = TRUE;
            }
            else
                return FALSE;
            break;
        }
        case TYP_FORMELFLD:
        {
            if(pCurShell->GetFrmType(0,FALSE) & FRMTYPE_TABLE)
            {
                pCurShell->StartAllAction();

                SvNumberFormatter* pFormatter = pCurShell->GetDoc()->GetNumberFormatter();
                const SvNumberformat* pEntry = pFormatter->GetEntry(nFormatId);

                if (pEntry)
                {
                    SfxStringItem aFormat(FN_NUMBER_FORMAT, pEntry->GetFormatstring());
                    pCurShell->GetView().GetViewFrame()->GetDispatcher()->
                        Execute(FN_NUMBER_FORMAT, SFX_CALLMODE_SYNCHRON, &aFormat, 0L);
                }

                SfxItemSet aBoxSet( pCurShell->GetAttrPool(),
                                RES_BOXATR_FORMULA, RES_BOXATR_FORMULA );

                String sFml( rData.sPar2 );
                if( sFml.EraseLeadingChars().Len() &&
                    '=' == sFml.GetChar( 0 ) )
                    sFml.Erase( 0, 1 );

                aBoxSet.Put( SwTblBoxFormula( sFml ));
                pCurShell->SetTblBoxFormulaAttrs( aBoxSet );
                pCurShell->UpdateTable();

                pCurShell->EndAllAction();
                return TRUE;

/*				// In der Tabelle Tabellenformeln einfuegen
                SwTblFieldType* pTyp = (SwTblFieldType*)pCurShell->GetFldType(
                                                        0, RES_TABLEFLD);
                pFld = new SwTblField(pTyp, rData.sPar2, nsSwGetSetExpType::GSE_EXPR, nFormatId);
                bTbl = TRUE;*/
            }
            else
            {
                SwGetExpFieldType* pTyp = (SwGetExpFieldType*)
                                            pCurShell->GetFldType(0, RES_GETEXPFLD);
                pFld = new SwGetExpField(pTyp, rData.sPar2, nsSwGetSetExpType::GSE_FORMULA, nFormatId);
                pFld->SetSubType(nSubType);
                bExp = TRUE;
            }
            break;
        }
        case TYP_SETREFPAGEFLD:
            pFld = new SwRefPageSetField( (SwRefPageSetFieldType*)
                                pCurShell->GetFldType( 0, RES_REFPAGESETFLD ),
                                (short)rData.sPar2.ToInt32(), 0 != nSubType  );
            bPageVar = TRUE;
            break;

        case TYP_GETREFPAGEFLD:
            pFld = new SwRefPageGetField( (SwRefPageGetFieldType*)
                            pCurShell->GetFldType( 0, RES_REFPAGEGETFLD ), nFormatId );
            bPageVar = TRUE;
            break;
        case TYP_DROPDOWN :
        {
            pFld = new SwDropDownField(pCurShell->GetFldType( 0, RES_DROPDOWN ));
            xub_StrLen nTokenCount = rData.sPar2.Len() ? rData.sPar2.GetTokenCount(DB_DELIM) : 0;
            Sequence<OUString> aEntries(nTokenCount);
            OUString* pArray = aEntries.getArray();
            for(xub_StrLen nToken = 0; nToken < nTokenCount; nToken++)
                pArray[nToken] = rData.sPar2.GetToken(nToken, DB_DELIM);
            ((SwDropDownField*)pFld)->SetItems(aEntries);
            ((SwDropDownField*)pFld)->SetName(rData.sPar1);
        }
        break;
        default:
        {	ASSERT(!this, "Falscher Feldtyp");
            return FALSE;
        }
    }
    ASSERT(pFld, "Feld nicht vorhanden");


     //the auto language flag has to be set prior to the language!
     pFld->SetAutomaticLanguage(rData.bIsAutomaticLanguage);
     USHORT nLang = GetCurrLanguage();
     pFld->SetLanguage(nLang);

    // Einfuegen
    pCurShell->StartAllAction();

    pCurShell->Insert(*pFld);

    if(bExp && bEvalExp)
        pCurShell->UpdateExpFlds(TRUE);

    if(bTbl)
    {
        pCurShell->Left(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
        pCurShell->UpdateFlds(*pFld);
        pCurShell->Right(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
    }
    else if( bPageVar )
        ((SwRefPageGetFieldType*)pCurShell->GetFldType( 0, RES_REFPAGEGETFLD ))->UpdateFlds();
    else if( TYP_GETREFFLD == rData.nTypeId )
        pFld->GetTyp()->Modify( 0, 0 );

    // temporaeres Feld loeschen
    delete pFld;

    pCurShell->EndAllAction();
    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung: Felder Update
 --------------------------------------------------------------------*/


void SwFldMgr::UpdateCurFld(ULONG nFormat,
                            const String& rPar1,
                            const String& rPar2,
                            SwField * _pTmpFld) // #111840#
{
    // Format aendern
    ASSERT(pCurFld, "kein Feld an der CursorPos");

    bool bDelete = false;
    SwField *pTmpFld;		// mb: fixed memory leak
    if (NULL != _pTmpFld)
    {
        pTmpFld = _pTmpFld;
    }
    else
    {
        pTmpFld = pCurFld->Copy();
        bDelete = true;
    }

    SwFieldType* pType   = pTmpFld->GetTyp();
    const USHORT nTypeId = pTmpFld->GetTypeId();

    SwWrtShell* pSh = pWrtShell ? pWrtShell : ::lcl_GetShell();
    DBG_ASSERT(pSh, "no SwWrtShell found");
    if(!pSh)
        return;
    pSh->StartAllAction();

    BOOL bSetPar2 = TRUE;
    BOOL bSetPar1 = TRUE;
    String sPar1( rPar1 );
    String sPar2( rPar2 );

    // Order to Format
    switch( nTypeId )
    {
        case TYP_DDEFLD:
        {
            //JP 28.08.95: DDE-Topics/-Items koennen Blanks in ihren
            //				Namen haben! Wird hier noch nicht beachtet.
            USHORT nTmpPos = sPar2.SearchAndReplace( ' ', sfx2::cTokenSeperator );
            sPar2.SearchAndReplace( ' ', sfx2::cTokenSeperator, nTmpPos );
            break;
        }

        case TYP_CHAPTERFLD:
        {
            USHORT nByte = (USHORT)rPar2.ToInt32();
            nByte = Max(USHORT(1), nByte);
            nByte = Min(nByte, USHORT(MAXLEVEL));
            nByte -= 1;
            ((SwChapterField*)pTmpFld)->SetLevel((BYTE)nByte);
            bSetPar2 = FALSE;
            break;
        }

        case TYP_SCRIPTFLD:
            ((SwScriptField*)pTmpFld)->SetCodeURL((BOOL)nFormat);
            break;

        case TYP_NEXTPAGEFLD:
            if( SVX_NUM_CHAR_SPECIAL == nFormat )
            {
                ((SwPageNumberField*)pCurFld)->SetUserString( sPar2 );
                sPar2 = '1';
            }
            else
            {
                if( nFormat + 2 == SVX_NUM_PAGEDESC )
                    nFormat = SVX_NUM_PAGEDESC;
                short nOff = (short)sPar2.ToInt32();
                nOff += 1;
                sPar2 = String::CreateFromInt32(nOff);
            }
            break;

        case TYP_PREVPAGEFLD:
            if( SVX_NUM_CHAR_SPECIAL == nFormat )
            {
                ((SwPageNumberField*)pCurFld)->SetUserString( sPar2 );
                sPar2 = String::CreateFromAscii(
                    RTL_CONSTASCII_STRINGPARAM("-1"));
            }
            else
            {
                if( nFormat + 2 == SVX_NUM_PAGEDESC )
                    nFormat = SVX_NUM_PAGEDESC;
                short nOff = (short)sPar2.ToInt32();
                nOff -= 1;
                sPar2 = String::CreateFromInt32(nOff);
            }
            break;

        case TYP_PAGENUMBERFLD:
        case TYP_GETREFPAGEFLD:
            if( nFormat + 2 == SVX_NUM_PAGEDESC )
                nFormat = SVX_NUM_PAGEDESC;
            break;

        case TYP_GETREFFLD:
            {
                bSetPar2 = FALSE;
                ((SwGetRefField*)pTmpFld)->SetSubType( (USHORT)rPar2.ToInt32() );
                USHORT nPos = rPar2.Search( '|' );
                if( STRING_NOTFOUND != nPos )
                    ((SwGetRefField*)pTmpFld)->SetSeqNo( (USHORT)rPar2.Copy( nPos + 1 ).ToInt32());
            }
            break;
        case TYP_DROPDOWN:
        {
            xub_StrLen nTokenCount = sPar2.Len() ? sPar2.GetTokenCount(DB_DELIM) : 0;
            Sequence<OUString> aEntries(nTokenCount);
            OUString* pArray = aEntries.getArray();
            for(xub_StrLen nToken = 0; nToken < nTokenCount; nToken++)
                pArray[nToken] = sPar2.GetToken(nToken, DB_DELIM);
            ((SwDropDownField*)pTmpFld)->SetItems(aEntries);
            ((SwDropDownField*)pTmpFld)->SetName(sPar1);
            bSetPar1 = bSetPar2 = FALSE;
        }
        break;
        case TYP_AUTHORITY :
        {    
            //#i99069# changes to a bibliography field should change the field type
            SwAuthorityField* pAuthorityField = static_cast<SwAuthorityField*>(pTmpFld);
            SwAuthorityFieldType* pAuthorityType = static_cast<SwAuthorityFieldType*>(pType);
            SwAuthEntry aTempEntry;
            for( USHORT i = 0; i < AUTH_FIELD_END; ++i )
                aTempEntry.SetAuthorField( (ToxAuthorityField)i,
                                rPar1.GetToken( i, TOX_STYLE_DELIMITER ));
            if( pAuthorityType->ChangeEntryContent( &aTempEntry ) )
            {
                pType->UpdateFlds();
                pSh->SetModified();
            }
        
            if( aTempEntry.GetAuthorField( AUTH_FIELD_IDENTIFIER ) == 
                pAuthorityField->GetFieldText( AUTH_FIELD_IDENTIFIER ) ) 
                bSetPar1 = FALSE; //otherwise it's a new or changed entry, the field needs to be updated    
            bSetPar2 = FALSE;
        }
        break;
    }

    // Format setzen
    // Format wegen NumberFormatter vor SetPar2 einstellen!
    pTmpFld->ChangeFormat(nFormat);

    if(bSetPar1)
        pTmpFld->SetPar1( sPar1 );
    if( bSetPar2 )
        pTmpFld->SetPar2( sPar2 );

    // Update anschmeissen
    if(nTypeId == TYP_DDEFLD ||
       nTypeId == TYP_USERFLD ||
       nTypeId == TYP_USRINPFLD)
    {
        pType->UpdateFlds();
        pSh->SetModified();
    }
    else {
        // mb: #32157
        pSh->SwEditShell::UpdateFlds(*pTmpFld);
        GetCurFld();
    }

    if (bDelete)
        delete pTmpFld;

    pSh->EndAllAction();
}

/*--------------------------------------------------------------------
    Beschreibung: ExpressionFields explizit evaluieren
 --------------------------------------------------------------------*/
void SwFldMgr::EvalExpFlds(SwWrtShell* pSh)
{
    if (pSh == NULL)
        pSh = pWrtShell ? pWrtShell : ::lcl_GetShell();

    if(pSh)
    {
        pSh->StartAllAction();
        pSh->UpdateExpFlds(TRUE);
        pSh->EndAllAction();
    }
}
USHORT SwFldMgr::GetCurrLanguage() const
{
    SwWrtShell* pSh = pWrtShell ? pWrtShell : ::lcl_GetShell();
    if( pSh )
        return pSh->GetCurLang();
    return SvxLocaleToLanguage( GetAppLocaleData().getLocale() );
}

void SwFieldType::_GetFldName()
{
    static const USHORT coFldCnt = STR_TYPE_END	- STR_TYPE_BEGIN;

    static USHORT __READONLY_DATA coFldNms[ coFldCnt ] = {
        FLD_DATE_STD,
        FLD_TIME_STD,
        STR_FILENAMEFLD,
        STR_DBNAMEFLD,
        STR_CHAPTERFLD,
        STR_PAGENUMBERFLD,
        STR_DOCSTATFLD,
        STR_AUTHORFLD,
        STR_SETFLD,
        STR_GETFLD,
        STR_FORMELFLD,
        STR_HIDDENTXTFLD,
        STR_SETREFFLD,
        STR_GETREFFLD,
        STR_DDEFLD,
        STR_MACROFLD,
        STR_INPUTFLD,
        STR_HIDDENPARAFLD,
        STR_DOCINFOFLD,
        STR_DBFLD,
        STR_USERFLD,
        STR_POSTITFLD,
        STR_TEMPLNAMEFLD,
        STR_SEQFLD,
        STR_DBNEXTSETFLD,
        STR_DBNUMSETFLD,
        STR_DBSETNUMBERFLD,
        STR_CONDTXTFLD,
        STR_NEXTPAGEFLD,
        STR_PREVPAGEFLD,
        STR_EXTUSERFLD,
        FLD_DATE_FIX,
        FLD_TIME_FIX,
        STR_SETINPUTFLD,
        STR_USRINPUTFLD,
        STR_SETREFPAGEFLD,
        STR_GETREFPAGEFLD,
        STR_INTERNETFLD,
        STR_JUMPEDITFLD,
        STR_SCRIPTFLD,
        STR_AUTHORITY,
        STR_COMBINED_CHARS,
        STR_DROPDOWN
    };

    // Infos fuer Felder einfuegen
    SwFieldType::pFldNames = new SvStringsDtor( (BYTE)coFldCnt, 2 );
    for( USHORT nIdx = 0; nIdx < coFldCnt; ++nIdx )
    {
        String* pTmp = new SW_RESSTR( coFldNms[ nIdx ] );
        pTmp->Assign( MnemonicGenerator::EraseAllMnemonicChars( *pTmp ) );
        SwFieldType::pFldNames->Insert(pTmp, nIdx );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL SwFldMgr::ChooseMacro(const String&)
{
    BOOL bRet = FALSE;

    // choose script dialog
    ::rtl::OUString aScriptURL = SfxApplication::ChooseScript();

    // the script selector dialog returns a valid script URL
    if ( aScriptURL.getLength() != 0 )
    {
        SetMacroPath( aScriptURL );
        bRet = TRUE;
    }

    return bRet;
}

void SwFldMgr::SetMacroPath(const String& rPath)
{
    sMacroPath = rPath;
    sMacroName = rPath;

    // try to set sMacroName member variable by parsing the macro path
    // using the new URI parsing services

    Reference< XMultiServiceFactory > xSMgr =
        ::comphelper::getProcessServiceFactory();

    Reference< uri::XUriReferenceFactory >
        xFactory( xSMgr->createInstance(
            ::rtl::OUString::createFromAscii(
                "com.sun.star.uri.UriReferenceFactory" ) ), UNO_QUERY );

    if ( xFactory.is() )
    {
        Reference< uri::XVndSunStarScriptUrl >
            xUrl( xFactory->parse( sMacroPath ), UNO_QUERY );

        if ( xUrl.is() )
        {
            sMacroName = xUrl->getName();
        }
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

ULONG SwFldMgr::GetDefaultFormat(USHORT nTypeId, BOOL bIsText, SvNumberFormatter* pFormatter, double* pVal)
{
    double fValue;
    short  nDefFormat;

    switch (nTypeId)
    {
        case TYP_TIMEFLD:
        case TYP_DATEFLD:
        {
            Date aDate;
            Date* pNullDate = pFormatter->GetNullDate();

            fValue = aDate - *pNullDate;

            Time aTime;

            ULONG nNumFmtTime = (ULONG)aTime.GetSec() + (ULONG)aTime.GetMin() * 60L +
                          (ULONG)aTime.GetHour() * 3600L;

            fValue += (double)nNumFmtTime / 86400.0;

            nDefFormat = (nTypeId == TYP_DATEFLD) ? NUMBERFORMAT_DATE : NUMBERFORMAT_TIME;
        }
        break;

        default:
            if (bIsText)
            {
                fValue = 0.0;
                nDefFormat = NUMBERFORMAT_TEXT;
            }
            else
            {
                fValue = 0.0;
                nDefFormat = NUMBERFORMAT_ALL;
            }
            break;
    }

    if (pVal)
        *pVal = fValue;

    return pFormatter->GetStandardFormat(nDefFormat, GetCurrLanguage());
}

/* -----------------------------01.03.01 16:46--------------------------------

 ---------------------------------------------------------------------------*/
Reference<XNumberingTypeInfo> SwFldMgr::GetNumberingInfo() const
{
    if(!xNumberingInfo.is())
    {
        Reference< XMultiServiceFactory > xMSF = ::comphelper::getProcessServiceFactory();
        Reference < XInterface > xI = xMSF->createInstance(
            ::rtl::OUString::createFromAscii(
                            "com.sun.star.text.DefaultNumberingProvider" ));
        Reference<XDefaultNumberingProvider> xDefNum(xI, UNO_QUERY);
        DBG_ASSERT(xDefNum.is(), "service missing: \"com.sun.star.text.DefaultNumberingProvider\"");
        ((SwFldMgr*)this)->xNumberingInfo = Reference<XNumberingTypeInfo>(xDefNum, UNO_QUERY);
    }
    return xNumberingInfo;
}
