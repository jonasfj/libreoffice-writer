/*************************************************************************
 *
 *  $RCSfile: xmltexti.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: mib $ $Date: 2001-03-06 11:05:07 $
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
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _SO_CLSIDS_HXX
#include <so3/clsids.hxx>
#endif
#ifndef _EMBOBJ_HXX 
#include <so3/embobj.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_ 
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _UNOCRSR_HXX
#include "unocrsr.hxx"
#endif
#ifndef _UNOOBJ_HXX
#include "unoobj.hxx"
#endif
#ifndef _UNOFRAME_HXX
#include "unoframe.hxx"
#endif
#ifndef _DOC_HXX 
#include "doc.hxx"
#endif
#ifndef _UNOCOLL_HXX
#include "unocoll.hxx"
#endif
#ifndef _SW3IO_HXX
#include <sw3io.hxx>
#endif
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif

#ifndef _XMLIMP_HXX
#include "xmlimp.hxx"
#endif
#ifndef _XMLTBLI_HXX
#include "xmltbli.hxx"
#endif
#ifndef _XMLTEXTI_HXX
#include "xmltexti.hxx"
#endif
#ifndef _XMLREDLINEIMPORTHELPER_HXX
#include "XMLRedlineImportHelper.hxx"
#endif

#ifndef _SW_APPLET_IMPL_HXX
#include <SwAppletImpl.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::xml::sax;


SwXMLTextImportHelper::SwXMLTextImportHelper(
        const Reference < XModel>& rModel,
        sal_Bool bInsertM, sal_Bool bStylesOnlyM, sal_Bool bProgress,
        sal_Bool bBlockM, sal_Bool bOrganizerM ) :
    XMLTextImportHelper( rModel, bInsertM, bStylesOnlyM, bProgress, bBlockM,
                         bOrganizerM ),
    pRedlineHelper(NULL)
{
}

SwXMLTextImportHelper::~SwXMLTextImportHelper()
{
    delete pRedlineHelper;
}

SvXMLImportContext *SwXMLTextImportHelper::CreateTableChildContext(
                SvXMLImport& rImport,
                sal_uInt16 nPrefix, const OUString& rLocalName,
                const Reference< XAttributeList > & xAttrList )
{
    return new SwXMLTableContext( 
                (SwXMLImport&)rImport, nPrefix, rLocalName, xAttrList );
}

sal_Bool SwXMLTextImportHelper::IsInHeaderFooter() const
{
    Reference<XUnoTunnel> xCrsrTunnel(
            ((SwXMLTextImportHelper *)this)->GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    SwXTextCursor *pTxtCrsr = 
                (SwXTextCursor*)xCrsrTunnel->getSomething(
                                            SwXTextCursor::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    return pDoc->IsInHeaderFooter( pTxtCrsr->GetPaM()->GetPoint()->nNode );
}

Reference< XPropertySet > SwXMLTextImportHelper::createAndInsertOLEObject(
           SvXMLImport& rImport,
        const OUString& rHRef,
        const OUString& rClassId,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    Reference < XPropertySet > xPropSet;

    sal_Int32 nPos = rHRef.indexOf( ':' );
    if( -1 == nPos )
        return xPropSet;

    String aObjName( rHRef.copy( nPos+1) );

    if( !aObjName.Len() )
        return xPropSet;

    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    SwXTextCursor *pTxtCrsr = 
                (SwXTextCursor*)xCrsrTunnel->getSomething(
                                            SwXTextCursor::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRM_SIZE, RES_FRM_SIZE );
    if( nWidth > 0 && nHeight > 0 )
    {
        nWidth = MM100_TO_TWIP( nWidth );
        if( nWidth < MINFLY )
            nWidth = MINFLY;
        nHeight = MM100_TO_TWIP( nHeight );
        if( nHeight < MINFLY )
            nHeight = MINFLY;
        aItemSet.Put( SwFmtFrmSize( ATT_FIX_SIZE, nWidth, nHeight ) );
    }
    SwFrmFmt *pFrmFmt = pDoc->InsertOLE( *pTxtCrsr->GetPaM(),
                                         aObjName, &aItemSet );
    xPropSet = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
    return xPropSet;
}
Reference< XPropertySet > SwXMLTextImportHelper::createApplet(
        const OUString &rCode,
        const OUString &rName,
        sal_Bool bMayScript,
        const OUString& rHRef,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    Reference < XPropertySet > xPropSet;
    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    SwXTextCursor *pTxtCrsr = 
                (SwXTextCursor*)xCrsrTunnel->getSomething(
                                            SwXTextCursor::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRM_SIZE, RES_FRM_SIZE );
    if( nWidth > 0 && nHeight > 0 )
    {
        nWidth = MM100_TO_TWIP( nWidth );
        if( nWidth < MINFLY )
            nWidth = MINFLY;
        nHeight = MM100_TO_TWIP( nHeight );
        if( nHeight < MINFLY )
            nHeight = MINFLY;
        aItemSet.Put( SwFmtFrmSize( ATT_FIX_SIZE, nWidth, nHeight ) );
    }
    pAppletImpl = new SwApplet_Impl( aItemSet );
    pAppletImpl->CreateApplet( rCode, rName, bMayScript, rHRef );
    SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(), 
                                       pAppletImpl->GetApplet(),
                                       &pAppletImpl->GetItemSet());
    xPropSet = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
    return xPropSet;
}
Reference< XPropertySet > SwXMLTextImportHelper::createPlugin(
        const OUString &rMimeType,
        const OUString& rHRef,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    Reference < XPropertySet > xPropSet;
    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    SwXTextCursor *pTxtCrsr = 
                (SwXTextCursor*)xCrsrTunnel->getSomething(
                                            SwXTextCursor::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRM_SIZE, RES_FRM_SIZE );
    if( nWidth > 0 && nHeight > 0 )
    {
        nWidth = MM100_TO_TWIP( nWidth );
        if( nWidth < MINFLY )
            nWidth = MINFLY;
        nHeight = MM100_TO_TWIP( nHeight );
        if( nHeight < MINFLY )
            nHeight = MINFLY;
        aItemSet.Put( SwFmtFrmSize( ATT_FIX_SIZE, nWidth, nHeight ) );
    }
       INetURLObject aURLObj;
    if( rHRef.getLength() && !aURLObj.SetURL( INetURLObject::RelToAbs(rHRef) ) )
        return xPropSet;
    SvStorageRef pStor = new SvStorage( aEmptyStr, STREAM_STD_READWRITE);
    SvFactory *pPlugInFactory = SvFactory::GetDefaultPlugInFactory();
    xPlugin = &pPlugInFactory->CreateAndInit( *pPlugInFactory, pStor );

    xPlugin->EnableSetModified( FALSE );
    xPlugin->SetPlugInMode( (USHORT)PLUGIN_EMBEDED );
    xPlugin->SetURL( aURLObj );
    xPlugin->SetMimeType( rMimeType );

    SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(), 
                                       xPlugin,
                                       &aItemSet);
    xPropSet = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
    return xPropSet;
}
Reference< XPropertySet > SwXMLTextImportHelper::createFloatingFrame(
        const OUString& rHRef,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    Reference < XPropertySet > xPropSet;
    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    SwXTextCursor *pTxtCrsr = 
                (SwXTextCursor*)xCrsrTunnel->getSomething(
                                            SwXTextCursor::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRM_SIZE, RES_FRM_SIZE );
    if( nWidth > 0 && nHeight > 0 )
    {
        nWidth = MM100_TO_TWIP( nWidth );
        if( nWidth < MINFLY )
            nWidth = MINFLY;
        nHeight = MM100_TO_TWIP( nHeight );
        if( nHeight < MINFLY )
            nHeight = MINFLY;
        aItemSet.Put( SwFmtFrmSize( ATT_FIX_SIZE, nWidth, nHeight ) );
    }
    SfxFrameDescriptor *pFrameDesc = new SfxFrameDescriptor( 0 );

    pFrameDesc->SetURL( INetURLObject::RelToAbs( rHRef ) );

    SvStorageRef pStor = new SvStorage( aEmptyStr, STREAM_STD_READWRITE );
    SfxFrameObjectRef pFrame = new SfxFrameObject();
    pFrame->DoInitNew( pStor );

    pFrame->EnableSetModified( FALSE );
    pFrame->SetFrameDescriptor( pFrameDesc );
    pFrame->EnableSetModified( TRUE );

    SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(), 
                                       pFrame,
                                       &aItemSet);
    xPropSet = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
    return xPropSet;
}
void SwXMLTextImportHelper::addParam( 
    const ::rtl::OUString &rName,
    const ::rtl::OUString &rValue,
    sal_Bool bApplet)
{
    if (bApplet)
    {
        if (!pAppletImpl)
            return;
        pAppletImpl->AppendParam( rName, rValue );
    }
    else
    {
        if (!xPlugin.Is())
            return;
        aCmdList.Append( rName, rValue );
    }
}
void SwXMLTextImportHelper::setAlternateText( 
    const ::rtl::OUString &rAlt,
    sal_Bool bApplet)
{
    if (bApplet)
    {
        if (!pAppletImpl)
            return;
        pAppletImpl->SetAltText( rAlt );
    }
    /*
    else
    {
        if (!xPlugin.Is())
            return;
    }
    */
}

