/*************************************************************************
 *
 *  $RCSfile: ww8par4.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: cmc $ $Date: 2002-04-29 09:50:28 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

#ifndef _SVSTOR_HXX
#include <so3/svstor.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
 
#ifndef _SVDOOLE2_HXX
#include <svx/svdoole2.hxx>
#endif
#ifndef _MSDFFIMP_HXX
#include <svx/msdffimp.hxx>
#endif
#ifndef _SVX_IMPGRF_HXX
#include <svx/impgrf.hxx>
#endif
#ifndef _MSOCXIMEX_HXX
#include <svx/msocximex.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _FMTANCHR_HXX
#include <fmtanchr.hxx>
#endif
#ifndef _FMTCNTNT_HXX
#include <fmtcntnt.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>			// fuer Ole-Node
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>			// Progress
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _FLTSHELL_HXX
#include <fltshell.hxx>
#endif
#ifndef _UNODRAW_HXX
#include <unodraw.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif

#ifndef _WW8SCAN_HXX
#include "ww8scan.hxx"			// __WW_NEEDS_COPY
#endif
#ifndef _WW8PAR_HXX
#include "ww8par.hxx"
#endif
#ifndef _WW8PAR2_HXX
#include "ww8par2.hxx"			// WWFlyPara::BoxUpWidth()
#endif

struct OLE_MFP
{
    INT16 mm;       // 0x6  int
    INT16 xExt;     // 0x8  int in 1/100 mm
    INT16 yExt;     // 0xa  int in 1/100 mm
    INT16 hMF;      // 0xc  int
};

using namespace ::com::sun::star;

SV_IMPL_OP_PTRARR_SORT(WW8AuthorInfos, WW8AuthorInfo_Ptr)
SV_IMPL_OP_PTRARR_SORT(WW8OleMaps, WW8OleMap_Ptr)

static BOOL SwWw8ReadScaling( long& rX, long& rY, SvStorageRef& rSrc1 )
{
    // Skalierungsfaktoren holen:
    //		Informationen in PIC-Stream ( durch ausprobieren )
    //		0x0  (l)cb
    //		0x08 .. 0x0a Flags ??
    //		0x08 Inh: 1 / 0
    //		0x09 Inh: 0,8,0x18
    //		0x0a Inh: immer 8, MAP_ANISOTROPIC ???
    //		0x0b Inh: immer 0
    //		0x0c, 0x10 Originalgroesse x,y in 1/100 mm
    //		0x14, 0x16 Originalgroesse x,y in tw
    //		0x2c, 0x30 Skalierung x,y in Promille
    //		0x34, 0x38, 0x3c, 0x40 Crop Left, Top, Right, Bot in tw

    SvStorageStreamRef xSrc3 = rSrc1->OpenStream( CREATE_CONST_ASC( "\3PIC" ),
        STREAM_STD_READ | STREAM_NOCREATE);
    SvStorageStream* pS = xSrc3;
    pS->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    pS->Seek( STREAM_SEEK_TO_END );

    ASSERT( pS->Tell() >=  76, "+OLE-PIC-Stream is shorter than 76 Byte" );

    INT32 nOrgWidth,
          nOrgHeight,
          nScaleX,
          nScaleY,
          nCropLeft,
          nCropTop,
          nCropRight,
          nCropBottom;
    pS->Seek( 0x14 );
    *pS >> nOrgWidth	// Original Size in 1/100 mm
        >> nOrgHeight;
    pS->Seek( 0x2c );
    *pS >> nScaleX		// Scaling in Promille
        >> nScaleY
        >> nCropLeft	// Cropping in 1/100 mm
        >> nCropTop
        >> nCropRight
        >> nCropBottom;

    rX = nOrgWidth  - nCropLeft - nCropRight;
    rY = nOrgHeight - nCropTop  - nCropBottom;
    if (10 > nScaleX || 65536 < nScaleX || 10 > nScaleY || 65536 < nScaleY)
    {
        ASSERT( !pS, "+OLE-Scalinginformation in PIC-Stream wrong" );
        return FALSE;
    }
    else
    {
        rX = (rX * nScaleX) / 1000;
        rY = (rY * nScaleY) / 1000;
    }
    return TRUE;
}

static BOOL SwWw6ReadMetaStream(GDIMetaFile& rWMF, OLE_MFP* pMfp, 
    SvStorageRef& rSrc1)
{
    SvStorageStreamRef xSrc2 = rSrc1->OpenStream( CREATE_CONST_ASC("\3META"),
        STREAM_STD_READ | STREAM_NOCREATE);
    SvStorageStream* pSt = xSrc2;
    pSt->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    ULONG nRead = pSt->Read( pMfp, sizeof(*pMfp ) );
                                // Mini-Placable-Header lesen
    if( nRead != sizeof( *pMfp ) )
        return FALSE;

#if defined  __BIGENDIAN
    pMfp->mm = SWAPSHORT( pMfp->mm );
    pMfp->xExt = SWAPSHORT( pMfp->xExt );
    pMfp->yExt = SWAPSHORT( pMfp->yExt );
#endif // __BIGENDIAN

    if( pMfp->mm == 94 || pMfp->mm == 99 )
    {
        ASSERT( !pSt, "+OLE: Falscher Metafile-Typ" );
        return FALSE;
    }
    if( pMfp->mm != 8 )
    {
        ASSERT( !pSt, "+OLE: Falscher Metafile-Typ ( nicht Anisotropic )" );
    }
    if( !pMfp->xExt || !pMfp->yExt )
    {
        ASSERT( !pSt, "+OLE: Groesse von 0 ???" );
        return FALSE;
    }
    BOOL bOk = ReadWindowMetafile( *pSt, rWMF );	// WMF lesen
                    // *pSt >> aWMF  geht nicht ohne placable Header
    if( !bOk || pSt->GetError() || rWMF.GetActionCount() == 0 ){
        ASSERT( !pSt, "+OLE: Konnte Metafile nicht lesen" );
        return FALSE;
    }

    rWMF.SetPrefMapMode( MapMode( MAP_100TH_MM ) );


    // MetaFile auf neue Groesse skalieren und
    // neue Groesse am MetaFile setzen
    Size		aOldSiz( rWMF.GetPrefSize() );
    Size		aNewSiz( pMfp->xExt, pMfp->yExt );
    Fraction	aFracX( aNewSiz.Width(), aOldSiz.Width() );
    Fraction	aFracY( aNewSiz.Height(), aOldSiz.Height() );

    rWMF.Scale( aFracX, aFracY );
    rWMF.SetPrefSize( aNewSiz );

    return TRUE;
}

static BOOL SwWw6ReadMacPICTStream( Graphic& rGraph, SvStorageRef& rSrc1 )
{
        // 03-META-Stream nicht da. Vielleicht ein 03-PICT ?
    SvStorageStreamRef xSrc4 = rSrc1->OpenStream( CREATE_CONST_ASC( "\3PICT" ));
    SvStorageStream* pStp = xSrc4;
    pStp->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    BYTE aTestA[10];		// Ist der 01Ole-Stream ueberhaupt vorhanden
    ULONG nReadTst = pStp->Read( aTestA, sizeof( aTestA ) );
    if(nReadTst != sizeof(aTestA))
        return FALSE;

    pStp->Seek( STREAM_SEEK_TO_BEGIN );

            // Mac-Pict steht im 03PICT-StorageStream
            // allerdings ohne die ersten 512 Bytes, die
            // bei einem MAC-PICT egal sind ( werden nicht ausgewertet )

    return SwWW8ImplReader::GetPictGrafFromStream( rGraph, *pStp );
}



SwFrmFmt* SwWW8ImplReader::ImportOle( const Graphic* pGrf,
                                     const SfxItemSet* pFlySet )
{
    SwFrmFmt* pFmt = 0;
    if( !(nIniFlags & WW8FL_NO_OLE ))
    {
        Graphic aGraph;
        SdrObject* pRet = ImportOleBase( aGraph, !(bIsHeader || bIsFooter),
            pGrf, pFlySet );

        // create flyset
        SfxItemSet* pTempSet = 0;
        if( !pFlySet )
        {
            pTempSet = new SfxItemSet( rDoc.GetAttrPool(), RES_FRMATR_BEGIN,
                                                              RES_FRMATR_END-1);

            pFlySet = pTempSet;


            // Abstand/Umrandung raus
            if( !bNew )
                Reader::ResetFrmFmtAttrs( *pTempSet );	

            SwFmtAnchor aAnchor( FLY_IN_CNTNT );
            aAnchor.SetAnchor( pPaM->GetPoint() );
            pTempSet->Put( aAnchor );

            const Size aSizeTwip = OutputDevice::LogicToLogic(
                                        aGraph.GetPrefSize(),
                                        aGraph.GetPrefMapMode(),
                                        MAP_TWIP );

            pTempSet->Put( SwFmtFrmSize( ATT_FIX_SIZE, aSizeTwip.Width(),
                                                     aSizeTwip.Height() ) );
            pTempSet->Put( SwFmtVertOrient( 0, VERT_TOP, FRAME ));

            if( pSFlyPara )			// OLE im Rahmen ?
            {						// ok, Rahmen auf Bildgroesse vergroessern
                                    //	( nur wenn Auto-Breite )
                pSFlyPara->BoxUpWidth( aSizeTwip.Width() );
            }
        }

        if( pRet )		// Ole-Object wurde eingefuegt
        {
            if( pRet->ISA( SdrOle2Obj ))
            {
#ifdef NO_OLE_SIZE_OPTIMIZE
                pFmt = rDoc.Insert( *pPaM, &((SdrOle2Obj*)pRet)->GetObjRef(), 
                    pFlySet );
#else
                if( !pOleMap)
                    pOleMap = new WW8OleMaps;

                WW8OleMap *pMap = new WW8OleMap( nObjLocFc, 
                    &((SdrOle2Obj*)pRet)->GetObjRef());
                const WW8OleMap *pEntry = pMap;
    
                USHORT nPos;
                if ( pOleMap->Seek_Entry(pMap, &nPos) )
                {
                    pEntry = pOleMap->GetObject( nPos );
                    delete pMap;
                }
                else if( 0 == pOleMap->Insert( pMap) )
                    delete pMap;
                
                pFmt = rDoc.Insert( *pPaM, pEntry->pWriterRef, pFlySet );
#endif
                //JP 10.4.2001: Bug 85614 - don't remove in DTOR the
                //				object from	our persist
                SvInPlaceObjectRef xEmpty;
                ((SdrOle2Obj*)pRet)->SetObjRef( xEmpty );
                delete pRet;		// das brauchen wir nicht mehr
            }
            else
                // any OCX Control
                pFmt = FindFrmFmt( pRet );
        }
        else if( GRAPHIC_GDIMETAFILE == aGraph.GetType() ||
                 GRAPHIC_BITMAP == aGraph.GetType() )
        {
            pFmt = rDoc.Insert( *pPaM,
                        aEmptyStr, aEmptyStr,  	// Name der Grafik !!
                        &aGraph,
                        pFlySet,            	// Attribute fuer den FlyFrm
                        0 );				   	// Attribute fuer die Grafik
        }
        delete pTempSet;
    }
    return pFmt;
}

BOOL SwWW8ImplReader::ImportOleWMF(SvStorageRef xSrc1,GDIMetaFile &rWMF,
    long &rX,long &rY)
{
    BOOL bOk=FALSE;
    OLE_MFP aMfp;
    if( SwWw6ReadMetaStream( rWMF, &aMfp, xSrc1 ) )
    {
        /* 
        take scaling factor as found in PIC and apply it to graphic.
        */
        SwWw8ReadScaling( rX, rY, xSrc1 );
        Size aFinalSize, aOrigSize;
        aFinalSize.Width() = rX;
        aFinalSize.Height() = rY;
        aFinalSize = OutputDevice::LogicToLogic(
            aFinalSize, MAP_TWIP, rWMF.GetPrefMapMode() );
        aOrigSize = rWMF.GetPrefSize();
        Fraction aScaleX(aFinalSize.Width(),aOrigSize.Width());
        Fraction aScaleY(aFinalSize.Height(),aOrigSize.Height());
        rWMF.Scale( aScaleX, aScaleY );
        bOk=TRUE;
    }
    return bOk;
}

