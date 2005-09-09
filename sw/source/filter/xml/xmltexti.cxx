/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmltexti.cxx,v $
 *
 *  $Revision: 1.42 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:27:50 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#pragma hdrstop

#include <comphelper/storagehelper.hxx>
#include <comphelper/processfactory.hxx>

#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDOBJECTCREATOR_HPP_
#include <com/sun/star/embed/XEmbedObjectCreator.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XLINKCREATOR_HPP_
#include <com/sun/star/embed/XLinkCreator.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDDEDOBJECT_HPP_
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XVISUALOBJECT_HPP_
#include <com/sun/star/embed/XVisualObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#include <sot/clsids.hxx>
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _XMLOFF_PRSTYLEI_HXX_
#include <xmloff/prstylei.hxx>
#endif
#ifndef _XMLOFF_PROPMAPPINGTYPES_HXX
#include <xmloff/maptype.hxx>
#endif
#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX
#include <xmloff/xmlprmap.hxx>
#endif
#ifndef _XMLOFF_TXTPRMAP_HXX
#include <xmloff/txtprmap.hxx>
#endif
#ifndef _XMLOFF_I18NMAP_HXX
#include <xmloff/i18nmap.hxx>
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
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif
#ifndef _FMTANCHR_HXX
#include <fmtanchr.hxx>
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
#ifndef _XMLOFF_XMLFILTERSERVICENAMES_H
#include <xmloff/XMLFilterServiceNames.h>
#endif

#ifndef _SW_APPLET_IMPL_HXX
#include <SwAppletImpl.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _SWDOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif

#ifndef _NDNOTXT_HXX
#include <ndnotxt.hxx>
#endif

// for locking SolarMutex: svapp + mutex
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#include <toolkit/helper/vclunohelper.hxx>
#include <svtools/embedhlp.hxx>
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::xml::sax;


struct XMLServiceMapEntry_Impl
{
    const sal_Char *sFilterService;
    sal_Int32	   nFilterServiceLen;

    sal_uInt32	n1;
    sal_uInt16	n2, n3;
    sal_uInt8	n4, n5, n6, n7, n8, n9, n10, n11;
};