void SwXMLTextImportHelper::endApplet()
{
    if (!pAppletImpl)
        return;
    pAppletImpl->FinishApplet();
    delete pAppletImpl; pAppletImpl = 0;
}
void SwXMLTextImportHelper::endPlugin()
{
    if (!xPlugin.Is())
        return;
    xPlugin->SetCommandList( aCmdList );
    xPlugin->EnableSetModified ( TRUE );
    xPlugin.Clear();
    aCmdList.Clear();
}

XMLTextImportHelper* SwXMLImport::CreateTextImport()
{
    return new SwXMLTextImportHelper( GetModel(), IsInsertMode(),
                                      IsStylesOnlyMode(), bShowProgress, 
                                      IsBlockMode(), IsOrganizerMode() );
}


// redlining helper methods
// (override to provide the real implementation)

void SwXMLTextImportHelper::RedlineAdd( 
    const OUString& rType,
    const OUString& rId,
    const OUString& rAuthor,
    const OUString& rComment,
    const util::DateTime& rDateTime)
{
    // create redline helper on demand
    if (NULL == pRedlineHelper)
        pRedlineHelper = new XMLRedlineImportHelper(IsInsertMode() || 
                                                    IsBlockMode() );

    pRedlineHelper->Add(rType, rId, rAuthor, rComment, rDateTime);
}

