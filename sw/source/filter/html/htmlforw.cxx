/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlforw.cxx,v $
 * $Revision: 1.24 $
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


#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/form/FormSubmitEncoding.hpp>
#include <com/sun/star/form/FormSubmitMethod.hpp>
#include <com/sun/star/form/FormButtonType.hpp>
#include <com/sun/star/script/XEventAttacher.hpp>
#include <com/sun/star/script/XEventAttacherManager.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/form/XFormsSupplier.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/form/FormComponentType.hpp>
#include <com/sun/star/awt/XTextLayoutConstrains.hpp>
#include <hintids.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <svl/macitem.hxx>
#include <tools/urlobj.hxx>
#include <svtools/htmlout.hxx>
#include <svtools/htmltokn.h>
#include <svtools/htmlkywd.hxx>
#include <svl/urihelper.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <svx/svdouno.hxx>
#include <svx/fmglob.hxx>
#include <svx/brshitem.hxx>
#include <svx/colritem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/postitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/crsditem.hxx>

#include <docsh.hxx>
#include <fmtanchr.hxx>
#include <docary.hxx>
#include <viewsh.hxx>
#include "pam.hxx"
#include "doc.hxx"
#include "ndtxt.hxx"
#include "dcontact.hxx"
#include "flypos.hxx"
#include "wrthtml.hxx"
#include "htmlfly.hxx"
#include "htmlform.hxx"


using namespace ::com::sun::star;
using ::rtl::OUString;
/*  */

const sal_uInt32 HTML_FRMOPTS_CONTROL   =
    0;
const sal_uInt32 HTML_FRMOPTS_CONTROL_CSS1  =
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SIZE |
    HTML_FRMOPT_S_SPACE |
    HTML_FRMOPT_BRCLEAR;
const sal_uInt32 HTML_FRMOPTS_IMG_CONTROL   =
    HTML_FRMOPT_ALIGN |
    HTML_FRMOPT_BRCLEAR;
const sal_uInt32 HTML_FRMOPTS_IMG_CONTROL_CSS1 =
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SPACE;


/*  */

struct HTMLControl
{
    // die Form, zu der das Control gehoert
    uno::Reference< container::XIndexContainer > xFormComps;
    ULONG nNdIdx;              		// der Node, in dem es verankert ist
    xub_StrLen nCount;              // wie viele Controls sind in dem Node

    HTMLControl( const uno::Reference< container::XIndexContainer > & rForm,
                 sal_uInt32 nIdx );
    ~HTMLControl();

    // operatoren fuer das Sort-Array
    sal_Bool operator==( const HTMLControl& rCtrl )
    {
        return nNdIdx == rCtrl.nNdIdx;
    }
    sal_Bool operator<( const HTMLControl& rCtrl )
    {
        return nNdIdx < rCtrl.nNdIdx;
    }
};

SV_IMPL_OP_PTRARR_SORT( HTMLControls, HTMLControl* )

/*  */

void lcl_html_outEvents( SvStream& rStrm,
                         const uno::Reference< form::XFormComponent > rFormComp,
                         sal_Bool bCfgStarBasic,
                         rtl_TextEncoding eDestEnc,
                           String *pNonConvertableChars )
{
    uno::Reference< container::XChild > xChild( rFormComp, uno::UNO_QUERY );
    uno::Reference< uno::XInterface > xParentIfc = xChild->getParent();
    ASSERT( xParentIfc.is(), "lcl_html_outEvents: no parent interface" );
    if( !xParentIfc.is() )
        return;
    uno::Reference< container::XIndexAccess > xIndexAcc( xParentIfc, uno::UNO_QUERY );
    uno::Reference< script::XEventAttacherManager > xEventManager( xParentIfc,
                                                              uno::UNO_QUERY );
    if( !xIndexAcc.is() || !xEventManager.is() )
        return;

    // Und die Position des ControlModel darin suchen
    sal_Int32 nCount = xIndexAcc->getCount(), nPos;
    for( nPos = 0 ; nPos < nCount; nPos++ )
    {
        uno::Any aTmp = xIndexAcc->getByIndex(nPos);
        ASSERT( aTmp.getValueType() ==
                        ::getCppuType( (uno::Reference<form::XFormComponent>*)0 ) ||
                aTmp.getValueType() ==
                        ::getCppuType( (uno::Reference<form::XForm>*)0 ),
                "lcl_html_outEvents: falsche Reflection" );
        if( aTmp.getValueType() ==
                    ::getCppuType( (uno::Reference< form::XFormComponent >*)0) )

        {
            if( rFormComp ==
                    *(uno::Reference< form::XFormComponent > *)aTmp.getValue() )
                break;
        }
        else if( aTmp.getValueType() ==
                            ::getCppuType( (uno::Reference< form::XForm>*)0) )
        {
            uno::Reference< form::XFormComponent > xFC(
                *(uno::Reference< form::XForm > *)aTmp.getValue(), uno::UNO_QUERY );
            if( rFormComp == xFC )
                break;
        }
    }

    if( nPos == nCount )
        return;

    uno::Sequence< script::ScriptEventDescriptor > aDescs =
            xEventManager->getScriptEvents( nPos );
    nCount = aDescs.getLength();
    if( !nCount )
        return;

    const script::ScriptEventDescriptor *pDescs = aDescs.getConstArray();
    for( sal_Int32 i = 0; i < nCount; i++ )
    {
        ScriptType eScriptType = EXTENDED_STYPE;
        String aScriptType( pDescs[i].ScriptType );
        if( aScriptType.EqualsIgnoreCaseAscii(SVX_MACRO_LANGUAGE_JAVASCRIPT) )
            eScriptType = JAVASCRIPT;
        else if( aScriptType.EqualsIgnoreCaseAscii(SVX_MACRO_LANGUAGE_STARBASIC ) )
            eScriptType = STARBASIC;
        if( JAVASCRIPT != eScriptType && !bCfgStarBasic )
            continue;

        String sListener( pDescs[i].ListenerType );
        xub_StrLen nTok = sListener.GetTokenCount( '.' );
        if( nTok )
            sListener = sListener.GetToken( nTok-1, '.' );
        String sMethod( pDescs[i].EventMethod );

        const sal_Char *pOpt = 0;
        for( sal_uInt16 j=0; aEventListenerTable[j]; j++ )
        {
            if( sListener.EqualsAscii( aEventListenerTable[j] ) &&
                sMethod.EqualsAscii( aEventMethodTable[j] ) )
            {
                pOpt = (STARBASIC==eScriptType ? aEventSDOptionTable
                                               : aEventOptionTable)[j];
                break;
            }
        }

        ByteString sOut( ' ' );
        if( pOpt && (EXTENDED_STYPE != eScriptType ||
                     !pDescs[i].AddListenerParam.getLength()) )
            sOut += pOpt;
        else
            (((sOut += OOO_STRING_SVTOOLS_HTML_O_sdevent)
                += ByteString( sListener, RTL_TEXTENCODING_ASCII_US)) += '-')
                += ByteString( sMethod, RTL_TEXTENCODING_ASCII_US);
        sOut += "=\"";
        rStrm << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rStrm, pDescs[i].ScriptCode, eDestEnc, pNonConvertableChars );
        rStrm << '\"';
        if( EXTENDED_STYPE == eScriptType &&
            pDescs[i].AddListenerParam.getLength() )
        {
            (((((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_sdaddparam)
                += ByteString( sListener, RTL_TEXTENCODING_ASCII_US)) += '-')
                += ByteString( sMethod, RTL_TEXTENCODING_ASCII_US))
                += "=\"";
            rStrm << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rStrm, pDescs[i].AddListenerParam,
                                      eDestEnc, pNonConvertableChars );
            rStrm << '\"';
        }
    }
}