#define SERVICE_MAP_ENTRY( app, s ) \
    { XML_IMPORT_FILTER_##app, sizeof(XML_IMPORT_FILTER_##app)-1, \
      SO3_##s##_CLASSID	}

const XMLServiceMapEntry_Impl aServiceMap[] =
{
    SERVICE_MAP_ENTRY( WRITER, SW ),
    SERVICE_MAP_ENTRY( CALC, SC ),
    SERVICE_MAP_ENTRY( DRAW, SDRAW ),
    SERVICE_MAP_ENTRY( IMPRESS, SIMPRESS ),
    SERVICE_MAP_ENTRY( CHART, SCH ),
    SERVICE_MAP_ENTRY( MATH, SM ),
    { 0, 0, 0, 0 }
};
static void lcl_putHeightAndWidth ( SfxItemSet &rItemSet,
        sal_Int32 nHeight, sal_Int32 nWidth,
        sal_Int32 *pTwipHeight=0, sal_Int32 *pTwipWidth=0 )
{
    if( nWidth > 0 && nHeight > 0 )
    {
        nWidth = MM100_TO_TWIP( nWidth );
        if( nWidth < MINFLY )
            nWidth = MINFLY;
        nHeight = MM100_TO_TWIP( nHeight );
        if( nHeight < MINFLY )
            nHeight = MINFLY;
        rItemSet.Put( SwFmtFrmSize( ATT_FIX_SIZE, nWidth, nHeight ) );
    }

    SwFmtAnchor aAnchor( FLY_AUTO_CNTNT );
    rItemSet.Put( aAnchor );

    if( pTwipWidth )
        *pTwipWidth = nWidth;
    if( pTwipHeight )
        *pTwipHeight = nHeight;
}

static void lcl_setObjectVisualArea( const uno::Reference< embed::XEmbeddedObject >& xObj,
                                    sal_Int64 nAspect,
                                    const Size& aVisSize,
                                    const MapUnit& aUnit )
{
    if( xObj.is() )
    {
        // convert the visual area to the objects units
        MapUnit aObjUnit = VCLUnoHelper::UnoEmbed2VCLMapUnit( xObj->getMapUnit( nAspect ) );
        Size aObjVisSize = OutputDevice::LogicToLogic( aVisSize, aUnit, aObjUnit );
        awt::Size aSz;
        aSz.Width = aObjVisSize.Width();
        aSz.Height = aObjVisSize.Height();

        try
        {
            xObj->setVisualAreaSize( nAspect, aSz );
        }
        catch( uno::Exception& )
        {
            OSL_ASSERT( "Couldn't set visual area of the object!\n" );
        }
    }
}

SwXMLTextImportHelper::SwXMLTextImportHelper(
        const Reference < XModel>& rModel,
        SvXMLImport& rImport,
        const Reference<XPropertySet> & rInfoSet,
        sal_Bool bInsertM, sal_Bool bStylesOnlyM, sal_Bool bProgress,
        sal_Bool bBlockM, sal_Bool bOrganizerM,
        sal_Bool bPreserveRedlineMode ) :
    XMLTextImportHelper( rModel, rImport, bInsertM, bStylesOnlyM, bProgress,
                         bBlockM, bOrganizerM ),
    pRedlineHelper( NULL )
{
    Reference<XPropertySet> xDocPropSet( rModel, UNO_QUERY );
    pRedlineHelper = new XMLRedlineImportHelper(
        bInsertM || bBlockM, xDocPropSet, rInfoSet );
}

SwXMLTextImportHelper::~SwXMLTextImportHelper()
{
    // #90463# the redline helper destructor sets properties on the document
    //         and may through an exception while doing so... catch this
    try
    {
        delete pRedlineHelper;
    }
    catch ( const RuntimeException& )
    {
        // ignore
    }
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
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    return pDoc->IsInHeaderFooter( pTxtCrsr->GetPaM()->GetPoint()->nNode );
}

SwOLENode *lcl_GetOLENode( const SwFrmFmt *pFrmFmt )
{
    SwOLENode *pOLENd = 0;
    if( pFrmFmt )
    {
        const SwFmtCntnt& rCntnt = pFrmFmt->GetCntnt();
        const SwNodeIndex *pNdIdx = rCntnt.GetCntntIdx();
        pOLENd = pNdIdx->GetNodes()[pNdIdx->GetIndex() + 1]->GetOLENode();
    }
    ASSERT( pOLENd, "Where is the OLE node" );
    return pOLENd;
}

Reference< XPropertySet > SwXMLTextImportHelper::createAndInsertOLEObject(
           SvXMLImport& rImport,
        const OUString& rHRef,
        const OUString& rStyleName,
        const OUString& rTblName,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    // this method will modify the document directly -> lock SolarMutex
    vos::OGuard aGuard(Application::GetSolarMutex());

    Reference < XPropertySet > xPropSet;

    sal_Int32 nPos = rHRef.indexOf( ':' );
    if( -1 == nPos )
        return xPropSet;

    OUString aObjName( rHRef.copy( nPos+1) );

    if( !aObjName.getLength() )
        return xPropSet;

    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                         RES_FRMATR_END );
    Size aTwipSize( 0, 0 );
    Rectangle aVisArea( 0, 0, nWidth, nHeight );
    lcl_putHeightAndWidth( aItemSet, nHeight, nWidth,
                           &aTwipSize.Height(), &aTwipSize.Width() );

    SwFrmFmt *pFrmFmt = 0;
    SwOLENode *pOLENd = 0;
    if( rHRef.copy( 0, nPos ).equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.ServiceName") ) )
    {
        sal_Bool bInsert = sal_False;
        SvGlobalName aClassName;
        const XMLServiceMapEntry_Impl *pEntry = aServiceMap;
        while( pEntry->sFilterService )
        {
            if( aObjName.equalsAsciiL( pEntry->sFilterService,
                                     pEntry->nFilterServiceLen ) )
            {
                aClassName = SvGlobalName( pEntry->n1, pEntry->n2,
                                           pEntry->n3, pEntry->n4,
                                           pEntry->n5, pEntry->n6,
                                           pEntry->n7, pEntry->n8,
                                           pEntry->n9, pEntry->n10,
                                           pEntry->n11  );
                bInsert = sal_True;
                break;
            }
            pEntry++;
        }

        if( bInsert )
        {
            Reference < embed::XStorage > xStorage = comphelper::OStorageHelper::GetTemporaryStorage();
            try
            {
                // create object with desired ClassId
                sal_Int64 nAspect = embed::Aspects::MSOLE_CONTENT;
                ::rtl::OUString aName = ::rtl::OUString::createFromAscii( "DummyName" );
                uno::Sequence < sal_Int8 > aClass( aClassName.GetByteSequence() );
                uno::Reference < embed::XEmbedObjectCreator > xFactory( ::comphelper::getProcessServiceFactory()->createInstance(
                        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.embed.EmbeddedObjectCreator")) ), uno::UNO_QUERY );
                uno::Reference < embed::XEmbeddedObject > xObj =
                    uno::Reference < embed::XEmbeddedObject >( xFactory->createInstanceInitNew(
                    aClass, ::rtl::OUString(), xStorage, aName,
                    uno::Sequence < beans::PropertyValue >() ), uno::UNO_QUERY );
                if ( xObj.is() )
                {
                    //TODO/LATER: is it enough to only set the VisAreaSize?
                    lcl_setObjectVisualArea( xObj, nAspect, aTwipSize, MAP_TWIP );
                }

                pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(), xObj, &aItemSet );
                pOLENd = lcl_GetOLENode( pFrmFmt );
                if( pOLENd )
                    aObjName = pOLENd->GetOLEObj().GetCurrentPersistName();
            }
            catch ( uno::Exception& )
            {
            }
        }
    }
    else
    {
        // check whether an object with this name already exists in the document
        String aName;
        SwClientIter aIter( *(SwModify*)pDoc->GetDfltGrfFmtColl() );
        for( SwCntntNode* pNd = (SwCntntNode*)aIter.First( TYPE( SwCntntNode ) );
                pNd; pNd = (SwCntntNode*)aIter.Next() )
        {
            SwOLENode* pOLENd = pNd->GetOLENode();
            if( pOLENd )
            {
                ::rtl::OUString aExistingName = pOLENd->GetOLEObj().GetCurrentPersistName();
                if ( aExistingName.equals( aObjName ) )
                {
                    OSL_ENSURE( sal_False, "The document contains duplicate object references, means it is partially broken, please let developers know how this document was generated!\n" );

                    ::rtl::OUString aTmpName = pDoc->GetPersist()->GetEmbeddedObjectContainer().CreateUniqueObjectName();
                    try
                    {
                        pDoc->GetPersist()->GetStorage()->copyElementTo( aObjName,
                                                                         pDoc->GetPersist()->GetStorage(),
                                                                         aTmpName );
                        aName = aTmpName;
                    }
                    catch ( uno::Exception& )
                    {
                        OSL_ENSURE( sal_False, "Couldn't create a copy of the object!\n" );
                    }

                    break;
                }
            }
        }
        
        if ( !aName.Len() )
            aName = aObjName;

        pFrmFmt = pDoc->InsertOLE( *pTxtCrsr->GetPaM(), aName, &aItemSet );
        aObjName = aName;
    }

    if( !pFrmFmt )
        return xPropSet;

    if( IsInsertMode() )
    {
        if( !pOLENd )
            pOLENd = lcl_GetOLENode( pFrmFmt );
        if( pOLENd )
            pOLENd->SetOLESizeInvalid( sal_True );
    }

    SwXFrame *pXFrame = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
    xPropSet = pXFrame;
    if( pDoc->GetDrawModel() )
        SwXFrame::GetOrCreateSdrObject(
                static_cast<SwFlyFrmFmt*>( pXFrame->GetFrmFmt() ) ); // req for z-order
    if( rTblName.getLength() )
    {
        const SwFmtCntnt& rCntnt = pFrmFmt->GetCntnt();
        const SwNodeIndex *pNdIdx = rCntnt.GetCntntIdx();
        SwOLENode *pOLENd = pNdIdx->GetNodes()[pNdIdx->GetIndex() + 1]->GetOLENode();
        ASSERT( pOLENd, "Where is the OLE node" );

        OUStringBuffer aBuffer( rTblName.getLength() );
        sal_Bool bQuoted = sal_False;
        sal_Bool bEscape = sal_False;
        sal_Bool bError = sal_False;
        for( sal_Int32 i=0; i < rTblName.getLength(); i++ )
        {
            sal_Bool bEndOfNameFound = sal_False;
            sal_Unicode c = rTblName[i];
            switch( c )
            {
            case '\'':
                if( bEscape )
                {
                    aBuffer.append( c );
                    bEscape = sal_False;
                }
                else if( bQuoted )
                {
                    bEndOfNameFound = sal_True;
                }
                else if( 0 == i )
                {
                    bQuoted = sal_True;
                }
                else
                {
                    bError = sal_True;
                }
                break;
            case '\\':
                if( bEscape )
                {
                    aBuffer.append( c );
                    bEscape = sal_False;
                }
                else
                {
                    bEscape = sal_True;
                }
                break;
            case ' ':
            case '.':
                if( !bQuoted )
                {
                    bEndOfNameFound = sal_True;
                }
                else
                {
                    aBuffer.append( c );
                    bEscape = sal_False;
                }
                break;
            default:
                {
                    aBuffer.append( c );
                    bEscape = sal_False;
                }
                break;
            }
            if( bError || bEndOfNameFound )
                break;
        }
        if( !bError )
        {
            OUString sTblName( aBuffer.makeStringAndClear() );
            pOLENd->SetChartTblName( GetRenameMap().Get( XML_TEXT_RENAME_TYPE_TABLE, sTblName ) );
        }
    }

    sal_Int32 nDrawAspect = 0;
    const XMLPropStyleContext *pStyle = 0;
    sal_Bool bHasSizeProps = sal_False;
    if( rStyleName.getLength() )
    {
        pStyle = FindAutoFrameStyle( rStyleName );
        if( pStyle )
        {
            UniReference < SvXMLImportPropertyMapper > xImpPrMap =
                pStyle->GetStyles()
                      ->GetImportPropertyMapper(pStyle->GetFamily());
            ASSERT( xImpPrMap.is(), "Where is the import prop mapper?" );
            if( xImpPrMap.is() )
            {
                UniReference<XMLPropertySetMapper> rPropMapper =
                xImpPrMap->getPropertySetMapper();

                sal_Int32 nCount = pStyle->GetProperties().size();
                for( sal_Int32 i=0; i < nCount; i++ )
                {
                    const XMLPropertyState& rProp = pStyle->GetProperties()[i];
                    sal_Int32 nIdx = rProp.mnIndex;
                    if( -1 == nIdx )
                        continue;

                    switch( rPropMapper->GetEntryContextId(nIdx) )
                    {
                    case CTF_OLE_VIS_AREA_LEFT:
                        {
                            sal_Int32 nVal = 0;
                            rProp.maValue >>= nVal;
                            aVisArea.setX( nVal );
                        }
                        break;
                    case CTF_OLE_VIS_AREA_TOP:
                        {
                            sal_Int32 nVal = 0;
                            rProp.maValue >>= nVal;
                            aVisArea.setY( nVal );
                        }
                        break;
                    case CTF_OLE_VIS_AREA_WIDTH:
                        {
                            sal_Int32 nVal = 0;
                            rProp.maValue >>= nVal;
                            aVisArea.setWidth( nVal );
                            bHasSizeProps = sal_True;
                        }
                        break;
                    case CTF_OLE_VIS_AREA_HEIGHT:
                        {
                            sal_Int32 nVal = 0;
                            rProp.maValue >>= nVal;
                            aVisArea.setHeight( nVal );
                            bHasSizeProps = sal_True;
                        }
                        break;
                    case CTF_OLE_DRAW_ASPECT:
                        {
                            rProp.maValue >>= nDrawAspect;
                        }
                        break;
                    }
                }
            }
        }
    }

    if ( bHasSizeProps )
    {
        Reference < embed::XEmbeddedObject > xObj =
                    pDoc->GetPersist()->GetEmbeddedObjectContainer().GetEmbeddedObject( aObjName );
        if( xObj.is() )
            lcl_setObjectVisualArea( xObj, ( nDrawAspect ? nDrawAspect : embed::Aspects::MSOLE_CONTENT ),
                                     aVisArea.GetSize(), MAP_100TH_MM );
    }

    return xPropSet;
}

Reference< XPropertySet > SwXMLTextImportHelper::createAndInsertOOoLink(
           SvXMLImport& rImport,
        const OUString& rHRef,
        const OUString& rStyleName,
        const OUString& rTblName,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    // this method will modify the document directly -> lock SolarMutex
    vos::OGuard aGuard(Application::GetSolarMutex());

    Reference < XPropertySet > xPropSet;

    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                         RES_FRMATR_END );
    Size aTwipSize( 0, 0 );
    Rectangle aVisArea( 0, 0, nWidth, nHeight );
    lcl_putHeightAndWidth( aItemSet, nHeight, nWidth,
                           &aTwipSize.Height(), &aTwipSize.Width() );

    // We'll need a (valid) URL. If we don't have do not insert the link and return early.
    // Copy URL into URL oject on the way.
       INetURLObject aURLObj;
    bool bValidURL = rHRef.getLength() != 0 &&
                     aURLObj.SetURL( URIHelper::SmartRel2Abs(
                                INetURLObject( GetXMLImport().GetBaseURL() ), rHRef ) );
    if( !bValidURL )
        return xPropSet;

    Reference < embed::XStorage > xStorage = comphelper::OStorageHelper::GetTemporaryStorage();
    try
    {
        // create object with desired ClassId
        ::rtl::OUString aName = ::rtl::OUString::createFromAscii( "DummyName" );
        uno::Reference < embed::XLinkCreator > xFactory( ::comphelper::getProcessServiceFactory()->createInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.embed.OOoEmbeddedObjectFactory")) ),
                uno::UNO_QUERY_THROW );

        uno::Sequence< beans::PropertyValue > aMediaDescriptor( 1 );
        aMediaDescriptor[0].Name = ::rtl::OUString::createFromAscii( "URL" );
        aMediaDescriptor[0].Value <<= ::rtl::OUString( aURLObj.GetMainURL( INetURLObject::NO_DECODE ) );
        if ( pDoc && pDoc->GetDocShell() && pDoc->GetDocShell()->GetMedium() )
        {
            uno::Reference< task::XInteractionHandler > xInteraction =
                                        pDoc->GetDocShell()->GetMedium()->GetInteractionHandler();
            if ( xInteraction.is() )
            {
                aMediaDescriptor.realloc( 2 );
                aMediaDescriptor[1].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "InteractionHandler" ) );
                aMediaDescriptor[1].Value <<= xInteraction;
            }
        }

        uno::Reference < embed::XEmbeddedObject > xObj(
            xFactory->createInstanceLink(
                xStorage, aName, aMediaDescriptor, uno::Sequence< beans::PropertyValue >() ),
            uno::UNO_QUERY_THROW );

        {
            SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(),
                                            xObj,
                                            &aItemSet );

            // TODO/LATER: in future may need a way to set replacement image url to the link ( may be even to the object ), needs oasis cws???

            SwXFrame *pXFrame = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
            xPropSet = pXFrame;
            if( pDoc->GetDrawModel() )
                SwXFrame::GetOrCreateSdrObject(
                        static_cast<SwFlyFrmFmt*>( pXFrame->GetFrmFmt() ) ); // req for z-order
        }
    }
    catch ( uno::Exception& )
    {
    }

    // TODO/LATER: should the rStyleName and rTblName be handled as for usual embedded object?

    return xPropSet;
}