SdrObject* SwWW8ImplReader::ImportOleBase( Graphic& rGraph, BOOL bTstOCX, 
    const Graphic* pGrf, const SfxItemSet* pFlySet )
{
    SdrObject* pRet = 0;
    if( !(nIniFlags & WW8FL_NO_OLE ))
    {
        ASSERT( pStg, "ohne storage geht hier fast gar nichts!" );

        ::SetProgressState( nProgress, rDoc.GetDocShell() );	 // Update

        long nX=0, nY=0;				// nX, nY is graphic size
        BOOL bOleOk = TRUE;

        String aSrcStgName = '_';
        // ergibt Name "_4711"
        aSrcStgName += String::CreateFromInt32( nObjLocFc );		

        SvStorageRef xSrc0 = pStg->OpenStorage(CREATE_CONST_ASC(
            SL::aObjectPool));

        if( pGrf )
        {
            rGraph = *pGrf;
            const Size aSizeTwip = OutputDevice::LogicToLogic(
                rGraph.GetPrefSize(), rGraph.GetPrefMapMode(), MAP_TWIP );
            nX = aSizeTwip.Width();
            nY = aSizeTwip.Height();
        }
        else
        {
            SvStorageRef xSrc1 = xSrc0->OpenStorage( aSrcStgName,
                STREAM_READWRITE| STREAM_SHARE_DENYALL );
#if 1
            GDIMetaFile aWMF;

            if (ImportOleWMF(xSrc1,aWMF,nX,nY))
                rGraph = Graphic( aWMF );
#else
            OLE_MFP aMfp;
            GDIMetaFile aWMF;
            if( SwWw6ReadMetaStream( aWMF, &aMfp, xSrc1 ) )
            {
                /* 
                take scaling factor as found in PIC and apply it to graphic.
                */
                SwWw8ReadScaling( nX, nY, xSrc1 );
                Size aFinalSize, aOrigSize;
                aFinalSize.Width() = nX;
                aFinalSize.Height() = nY;
                aFinalSize = OutputDevice::LogicToLogic(
                    aFinalSize, MAP_TWIP, aWMF.GetPrefMapMode() );
                aOrigSize = aWMF.GetPrefSize();
                Fraction aScaleX(aFinalSize.Width(),aOrigSize.Width());
                Fraction aScaleY(aFinalSize.Height(),aOrigSize.Height());
                aWMF.Scale( aScaleX, aScaleY );
                rGraph = Graphic( aWMF );
            }
#endif
            // 03-META-Stream nicht da. Vielleicht ein 03-PICT ?
            else if( SwWw6ReadMacPICTStream( rGraph, xSrc1 ) )
            {
                const Size aSizeTwip = OutputDevice::LogicToLogic(
                    rGraph.GetPrefSize(), rGraph.GetPrefMapMode(), MAP_TWIP );
                nX = aSizeTwip.Width();
                nY = aSizeTwip.Height();
                // PICT: kein WMF da -> Grafik statt OLE
                bOleOk = FALSE;
            }
        }		// StorageStreams wieder zu

        SvStorageRef xSrc1 = xSrc0->OpenStorage( aSrcStgName,
            STREAM_READWRITE| STREAM_SHARE_DENYALL );

        if( bTstOCX )
        {
            if(!pFormImpl)
                pFormImpl = new SwMSConvertControls(rDoc.GetDocShell(),pPaM);
            uno::Reference< drawing::XShape > xRef;
            if( pFormImpl->ReadOCXStream( xSrc1,&xRef,FALSE))
            {
                uno::Reference< beans::XPropertySet >  
                    xPropSet( xRef, uno::UNO_QUERY );
                uno::Reference< lang::XUnoTunnel> 
                    xTunnel( xPropSet, uno::UNO_QUERY);

                if( xTunnel.is() )
                {
                    SwXShape *pSwShape = (SwXShape*)xTunnel->getSomething(
                        SwXShape::getUnoTunnelId() );

                    if( pSwShape )
                    {
                        SwFrmFmt* pFrmFmt = 
                            (SwFrmFmt*)pSwShape->GetRegisteredIn();

                        if( pFrmFmt )
                            pRet = pFrmFmt->FindSdrObject();
                    }
                }
                return pRet;
            }
        }

        if( GRAPHIC_GDIMETAFILE == rGraph.GetType() ||
            GRAPHIC_BITMAP == rGraph.GetType() )
        {
            ::SetProgressState( nProgress, rDoc.GetDocShell() );	 // Update

            const SwFmtFrmSize* pSize;
            Point aTmpPoint;
            Rectangle aRect( aTmpPoint, Size( nX, nY ) );

            if( pFlySet && 0 != ( pSize = (SwFmtFrmSize*)pFlySet->GetItem(
                RES_FRM_SIZE, FALSE )) )
            {
                    aRect.SetSize( pSize->GetSize() );
            }

            if( bOleOk && !( nIniFlags & WW8FL_OLE_TO_GRAF ))
            {
                ULONG nOldPos = pDataStream->Tell();
                pDataStream->Seek(STREAM_SEEK_TO_END);
                SvStream *pTmpData = 0;
                if (nObjLocFc < pDataStream->Tell())
                {
                    pTmpData = pDataStream;
                    pTmpData->Seek( nObjLocFc );
                }

                SvStorageRef xDst0( rDoc.GetDocShell()->GetStorage() );

                pRet = SvxMSDffManager::CreateSdrOLEFromStorage(
                    aSrcStgName, xSrc0, xDst0, rGraph, aRect, pTmpData,
                    SwMSDffManager::GetFilterFlags());
                pDataStream->Seek( nOldPos );
            }
        }
    }
    return pRet;
}