sal_Bool lcl_html_isHTMLControl( sal_Int16 nClassId )
{
    sal_Bool bRet = sal_False;

    switch( nClassId )
    {
    case form::FormComponentType::TEXTFIELD:
    case form::FormComponentType::COMMANDBUTTON:
    case form::FormComponentType::RADIOBUTTON:
    case form::FormComponentType::CHECKBOX:
    case form::FormComponentType::LISTBOX:
    case form::FormComponentType::IMAGEBUTTON:
    case form::FormComponentType::FILECONTROL:
        bRet = sal_True;
        break;
    }

    return bRet;
}

sal_Bool SwHTMLWriter::HasControls() const
{
    sal_uInt32 nStartIdx = pCurPam->GetPoint()->nNode.GetIndex();
    sal_uInt16 i;

    // Skip all controls in front of the current paragraph
    for( i = 0; i < aHTMLControls.Count() &&
        aHTMLControls[i]->nNdIdx < nStartIdx; i++ )
        ;

    return i < aHTMLControls.Count() && aHTMLControls[i]->nNdIdx == nStartIdx;
}

void SwHTMLWriter::OutForm( sal_Bool bTag_On, const SwStartNode *pStartNd )
{
    if( bPreserveForm )     // wir sind in einer Tabelle oder einem Bereich
        return;             // ueber dem eine Form aufgespannt wurde

    if( !bTag_On )
    {
        // die Form beenden wenn alle Controls ausgegeben wurden
        if( pxFormComps && pxFormComps->is() &&
            (*pxFormComps)->getCount() == nFormCntrlCnt )
        {
            OutForm( sal_False, *pxFormComps );
            (*pxFormComps) = 0;
        }
        return;
    }

    uno::Reference< container::XIndexContainer > xNewFormComps;  // die neue Form
    sal_uInt32 nStartIdx = pStartNd ? pStartNd->GetIndex()
                                    : pCurPam->GetPoint()->nNode.GetIndex();

    // Ueberspringen von Controls vor dem interesanten Bereich
    sal_uInt16 i;
    for( i = 0; i < aHTMLControls.Count() &&
        aHTMLControls[i]->nNdIdx < nStartIdx; i++ )
        ;

    if( !pStartNd )
    {
        // Check fuer einen einzelnen Node: da ist nur interessant, ob
        // es zu dem Node ein Control gibt und zu welcher Form es gehoert
        if( i < aHTMLControls.Count() &&
            aHTMLControls[i]->nNdIdx == nStartIdx )
            xNewFormComps = aHTMLControls[i]->xFormComps;
    }
    else
    {
        // wir klappern eine Tabelle/einen Bereich ab: hier interessiert uns:
        // - ob es Controls mit unterschiedlichen Start-Nodes gibt
        // - ob es eine Form gibt, fuer die nicht alle Controls in der
        //   Tabelle/dem Bereich liegen

        uno::Reference< container::XIndexContainer > xCurrentFormComps;// die aktuelle Form in der Tabelle
        const SwStartNode *pCurrentStNd = 0; // und der Start-Node eines Ctrls
        xub_StrLen nCurrentCtrls = 0;   // und die in ihr gefundenen Controls
        sal_uInt32 nEndIdx =  pStartNd->EndOfSectionIndex();
        for( ; i < aHTMLControls.Count() &&
            aHTMLControls[i]->nNdIdx <= nEndIdx; i++ )
        {
            const SwStartNode *pCntrlStNd =
                pDoc->GetNodes()[aHTMLControls[i]->nNdIdx]->StartOfSectionNode();

            if( xCurrentFormComps.is() )
            {
                // Wir befinden uns bereits in einer Form ...
                if( xCurrentFormComps==aHTMLControls[i]->xFormComps )
                {
                    // ... und das Control befindet sich auch darin ...
                    if( pCurrentStNd!=pCntrlStNd )
                    {
                        // ... aber es liegt in einer anderen Zelle:
                        // Dann muessen eir eine Form ueber der Tabelle
                        // aufmachen
                        xNewFormComps = xCurrentFormComps;
                        break;
                    }
                    nCurrentCtrls = nCurrentCtrls + aHTMLControls[i]->nCount;
                }
                else
                {
                    // ... aber das Control liegt in einer anderen Zelle:
                    // Da tun wir so, als ob wir eine neue Form aufmachen
                    // und suchen weiter.
                    xCurrentFormComps = aHTMLControls[i]->xFormComps;
                    pCurrentStNd = pCntrlStNd;
                    nCurrentCtrls = aHTMLControls[i]->nCount;
                }
            }
            else
            {
                // Wir befinden uns noch in keiner Form:
                // Da tun wir mal so, als ob wie wir die Form aufmachen.
                xCurrentFormComps = aHTMLControls[i]->xFormComps;
                pCurrentStNd = pCntrlStNd;
                nCurrentCtrls = aHTMLControls[i]->nCount;
            }
        }
        if( !xNewFormComps.is() && xCurrentFormComps.is() &&
            nCurrentCtrls != xCurrentFormComps->getCount() )
        {
            // In der Tablle/dem Bereich sollte eine Form aufgemacht werden,
            // die nicht vollstaendig in der Tabelle liegt. Dan muessen
            // wie die Form jetzt ebenfalls oeffen.
            xNewFormComps = xCurrentFormComps;
        }
    }

    if( xNewFormComps.is() &&
        (!pxFormComps || !(xNewFormComps == *pxFormComps)) )
    {
        // Es soll eine Form aufgemacht werden ...
        if( pxFormComps && pxFormComps->is() )
        {
            // .. es ist aber noch eine Form offen: Das ist in
            // jedem Fall eine Fehler, aber wir schliessen die alte
            // Form trotzdem
            OutForm( sal_False, *pxFormComps );

            //!!!nWarn = 1; // Control wird falscher Form zugeordnet
        }

        if( !pxFormComps )
            pxFormComps = new uno::Reference< container::XIndexContainer > ;
        *pxFormComps = xNewFormComps;

        OutForm( sal_True, *pxFormComps );
        uno::Reference< beans::XPropertySet >  xTmp;
        OutHiddenControls( *pxFormComps, xTmp );
    }
}