Reference< XPropertySet > SwXMLTextImportHelper::createAndInsertApplet(
        const OUString &rName,
        const OUString &rCode,
        sal_Bool bMayScript,
        const OUString& rHRef,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    // this method will modify the document directly -> lock SolarMutex
    vos::OGuard aGuard(Application::GetSolarMutex());

    Reference < XPropertySet > xPropSet;
    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                         RES_FRMATR_END );
    lcl_putHeightAndWidth( aItemSet, nHeight, nWidth);

    SwApplet_Impl aAppletImpl ( aItemSet );

    String sCodeBase;
    if( rHRef.getLength() )
        sCodeBase = GetXMLImport().GetAbsoluteReference( rHRef );

    aAppletImpl.CreateApplet ( rCode, rName, bMayScript, sCodeBase, GetXMLImport().GetDocumentBase() );

    // set the size of the applet
    lcl_setObjectVisualArea( aAppletImpl.GetApplet(),
                            embed::Aspects::MSOLE_CONTENT,
                            Size( nWidth, nHeight ),
                            MAP_100TH_MM );

    SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(),
                                       aAppletImpl.GetApplet(),
                                       &aAppletImpl.GetItemSet());
    SwXFrame *pXFrame = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
    xPropSet = pXFrame;
    if( pDoc->GetDrawModel() )
        SwXFrame::GetOrCreateSdrObject(
                static_cast<SwFlyFrmFmt*>( pXFrame->GetFrmFmt() ) ); // req for z-order

    return xPropSet;
}

