/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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


#include <hintids.hxx>

#ifndef _RTL_USTRING_HXX //autogen
#include <rtl/ustring.hxx>
#endif
#include <svtools/imap.hxx>
#include <svtools/imapobj.hxx>
#include <basic/sbx.hxx>
#include <frmfmt.hxx>
#include <fmtinfmt.hxx>
#include <fmturl.hxx>
#include <frmatr.hxx>
#include <docary.hxx>
#include <doc.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <swevent.hxx>

using namespace ::com::sun::star::uno;
using ::rtl::OUString;

static Sequence<Any> *lcl_docbasic_convertArgs( SbxArray& rArgs )
{
    Sequence<Any> *pRet = 0;

    USHORT nCount = rArgs.Count();
    if( nCount > 1 )
    {
        nCount--;
        pRet = new Sequence<Any>( nCount );
        Any *pUnoArgs = pRet->getArray();
        for( USHORT i=0; i<nCount; i++ )
        {
            SbxVariable *pVar = rArgs.Get( i+1 );
            switch( pVar->GetType() )
            {
            case SbxSTRING:
                pUnoArgs[i] <<= OUString( pVar->GetString() );
                break;
            case SbxCHAR:
                pUnoArgs[i] <<= (sal_Int16)pVar->GetChar() ;
                break;
            case SbxUSHORT:
                pUnoArgs[i] <<= (sal_Int16)pVar->GetUShort();
                break;
            case SbxLONG:
                pUnoArgs[i] <<= (sal_Int32)pVar->GetLong();
                break;
            default:
                pUnoArgs[i].setValue(0, ::getVoidCppuType());
                break;
            }
        }
    }

    return pRet;
}

BOOL SwDoc::ExecMacro( const SvxMacro& rMacro, String* pRet, SbxArray* pArgs )
{
    ErrCode eErr = 0;
    switch( rMacro.GetScriptType() )
    {
    case STARBASIC:
        {
            SbxBaseRef aRef;
            SbxValue* pRetValue = new SbxValue;
            aRef = pRetValue;
            eErr = pDocShell->CallBasic( rMacro.GetMacName(),
                                         rMacro.GetLibName(),
                                         0, pArgs, pRet ? pRetValue : 0 );

            if( pRet && SbxNULL <  pRetValue->GetType() &&
                        SbxVOID != pRetValue->GetType() )
                // gueltiger Wert, also setzen
                *pRet = pRetValue->GetString();
        }
        break;
    case JAVASCRIPT:
        // ignore JavaScript calls
        break;
    case EXTENDED_STYPE:
        {
            Sequence<Any> *pUnoArgs = 0;
            if( pArgs )
            {
                // better to rename the local function to lcl_translateBasic2Uno and
                // a much shorter routine can be found in sfx2/source/doc/objmisc.cxx
                pUnoArgs = lcl_docbasic_convertArgs( *pArgs );
            }

            if (!pUnoArgs)
            {
                pUnoArgs = new Sequence< Any > (0);
            }

            // TODO - return value is not handled
            Any aRet;
            Sequence< sal_Int16 > aOutArgsIndex;
            Sequence< Any > aOutArgs;

            OSL_TRACE( "SwDoc::ExecMacro URL is %s", ByteString( rMacro.GetMacName(),
                RTL_TEXTENCODING_UTF8).GetBuffer() );

            eErr = pDocShell->CallXScript(
                rMacro.GetMacName(), *pUnoArgs, aRet, aOutArgsIndex, aOutArgs);

            //*pRet = pRetValue->GetString();
            // use the AnyConverter to return a String if appropriate?

            // need to call something like lcl_translateUno2Basic
            // pArgs = lcl_translateUno2Basic( pUnoArgs );

            delete pUnoArgs;
            break;
        }
    }

    return 0 == eErr;
}



