/*************************************************************************
 *
 *  $RCSfile: xmltexte.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: mib $ $Date: 2001-03-16 12:50:14 $
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

#ifndef _IPOBJ_HXX 
#include <so3/ipobj.hxx>
#endif

#ifndef _XMLOFF_FAMILIES_HXX_
#include <xmloff/families.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_XMLKYWD_HXX
#include <xmloff/xmlkywd.hxx>
#endif

#include <svx/svdobj.hxx>
#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _UNOSTYLE_HXX
#include <unostyle.hxx>
#endif
#ifndef _UNOFRAME_HXX
#include <unoframe.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif

#ifndef _XMLEXP_HXX
#include "xmlexp.hxx"
#endif
#ifndef _XMLTEXTE_HXX
#include "xmltexte.hxx"
#endif

#ifndef _APPLET_HXX //autogen
#include <so3/applet.hxx>
#endif
#ifndef _PLUGIN_HXX //autogen
#include <so3/plugin.hxx>
#endif

#ifndef _FRAMEOBJ_HXX //autogen
#include <sfx2/frameobj.hxx>
#endif

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _SW_APPLET_IMPL_HXX
#include <SwAppletImpl.hxx>
#endif

#define _SVSTDARR_ULONGS
#include <svtools/svstdarr.hxx>

#ifndef _SFX_FRMDESCRHXX
#include <sfx2/frmdescr.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::style;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;

enum SvEmbeddedObjectTypes
{
    SV_EMBEDDED_UNKNOWN,
    SV_EMBEDDED_APPLET,
    SV_EMBEDDED_PLUGIN,
    SV_EMBEDDED_FRAME
};

// ---------------------------------------------------------------------

SwNoTxtNode *SwXMLTextParagraphExport::GetNoTxtNode(
    const Reference < XPropertySet >& rPropSet ) const
{
    Reference<XUnoTunnel> xCrsrTunnel( rPropSet, UNO_QUERY );
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for embedded" );
    SwXFrame *pFrame = 
                (SwXFrame *)xCrsrTunnel->getSomething(
                                    SwXFrame::getUnoTunnelId() );
    ASSERT( pFrame, "SwXFrame missing" );
    SwFrmFmt *pFrmFmt = pFrame->GetFrmFmt();
    const SwFmtCntnt& rCntnt = pFrmFmt->GetCntnt();
    const SwNodeIndex *pNdIdx = rCntnt.GetCntntIdx();
    return	pNdIdx->GetNodes()[pNdIdx->GetIndex() + 1]->GetNoTxtNode();
}

void SwXMLTextParagraphExport::exportStyleContent(
        const Reference< XStyle > & rStyle )
{

    const SwXStyle* pStyle = 0;
    Reference<XUnoTunnel> xStyleTunnel( rStyle, UNO_QUERY);
    if( xStyleTunnel.is() )
    {
        pStyle = (SwXStyle*)xStyleTunnel->getSomething(
                                            SwXStyle::getUnoTunnelId() );
    }
    if( pStyle && SFX_STYLE_FAMILY_PARA == pStyle->GetFamily() )
    {
        const SwDoc *pDoc = pStyle->GetDoc();
        const SwTxtFmtColl *pColl =
            pDoc->FindTxtFmtCollByName( SwXStyleFamilies::GetUIName( pStyle->GetStyleName(), SFX_STYLE_FAMILY_PARA ) );
        ASSERT( pColl, "There is the text collection?" );
        if( pColl && RES_CONDTXTFMTCOLL == pColl->Which() )
        {
            const SwFmtCollConditions& rConditions =
                ((const SwConditionTxtFmtColl *)pColl)->GetCondColls();
            for( sal_uInt16 i=0; i < rConditions.Count(); i++ )
            {
                const SwCollCondition& rCond = *rConditions[i];

                const sal_Char *pFunc = 0;
                OUStringBuffer sBuffer( 20 );
                switch( rCond.GetCondition() )
                {
                case PARA_IN_LIST:
                    pFunc = sXML_list_level;
                    sBuffer.append( (sal_Int32)(rCond.GetSubCondition()+1) );
                    break;
                case PARA_IN_OUTLINE:
                    pFunc = sXML_outline_level;
                    sBuffer.append( (sal_Int32)(rCond.GetSubCondition()+1) );
                    break;
                case PARA_IN_FRAME:
                    pFunc = sXML_text_box;
                    break;
                case PARA_IN_TABLEHEAD:
                    pFunc = sXML_table_header;
                    break;
                case PARA_IN_TABLEBODY:
                    pFunc = sXML_table;
                    break;
                case PARA_IN_SECTION:
                    pFunc = sXML_section;
                    break;
                case PARA_IN_FOOTENOTE:
                    pFunc = sXML_footnote;
                    break;
                case PARA_IN_FOOTER:
                    pFunc = sXML_footer;
                    break;
                case PARA_IN_HEADER:
                    pFunc = sXML_header;
                    break;
                case PARA_IN_ENDNOTE:
                    pFunc = sXML_endnote;
                    break;
                }
                OUString sVal( sBuffer.makeStringAndClear() );
                
                DBG_ASSERT( pFunc, "SwXMLExport::ExportFmt: unknon condition" );
                if( pFunc )
                {
                    sBuffer.appendAscii( pFunc );
                    sBuffer.append( (sal_Unicode)'(' );
                    sBuffer.append( (sal_Unicode)')' );
                    if( sVal.getLength() )
                    {
                        sBuffer.append( (sal_Unicode)'=' );
                        sBuffer.append( sVal );
                    }
                        
                    GetExport().AddAttribute( XML_NAMESPACE_STYLE,
                                sXML_condition,
                                sBuffer.makeStringAndClear() );
                    const String& rName =
                        SwXStyleFamilies::GetProgrammaticName(
                                    rCond.GetTxtFmtColl()->GetName(),
                                    SFX_STYLE_FAMILY_PARA );
                    GetExport().AddAttribute( XML_NAMESPACE_STYLE,
                                sXML_apply_style_name, rName );
                    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE,
                                              sXML_map, sal_True, sal_True );
                }
            }
        }
    }
}

SwXMLTextParagraphExport::SwXMLTextParagraphExport(
        SwXMLExport& rExp,
         SvXMLAutoStylePoolP& rAutoStylePool ) :
    XMLTextParagraphExport( rExp, rAutoStylePool ),
    sTextTable( RTL_CONSTASCII_USTRINGPARAM( "TextTable" ) ),
    sEmbeddedObjectProtocol( RTL_CONSTASCII_USTRINGPARAM( "vnd.sun.star.EmbeddedObject:" ) )
{
}

SwXMLTextParagraphExport::~SwXMLTextParagraphExport()
{
}

void SwXMLTextParagraphExport::getTextEmbeddedObjectProperties(
    const Reference < XPropertySet >& rPropSet,
    OUString& rURL,
    sal_Bool& rExtern ) const
{
    SwOLENode *pOLENd = GetNoTxtNode( rPropSet )->GetOLENode();
    SwOLEObj& rOLEObj = pOLENd->GetOLEObj();

    rURL = sEmbeddedObjectProtocol;
    rURL += rOLEObj.GetName();
    SvInfoObject *pInfo =
        pOLENd->GetDoc()->GetPersist()->Find( rOLEObj.GetName() );
    if( pInfo )
    {
        SvGlobalName aClassName( pInfo->GetClassName() );
        rExtern = !SvFactory::IsIntern( aClassName, 0 );
    }
}

void SwXMLTextParagraphExport::setTextEmbeddedGraphicURL(
    const Reference < XPropertySet >& rPropSet,
    OUString& rURL) const
{
    if( !rURL.getLength() )
        return;

    SwGrfNode *pGrfNd = GetNoTxtNode( rPropSet )->GetGrfNode();
    if( !pGrfNd->IsGrfLink() )
    {
        String aNewURL( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.Package:") );
        aNewURL += String(rURL.copy( 1 ) );
        pGrfNd->SetNewStreamName( aNewURL );
    }
}

static void lcl_addParam ( SvXMLExport &rExport, const SvCommand &rCommand )
{
    rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_name, rCommand.GetCommand() );
    rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_value, rCommand.GetArgument() );
    SvXMLElementExport aElem( rExport, XML_NAMESPACE_DRAW, sXML_param, sal_False, sal_True );
}
static void lcl_addURL ( SvXMLExport &rExport, const String &rURL)
{
    String sRelURL( INetURLObject::AbsToRel(rURL, 
                      INetURLObject::WAS_ENCODED, 
                      INetURLObject::DECODE_UNAMBIGUOUS) );
    if (sRelURL.Len())
    {
        rExport.AddAttribute      ( XML_NAMESPACE_XLINK, sXML_href, sRelURL );
        rExport.AddAttributeASCII ( XML_NAMESPACE_XLINK, sXML_type, sXML_simple );
        rExport.AddAttributeASCII ( XML_NAMESPACE_XLINK, sXML_show, sXML_embed );
        rExport.AddAttributeASCII ( XML_NAMESPACE_XLINK, sXML_actuate, sXML_onLoad );
    }
}

void SwXMLTextParagraphExport::_exportTextEmbedded(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    SwOLENode *pOLENd = GetNoTxtNode( rPropSet )->GetOLENode();
    SwOLEObj& rOLEObj = pOLENd->GetOLEObj();
    SvPlugInObjectRef pPlugin ( rOLEObj.GetOleRef() );
    SvAppletObjectRef pApplet ( rOLEObj.GetOleRef() );
    SfxFrameObjectRef pFrame  ( rOLEObj.GetOleRef() );
    SvEmbeddedObjectTypes nType = SV_EMBEDDED_UNKNOWN;
    SvULongs aParams;
    sal_Char *pElementName;
    SvXMLExport &rExport = GetExport();

    if (pApplet.Is())
        nType = SV_EMBEDDED_APPLET;
    else if (pPlugin.Is())
        nType = SV_EMBEDDED_PLUGIN;
    else if (pApplet.Is())
        nType = SV_EMBEDDED_APPLET;

    if ( nType == SV_EMBEDDED_UNKNOWN )
    {
        XMLTextParagraphExport::_exportTextEmbedded ( rPropSet, rPropSetInfo );
    }
    else
    {
        // First the stuff common to each of Applet/Plugin/Floating Frame
        OUString sStyle;
        Any aAny;
        if( rPropSetInfo->hasPropertyByName( sFrameStyleName ) )
        {
            aAny = rPropSet->getPropertyValue( sFrameStyleName );
            aAny >>= sStyle;
        }

        OUString sAutoStyle( sStyle );
        sAutoStyle = Find( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet, sStyle );
        if( sAutoStyle.getLength() )
            rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_style_name,
                                  sAutoStyle );
        addTextFrameAttributes( rPropSet, sal_False );

        switch (nType)
        {
            case SV_EMBEDDED_APPLET:
            {
                // It's an applet!
                const XubString & rURL = pApplet->GetCodeBase();
                if (rURL.Len() )
                    lcl_addURL(rExport, rURL);

                const String &rName = pApplet->GetName();
                if (rName.Len())
                    rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_name, rName );

                rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_code, pApplet->GetClass() );
                
                const SvCommandList& rCommands = pApplet->GetCommandList();

                ULONG i = rCommands.Count();
                while ( i > 0 )
                {
                    const SvCommand& rCommand = rCommands [ --i ];
                    const String &rName = rCommand.GetCommand();
                    USHORT nType = SwApplet_Impl::GetOptionType( rName, TRUE );
                    if ( nType == SWHTML_OPTTYPE_TAG)
                        rExport.AddAttribute( XML_NAMESPACE_DRAW, rName, rCommand.GetArgument());
                    else if (SWHTML_OPTTYPE_PARAM == nType )
                        aParams.Insert( i, aParams.Count() );
                }
                
                if ( pApplet->IsMayScript ( ) )
                    rExport.AddAttributeASCII( XML_NAMESPACE_DRAW, sXML_may_script, sXML_true );
                else
                    rExport.AddAttributeASCII( XML_NAMESPACE_DRAW, sXML_may_script, sXML_false );
                pElementName = sXML_applet;
                break;
            }
            case SV_EMBEDDED_PLUGIN:
            {
                // It's a plugin!
                lcl_addURL( rExport, pPlugin->GetURL()->GetMainURL() );
                const String &rType = pPlugin->GetMimeType();
                if (rType.Len())
                    rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_mime_type, rType );
                pElementName = sXML_plugin;
                break;
            }
            case SV_EMBEDDED_FRAME:
            {
                // It's a floating frame!
                const SfxFrameDescriptor *pDescriptor = pFrame->GetFrameDescriptor();
               
                lcl_addURL( rExport, pDescriptor->GetURL().GetMainURL() );

                const String &rName = pDescriptor->GetName();
                if (rName.Len())
                    rExport.AddAttribute( XML_NAMESPACE_DRAW, sXML_name, rName );
                pElementName = sXML_floating_frame;
                break;
            }
            default:
                ASSERT( 0, "unknown object type! Base class should have been called!" );
        }
        SvXMLElementExport aElem( rExport, XML_NAMESPACE_DRAW, pElementName, sal_False, sal_True );
        if ( nType == SV_EMBEDDED_APPLET)
        {
            const SvCommandList& rCommands = pApplet->GetCommandList();
            USHORT ii = aParams.Count();
            while ( ii > 0 )
            {
                const SvCommand& rCommand = rCommands [ aParams [ --ii] ];
                lcl_addParam (rExport, rCommand );
            }
        } 
        else if (nType == SV_EMBEDDED_PLUGIN )
        {
            const SvCommandList& rCommands = pPlugin->GetCommandList();
            ULONG nCommands = rCommands.Count();
            for ( ULONG i = 0; i < nCommands; i++)
            {
                const SvCommand& rCommand = rCommands [ i ];
                const String& rName = rCommand.GetCommand();
                if (SwApplet_Impl::GetOptionType( rName, FALSE ) == SWHTML_OPTTYPE_TAG )
                    lcl_addParam (rExport, rCommand );
            }
        }
        // Lastly the stuff common to each of Applet/Plugin/Floating Frame
        exportEvents( rPropSet );
        exportAlternativeText( rPropSet, rPropSetInfo ); 
        exportContour( rPropSet, rPropSetInfo );
    }
}