Reference< XPropertySet > SwXMLTextImportHelper::createAndInsertPlugin(
        const OUString &rMimeType,
        const OUString& rHRef,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    Reference < XPropertySet > xPropSet;
    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                         RES_FRMATR_END );
    lcl_putHeightAndWidth( aItemSet, nHeight, nWidth);

    // We'll need a (valid) URL, or we need a MIME type. If we don't have
    // either, do not insert plugin and return early. Copy URL into URL oject
    // on the way.
       INetURLObject aURLObj;

    bool bValidURL = rHRef.getLength() != 0 &&
                     aURLObj.SetURL( URIHelper::SmartRel2Abs( INetURLObject( GetXMLImport().GetBaseURL() ), rHRef ) );
    bool bValidMimeType = rMimeType.getLength() != 0;
    if( !bValidURL && !bValidMimeType )
        return xPropSet;

    Reference < embed::XStorage > xStorage = comphelper::OStorageHelper::GetTemporaryStorage();
    try
    {
        // create object with desired ClassId
        ::rtl::OUString aName = ::rtl::OUString::createFromAscii( "DummyName" );
        uno::Sequence < sal_Int8 > aClass( SvGlobalName( SO3_PLUGIN_CLASSID ).GetByteSequence() );
        uno::Reference < embed::XEmbedObjectCreator > xFactory( ::comphelper::getProcessServiceFactory()->createInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.embed.EmbeddedObjectCreator")) ), uno::UNO_QUERY );
        uno::Reference < embed::XEmbeddedObject > xObj =
            uno::Reference < embed::XEmbeddedObject >( xFactory->createInstanceInitNew(
            aClass, ::rtl::OUString(), xStorage, aName,
            uno::Sequence < beans::PropertyValue >() ), uno::UNO_QUERY );

        // set size to the object
        lcl_setObjectVisualArea( xObj,
                                embed::Aspects::MSOLE_CONTENT,
                                Size( nWidth, nHeight ),
                                MAP_100TH_MM );

        if ( svt::EmbeddedObjectRef::TryRunningState( xObj ) )
        {
            uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
            if ( xSet.is() )
            {
                if( bValidURL )
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginURL"),
                        makeAny( ::rtl::OUString( aURLObj.GetMainURL( INetURLObject::NO_DECODE ) ) ) );
                if( bValidMimeType )
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginMimeType"),
                        makeAny( ::rtl::OUString( rMimeType ) ) );
            }

            SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(),
                                            xObj,
                                            &aItemSet);
            SwXFrame *pXFrame = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
            xPropSet = pXFrame;
            if( pDoc->GetDrawModel() )
                SwXFrame::GetOrCreateSdrObject(
                        static_cast<SwFlyFrmFmt*>( pXFrame->GetFrmFmt() ) ); // req for z-order
        }
    }
    catch ( uno::Exception& )
    {
    }

    return xPropSet;
}
Reference< XPropertySet > SwXMLTextImportHelper::createAndInsertFloatingFrame(
        const OUString& rName,
        const OUString& rHRef,
        const OUString& rStyleName,
        sal_Int32 nWidth, sal_Int32 nHeight )
{
    // this method will modify the document directly -> lock SolarMutex
    vos::OGuard aGuard(Application::GetSolarMutex());

    Reference < XPropertySet > xPropSet;
    Reference<XUnoTunnel> xCrsrTunnel( GetCursor(), UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    SfxItemSet aItemSet( pDoc->GetAttrPool(), RES_FRMATR_BEGIN,
                         RES_FRMATR_END );
    lcl_putHeightAndWidth( aItemSet, nHeight, nWidth);

    ScrollingMode eScrollMode = ScrollingAuto;
    sal_Bool bHasBorder = sal_False;
    sal_Bool bIsBorderSet = sal_False;
    Size aMargin( SIZE_NOT_SET, SIZE_NOT_SET );
    const XMLPropStyleContext *pStyle = 0;
    if( rStyleName.getLength() )
    {
        pStyle = FindAutoFrameStyle( rStyleName );
        if( pStyle )
        {
            UniReference < SvXMLImportPropertyMapper > xImpPrMap =
                pStyle->GetStyles()
                      ->GetImportPropertyMapper(pStyle->GetFamily());
            ASSERT( xImpPrMap.is(), "Where is the import prop mapper?" );
            if( xImpPrMap.is() )
            {
                UniReference<XMLPropertySetMapper> rPropMapper =
                xImpPrMap->getPropertySetMapper();

                sal_Int32 nCount = pStyle->GetProperties().size();
                for( sal_Int32 i=0; i < nCount; i++ )
                {
                    const XMLPropertyState& rProp = pStyle->GetProperties()[i];
                    sal_Int32 nIdx = rProp.mnIndex;
                    if( -1 == nIdx )
                        continue;

                    switch( rPropMapper->GetEntryContextId(nIdx) )
                    {
                    case CTF_FRAME_DISPLAY_SCROLLBAR:
                        {
                            sal_Bool bYes = *(sal_Bool *)rProp.maValue.getValue();
                            eScrollMode = bYes ? ScrollingYes : ScrollingNo;
                        }
                        break;
                    case CTF_FRAME_DISPLAY_BORDER:
                        {
                            bHasBorder = *(sal_Bool *)rProp.maValue.getValue();
                            bIsBorderSet = sal_True;
                        }
                        break;
                    case CTF_FRAME_MARGIN_HORI:
                        {
                            sal_Int32 nVal = SIZE_NOT_SET;
                            rProp.maValue >>= nVal;
                            aMargin.Width() = nVal;
                        }
                        break;
                    case CTF_FRAME_MARGIN_VERT:
                        {
                            sal_Int32 nVal = SIZE_NOT_SET;
                            rProp.maValue >>= nVal;
                            aMargin.Height() = nVal;
                        }
                        break;
                    }
                }
            }
        }
    }

    Reference < embed::XStorage > xStorage = comphelper::OStorageHelper::GetTemporaryStorage();
    try
    {
        // create object with desired ClassId
        ::rtl::OUString aName = ::rtl::OUString::createFromAscii( "DummyName" );
        uno::Sequence < sal_Int8 > aClass( SvGlobalName( SO3_IFRAME_CLASSID ).GetByteSequence() );
        uno::Reference < embed::XEmbedObjectCreator > xFactory( ::comphelper::getProcessServiceFactory()->createInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.embed.EmbeddedObjectCreator")) ), uno::UNO_QUERY );
        uno::Reference < embed::XEmbeddedObject > xObj =
            uno::Reference < embed::XEmbeddedObject >( xFactory->createInstanceInitNew(
            aClass, ::rtl::OUString(), xStorage, aName,
            uno::Sequence < beans::PropertyValue >() ), uno::UNO_QUERY );

        // set size to the object
        lcl_setObjectVisualArea( xObj,
                                embed::Aspects::MSOLE_CONTENT,
                                Size( nWidth, nHeight ),
                                MAP_100TH_MM );

        if ( svt::EmbeddedObjectRef::TryRunningState( xObj ) )
        {
            uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
            if ( xSet.is() )
            {
                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameURL"),
                    makeAny( ::rtl::OUString( URIHelper::SmartRel2Abs(
                            INetURLObject( GetXMLImport().GetBaseURL() ), rHRef ) ) ) );

                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameName"),
                    makeAny( ::rtl::OUString( rName ) ) );

                if ( eScrollMode == ScrollingAuto )
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsAutoScroll"),
                        makeAny( sal_True ) );
                else
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsScrollingMode"),
                        makeAny( (sal_Bool) (eScrollMode == ScrollingYes) ) );

                if ( bIsBorderSet )
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsBorder"),
                        makeAny( bHasBorder ) );
                else
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsAutoBorder"),
                        makeAny( sal_True ) );

                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameMarginWidth"),
                    makeAny( sal_Int32( aMargin.Width() ) ) );

                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameMarginHeight"),
                    makeAny( sal_Int32( aMargin.Height() ) ) );
            }

            SwFrmFmt *pFrmFmt = pDoc->Insert( *pTxtCrsr->GetPaM(),
                                            xObj,
                                            &aItemSet);
            SwXFrame *pXFrame = SwXFrames::GetObject( *pFrmFmt, FLYCNTTYPE_OLE );
            xPropSet = pXFrame;
            if( pDoc->GetDrawModel() )
                SwXFrame::GetOrCreateSdrObject(
                        static_cast<SwFlyFrmFmt*>( pXFrame->GetFrmFmt() ) ); // req for z-order
        }
    }
    catch ( uno::Exception& )
    {
    }

    return xPropSet;
}