void SwHTMLWriter::OutHiddenForms()
{
    // Ohne DrawModel kann es auch keine Controls geben. Dann darf man
    // auch nicht per UNO auf das Dok zugreifen, weil sonst ein DrawModel
    // angelegt wird.
    if( !pDoc->GetDrawModel() )
        return;

    SwDocShell *pDocSh = pDoc->GetDocShell();
    if( !pDocSh )
        return;

    uno::Reference< drawing::XDrawPageSupplier > xDPSupp( pDocSh->GetBaseModel(),
                                                     uno::UNO_QUERY );
    ASSERT( xDPSupp.is(), "XTextDocument nicht vom XModel erhalten" );
    uno::Reference< drawing::XDrawPage > xDrawPage = xDPSupp->getDrawPage();

    ASSERT( xDrawPage.is(), "XDrawPage nicht erhalten" );
    if( !xDrawPage.is() )
        return;

    uno::Reference< form::XFormsSupplier > xFormsSupplier( xDrawPage, uno::UNO_QUERY );
    ASSERT( xFormsSupplier.is(),
            "XFormsSupplier nicht vom XDrawPage erhalten" );

    uno::Reference< container::XNameContainer > xTmp = xFormsSupplier->getForms();
    ASSERT( xTmp.is(), "XForms nicht erhalten" );
    uno::Reference< container::XIndexContainer > xForms( xTmp, uno::UNO_QUERY );
    ASSERT( xForms.is(), "XForms ohne container::XIndexContainer?" );

    sal_Int32 nCount = xForms->getCount();
    for( sal_Int32 i=0; i<nCount; i++)
    {
        uno::Any aTmp = xForms->getByIndex( i );
        ASSERT( aTmp.getValueType() ==
                        ::getCppuType((uno::Reference< form::XForm >*)0),
                "OutHiddenForms: falsche Reflection" );
        if( aTmp.getValueType() ==
                    ::getCppuType((uno::Reference< form::XForm >*)0) )
            OutHiddenForm( *(uno::Reference< form::XForm > *)aTmp.getValue() );
    }
}

void SwHTMLWriter::OutHiddenForm( const uno::Reference< form::XForm > & rForm )
{
    uno::Reference< container::XIndexContainer > xFormComps( rForm, uno::UNO_QUERY );
    if( !xFormComps.is() )
        return;

    sal_Int32 nCount = xFormComps->getCount();
    sal_Bool bHiddenOnly = nCount > 0, bHidden = sal_False;
    for( sal_Int32 i=0; i<nCount; i++ )
    {
        uno::Any aTmp = xFormComps->getByIndex( i );
        ASSERT( aTmp.getValueType() ==
                        ::getCppuType((uno::Reference<form::XFormComponent>*)0),
                "OutHiddenForm: falsche Reflection" );
        if( aTmp.getValueType() !=
                    ::getCppuType((uno::Reference<form::XFormComponent>*)0) )
            continue;

        uno::Reference< form::XFormComponent > xFormComp =
            *(uno::Reference< form::XFormComponent > *)aTmp.getValue();
        uno::Reference< form::XForm > xForm( xFormComp, uno::UNO_QUERY );
        if( xForm.is() )
            OutHiddenForm( xForm );

        if( bHiddenOnly )
        {
            uno::Reference< beans::XPropertySet >  xPropSet( xFormComp, uno::UNO_QUERY );
            OUString sPropName = OUString::createFromAscii( "ClassId" );
            if( xPropSet->getPropertySetInfo()->hasPropertyByName( sPropName ) )
            {
                uno::Any aAny2 = xPropSet->getPropertyValue( sPropName );
                if( aAny2.getValueType() == ::getCppuType((sal_Int16*)0) )
                {
                    if( form::FormComponentType::HIDDENCONTROL ==
                                                *(sal_Int16*)aAny2.getValue() )
                        bHidden = sal_True;
                    else if( lcl_html_isHTMLControl(
                                            *(sal_Int16*)aAny2.getValue() ) )
                        bHiddenOnly = sal_False;
                }
            }
        }
    }

    if( bHidden && bHiddenOnly )
    {
        OutForm( sal_True, xFormComps );
        uno::Reference< beans::XPropertySet > xTmp;
        OutHiddenControls( xFormComps, xTmp );
        OutForm( sal_False, xFormComps );
    }
}

void SwHTMLWriter::OutForm( sal_Bool bOn,
                const uno::Reference< container::XIndexContainer > & rFormComps )
{
    nFormCntrlCnt = 0;

    if( !bOn )
    {
        DecIndentLevel(); // Inhalt der Form einruecken
        if( bLFPossible )
            OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_form, sal_False );
        bLFPossible = sal_True;

        return;
    }

    // die neue Form wird geoeffnet
    if( bLFPossible )
        OutNewLine();
    ByteString sOut( '<' );
    sOut += OOO_STRING_SVTOOLS_HTML_form;

    uno::Reference< beans::XPropertySet > xFormPropSet( rFormComps, uno::UNO_QUERY );

    uno::Any aTmp = xFormPropSet->getPropertyValue(
                                    OUString::createFromAscii( "Name" ) );
    if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
        ((OUString*)aTmp.getValue())->getLength() )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"";
        Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( Strm(), *(OUString*)aTmp.getValue(),
                                  eDestEnc, &aNonConvertableCharacters );
        sOut = '\"';
    }

    aTmp = xFormPropSet->getPropertyValue(
                    OUString::createFromAscii( "TargetURL" ) );
    if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
        ((OUString*)aTmp.getValue())->getLength() )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_action) += "=\"";
        Strm() << sOut.GetBuffer();
        String aURL( *(OUString*)aTmp.getValue() );
        aURL = URIHelper::simpleNormalizedMakeRelative( GetBaseURL(), aURL);
        HTMLOutFuncs::Out_String( Strm(), aURL, eDestEnc, &aNonConvertableCharacters );
        sOut = '\"';
    }

    aTmp = xFormPropSet->getPropertyValue(
                    OUString::createFromAscii( "SubmitMethod" ) );
    if( aTmp.getValueType() == ::getCppuType((const form::FormSubmitMethod*)0) )
    {
        form::FormSubmitMethod eMethod =
                *( form::FormSubmitMethod*)aTmp.getValue();
        if( form::FormSubmitMethod_POST==eMethod )
        {
            ((((sOut += ' ')
                += OOO_STRING_SVTOOLS_HTML_O_method) += "=\"")
                += OOO_STRING_SVTOOLS_HTML_METHOD_post) += '\"';
        }
    }
    aTmp = xFormPropSet->getPropertyValue(
                    OUString::createFromAscii( "SubmitEncoding" ) );
    if( aTmp.getValueType()==::getCppuType((const form::FormSubmitEncoding*)0) )
    {
        form::FormSubmitEncoding eEncType =
                    *( form::FormSubmitEncoding*)aTmp.getValue();
        const sal_Char *pStr = 0;
        switch( eEncType )
        {
        case form::FormSubmitEncoding_MULTIPART:
            pStr = OOO_STRING_SVTOOLS_HTML_ET_multipart;
            break;
        case form::FormSubmitEncoding_TEXT:
            pStr = OOO_STRING_SVTOOLS_HTML_ET_text;
            break;
        default:
            ;
        }

        if( pStr )
        {
            ((((sOut += ' ')
                += OOO_STRING_SVTOOLS_HTML_O_enctype) += "=\"")
                += pStr) += '\"';
        }
    }

    aTmp = xFormPropSet->getPropertyValue(
                        OUString::createFromAscii( "TargetFrame" ) );
    if( aTmp.getValueType() == ::getCppuType((const OUString*)0)&&
        ((OUString*)aTmp.getValue())->getLength() )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_target) += "=\"";
        Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( Strm(), *(OUString*)aTmp.getValue(),
                                  eDestEnc, &aNonConvertableCharacters );
        sOut = '\"';
    }

    Strm() << sOut.GetBuffer();
    uno::Reference< form::XFormComponent > xFormComp( rFormComps, uno::UNO_QUERY );
    lcl_html_outEvents( Strm(), xFormComp, bCfgStarBasic, eDestEnc, &aNonConvertableCharacters );
    Strm() << '>';

    IncIndentLevel(); // Inhalt der Form einruecken
    bLFPossible = sal_True;
}