Reference<XTextCursor> SwXMLTextImportHelper::RedlineCreateText(
    Reference<XTextCursor> & rOldCursor,
    const OUString& rId)
{
    Reference<XTextCursor> xRet;

    if (NULL != pRedlineHelper)
    {
        xRet = pRedlineHelper->CreateRedlineTextSection(rOldCursor, rId);
    }

    return xRet;
}

void SwXMLTextImportHelper::RedlineSetCursor(
    const OUString& rId,
    sal_Bool bStart,
    sal_Bool bIsOutsideOfParagraph)
{
    if (NULL != pRedlineHelper)
        pRedlineHelper->SetCursor(rId, bStart, GetCursor()->getStart(),
                                  bIsOutsideOfParagraph);
    // else: ignore redline (wasn't added before, else we'd have a helper)
}

void SwXMLTextImportHelper::RedlineAdjustStartNodeCursor(
    sal_Bool bStart)
{
    OUString& rId = GetOpenRedlineId();
    if ((NULL != pRedlineHelper) && (rId.getLength() > 0))
    {
        pRedlineHelper->AdjustStartNodeCursor(rId, bStart, 
                                              GetCursor()->getStart());
        ResetOpenRedlineId();
    }
    // else: ignore redline (wasn't added before, or no open redline ID
}