void SwWW8ImplReader::ReadRevMarkAuthorStrTabl( SvStream& rStrm,
    INT32 nTblPos, INT32 nTblSiz, SwDoc& rDocOut )
{
    SvStrings aAuthorNames( 0, 16 );
    WW8ReadSTTBF( !bVer67, rStrm, nTblPos, nTblSiz, bVer67 ? 2 : 0,
                    eStructCharSet, aAuthorNames );

    for( USHORT nAuthor = 0; nAuthor < aAuthorNames.Count(); ++nAuthor )
    {
        // Store author in doc
        USHORT nSWId = rDocOut.InsertRedlineAuthor( *aAuthorNames[ nAuthor ] );
        // Store matchpair
        if( !pAuthorInfos )
            pAuthorInfos = new WW8AuthorInfos;
        WW8AuthorInfo* pAutorInfo = new WW8AuthorInfo( nAuthor, nSWId );
        if( 0 == pAuthorInfos->Insert( pAutorInfo ) )
            delete pAutorInfo;
    }

    aAuthorNames.DeleteAndDestroy( 0, aAuthorNames.Count() );
}

/*
   Revision Marks ( == Redlining )
*/
// insert or delete content (change char attributes resp.)
void SwWW8ImplReader::Read_CRevisionMark(SwRedlineType eType, 
    const BYTE* pData, short nLen )
{
    // there *must* be a SprmCIbstRMark[Del] and a SprmCDttmRMark[Del]
    // pointing to the very same char position as our SprmCFRMark[Del]
    if( !pPlcxMan ) return;
    const BYTE* pSprmCIbstRMark;
    const BYTE* pSprmCDttmRMark;
    if( REDLINE_FORMAT == eType )
    {
        pSprmCIbstRMark = pData+1;
        pSprmCDttmRMark = pData+3;
    }
    else
    {
        BOOL bIns = (REDLINE_INSERT == eType);
        if( bVer67 )
        {
            pSprmCIbstRMark = pPlcxMan->HasCharSprm( 69 );
            pSprmCDttmRMark = pPlcxMan->HasCharSprm( 70 );
        }
        else
        {
            pSprmCIbstRMark = pPlcxMan->HasCharSprm( bIns ? 0x4804 : 0x4863 );
            pSprmCDttmRMark = pPlcxMan->HasCharSprm( bIns ? 0x6805 : 0x6864 );
//			pSprmCIdslRMark = pPlcxMan->HasCharSprm( bIns ? 0x4807 : 0x4867 );
        }
    }
    if( !pSprmCIbstRMark || !pSprmCDttmRMark ) return;

    if( nLen < 0 )
    {
        // end of current revision mark
        pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_FLTR_REDLINE );
    }
    else
    {
        // start of new revision mark
        USHORT nWWAutNo = SVBT16ToShort( pSprmCIbstRMark );
        UINT32 nWWDate  = SVBT32ToLong(  pSprmCDttmRMark );
        WW8AuthorInfo aEntry( nWWAutNo );
        USHORT nPos;
        if( pAuthorInfos && pAuthorInfos->Seek_Entry( &aEntry, &nPos ) )
        {
            const WW8AuthorInfo* pAuthor = pAuthorInfos->GetObject( nPos );
            if( pAuthor )
            {
                USHORT        nAutorNo = pAuthor->nOurId;
                DateTime      aStamp(WW8ScannerBase::WW8DTTM2DateTime(nWWDate));

                SwFltRedline  aNewAttr(eType, nAutorNo, aStamp);

                const SwFltRedline* pOldAttr =
                    (const SwFltRedline*)pCtrlStck->GetOpenStackAttr(
                                                        *pPaM->GetPoint(),
                                                        RES_FLTR_REDLINE );
                // 1st look if there is already another redline Attribute
                // set on this text-region
                // If so, we take it's data and store it as 'previous'
                //                              or the other way around
                if( pOldAttr )
                {
#if 0
                    // Insert on top of Delete ?  This is not allowed !
                    BOOL bDateWrongWayAround
                            =    (REDLINE_INSERT == eType)
                              && (REDLINE_DELETE == pOldAttr->eType);
                    if(    bDateWrongWayAround

                        || (      (aStamp < pOldAttr->aStamp)
                             && ! (    (REDLINE_INSERT == pOldAttr->eType)
                                    && (REDLINE_DELETE == eType)
                                  )
                           )
                      )
#else
                    /*
                    ##928##
                    Only use hack to ignore inserts on deletes, do not
                    disallow deletes on property changes
                    */
                    // Insert on top of Delete ?  This is not allowed !
                    BOOL bDateWrongWayAround =
                    (
                        (REDLINE_INSERT == eType)
                        && (REDLINE_DELETE == pOldAttr->eType)
                        && (aStamp < pOldAttr->aStamp)
                    );
                    if (bDateWrongWayAround)
#endif
                    {
                        if(     bDateWrongWayAround
                            && !(nAutorNo == pOldAttr->nAutorNo) )
                        {
                            aNewAttr.eTypePrev    = eType;
                            aNewAttr.nAutorNoPrev = nAutorNo;
                            aNewAttr.aStampPrev   = aStamp;
                            aNewAttr.eType        = pOldAttr->eType;
                            aNewAttr.nAutorNo     = pOldAttr->nAutorNo;
                            aNewAttr.aStamp       = pOldAttr->aStamp;
                        }
                        // else do nothing: so only the INSERT will be stored!
                    }
                    else
                    {
                        aNewAttr.eTypePrev    = pOldAttr->eType;
                        aNewAttr.nAutorNoPrev = pOldAttr->nAutorNo;
                        aNewAttr.aStampPrev   = pOldAttr->aStamp;
                    }
                }
                NewAttr( aNewAttr );
            }
        }
    }
}
// insert new content
void SwWW8ImplReader::Read_CFRMark(USHORT , const BYTE* pData, short nLen)
{
    Read_CRevisionMark( REDLINE_INSERT, pData, nLen );
}
// delete old content
void SwWW8ImplReader::Read_CFRMarkDel(USHORT , const BYTE* pData, short nLen)
{
    Read_CRevisionMark( REDLINE_DELETE, pData, nLen );
}
// change properties of content ( == char formating)
void SwWW8ImplReader::Read_CPropRMark(USHORT , const BYTE* pData, short nLen)
{
    // complex (len is always 7)
    // 1 byte  - chp.fPropRMark
    // 2 bytes - chp.ibstPropRMark
    // 4 bytes - chp.dttmPropRMark;
    Read_CRevisionMark( REDLINE_FORMAT, pData, nLen );
}