void SwXMLTextImportHelper::endAppletOrPlugin(
        const Reference < XPropertySet > &rPropSet,
        ::std::map < const ::rtl::OUString, ::rtl::OUString, ::comphelper::UStringLess > &rParamMap)
{
    // this method will modify the document directly -> lock SolarMutex
    vos::OGuard aGuard(Application::GetSolarMutex());

    Reference<XUnoTunnel> xCrsrTunnel( rPropSet, UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for embedded" );
    SwXFrame *pFrame =
                (SwXFrame *)xCrsrTunnel->getSomething(
                                    SwXFrame::getUnoTunnelId() );
    ASSERT( pFrame, "SwXFrame missing" );
    SwFrmFmt *pFrmFmt = pFrame->GetFrmFmt();
    const SwFmtCntnt& rCntnt = pFrmFmt->GetCntnt();
    const SwNodeIndex *pNdIdx = rCntnt.GetCntntIdx();
    SwOLENode *pOLENd = pNdIdx->GetNodes()[pNdIdx->GetIndex() + 1]->GetNoTxtNode()->GetOLENode();
    SwOLEObj& rOLEObj = pOLENd->GetOLEObj();

    uno::Reference < embed::XEmbeddedObject > xEmbObj( rOLEObj.GetOleRef() );
    if ( svt::EmbeddedObjectRef::TryRunningState( xEmbObj ) )
    {
        uno::Reference < beans::XPropertySet > xSet( xEmbObj->getComponent(), uno::UNO_QUERY );
        if ( xSet.is() )
        {
            const sal_Int32 nCount = rParamMap.size();
            uno::Sequence< beans::PropertyValue > aCommandSequence( nCount );

            ::std::map < const ::rtl::OUString, ::rtl::OUString, ::comphelper::UStringLess > ::iterator aIter = rParamMap.begin();
            ::std::map < const ::rtl::OUString, ::rtl::OUString, ::comphelper::UStringLess > ::iterator aEnd = rParamMap.end();
            sal_Int32 nIndex=0;
            while (aIter != aEnd )
            {
                aCommandSequence[nIndex].Name = (*aIter).first;
                aCommandSequence[nIndex].Handle = -1;
                aCommandSequence[nIndex].Value = makeAny( OUString((*aIter).second) );
                aCommandSequence[nIndex].State = beans::PropertyState_DIRECT_VALUE;
                aIter++, nIndex++;
            }

            // unfortunately the names of the properties are depending on the object
            ::rtl::OUString aParaName = ::rtl::OUString::createFromAscii("AppletCommands");
            try
            {
                xSet->setPropertyValue( aParaName, makeAny( aCommandSequence ) );
            }
            catch ( uno::Exception& )
            {
                aParaName = ::rtl::OUString::createFromAscii("PluginCommands");
                try
                {
                    xSet->setPropertyValue( aParaName, makeAny( aCommandSequence ) );
                }
                catch ( uno::Exception& )
                {
                }
            }
        }
    }
}

XMLTextImportHelper* SwXMLImport::CreateTextImport()
{
    return new SwXMLTextImportHelper( GetModel(), *this, getImportInfo(),
                                      IsInsertMode(),
                                      IsStylesOnlyMode(), bShowProgress,
                                      IsBlockMode(), IsOrganizerMode(),
                                      bPreserveRedlineMode );
}


// redlining helper methods
// (override to provide the real implementation)

void SwXMLTextImportHelper::RedlineAdd(
    const OUString& rType,
    const OUString& rId,
    const OUString& rAuthor,
    const OUString& rComment,
    const util::DateTime& rDateTime,
    sal_Bool bMergeLastPara)
{
    // create redline helper on demand
    DBG_ASSERT(NULL != pRedlineHelper, "helper should have been created in constructor");
    if (NULL != pRedlineHelper)
        pRedlineHelper->Add(rType, rId, rAuthor, rComment, rDateTime,
                            bMergeLastPara);
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
    if (NULL != pRedlineHelper) {
        Reference<XTextRange> xTextRange( GetCursor()->getStart() );
        pRedlineHelper->SetCursor(rId, bStart, xTextRange,
                                  bIsOutsideOfParagraph);
    }
    // else: ignore redline (wasn't added before, else we'd have a helper)
}

void SwXMLTextImportHelper::RedlineAdjustStartNodeCursor(
    sal_Bool bStart)
{
    OUString rId = GetOpenRedlineId();
    if ((NULL != pRedlineHelper) && (rId.getLength() > 0))
    {
        Reference<XTextRange> xTextRange( GetCursor()->getStart() );
        pRedlineHelper->AdjustStartNodeCursor(rId, bStart, xTextRange );
        ResetOpenRedlineId();
    }
    // else: ignore redline (wasn't added before, or no open redline ID
}

void SwXMLTextImportHelper::SetShowChanges( sal_Bool bShowChanges )
{
    if ( NULL != pRedlineHelper )
        pRedlineHelper->SetShowChanges( bShowChanges );
}

void SwXMLTextImportHelper::SetRecordChanges( sal_Bool bRecordChanges )
{
    if ( NULL != pRedlineHelper )
        pRedlineHelper->SetRecordChanges( bRecordChanges );
}

void SwXMLTextImportHelper::SetChangesProtectionKey(
    const Sequence<sal_Int8> & rKey )
{
    if ( NULL != pRedlineHelper )
        pRedlineHelper->SetProtectionKey( rKey );
}