void SwHTMLWriter::OutHiddenControls(
        const uno::Reference< container::XIndexContainer > & rFormComps,
        const uno::Reference< beans::XPropertySet > & rPropSet )
{
    sal_Int32 nCount = rFormComps->getCount();
    sal_Int32 nPos = 0;
    sal_Bool bDone = sal_False;
    if( rPropSet.is() )
    {
        uno::Reference< form::XFormComponent > xFC( rPropSet, uno::UNO_QUERY );
        for( nPos=0; !bDone && nPos < nCount; nPos++ )
        {
            uno::Any aTmp = rFormComps->getByIndex( nPos );
            ASSERT( aTmp.getValueType() ==
                        ::getCppuType((uno::Reference< form::XFormComponent>*)0),
                    "OutHiddenControls: falsche Reflection" );
            bDone = aTmp.getValueType() ==
                        ::getCppuType((uno::Reference< form::XFormComponent>*)0) &&
                    *(uno::Reference< form::XFormComponent > *)aTmp.getValue() ==
                        xFC;
        }
    }

    for( ; nPos < nCount; nPos++ )
    {
        uno::Any aTmp = rFormComps->getByIndex( nPos );
        ASSERT( aTmp.getValueType() ==
                        ::getCppuType((uno::Reference< form::XFormComponent>*)0),
                "OutHiddenControls: falsche Reflection" );
        if( aTmp.getValueType() !=
                    ::getCppuType((uno::Reference< form::XFormComponent>*)0) )
            continue;
        uno::Reference< form::XFormComponent > xFC =
                *(uno::Reference< form::XFormComponent > *)aTmp.getValue();
        uno::Reference< beans::XPropertySet > xPropSet( xFC, uno::UNO_QUERY );

        OUString sPropName = OUString::createFromAscii( "ClassId" );
        if( !xPropSet->getPropertySetInfo()->hasPropertyByName( sPropName ) )
            continue;

        aTmp = xPropSet->getPropertyValue( sPropName );
        if( aTmp.getValueType() != ::getCppuType((const sal_Int16*)0) )
            continue;

        if( form::FormComponentType::HIDDENCONTROL ==
                                            *(sal_Int16*) aTmp.getValue() )
        {
            if( bLFPossible )
                OutNewLine( sal_True );
            ByteString sOut( '<' );
            ((((sOut += OOO_STRING_SVTOOLS_HTML_input) += ' ') +=
                OOO_STRING_SVTOOLS_HTML_O_type) += '=') += OOO_STRING_SVTOOLS_HTML_IT_hidden;

            aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "Name" ) );
            if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
                ((OUString*)aTmp.getValue())->getLength() )
            {
                (( sOut += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_name ) += "=\"";
                Strm() << sOut.GetBuffer();
                HTMLOutFuncs::Out_String( Strm(), *(OUString*)aTmp.getValue(),
                                          eDestEnc, &aNonConvertableCharacters );
                sOut = '\"';
            }
            aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "HiddenValue" ) );
            if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
                ((OUString*)aTmp.getValue())->getLength() )
            {
                ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_value) += "=\"";
                Strm() << sOut.GetBuffer();
                HTMLOutFuncs::Out_String( Strm(), *(OUString*)aTmp.getValue(),
                                          eDestEnc, &aNonConvertableCharacters );
                sOut = '\"';
            }
            sOut += '>';
            Strm() << sOut.GetBuffer();

            nFormCntrlCnt++;
        }
        else if( lcl_html_isHTMLControl( *(sal_Int16*) aTmp.getValue() ) )
        {
            break;
        }
    }
}

/*  */

// hier folgen die Ausgabe-Routinen, dadurch sind die form::Forms gebuendelt:

const SdrObject *SwHTMLWriter::GetHTMLControl( const SwDrawFrmFmt& rFmt )
{
    // es muss ein Draw-Format sein
    ASSERT( RES_DRAWFRMFMT == rFmt.Which(),
            "GetHTMLControl nuer fuer Draw-Formate erlaubt" );

    // Schauen, ob es ein SdrObject dafuer gibt
    const SdrObject *pObj = rFmt.FindSdrObject();
    if( !pObj || FmFormInventor != pObj->GetObjInventor() )
        return 0;

    SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, pObj );
    uno::Reference< awt::XControlModel >  xControlModel =
            pFormObj->GetUnoControlModel();

    ASSERT( xControlModel.is(), "UNO-Control ohne Model" );
    if( !xControlModel.is() )
        return 0;

    uno::Reference< beans::XPropertySet >  xPropSet( xControlModel, uno::UNO_QUERY );

    OUString sPropName = OUString::createFromAscii( "ClassId" );
    if( !xPropSet->getPropertySetInfo()->hasPropertyByName( sPropName ) )
        return 0;

    uno::Any aTmp = xPropSet->getPropertyValue( sPropName );
    if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0)&&
        lcl_html_isHTMLControl( *(sal_Int16*) aTmp.getValue() ) )
    {
        return pObj;
    }

    return 0;
}