USHORT SwDoc::CallEvent( USHORT nEvent, const SwCallMouseEvent& rCallEvent,
                    BOOL bCheckPtr, SbxArray* pArgs, const Link* )
{
    if( !pDocShell )		// ohne DocShell geht das nicht!
        return 0;

    USHORT nRet = 0;
    const SvxMacroTableDtor* pTbl = 0;
    switch( rCallEvent.eType )
    {
    case EVENT_OBJECT_INETATTR:
        if( bCheckPtr  )
        {
            const SfxPoolItem* pItem;
            USHORT n, nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_INETFMT );
            for( n = 0; n < nMaxItems; ++n )
                if( 0 != (pItem = GetAttrPool().GetItem( RES_TXTATR_INETFMT, n ) )
                    && rCallEvent.PTR.pINetAttr == pItem )
                {
                    bCheckPtr = FALSE;		// als Flag missbrauchen
                    break;
                }
        }
        if( !bCheckPtr )
            pTbl = rCallEvent.PTR.pINetAttr->GetMacroTbl();
        break;

    case EVENT_OBJECT_URLITEM:
    case EVENT_OBJECT_IMAGE:
        {
            const SwFrmFmtPtr pFmt = (SwFrmFmtPtr)rCallEvent.PTR.pFmt;
            if( bCheckPtr )
            {
                USHORT nPos = GetSpzFrmFmts()->GetPos( pFmt );
                if( USHRT_MAX != nPos )
                    bCheckPtr = FALSE;		// als Flag missbrauchen
            }
            if( !bCheckPtr )
                pTbl = &pFmt->GetMacro().GetMacroTable();
        }
        break;

    case EVENT_OBJECT_IMAGEMAP:
        {
            const IMapObject* pIMapObj = rCallEvent.PTR.IMAP.pIMapObj;
            if( bCheckPtr )
            {
                const SwFrmFmtPtr pFmt = (SwFrmFmtPtr)rCallEvent.PTR.IMAP.pFmt;
                USHORT nPos = GetSpzFrmFmts()->GetPos( pFmt );
                const ImageMap* pIMap;
                if( USHRT_MAX != nPos &&
                    0 != (pIMap = pFmt->GetURL().GetMap()) )
                {
                    for( nPos = pIMap->GetIMapObjectCount(); nPos; )
                        if( pIMapObj == pIMap->GetIMapObject( --nPos ))
                        {
                            bCheckPtr = FALSE;		// als Flag missbrauchen
                            break;
                        }
                }
            }
            if( !bCheckPtr )
                pTbl = &pIMapObj->GetMacroTable();
        }
        break;
    default:
        break;
    }

    if( pTbl )
    {
        nRet = 0x1;
        if( pTbl->IsKeyValid( nEvent ) )
        {
            const SvxMacro& rMacro = *pTbl->Get( nEvent );
            if( STARBASIC == rMacro.GetScriptType() )
            {
                nRet += 0 == pDocShell->CallBasic( rMacro.GetMacName(),
                                    rMacro.GetLibName(), 0, pArgs ) ? 1 : 0;
            }
            else if( EXTENDED_STYPE == rMacro.GetScriptType() )
            {
                Sequence<Any> *pUnoArgs = 0;

                if( pArgs )
                {
                    pUnoArgs = lcl_docbasic_convertArgs( *pArgs );
                }

                if (!pUnoArgs)
                {
                    pUnoArgs = new Sequence <Any> (0);
                }

                Any aRet;
                Sequence< sal_Int16 > aOutArgsIndex;
                Sequence< Any > aOutArgs;

                OSL_TRACE( "SwDoc::CallEvent URL is %s", ByteString(
                    rMacro.GetMacName(), RTL_TEXTENCODING_UTF8).GetBuffer() );

                nRet += 0 == pDocShell->CallXScript(
                    rMacro.GetMacName(), *pUnoArgs,aRet, aOutArgsIndex, aOutArgs) ? 1 : 0;

                //*pRet = pRetValue->GetString();
                // use the AnyConverter to return a String if appropriate?

                // need to call something like lcl_translateUno2Basic
                // pArgs = lcl_translateUno2Basic( pUnoArgs );

                delete pUnoArgs;
            }
            // JavaScript calls are ignored
        }
    }
    return nRet;
}