static void GetControlSize( const SdrObject& rSdrObj, Size& rSz,
                            SwDoc *pDoc )
{
    ViewShell *pVSh = 0;
    pDoc->GetEditShell( &pVSh );
    if( !pVSh )
        return;

    SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, &rSdrObj );
    uno::Reference< awt::XControl >  xControl;
    SdrView* pDrawView = pVSh->GetDrawView();
    ASSERT( pDrawView && pVSh->GetWin(), "no DrawView or window!" );
    if ( pDrawView && pVSh->GetWin() )
        xControl = pFormObj->GetUnoControl( *pDrawView, *pVSh->GetWin() );
    uno::Reference< awt::XTextLayoutConstrains > xLC( xControl, uno::UNO_QUERY );
    ASSERT( xLC.is(), "kein XTextLayoutConstrains" );
    if( !xLC.is() )
        return;

    sal_Int16 nCols=0, nLines=0;
    xLC->getColumnsAndLines( nCols, nLines );
    rSz.Width() = nCols;
    rSz.Height() = nLines;
}

Writer& OutHTML_DrawFrmFmtAsControl( Writer& rWrt,
                                     const SwDrawFrmFmt& rFmt,
                                     const SdrObject& rSdrObject,
                                     sal_Bool bInCntnr )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, &rSdrObject );
    uno::Reference< awt::XControlModel > xControlModel =
        pFormObj->GetUnoControlModel();

    ASSERT( xControlModel.is(), "UNO-Control ohne Model" );
    if( !xControlModel.is() )
        return rWrt;

    uno::Reference< beans::XPropertySet > xPropSet( xControlModel, uno::UNO_QUERY );
    uno::Reference< beans::XPropertySetInfo > xPropSetInfo =
            xPropSet->getPropertySetInfo();

//!!!   if( rHTMLWrt.pForm != pVCSbxCtrl->GetVCForm() )
//!!!       rHTMLWrt.nWarn = 1; // Control wird falscher Form zugeordnet
    rHTMLWrt.nFormCntrlCnt++;

    enum Tag { TAG_INPUT, TAG_SELECT, TAG_TEXTAREA, TAG_NONE };
    static char const * const TagNames[] = {
        OOO_STRING_SVTOOLS_HTML_input, OOO_STRING_SVTOOLS_HTML_select,
        OOO_STRING_SVTOOLS_HTML_textarea };
    Tag eTag = TAG_INPUT;
    enum Type {
        TYPE_TEXT, TYPE_PASSWORD, TYPE_CHECKBOX, TYPE_RADIO, TYPE_FILE,
        TYPE_SUBMIT, TYPE_IMAGE, TYPE_RESET, TYPE_BUTTON, TYPE_NONE };
    static char const * const TypeNames[] = {
        OOO_STRING_SVTOOLS_HTML_IT_text, OOO_STRING_SVTOOLS_HTML_IT_password,
        OOO_STRING_SVTOOLS_HTML_IT_checkbox, OOO_STRING_SVTOOLS_HTML_IT_radio,
        OOO_STRING_SVTOOLS_HTML_IT_file, OOO_STRING_SVTOOLS_HTML_IT_submit,
        OOO_STRING_SVTOOLS_HTML_IT_image, OOO_STRING_SVTOOLS_HTML_IT_reset,
        OOO_STRING_SVTOOLS_HTML_IT_button };
    Type eType = TYPE_NONE;
    OUString sValue;
    ByteString sOptions;
    sal_Bool bEmptyValue = sal_False;
    uno::Any aTmp = xPropSet->getPropertyValue(
                    OUString::createFromAscii( "ClassId" ) );
    sal_Int16 nClassId = *(sal_Int16*) aTmp.getValue();
    sal_uInt32 nFrmOpts = HTML_FRMOPTS_CONTROL;
    switch( nClassId )
    {
    case form::FormComponentType::CHECKBOX:
    case form::FormComponentType::RADIOBUTTON:
        eType = (form::FormComponentType::CHECKBOX == nClassId
                    ? TYPE_CHECKBOX : TYPE_RADIO);
        aTmp = xPropSet->getPropertyValue(
                        OUString::createFromAscii( "DefaultState" ) );
        if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0) &&
            STATE_NOCHECK != *(sal_Int16*) aTmp.getValue() )
        {
            (sOptions += ' ') += OOO_STRING_SVTOOLS_HTML_O_checked;
        }

        aTmp = xPropSet->getPropertyValue(
                        OUString::createFromAscii( "RefValue" ) );
        if( aTmp.getValueType() == ::getCppuType((const OUString*)0) )

        {
            const OUString& rVal = *(OUString*)aTmp.getValue();
            if( !rVal.getLength() )
                bEmptyValue = sal_True;
            else if( rVal.compareToAscii( OOO_STRING_SVTOOLS_HTML_on ) != 0 )
                sValue = rVal;
        }
        break;

    case form::FormComponentType::COMMANDBUTTON:
        {
            form::FormButtonType eButtonType = form::FormButtonType_PUSH;
            aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "ButtonType" ) );
            if( aTmp.getValueType() ==
                            ::getCppuType((const form::FormButtonType*)0) )
                eButtonType = *( form::FormButtonType*)aTmp.getValue();

            switch( eButtonType )
            {
            case form::FormButtonType_RESET:
                eType = TYPE_RESET;
                break;
            case form::FormButtonType_SUBMIT:
                eType = TYPE_SUBMIT;
                break;
            case form::FormButtonType_PUSH:
            default:
                eType = TYPE_BUTTON;
            }

            aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "Label" ) );
            if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
                ((OUString*)aTmp.getValue())->getLength() )
            {
                sValue = *(OUString*)aTmp.getValue();
            }
        }
        break;

    case form::FormComponentType::LISTBOX:
        if( rHTMLWrt.bLFPossible )
            rHTMLWrt.OutNewLine( sal_True );
        eTag = TAG_SELECT;
        aTmp = xPropSet->getPropertyValue(
                        OUString::createFromAscii( "Dropdown" ) );
        if( aTmp.getValueType() == ::getBooleanCppuType() &&
            !*(sal_Bool*)aTmp.getValue() )
        {
            Size aSz( 0, 0 );
            GetControlSize( rSdrObject, aSz, rWrt.pDoc );

            // wieviele sind sichtbar ??
            if( aSz.Height() )
                (((sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_size ) += '=' )
                    += ByteString::CreateFromInt32( aSz.Height() );

            aTmp = xPropSet->getPropertyValue(
                        OUString::createFromAscii( "MultiSelection" ) );
            if( aTmp.getValueType() == ::getBooleanCppuType() &&
                *(sal_Bool*)aTmp.getValue() )
            {
                (sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_multiple;
            }
        }
        break;

    case form::FormComponentType::TEXTFIELD:
        {
            Size aSz( 0, 0 );
            GetControlSize( rSdrObject, aSz, rWrt.pDoc );

            sal_Bool bMultiLine = sal_False;
            OUString sMultiLine( OUString::createFromAscii( "MultiLine" ) );
            if( xPropSetInfo->hasPropertyByName( sMultiLine ) )
            {
                aTmp = xPropSet->getPropertyValue( sMultiLine );
                bMultiLine = aTmp.getValueType() == ::getBooleanCppuType() &&
                             *(sal_Bool*)aTmp.getValue();
            }

            if( bMultiLine )
            {
                if( rHTMLWrt.bLFPossible )
                    rHTMLWrt.OutNewLine( sal_True );
                eTag = TAG_TEXTAREA;

                if( aSz.Height() )
                    (((sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_rows ) += '=' )
                        += ByteString::CreateFromInt32( aSz.Height() );
                if( aSz.Width() )
                    (((sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_cols ) += '=' )
                        += ByteString::CreateFromInt32( aSz.Width() );

                aTmp = xPropSet->getPropertyValue(
                                OUString::createFromAscii( "HScroll" ) );
                if( aTmp.getValueType() == ::getVoidCppuType() ||
                    (aTmp.getValueType() == ::getBooleanCppuType() &&
                    !*(sal_Bool*)aTmp.getValue()) )
                {
                    const sal_Char *pWrapStr = 0;
                    aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "HardLineBreaks" ) );
                    pWrapStr =
                        (aTmp.getValueType() == ::getBooleanCppuType() &&
                        *(sal_Bool*)aTmp.getValue()) ? OOO_STRING_SVTOOLS_HTML_WW_hard
                                                     : OOO_STRING_SVTOOLS_HTML_WW_soft;
                    (((sOptions += ' ') += OOO_STRING_SVTOOLS_HTML_O_wrap) += '=') += pWrapStr;
                }
            }
            else
            {
                eType = TYPE_TEXT;
                OUString sEchoChar( OUString::createFromAscii( "EchoChar" ) );
                if( xPropSetInfo->hasPropertyByName( sEchoChar ) )
                {
                    aTmp = xPropSet->getPropertyValue( sEchoChar );
                    if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0) &&
                        *(sal_Int16*)aTmp.getValue() != 0 )
                        eType = TYPE_PASSWORD;
                }

                if( aSz.Width() )
                    (((sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_size ) += '=' )
                        += ByteString::CreateFromInt32( aSz.Width() );

                aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "MaxTextLen" ) );
                if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0) &&
                    *(sal_Int16*) aTmp.getValue() != 0 )
                {
                    (((sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_maxlength ) += '=' )
                        += ByteString::CreateFromInt32(
                                *(sal_Int16*) aTmp.getValue() );
                }

                OUString sDefaultText( OUString::createFromAscii( "DefaultText" ) );
                if( xPropSetInfo->hasPropertyByName( sDefaultText ) )
                {
                    aTmp = xPropSet->getPropertyValue( sDefaultText );
                    if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
                        ((OUString*)aTmp.getValue())->getLength() )
                    {
                        sValue = *(OUString*)aTmp.getValue();
                    }
                }
            }
        }
        break;

    case form::FormComponentType::FILECONTROL:
        {
            Size aSz( 0, 0 );
            GetControlSize( rSdrObject, aSz, rWrt.pDoc );
            eType = TYPE_FILE;

            if( aSz.Width() )
                (((sOptions += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_size ) += '=' )
                    += ByteString::CreateFromInt32( aSz.Width() );

            // VALUE vim form aus Sicherheitsgruenden nicht exportieren
        }
        break;


    case form::FormComponentType::IMAGEBUTTON:
        eType = TYPE_IMAGE;
        nFrmOpts = HTML_FRMOPTS_IMG_CONTROL;
        break;

    default:                // kennt HTML nicht
        eTag = TAG_NONE;        // also ueberspringen
        break;
    }

    if( eTag == TAG_NONE )
        return rWrt;

    ByteString sOut( '<' );
    sOut += TagNames[eTag];
    if( eType != TYPE_NONE )
        (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_type) += '=') +=
            TypeNames[eType];

    aTmp = xPropSet->getPropertyValue( OUString::createFromAscii( "Name" ) );
    if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
        ((OUString*)aTmp.getValue())->getLength() )
    {
        (( sOut += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_name ) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), *(OUString*)aTmp.getValue(),
                                  rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
        sOut = '\"';
    }

    aTmp = xPropSet->getPropertyValue( OUString::createFromAscii( "Enabled" ) );
    if( aTmp.getValueType() == ::getBooleanCppuType() &&
        !*(sal_Bool*)aTmp.getValue() )
    {
        (( sOut += ' ' ) += OOO_STRING_SVTOOLS_HTML_O_disabled );
    }

    if( sValue.getLength() || bEmptyValue )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_value) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), sValue, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
        sOut = '\"';
    }

    sOut += sOptions;

    if( TYPE_IMAGE == eType )
    {
        aTmp = xPropSet->getPropertyValue(
                    OUString::createFromAscii( "ImageURL" ) );
        if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
            ((OUString*)aTmp.getValue())->getLength() )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_src) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();

            HTMLOutFuncs::Out_String( rWrt.Strm(),
                        URIHelper::simpleNormalizedMakeRelative( rWrt.GetBaseURL(), *(OUString*)aTmp.getValue()),
                        rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }

        Size aTwipSz( rSdrObject.GetLogicRect().GetSize() );
        Size aPixelSz( 0, 0 );
        if( (aTwipSz.Width() || aTwipSz.Height()) &&
            Application::GetDefaultDevice() )
        {
            aPixelSz =
                Application::GetDefaultDevice()->LogicToPixel( aTwipSz,
                                                    MapMode(MAP_TWIP) );
            if( !aPixelSz.Width() && aTwipSz.Width() )
                aPixelSz.Width() = 1;
            if( !aPixelSz.Height() && aTwipSz.Height() )
                aPixelSz.Height() = 1;
        }

        if( aPixelSz.Width() )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_width) += '=')
                += ByteString::CreateFromInt32( aPixelSz.Width() );

        if( aPixelSz.Height() )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_height) += '=')
                += ByteString::CreateFromInt32( aPixelSz.Height() );
    }

    aTmp = xPropSet->getPropertyValue(
                    OUString::createFromAscii( "TabIndex" ) );
    if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0) )
    {
        sal_Int16 nTabIndex = *(sal_Int16*) aTmp.getValue();
        if( nTabIndex > 0 )
        {
            if( nTabIndex >= 32767 )
                nTabIndex = 32767;

            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_tabindex) += '=')
                += ByteString::CreateFromInt32( nTabIndex );
        }
    }

    if( sOut.Len() )
    {
        rWrt.Strm() << sOut.GetBuffer();
        sOut.Erase();
    }

    ASSERT( !bInCntnr, "Container wird fuer Controls nicht unterstuertzt" );
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_DRAW ) && !bInCntnr )
    {
        // Wenn Zeichen-Objekte nicht absolut positioniert werden duerfen,
        // das entsprechende Flag loeschen.
        nFrmOpts |= (TYPE_IMAGE == eType
                            ? HTML_FRMOPTS_IMG_CONTROL_CSS1
                            : HTML_FRMOPTS_CONTROL_CSS1);
    }
    ByteString aEndTags;
    if( nFrmOpts != 0 )
        rHTMLWrt.OutFrmFmtOptions( rFmt, aEmptyStr, aEndTags, nFrmOpts );

    if( rHTMLWrt.bCfgOutStyles )
    {
        sal_Bool bEdit = TAG_TEXTAREA == eTag || TYPE_FILE == eType ||
                     TYPE_TEXT == eType;

        SfxItemSet aItemSet( rHTMLWrt.pDoc->GetAttrPool(), RES_CHRATR_BEGIN,
                             RES_CHRATR_END );
        OUString sPropName = OUString::createFromAscii( "BackgroundColor" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const sal_Int32*)0) )
            {
                Color aCol(*(sal_Int32*)aTmp .getValue());
                aItemSet.Put( SvxBrushItem( aCol, RES_CHRATR_BACKGROUND ) );
            }
        }
        sPropName = OUString::createFromAscii( "TextColor" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const sal_Int32*)0) )
            {
                Color aColor( *(sal_Int32*)aTmp .getValue() );
                aItemSet.Put( SvxColorItem( aColor, RES_CHRATR_COLOR ) );
            }
        }
        sPropName = OUString::createFromAscii( "FontHeight" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const float*)0) )

            {
                float nHeight = *(float*)aTmp.getValue();
                if( nHeight > 0  && (!bEdit || nHeight != 10.) )
                    aItemSet.Put( SvxFontHeightItem( sal_Int16(nHeight * 20.), 100, RES_CHRATR_FONTSIZE ) );
            }
        }
        sPropName = OUString::createFromAscii( "FontName" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
                ((OUString*)aTmp.getValue())->getLength() )
            {
                Font aFixedFont( OutputDevice::GetDefaultFont(
                                    DEFAULTFONT_FIXED, LANGUAGE_ENGLISH_US,
                                    DEFAULTFONT_FLAGS_ONLYONE ) );
                String aFName( *(OUString*)aTmp.getValue() );
                if( !bEdit || aFName != aFixedFont.GetName() )
                {
                    FontFamily eFamily = FAMILY_DONTKNOW;
                    sPropName = OUString::createFromAscii( "FontFamily" );
                    if( xPropSetInfo->hasPropertyByName( sPropName ) )
                    {
                        aTmp = xPropSet->getPropertyValue( sPropName );
                        if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0))
                            eFamily = (FontFamily)*(sal_Int16*) aTmp.getValue();
                    }
                    SvxFontItem aItem( eFamily, aFName, aEmptyStr, PITCH_DONTKNOW, RTL_TEXTENCODING_DONTKNOW, RES_CHRATR_FONT );
                    aItemSet.Put( aItem );
                }
            }
        }
        sPropName = OUString::createFromAscii( "FontWeight" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const float*)0) )
            {
                FontWeight eWeight =
                    VCLUnoHelper::ConvertFontWeight( *(float*)aTmp.getValue() );
                if( eWeight != WEIGHT_DONTKNOW && eWeight != WEIGHT_NORMAL )
                    aItemSet.Put( SvxWeightItem( eWeight, RES_CHRATR_WEIGHT ) );
            }
        }
        sPropName = OUString::createFromAscii( "FontSlant" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0))
            {
                FontItalic eItalic = (FontItalic)*(sal_Int16*)aTmp.getValue();
                if( eItalic != ITALIC_DONTKNOW && eItalic != ITALIC_NONE )
                    aItemSet.Put( SvxPostureItem( eItalic, RES_CHRATR_POSTURE ) );
            }
        }
        sPropName = OUString::createFromAscii( "FontUnderline" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0) )
            {
                FontUnderline eUnderline =
                    (FontUnderline)*(sal_Int16*)aTmp.getValue();
                if( eUnderline != UNDERLINE_DONTKNOW  &&
                    eUnderline != UNDERLINE_NONE )
                    aItemSet.Put( SvxUnderlineItem( eUnderline, RES_CHRATR_UNDERLINE ) );
            }
        }
        sPropName = OUString::createFromAscii( "FontStrikeout" );
        if( xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp = xPropSet->getPropertyValue( sPropName );
            if( aTmp.getValueType() == ::getCppuType((const sal_Int16*)0))
            {
                FontStrikeout eStrikeout =
                    (FontStrikeout)*(sal_Int16*)aTmp.getValue();
                if( eStrikeout != STRIKEOUT_DONTKNOW &&
                    eStrikeout != STRIKEOUT_NONE )
                    aItemSet.Put( SvxCrossedOutItem( eStrikeout, RES_CHRATR_CROSSEDOUT ) );
            }
        }

        rHTMLWrt.OutCSS1_FrmFmtOptions( rFmt, nFrmOpts, &rSdrObject,
                                        &aItemSet );
    }

    uno::Reference< form::XFormComponent >  xFormComp( xControlModel, uno::UNO_QUERY );
    lcl_html_outEvents( rWrt.Strm(), xFormComp, rHTMLWrt.bCfgStarBasic,
                        rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );

    rWrt.Strm() << '>';

    if( TAG_SELECT == eTag )
    {
        aTmp = xPropSet->getPropertyValue(
                    OUString::createFromAscii( "StringItemList" ) );
        if( aTmp.getValueType() == ::getCppuType((uno::Sequence<OUString>*)0) )
        {
            rHTMLWrt.IncIndentLevel(); // der Inhalt von Select darf
                                       // eingerueckt werden
            uno::Sequence<OUString> aList( *(uno::Sequence<OUString>*)aTmp.getValue() );
            sal_Int32 nCnt = aList.getLength();
            const OUString *pStrings = aList.getConstArray();

            const OUString *pValues = 0;
            sal_Int32 nValCnt = 0;
            aTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "ListSource" ) );
            uno::Sequence<OUString> aValList;
            if( aTmp.getValueType() == ::getCppuType((uno::Sequence<OUString>*)0) )
            {
                aValList = *(uno::Sequence<OUString>*)aTmp.getValue();
                nValCnt = aValList.getLength();
                pValues = aValList.getConstArray();
            }

            uno::Any aSelTmp = xPropSet->getPropertyValue(
                            OUString::createFromAscii( "DefaultSelection" ) );
            const sal_Int16 *pSels = 0;
            sal_Int32 nSel = 0;
            sal_Int32 nSelCnt = 0;
            uno::Sequence<sal_Int16> aSelList;
            if( aSelTmp.getValueType() ==::getCppuType((uno::Sequence<sal_Int16>*)0))
            {
                aSelList = *(uno::Sequence<sal_Int16>*)aSelTmp.getValue();
                nSelCnt = aSelList.getLength();
                pSels = aSelList.getConstArray();
            }

            for( sal_Int32 i = 0; i < nCnt; i++ )
            {
                OUString sVal;
                sal_Bool bSelected = sal_False, bEmptyVal = sal_False;
                if( i < nValCnt )
                {
                    const OUString& rVal = pValues[i];
                    if( rVal.compareToAscii( "$$$empty$$$" ) == 0 )
                        bEmptyVal = sal_True;
                    else
                        sVal = rVal;
                }

                bSelected = (nSel < nSelCnt) && pSels[nSel] == i;
                if( bSelected )
                    nSel++;

                rHTMLWrt.OutNewLine(); // jede Option bekommt eine eigene Zeile
                (sOut = '<') += OOO_STRING_SVTOOLS_HTML_option;
                if( sVal.getLength() || bEmptyVal )
                {
                    ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_value) += "=\"";
                    rWrt.Strm() << sOut.GetBuffer();
                    HTMLOutFuncs::Out_String( rWrt.Strm(), sVal,
                        rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
                    sOut = '\"';
                }
                if( bSelected )
                    (sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_selected;

                sOut += '>';
                rWrt.Strm() << sOut.GetBuffer();

                HTMLOutFuncs::Out_String( rWrt.Strm(), pStrings[i],
                                          rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            }
            HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_option, sal_False );

            rHTMLWrt.DecIndentLevel();
            rHTMLWrt.OutNewLine();// das </SELECT> bekommt eine eigene Zeile
        }
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_select, sal_False );
    }
    else if( TAG_TEXTAREA == eTag )
    {
        // In TextAreas duerfen keine zusaetzlichen Spaces oder LF exportiert
        // werden!
        String sVal;
        aTmp = xPropSet->getPropertyValue(
                        OUString::createFromAscii( "DefaultText" ) );
        if( aTmp.getValueType() == ::getCppuType((const OUString*)0)&&
            ((OUString*)aTmp.getValue())->getLength() )
        {
            sVal = String( *(OUString*)aTmp.getValue() );
        }
        if( sVal.Len() )
        {
            sVal.ConvertLineEnd( LINEEND_LF );
            xub_StrLen nPos = 0;
            while ( nPos != STRING_NOTFOUND )
            {
                if( nPos )
                    rWrt.Strm() << SwHTMLWriter::sNewLine;
                String aLine = sVal.GetToken( 0, 0x0A, nPos );
                HTMLOutFuncs::Out_String( rWrt.Strm(), aLine,
                                        rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            }
        }
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_textarea, sal_False );
    }
    else if( TYPE_CHECKBOX == eType || TYPE_RADIO == eType )
    {
        aTmp = xPropSet->getPropertyValue( OUString::createFromAscii("Label") );
        if( aTmp.getValueType() == ::getCppuType((const OUString*)0) &&
            ((OUString*)aTmp.getValue())->getLength() )
        {
            sValue = *(OUString*)aTmp.getValue();
            HTMLOutFuncs::Out_String( rWrt.Strm(), sValue,
                rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters ) << ' ';
        }
    }

    if( aEndTags.Len() )
        rWrt.Strm() << aEndTags.GetBuffer();

    // Controls sind nicht absatz-gebunden, deshalb kein LF mehr ausgeben!
    rHTMLWrt.bLFPossible = sal_False;

    if( rHTMLWrt.pxFormComps && rHTMLWrt.pxFormComps->is() )
        rHTMLWrt.OutHiddenControls( *rHTMLWrt.pxFormComps, xPropSet );
    return rWrt;
}

/*  */

// Ermitteln, ob eine Format zu einem Control gehoert und wenn ja
// dessen Form zurueckgeben
static void AddControl( HTMLControls& rControls,
                        const SdrObject *pSdrObj,
                        sal_uInt32 nNodeIdx )
{
    SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, pSdrObj );
    ASSERT( pFormObj, "Doch kein FormObj" );
    uno::Reference< awt::XControlModel > xControlModel =
            pFormObj->GetUnoControlModel();
    if( !xControlModel.is() )
        return;

    uno::Reference< form::XFormComponent >  xFormComp( xControlModel, uno::UNO_QUERY );
    uno::Reference< uno::XInterface >  xIfc = xFormComp->getParent();
    uno::Reference< form::XForm >  xForm(xIfc, uno::UNO_QUERY);

    ASSERT( xForm.is(), "Wo ist die Form?" );
    if( xForm.is() )
    {
        uno::Reference< container::XIndexContainer >  xFormComps( xForm, uno::UNO_QUERY );
        HTMLControl *pHCntrl = new HTMLControl( xFormComps, nNodeIdx );
        if( !rControls.C40_PTR_INSERT( HTMLControl, pHCntrl ) )
        {
            sal_uInt16 nPos = 0;
            if( rControls.Seek_Entry(pHCntrl,&nPos) &&
                rControls[nPos]->xFormComps==xFormComps )
                rControls[nPos]->nCount++;
            delete pHCntrl;
        }
    }
}


void SwHTMLWriter::GetControls()
{
    // Idee: die absatz- und zeichengebundenen Controls werden erst einmal
    // eingesammelt. Dabei wird fuer jedes Control des Absatz-Position
    // und VCForm in einem Array gemerkt.
    // Ueber dieses Array laesst sich dann feststellen, wo form::Forms geoeffnet
    // und geschlossen werden muessen.
    sal_uInt16 i;
    if( pHTMLPosFlyFrms )
    {
        // die absatz-gebundenen Controls einsammeln
        for( i=0; i<pHTMLPosFlyFrms->Count(); i++ )
        {
            const SwHTMLPosFlyFrm* pPosFlyFrm = pHTMLPosFlyFrms->GetObject( i );
            if( HTML_OUT_CONTROL != pPosFlyFrm->GetOutFn() )
                continue;

            const SdrObject *pSdrObj = pPosFlyFrm->GetSdrObject();
            ASSERT( pSdrObj, "Wo ist das SdrObject?" );
            if( !pSdrObj )
                continue;

            AddControl( aHTMLControls, pSdrObj,
                        pPosFlyFrm->GetNdIndex().GetIndex() );
        }
    }

    // und jetzt die in einem zeichengebundenen Rahmen
    const SwSpzFrmFmts* pSpzFrmFmts = pDoc->GetSpzFrmFmts();
    for( i=0; i<pSpzFrmFmts->Count(); i++ )
    {
        const SwFrmFmt *pFrmFmt = (*pSpzFrmFmts)[i];
        if( RES_DRAWFRMFMT != pFrmFmt->Which() )
            continue;

        const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
        const SwPosition *pPos = rAnchor.GetCntntAnchor();
        if ((FLY_AS_CHAR != rAnchor.GetAnchorId()) || !pPos)
            continue;

        const SdrObject *pSdrObj =
            SwHTMLWriter::GetHTMLControl( *(const SwDrawFrmFmt*)pFrmFmt );
        if( !pSdrObj )
            continue;

        AddControl( aHTMLControls, pSdrObj, pPos->nNode.GetIndex() );
    }
}

/*  */

HTMLControl::HTMLControl(
        const uno::Reference< container::XIndexContainer > & rFormComps,
        sal_uInt32 nIdx ) :
    xFormComps( rFormComps ), nNdIdx( nIdx ), nCount( 1 )
{}


HTMLControl::~HTMLControl()
{}


