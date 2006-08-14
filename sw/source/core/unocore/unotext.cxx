/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unotext.cxx,v $
 *
 *  $Revision: 1.29 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 16:57:52 $
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

#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_CONTROLCHARACTER_HPP_
#include <com/sun/star/text/ControlCharacter.hpp>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _UNOPORT_HXX
#include <unoport.hxx>
#endif
#ifndef _UNOTBL_HXX
#include <unotbl.hxx>
#endif
#ifndef _UNOIDX_HXX
#include <unoidx.hxx>
#endif
#ifndef _UNOFRAME_HXX
#include <unoframe.hxx>
#endif
#ifndef _UNOFIELD_HXX
#include <unofield.hxx>
#endif
#ifndef _UNODRAW_HXX
#include <unodraw.hxx>
#endif
#ifndef _UNOREDLINE_HXX
#include <unoredline.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef _UNOCRSRHELPER_HXX
#include <unocrsrhelper.hxx>
#endif
#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _SWUNDO_HXX //autogen
#include <swundo.hxx>
#endif
#ifndef _SECTION_HXX //autogen
#include <section.hxx>
#endif
#ifndef _BOOKMRK_HXX //autogen
#include <bookmrk.hxx>
#endif
#ifndef _FMTHBSH_HXX //autogen
#include <fmthbsh.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::rtl;

const sal_Char cInvalidObject[] = "this object is invalid";
/******************************************************************
 * SwXText
 ******************************************************************/
/*-- 09.12.98 12:44:07---------------------------------------------------

  -----------------------------------------------------------------------*/

const SwStartNode *SwXText::GetStartNode() const
{
    return GetDoc()->GetNodes().GetEndOfContent().StartOfSectionNode();
}

uno::Reference< XTextCursor >   SwXText::createCursor() throw ( ::com::sun::star::uno::RuntimeException)
{
    uno::Reference< XTextCursor >  xRet;
    OUString sRet;
    if(IsValid())
    {
        SwNode& rNode = GetDoc()->GetNodes().GetEndOfContent();
        SwPosition aPos(rNode);
        xRet =  (XWordCursor*)new SwXTextCursor(this, aPos, GetTextType(), GetDoc());
        xRet->gotoStart(sal_False);
    }
    return xRet;
}
/* -----------------------------15.03.2002 12:39------------------------------

 ---------------------------------------------------------------------------*/
SwXText::SwXText(SwDoc* pDc, CursorType eType) :
    pDoc(pDc),
    bObjectValid(0 != pDc),
    eCrsrType(eType),
    _pMap(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT))
{
}
/*-- 09.12.98 12:43:55---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXText::~SwXText()
{
}
/*-- 09.12.98 12:43:02---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL SwXText::queryInterface( const uno::Type& rType ) throw(uno::RuntimeException)
{
    const uno::Type& rXTextType = ::getCppuType((uno::Reference< XText >*)0);
    const uno::Type& rXTextRangeCompareType = ::getCppuType((uno::Reference< XTextRangeCompare >*)0);
    const uno::Type& rXSimpleTextType = ::getCppuType((const uno::Reference< XSimpleText >*)0);
    const uno::Type& rXTextRangeType = ::getCppuType((uno::Reference< XTextRange >*)0);
    const uno::Type& rXTypeProviderType = ::getCppuType((uno::Reference< lang::XTypeProvider >*)0);
    const uno::Type& rXTextContentInsert = ::getCppuType((uno::Reference< XRelativeTextContentInsert >*)0);
    const uno::Type& rXTextContentRemove = ::getCppuType((uno::Reference< XRelativeTextContentRemove >*)0);
    const uno::Type& rXPropertySet = ::getCppuType((uno::Reference<XPropertySet>*)0);
    const uno::Type& rXUnoTunnel = ::getCppuType((uno::Reference< XUnoTunnel >*)0);

    uno::Any aRet;
    if(rType == rXTextType)
    {
        uno::Reference< XText > xRet = this;
        aRet.setValue(&xRet, rXTextType);
    }
    else if(rType == rXSimpleTextType)
    {
        uno::Reference< XSimpleText > xRet = this;
        aRet.setValue(&xRet, rXSimpleTextType);
    }
    else if(rType == rXTextRangeType)
    {
        uno::Reference< XTextRange > xRet = this;
        aRet.setValue(&xRet, rXTextRangeType);
    }
    else if(rType == rXTextRangeCompareType)
    {
        uno::Reference< XTextRangeCompare > xRet = this;
        aRet.setValue(&xRet, rXTextRangeCompareType);
    }
    else if(rType == rXTypeProviderType)
    {
        uno::Reference< lang::XTypeProvider > xRet = this;
        aRet.setValue(&xRet, rXTypeProviderType);
    }
    else if(rType == rXTextContentInsert)
    {
        uno::Reference< XRelativeTextContentInsert > xRet = this;
        aRet.setValue(&xRet, rXTextContentInsert);
    }
    else if(rType == rXTextContentRemove)
    {
        uno::Reference< XRelativeTextContentRemove > xRet = this;
        aRet.setValue(&xRet, rXTextContentRemove);
    }
    else if(rType == rXPropertySet)
    {
        uno::Reference< XPropertySet > xRet = this;
        aRet.setValue(&xRet, rXPropertySet);
    }
    else if(rType == rXUnoTunnel )
    {
        uno::Reference< XUnoTunnel > xRet = this;
        aRet.setValue(&xRet, rXUnoTunnel);
    }
    return aRet;
}
/* -----------------------------15.03.00 17:42--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< uno::Type > SAL_CALL SwXText::getTypes() throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aRet(6);
    uno::Type* pTypes = aRet.getArray();
    pTypes[0] = ::getCppuType((uno::Reference< XText >*)0);
    pTypes[1] = ::getCppuType((uno::Reference< XTextRangeCompare >*)0);
    pTypes[2] = ::getCppuType((uno::Reference< XRelativeTextContentInsert >*)0);
    pTypes[3] = ::getCppuType((uno::Reference< XRelativeTextContentRemove >*)0);
    pTypes[4] = ::getCppuType((uno::Reference< XUnoTunnel >*)0);
    pTypes[5] = ::getCppuType((uno::Reference< XPropertySet >*)0);

    return aRet;
}

/*-- 09.12.98 12:43:14---------------------------------------------------
    Gehoert der Range in den Text ? - dann einfuegen
  -----------------------------------------------------------------------*/
void SwXText::insertString(const uno::Reference< XTextRange > & xTextRange,
                                const OUString& aString, sal_Bool bAbsorb)
                                throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(GetDoc() && xTextRange.is())
    {
        uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
        SwXTextRange* pRange = 0;
        OTextCursorHelper* pCursor = 0;
        if(xRangeTunnel.is())
        {
            pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                    SwXTextRange::getUnoTunnelId());
            pCursor = (OTextCursorHelper*)xRangeTunnel->getSomething(
                                    OTextCursorHelper::getUnoTunnelId());
        }

        if(pRange && pRange->GetDoc()  == GetDoc() ||
            pCursor && pCursor->GetDoc()  == GetDoc())
        {
            const SwStartNode* pOwnStartNode = GetStartNode();
            if(pCursor)
            {
                const SwStartNode* pTmp = pCursor->GetPaM()->GetNode()->StartOfSectionNode();
                while(pTmp && pTmp->IsSectionNode())
                {
                    pTmp = pTmp->StartOfSectionNode();
                }
                if( !pOwnStartNode || pOwnStartNode != pTmp)
                {
                    throw uno::RuntimeException();
                }
            }
            else //dann pRange
            {
                SwBookmark* pBkm = pRange->GetBookmark();
                const SwStartNode* pTmp = pBkm->GetPos().nNode.GetNode().StartOfSectionNode();
                while( pTmp && pTmp->IsSectionNode())
                {
                    pTmp = pTmp->StartOfSectionNode();
                }
                if( !pOwnStartNode || pOwnStartNode != pTmp)
                {
                    throw uno::RuntimeException();
                }
            }
            if(bAbsorb)
            {
                //!! scan for CR characters and inserting the paragraph breaks
                //!! has to be done in the called function.
                //!! Implemented in SwXTextRange::DeleteAndInsert
                xTextRange->setString(aString);
            }
            else
            {
                //hier wird ein PaM angelegt, der vor dem Parameter-PaM liegt, damit der
                //Text davor eingefuegt wird
                UnoActionContext aContext(GetDoc());
                const SwPosition* pPos = pCursor ? pCursor->GetPaM()->Start() : pRange->GetBookmark()->Start();
                SwPaM aInsertPam(*pPos);
                sal_Bool bGroupUndo = GetDoc()->DoesGroupUndo();
                GetDoc()->DoGroupUndo(sal_False);

                SwUnoCursorHelper::DocInsertStringSplitCR( *GetDoc(), aInsertPam, aString );

                GetDoc()->DoGroupUndo(bGroupUndo);
            }
        }
        else
        {
            throw uno::RuntimeException();
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}
/*-- 09.12.98 12:43:16---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::insertControlCharacter(const uno::Reference< XTextRange > & xTextRange,
                sal_Int16 nControlCharacter, sal_Bool bAbsorb)
                throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(GetDoc() && xTextRange.is())
    {
        SwUnoInternalPaM aPam(*GetDoc());
        if(SwXTextRange::XTextRangeToSwPaM(aPam, xTextRange))
        {
            //Steuerzeichen einfuegen
            SwPaM aTmp(*aPam.Start());
            if(bAbsorb && aPam.HasMark())
                pDoc->DeleteAndJoin(aPam);

            sal_Unicode cIns = 0;
            switch( nControlCharacter )
            {
                case ControlCharacter::PARAGRAPH_BREAK :
                    // eine Tabellen Zelle wird jetzt zu einer normalen Textzelle!
                    pDoc->ClearBoxNumAttrs( aTmp.GetPoint()->nNode );
                    pDoc->SplitNode( *aTmp.GetPoint(), sal_False );
                    break;
                case ControlCharacter::APPEND_PARAGRAPH:
                {
                    pDoc->ClearBoxNumAttrs( aTmp.GetPoint()->nNode );
                    pDoc->AppendTxtNode( *aTmp.GetPoint() );

                    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
                    SwXTextRange* pRange = 0;
                    OTextCursorHelper* pCursor = 0;
                    if(xRangeTunnel.is())
                    {
                        pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                                SwXTextRange::getUnoTunnelId());
                        pCursor = (OTextCursorHelper*)xRangeTunnel->getSomething(
                                                OTextCursorHelper::getUnoTunnelId());
                    }
                    if(pRange)
                    {
                        pRange->_CreateNewBookmark(aTmp);
                    }
                    else if(pCursor)
                    {
                        SwPaM* pCrsr = pCursor->GetPaM();
                        *pCrsr->GetPoint() = *aTmp.GetPoint();
                        pCrsr->DeleteMark();
                    }
                }
                break;
                case ControlCharacter::LINE_BREAK:	cIns = 10; 		break;
                case ControlCharacter::SOFT_HYPHEN:	cIns = CHAR_SOFTHYPHEN;	break;
                case ControlCharacter::HARD_HYPHEN:	cIns = CHAR_HARDHYPHEN; break;
                case ControlCharacter::HARD_SPACE:	cIns = CHAR_HARDBLANK;	break;
            }
            if( cIns )
                pDoc->Insert( aTmp, cIns );

            if(bAbsorb)
            {
                uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
                SwXTextRange* pRange = 0;
                OTextCursorHelper* pCursor = 0;
                if(xRangeTunnel.is())
                {
                    pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                            SwXTextRange::getUnoTunnelId());
                    pCursor = (OTextCursorHelper*)xRangeTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId());
                }

                SwCursor aCrsr(*aTmp.GetPoint());
                SwXTextCursor::SelectPam(aCrsr, sal_True);
                aCrsr.Left(1, CRSR_SKIP_CHARS, FALSE, FALSE);
                //hier muss der uebergebene PaM umgesetzt werden:
                if(pRange)
                    pRange->_CreateNewBookmark(aCrsr);
                else
                {
                    SwPaM* pUnoCrsr = pCursor->GetPaM();
                    *pUnoCrsr->GetPoint() = *aCrsr.GetPoint();
                    if(aCrsr.HasMark())
                    {
                        pUnoCrsr->SetMark();
                        *pUnoCrsr->GetMark() = *aCrsr.GetMark();
                    }
                    else
                        pUnoCrsr->DeleteMark();
                }
            }
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 09.12.98 12:43:17---------------------------------------------------

  -----------------------------------------------------------------------*/

void SwXText::insertTextContent(const uno::Reference< XTextRange > & xRange,
                const uno::Reference< XTextContent > & xContent, sal_Bool bAbsorb)
                throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    // erstmal testen, ob der Range an der richtigen Stelle ist und dann
    // am Sw-Content attachToRange aufrufen
    if(!GetDoc())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    if(xRange.is() && xContent.is())
    {
        SwUnoInternalPaM aPam(*GetDoc());
        if(SwXTextRange::XTextRangeToSwPaM(aPam, xRange))
        {
            uno::Reference<lang::XUnoTunnel> xRangeTunnel( xRange, uno::UNO_QUERY);
            SwXTextRange* pRange = 0;
            OTextCursorHelper* pCursor = 0;
            SwXTextPortion* pPortion = 0;
            SwXText* pText = 0;
            if(xRangeTunnel.is())
            {
                pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                        SwXTextRange::getUnoTunnelId());
                pCursor = (OTextCursorHelper*)xRangeTunnel->getSomething(
                                        OTextCursorHelper::getUnoTunnelId());
                pPortion = (SwXTextPortion*)xRangeTunnel->getSomething(
                                        SwXTextPortion::getUnoTunnelId());
                pText = (SwXText*)xRangeTunnel->getSomething(
                                        SwXText::getUnoTunnelId());
            }


            uno::Reference< XTextCursor >  xOwnCursor = createCursor();
            uno::Reference<lang::XUnoTunnel> xOwnTunnel( xOwnCursor, uno::UNO_QUERY);
            OTextCursorHelper* pOwnCursor = (OTextCursorHelper*)xOwnTunnel->getSomething(
                                    OTextCursorHelper::getUnoTunnelId());

            const SwStartNode* pOwnStartNode = GetStartNode();
            SwStartNodeType eSearchNodeType = SwNormalStartNode;
            switch(eCrsrType)
            {
                case CURSOR_FRAME:		eSearchNodeType = SwFlyStartNode;		break;
                case CURSOR_TBLTEXT: 	eSearchNodeType = SwTableBoxStartNode;	break;
                case CURSOR_FOOTNOTE: 	eSearchNodeType = SwFootnoteStartNode; 	break;
                case CURSOR_HEADER:		eSearchNodeType = SwHeaderStartNode;	break;
                case CURSOR_FOOTER:		eSearchNodeType = SwFooterStartNode;	break;
                //case CURSOR_INVALID:
                //case CURSOR_BODY:
            }

            const SwNode* pSrcNode = 0;
            if(pCursor && pCursor->GetPaM())
            {
                pSrcNode = pCursor->GetPaM()->GetNode();
            }
            else if (pRange && pRange->GetBookmark())
            {
                SwBookmark* pBkm = pRange->GetBookmark();
                pSrcNode = &pBkm->GetPos().nNode.GetNode();
            }
            else if (pPortion && pPortion->GetCrsr())
            {
                pSrcNode = pPortion->GetCrsr()->GetNode();
            }
            else if (pText)
            {
                Reference<XTextCursor> xTextCursor = pText->createCursor();
                xTextCursor->gotoEnd(sal_True);
                Reference<XUnoTunnel> xCrsrTunnel( xTextCursor, UNO_QUERY );
                pCursor = (OTextCursorHelper*)xCrsrTunnel->getSomething(
                        OTextCursorHelper::getUnoTunnelId() );
                pSrcNode = pCursor->GetPaM()->GetNode();
            }
            else
                throw lang::IllegalArgumentException();

            const SwStartNode* pTmp = pSrcNode->FindSttNodeByType(eSearchNodeType);

            //SectionNodes ueberspringen
            while(pTmp && pTmp->IsSectionNode())
            {
                pTmp = pTmp->StartOfSectionNode();
            }
            //if the document starts with a section
            while(pOwnStartNode->IsSectionNode())
            {
                pOwnStartNode = pOwnStartNode->StartOfSectionNode();
            }
            //this checks if (this) and xRange are in the same XText interface
            if(pOwnStartNode != pTmp)
            {
                RuntimeException aRunException;
                aRunException.Message = C2U("text interface and cursor not related");
                throw aRunException;
            }
            // Sonderbehandlung fuer Contents, die den Range nicht ersetzen, sonder darueber gelegt werden
            // Bookmarks, IndexEntry
            sal_Bool bAttribute = sal_False;
            uno::Reference<lang::XUnoTunnel> xContentTunnel( xContent, uno::UNO_QUERY);
            if(!xContentTunnel.is())
            {
                IllegalArgumentException aArgException;
                aArgException.Message = C2U("text content doesn't support com::sun::star::lang::XUnoTunnel");
                throw aArgException;
            }
            SwXDocumentIndexMark* pDocumentIndexMark =
                (SwXDocumentIndexMark*)xContentTunnel->getSomething(
                                        SwXDocumentIndexMark::getUnoTunnelId());


            SwXTextSection* pSection = (SwXTextSection*)xContentTunnel->getSomething(
                                        SwXTextSection::getUnoTunnelId());

            SwXBookmark* pBookmark = (SwXBookmark*)xContentTunnel->getSomething(
                                        SwXBookmark::getUnoTunnelId());


            SwXReferenceMark* pReferenceMark = (SwXReferenceMark*)
                        xContentTunnel->getSomething(
                                        SwXReferenceMark::getUnoTunnelId());

            bAttribute = pBookmark || pDocumentIndexMark || pSection || pReferenceMark;

            if(bAbsorb && !bAttribute)
            {
                xRange->setString(aEmptyStr);
            }
            //hier wird tatsaechlich eingefuegt
            uno::Reference< XTextRange >  xTempRange;
            if(bAttribute && bAbsorb)
                xTempRange = xRange;
            else
                xTempRange = xRange->getStart();
            SwXTextTable* pTable = (SwXTextTable*)
                        xContentTunnel->getSomething(
                                        SwXTextTable::getUnoTunnelId());

            if(pTable)
                pTable->attachToRange(xTempRange);
            else
            {
                if(pBookmark)
                    pBookmark ->attachToRange(xTempRange);
                else
                {
                    if(pSection)
                        pSection ->attachToRange(xTempRange);
                    else
                    {
                        SwXFootnote* pFootnote = (SwXFootnote*)
                                xContentTunnel->getSomething(
                                                SwXFootnote::getUnoTunnelId());

                        if(pFootnote)
                            pFootnote->attachToRange(xTempRange);
                        else
                        {
                            if(pReferenceMark)
                                pReferenceMark->attachToRange(xTempRange);
                            else
                            {
                                SwXFrame* pFrame = (SwXFrame*)
                                    xContentTunnel->getSomething(
                                                    SwXFrame::getUnoTunnelId());

                                if(pFrame)
                                    pFrame->attachToRange(xTempRange);
                                else
                                {
                                    SwXDocumentIndex* pDocumentIndex = (SwXDocumentIndex*)
                                        xContentTunnel->getSomething(
                                                        SwXDocumentIndex::getUnoTunnelId());

                                    if(pDocumentIndex)
                                        pDocumentIndex->attachToRange(xTempRange);
                                    else
                                    {
                                        if(pDocumentIndexMark)
                                            pDocumentIndexMark->attachToRange(xTempRange);
                                        else
                                        {
                                            SwXTextField* pTextField = (SwXTextField*)
                                                xContentTunnel->getSomething(
                                                                SwXTextField::getUnoTunnelId());

                                            if(pTextField)
                                                pTextField->attachToRange(xTempRange);
                                            else
                                            {
                                                uno::Reference<beans::XPropertySet> xShapeProperties(xContent, uno::UNO_QUERY);
                                                SwXShape* pShape = 0;
                                                if(xShapeProperties.is())
                                                    pShape = (SwXShape*)xContentTunnel->getSomething(
                                                                            SwXShape::getUnoTunnelId());
                                                if(pShape)
                                                {
                                                    uno::Any aPos(&xRange,
                                                        ::getCppuType((uno::Reference<XTextRange>*)0));
                                                    pShape->setPropertyValue(C2U("TextRange"), aPos);

                                                    uno::Reference<frame::XModel> xModel =
                                                                    pDoc->GetDocShell()->GetBaseModel();
                                                    uno::Reference<drawing::XDrawPageSupplier> xPageSupp(
                                                                xModel, uno::UNO_QUERY);

                                                    uno::Reference<drawing::XDrawPage> xPage = xPageSupp->getDrawPage();

                                                    uno::Reference<drawing::XShape> xShape((cppu::OWeakObject*)pShape,
                                                                                                            uno::UNO_QUERY);
                                                    //nuer die XShapes haengen an der Sw-Drawpage
                                                    uno::Reference<drawing::XShapes> xShps(xPage, uno::UNO_QUERY);
                                                    xShps->add(xShape);
                                                }
                                                else
                                                {
                                                    IllegalArgumentException aArgException;
                                                    aArgException.Message = C2U("unknown text content");
                                                    throw aArgException;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            IllegalArgumentException aIllegal;
            aIllegal.Message = C2U("first parameter invalid");
            throw aIllegal;
        }
    }
    else
    {
        IllegalArgumentException aIllegal;
        if(!xRange.is())
            aIllegal.Message = C2U("first parameter invalid;");
        if(!xContent.is())
            aIllegal.Message += C2U("second parameter invalid");
        throw aIllegal;
    }

}
/* -----------------------------10.07.00 15:40--------------------------------

 ---------------------------------------------------------------------------*/
void SwXText::insertTextContentBefore(
    const Reference< XTextContent>& xNewContent,
    const Reference< XTextContent>& xSuccessor)
        throw(IllegalArgumentException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }

    SwXParagraph* pPara = SwXParagraph::GetImplementation(xNewContent);
    if(!pPara || !pPara->IsDescriptor() || !xSuccessor.is())
        throw IllegalArgumentException();

    sal_Bool bRet = FALSE;
    SwXTextSection* pXSection = SwXTextSection::GetImplementation( xSuccessor );
    SwXTextTable* pXTable = SwXTextTable::GetImplementation(xSuccessor );
    SwFrmFmt* pTableFmt = pXTable ? pXTable->GetFrmFmt() : 0;
    SwUnoCrsr* pUnoCrsr = 0;
    if(pTableFmt && pTableFmt->GetDoc() == GetDoc())
    {
        SwTable* pTable = SwTable::FindTable( pTableFmt );
        SwTableNode* pTblNode = pTable->GetTableNode();

        SwNodeIndex aTblIdx(  *pTblNode, -1 );
        SwPosition aBefore(aTblIdx);
        bRet = GetDoc()->AppendTxtNode( aBefore );
        pUnoCrsr = GetDoc()->CreateUnoCrsr( aBefore, FALSE);
    }
    else if(pXSection &&
        pXSection->GetFmt() &&
            pXSection->GetFmt()->GetDoc() == GetDoc())
    {
        SwSectionFmt* pSectFmt = pXSection->GetFmt();
        SwSectionNode* pSectNode = pSectFmt->GetSectionNode();

        SwNodeIndex aSectIdx(  *pSectNode, -1 );
        SwPosition aBefore(aSectIdx);
        bRet = GetDoc()->AppendTxtNode( aBefore );
        pUnoCrsr = GetDoc()->CreateUnoCrsr( aBefore, FALSE);
    }
    if(!bRet)
        throw IllegalArgumentException();
    else
    {
        pPara->attachToText(this, pUnoCrsr);
    }

}
/* -----------------------------10.07.00 15:40--------------------------------

 ---------------------------------------------------------------------------*/
void SwXText::insertTextContentAfter(
    const Reference< XTextContent>& xNewContent,
    const Reference< XTextContent>& xPredecessor)
        throw(IllegalArgumentException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc())
        throw RuntimeException();
    SwXParagraph* pPara = SwXParagraph::GetImplementation(xNewContent);
    if(!pPara || !pPara->IsDescriptor() || !xPredecessor.is())
        throw IllegalArgumentException();

    SwUnoCrsr* pUnoCrsr = 0;
    SwXTextSection* pXSection = SwXTextSection::GetImplementation( xPredecessor );
    SwXTextTable* pXTable = SwXTextTable::GetImplementation(xPredecessor );
    SwFrmFmt* pTableFmt = pXTable ? pXTable->GetFrmFmt() : 0;
    sal_Bool bRet = FALSE;
    if(pTableFmt && pTableFmt->GetDoc() == GetDoc())
    {
        SwTable* pTable = SwTable::FindTable( pTableFmt );
        SwTableNode* pTblNode = pTable->GetTableNode();

        SwEndNode* pTableEnd = pTblNode->EndOfSectionNode();
        SwPosition aTableEnd(*pTableEnd);
        bRet = GetDoc()->AppendTxtNode( aTableEnd );
        pUnoCrsr = GetDoc()->CreateUnoCrsr( aTableEnd, FALSE);
    }
    else if(pXSection &&
        pXSection->GetFmt() &&
            pXSection->GetFmt()->GetDoc() == GetDoc())
    {
        SwSectionFmt* pSectFmt = pXSection->GetFmt();
        SwSectionNode* pSectNode = pSectFmt->GetSectionNode();
        SwEndNode* pEnd = pSectNode->EndOfSectionNode();
        SwPosition aEnd(*pEnd);
        bRet = GetDoc()->AppendTxtNode( aEnd );
        pUnoCrsr = GetDoc()->CreateUnoCrsr( aEnd, FALSE);
    }
    if(!bRet)
        throw IllegalArgumentException();
    else
    {
        pPara->attachToText(this, pUnoCrsr);
    }
}
/* -----------------------------10.07.00 15:40--------------------------------

 ---------------------------------------------------------------------------*/
void SwXText::removeTextContentBefore(
    const Reference< XTextContent>& xSuccessor)
        throw(IllegalArgumentException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }

    sal_Bool bRet = FALSE;
    SwXTextSection* pXSection = SwXTextSection::GetImplementation( xSuccessor );
    SwXTextTable* pXTable = SwXTextTable::GetImplementation( xSuccessor );
    SwFrmFmt* pTableFmt = pXTable ? pXTable->GetFrmFmt() : 0;
    if(pTableFmt && pTableFmt->GetDoc() == GetDoc())
    {
        SwTable* pTable = SwTable::FindTable( pTableFmt );
        SwTableNode* pTblNode = pTable->GetTableNode();

        SwNodeIndex aTblIdx(  *pTblNode, -1 );
        if(aTblIdx.GetNode().IsTxtNode())
        {
            SwPaM aBefore(aTblIdx);
            bRet = GetDoc()->DelFullPara( aBefore );
        }
    }
    else if(pXSection &&
        pXSection->GetFmt() &&
            pXSection->GetFmt()->GetDoc() == GetDoc())
    {
        SwSectionFmt* pSectFmt = pXSection->GetFmt();
        SwSectionNode* pSectNode = pSectFmt->GetSectionNode();

        SwNodeIndex aSectIdx(  *pSectNode, -1 );
        if(aSectIdx.GetNode().IsTxtNode())
        {
            SwPaM aBefore(aSectIdx);
            bRet = GetDoc()->DelFullPara( aBefore );
        }
    }
    if(!bRet)
        throw IllegalArgumentException();
}
/* -----------------------------10.07.00 15:40--------------------------------

 ---------------------------------------------------------------------------*/
void SwXText::removeTextContentAfter(const Reference< XTextContent>& xPredecessor)
        throw(IllegalArgumentException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!GetDoc())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }

    sal_Bool bRet = FALSE;
    SwXTextSection* pXSection = SwXTextSection::GetImplementation( xPredecessor );
    SwXTextTable* pXTable = SwXTextTable::GetImplementation(xPredecessor );
    SwFrmFmt* pTableFmt = pXTable ? pXTable->GetFrmFmt() : 0;
    if(pTableFmt && pTableFmt->GetDoc() == GetDoc())
    {
        SwTable* pTable = SwTable::FindTable( pTableFmt );
        SwTableNode* pTblNode = pTable->GetTableNode();
        SwEndNode* pTableEnd = pTblNode->EndOfSectionNode();

        SwNodeIndex aTblIdx(  *pTableEnd, 1 );
        if(aTblIdx.GetNode().IsTxtNode())
        {
            SwPaM aPaM(aTblIdx);
            bRet = GetDoc()->DelFullPara( aPaM );
        }
    }
    else if(pXSection &&
        pXSection->GetFmt() &&
            pXSection->GetFmt()->GetDoc() == GetDoc())
    {
        SwSectionFmt* pSectFmt = pXSection->GetFmt();
        SwSectionNode* pSectNode = pSectFmt->GetSectionNode();
        SwEndNode* pEnd = pSectNode->EndOfSectionNode();
        SwNodeIndex aSectIdx(  *pEnd, 1 );
        if(aSectIdx.GetNode().IsTxtNode())
        {
            SwPaM aAfter(aSectIdx);
            bRet = GetDoc()->DelFullPara( aAfter );
        }
    }
    if(!bRet)
        throw IllegalArgumentException();
}
/*-- 09.12.98 12:43:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::removeTextContent(const uno::Reference< XTextContent > & xContent)
    throw( container::NoSuchElementException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!xContent.is())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U("first parameter invalid");
        throw aRuntime;
    }
    else
        xContent->dispose();
}
/*-- 09.12.98 12:43:22---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XText >  SwXText::getText(void)
        throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XText >  xRet = (SwXText*)this;
    return xRet;

}
/*-- 09.12.98 12:43:24---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXText::getStart(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  xRef = createCursor();
    if(!xRef.is())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    xRef->gotoStart(sal_False);
    uno::Reference< XTextRange >  xRet(xRef, uno::UNO_QUERY);
    return xRet;
}
/*-- 09.12.98 12:43:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextRange >  SwXText::getEnd(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  xRef = createCursor();
    if(!xRef.is())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    else
        xRef->gotoEnd(sal_False);
    uno::Reference< XTextRange >  xRet(xRef, uno::UNO_QUERY);;

    return xRet;
}
/*-- 09.12.98 12:43:29---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXText::getString(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  xRet = createCursor();
    if(!xRet.is())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    else
    {
        xRet->gotoEnd(sal_True);
    }
    return xRet->getString();
}
/*-- 09.12.98 12:43:30---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::setString(const OUString& aString) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    const SwStartNode* pStartNode = GetStartNode();
    if(!pStartNode)
        throw uno::RuntimeException();

    GetDoc()->StartUndo(UNDO_START, NULL);
    //insert an empty paragraph at the start and at the end to ensure that
    //all tables and sections can be removed by the selecting XTextCursor
    {
        SwPosition aStartPos(*pStartNode);
        const SwEndNode* pEnd = pStartNode->EndOfSectionNode();
        SwNodeIndex aEndIdx(*pEnd);
        aEndIdx--;
        //the inserting of nodes should only be done if really necessary
        //to prevent #97924# (removes paragraph attributes when setting the text
        //e.g. of a table cell
        BOOL bInsertNodes = FALSE;
        SwNodeIndex aStartIdx(*pStartNode);
        do
        {
            aStartIdx++;
            SwNode& rCurrentNode = aStartIdx.GetNode();
            if(rCurrentNode.GetNodeType() == ND_SECTIONNODE
                ||rCurrentNode.GetNodeType() == ND_TABLENODE)
            {
                bInsertNodes = TRUE;
                break;
            }
        }
        while(aStartIdx < aEndIdx);
        if(bInsertNodes)
        {
            GetDoc()->AppendTxtNode( aStartPos );
            SwPosition aEndPos(aEndIdx.GetNode());
            SwPaM aPam(aEndPos);
            GetDoc()->AppendTxtNode( *aPam.Start() );
        }
    }

    uno::Reference< XTextCursor >  xRet = createCursor();
    if(!xRet.is())
    {
        GetDoc()->EndUndo(UNDO_END, NULL);
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    else
    {
        xRet->gotoEnd(sal_True);
    }
    xRet->setString(aString);
    GetDoc()->EndUndo(UNDO_END, NULL);
}
/* -----------------------------28.03.00 11:12--------------------------------
    Description: Checks if pRange/pCursor are member of the same text interface.
                Only one of the pointers has to be set!
 ---------------------------------------------------------------------------*/
sal_Bool	SwXText::CheckForOwnMember(
    const SwXTextRange* pRange,
    const OTextCursorHelper* pCursor)
        throw(IllegalArgumentException, RuntimeException)
{
    DBG_ASSERT((!pRange || !pCursor) && (pRange || pCursor), "only one pointer will be checked" )
    Reference<XTextCursor> xOwnCursor = createCursor();

    uno::Reference<lang::XUnoTunnel> xTunnel( xOwnCursor, uno::UNO_QUERY);
    OTextCursorHelper* pOwnCursor = 0;
    if(xTunnel.is())
    {
        pOwnCursor = (OTextCursorHelper*)xTunnel->getSomething(OTextCursorHelper::getUnoTunnelId());
    }
    DBG_ASSERT(pOwnCursor, "OTextCursorHelper::getUnoTunnelId() ??? ")
    const SwStartNode* pOwnStartNode = pOwnCursor->GetPaM()->GetNode()->StartOfSectionNode();
    SwStartNodeType eSearchNodeType = SwNormalStartNode;
    switch(eCrsrType)
    {
        case CURSOR_FRAME:		eSearchNodeType = SwFlyStartNode;		break;
        case CURSOR_TBLTEXT: 	eSearchNodeType = SwTableBoxStartNode;	break;
        case CURSOR_FOOTNOTE: 	eSearchNodeType = SwFootnoteStartNode; 	break;
        case CURSOR_HEADER:		eSearchNodeType = SwHeaderStartNode;	break;
        case CURSOR_FOOTER:		eSearchNodeType = SwFooterStartNode;	break;
        //case CURSOR_INVALID:
        //case CURSOR_BODY:
    }

    const SwNode* pSrcNode;
    if(pCursor)
    {
        pSrcNode = pCursor->GetPaM()->GetNode();
    }
    else //dann pRange
    {
        SwBookmark* pBkm = pRange->GetBookmark();
        pSrcNode = &pBkm->GetPos().nNode.GetNode();
    }
    const SwStartNode* pTmp = pSrcNode->FindSttNodeByType(eSearchNodeType);

    //SectionNodes ueberspringen
    while(pTmp && pTmp->IsSectionNode())
    {
        pTmp = pTmp->StartOfSectionNode();
    }
    //if the document starts with a section
    while(pOwnStartNode->IsSectionNode())
    {
        pOwnStartNode = pOwnStartNode->StartOfSectionNode();
    }
    //this checks if (this) and xRange are in the same XText interface
    return(pOwnStartNode == pTmp);
}

/* -----------------------------28.03.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int16 SwXText::ComparePositions(
    const Reference<XTextRange>& xPos1,
    const Reference<XTextRange>& xPos2)
            throw(IllegalArgumentException, RuntimeException)
{
    sal_Int16 nCompare;
    SwUnoInternalPaM aPam1(*GetDoc());
    SwUnoInternalPaM aPam2(*GetDoc());

    BOOL bExcept = FALSE;
    if(SwXTextRange::XTextRangeToSwPaM(aPam1, xPos1) &&
            SwXTextRange::XTextRangeToSwPaM(aPam2, xPos2))
    {
        uno::Reference<lang::XUnoTunnel> xRangeTunnel1( xPos1, UNO_QUERY);
        SwXTextRange* pRange1 = 0;
        OTextCursorHelper* pCursor1 = 0;
        if(xRangeTunnel1.is())
        {
            pRange1 = (SwXTextRange*)xRangeTunnel1->getSomething(
                                    SwXTextRange::getUnoTunnelId());
            pCursor1 = (OTextCursorHelper*)xRangeTunnel1->getSomething(
                                    OTextCursorHelper::getUnoTunnelId());
        }
        uno::Reference<lang::XUnoTunnel> xRangeTunnel2( xPos2, UNO_QUERY);
        SwXTextRange* pRange2 = 0;
        OTextCursorHelper* pCursor2 = 0;
        if(xRangeTunnel2.is())
        {
            pRange2 = (SwXTextRange*)xRangeTunnel2->getSomething(
                                    SwXTextRange::getUnoTunnelId());
            pCursor2 = (OTextCursorHelper*)xRangeTunnel2->getSomething(
                                    OTextCursorHelper::getUnoTunnelId());
        }

        if((pRange1||pCursor1) && (pRange2||pCursor2))
        {
            if(CheckForOwnMember(pRange1, pCursor1)
                && CheckForOwnMember( pRange2, pCursor2))
            {
                const SwPosition *pStart1 = 0;
                const SwPosition *pStart2 = 0;

                if(pRange1)
                    pStart1 = pRange1->GetBookmark() ? pRange1->GetBookmark()->Start() : 0;
                else
                    pStart1 = pCursor1->GetPaM() ? pCursor1->GetPaM()->Start() : 0;

                if(pRange2)
                    pStart2 = pRange2->GetBookmark() ? pRange2->GetBookmark()->Start() : 0;
                else
                    pStart2 = pCursor2->GetPaM() ? pCursor2->GetPaM()->Start() : 0;

                if(pStart1 && pStart2)
                {
                    if(*pStart1 < *pStart2)
                        nCompare = 1;
                    else if(*pStart1 > *pStart2)
                        nCompare = -1;
                    else
                    {
                        DBG_ASSERT(*pStart1 == *pStart2, "SwPositions should be equal here")
                        nCompare = 0;
                    }
                }
                else
                    bExcept = TRUE;
            }
            else
                bExcept = TRUE;
        }
        else
            bExcept = TRUE;
    }
    else
        bExcept = TRUE;
    if(bExcept)
        throw IllegalArgumentException();

    return nCompare;
}

/*-- 28.03.00 10:37:22---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int16 SwXText::compareRegionStarts(
    const Reference<XTextRange>& xR1,
    const Reference<XTextRange>& xR2)
            throw(IllegalArgumentException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!xR1.is() || !xR2.is())
        throw IllegalArgumentException();
    Reference<XTextRange> xStart1 = xR1->getStart();
    Reference<XTextRange> xStart2 = xR2->getStart();

    return ComparePositions(xStart1, xStart2);
}
/*-- 28.03.00 10:37:25---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int16 SwXText::compareRegionEnds(
    const Reference<XTextRange>& xR1,
    const Reference<XTextRange>& xR2)
                    throw(IllegalArgumentException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!xR1.is() || !xR2.is())
        throw IllegalArgumentException();
    Reference<XTextRange> xEnd1 = xR1->getEnd();
    Reference<XTextRange> xEnd2 = xR2->getEnd();

    return ComparePositions(xEnd1, xEnd2);
}

/*-- 15.03.2002 12:30:40---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo > SwXText::getPropertySetInfo(  )
    throw(uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo > xInfo =
        new SfxItemPropertySetInfo(_pMap);
    return xInfo;
}
/*-- 15.03.2002 12:30:42---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::setPropertyValue( const ::rtl::OUString& aPropertyName, const uno::Any& aValue )
    throw(beans::UnknownPropertyException, beans::PropertyVetoException,
    lang::IllegalArgumentException, lang::WrappedTargetException,
    uno::RuntimeException)
{
    throw lang::IllegalArgumentException();
}
/*-- 15.03.2002 12:30:42---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXText::getPropertyValue(
    const ::rtl::OUString& rPropertyName )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException,
                uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw  uno::RuntimeException();
    const SfxItemPropertyMap*   pMap = SfxItemPropertyMap::GetByName(
                                                    _pMap, rPropertyName);
    uno::Any aRet;
    if(pMap)
    {
        switch(pMap->nWID)
        {
//          no code necessary - the redline is always located at the end node
//            case FN_UNO_REDLINE_NODE_START:
//            break;
            case FN_UNO_REDLINE_NODE_END:
            {
                const SwRedlineTbl& rRedTbl = GetDoc()->GetRedlineTbl();
                USHORT nRedTblCount = rRedTbl.Count();
                if ( nRedTblCount > 0 )
                {
                    const SwStartNode* pStartNode = GetStartNode();
                    ULONG nOwnIndex = pStartNode->EndOfSectionIndex();
                    for(USHORT nRed = 0; nRed < nRedTblCount; nRed++)
                    {
                        const SwRedline* pRedline = rRedTbl[nRed];
                        const SwPosition* pRedStart = pRedline->Start();
                        const SwNodeIndex nRedNode = pRedStart->nNode;
                        if(nOwnIndex == nRedNode.GetIndex())
                        {
                            aRet <<= SwXRedlinePortion::CreateRedlineProperties(*pRedline, TRUE);
                            break;
                        }
                    }
                }
            }
            break;
        }
    }
    else
    {
        UnknownPropertyException aExcept;
        aExcept.Message = C2U("Unknown property: ");
        aExcept.Message += rPropertyName;
        throw aExcept;
    }
    return aRet;

}
/*-- 15.03.2002 12:30:42---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::addPropertyChangeListener(
    const ::rtl::OUString& aPropertyName,
    const uno::Reference< beans::XPropertyChangeListener >& xListener )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    throw uno::RuntimeException();
}
/*-- 15.03.2002 12:30:43---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::removePropertyChangeListener(
    const ::rtl::OUString& aPropertyName,
    const uno::Reference< beans::XPropertyChangeListener >& aListener )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException,
            uno::RuntimeException)
{
    throw uno::RuntimeException();
}
/*-- 15.03.2002 12:30:43---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::addVetoableChangeListener(
    const ::rtl::OUString& PropertyName,
    const uno::Reference< beans::XVetoableChangeListener >& aListener )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    throw uno::RuntimeException();
}
/*-- 15.03.2002 12:30:43---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXText::removeVetoableChangeListener(
    const ::rtl::OUString& PropertyName,
    const uno::Reference< beans::XVetoableChangeListener >& aListener )
        throw(beans::UnknownPropertyException, lang::WrappedTargetException,
        uno::RuntimeException)
{
    throw uno::RuntimeException();
}

/* -----------------------------08.01.01 09:07--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXText::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------08.01.01 09:07--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SwXText::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}

/******************************************************************
 * SwXBodyText
 ******************************************************************/
SwXBodyText::SwXBodyText(SwDoc* pDoc) :
    SwXText(pDoc, CURSOR_BODY)
{
}

/*-- 10.12.98 11:17:27---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXBodyText::~SwXBodyText()
{

}
/* -----------------------------06.04.00 16:33--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXBodyText::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXBodyText");
}
/* -----------------------------06.04.00 16:33--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXBodyText::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.Text") == rServiceName;
}
/* -----------------------------06.04.00 16:33--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXBodyText::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Text");
    return aRet;
}
/*-- 10.12.98 11:17:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXBodyText::queryAggregation(
    const uno::Type& rType )
        throw(::com::sun::star::uno::RuntimeException)
{
    uno::Any aRet;
    const uno::Type& rXEnumerationAccessType = ::getCppuType((uno::Reference< container::XEnumerationAccess >*)0);
    const uno::Type& rXElementAccessType = ::getCppuType((uno::Reference< container::XElementAccess >*)0);
    const uno::Type& rXServiceInfoType = ::getCppuType((uno::Reference< lang::XServiceInfo >*)0);

    if(rType == rXEnumerationAccessType)
    {
        Reference<container::XEnumerationAccess> xRet = this;
        aRet.setValue(&xRet, rXEnumerationAccessType);
    }
    else if(rType == rXElementAccessType)
    {
        Reference<container::XElementAccess> xRet = this;
        aRet.setValue(&xRet, rXElementAccessType);
    }
    else if(rType == rXServiceInfoType)
    {
        Reference<lang::XServiceInfo> xRet = this;
        aRet.setValue(&xRet, rXServiceInfoType);
    }
    else
    {
        aRet = SwXText::queryInterface( rType );
    }
    if(aRet.getValueType() == ::getCppuVoidType())
        aRet = OWeakAggObject::queryAggregation( rType );
    return aRet;
}

/*-- 10.12.98 11:17:28---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< uno::Type > SwXBodyText::getTypes(  ) throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aTypes = SwXBodyTextBaseClass::getTypes();
    uno::Sequence< uno::Type > aTextTypes = SwXText::getTypes();
    long nIndex = aTypes.getLength();
    aTypes.realloc(aTypes.getLength() + aTextTypes.getLength());
    uno::Type* pTypes = aTypes.getArray();
    const uno::Type* pTextTypes = aTextTypes.getConstArray();
    for(int i = 0; i < aTextTypes.getLength(); i++)
        pTypes[nIndex++] = pTextTypes[i];
    return aTypes;
}
/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< sal_Int8 > SwXBodyText::getImplementationId(  ) throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static Sequence< sal_Int8 > aId( 16 );
    static sal_Bool bInit = sal_False;
    if(!bInit)
    {
        rtl_createUuid( (sal_uInt8 *)(aId.getArray() ), 0, sal_True );
        bInit = sal_True;
    }
    return aId;
}
/*-- 10.12.98 11:17:28---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL
    SwXBodyText::queryInterface( const uno::Type& rType )
        throw(uno::RuntimeException)
{
      uno::Any aRet = SwXText::queryInterface( rType );
    if(aRet.getValueType() == ::getCppuVoidType())
        aRet = SwXBodyTextBaseClass::queryInterface( rType );
    return aRet;
}
/* -----------------------------05.01.00 11:07--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< XTextCursor >  SwXBodyText::CreateTextCursor(sal_Bool bIgnoreTables)
{
    uno::Reference< XTextCursor >  xRet;
    if(IsValid())
    {
        SwNode& rNode = GetDoc()->GetNodes().GetEndOfContent();
        //erstmal sicherstellen, dass wir nicht in einer Tabelle stehen
        SwPaM aPam(rNode);
        aPam.Move( fnMoveBackward, fnGoDoc );
        if(!bIgnoreTables)
        {
            SwTableNode* pTblNode = aPam.GetNode()->FindTableNode();
            SwCntntNode* pCont = 0;
            while( pTblNode )
            {
                aPam.GetPoint()->nNode = *pTblNode->EndOfSectionNode();
                pCont = GetDoc()->GetNodes().GoNext(&aPam.GetPoint()->nNode);
                pTblNode = pCont->FindTableNode();
            }
            if(pCont)
                aPam.GetPoint()->nContent.Assign(pCont, 0);
        }
        xRet =  (XWordCursor*)new SwXTextCursor(this, *aPam.GetPoint(), CURSOR_BODY, GetDoc());
    }
    return xRet;
}
/*-- 10.12.98 11:17:29---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextCursor >  SwXBodyText::createTextCursor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  aRef = CreateTextCursor(sal_False);
    if(!aRef.is())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    return aRef;
}
/*-- 10.12.98 11:17:29---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextCursor >  SwXBodyText::createTextCursorByRange(
            const uno::Reference< XTextRange > & aTextPosition) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  aRef;
    if(!IsValid())
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    SwUnoInternalPaM aPam(*GetDoc());
    if(SwXTextRange::XTextRangeToSwPaM(aPam, aTextPosition))
    {
        SwNode& rNode = GetDoc()->GetNodes().GetEndOfContent();

        SwStartNode* p1 = aPam.GetNode()->StartOfSectionNode();
        //document starts with a section?
        while(p1->IsSectionNode())
        {
            p1 = p1->StartOfSectionNode();
        }
        SwStartNode* p2 = rNode.StartOfSectionNode();

        if(p1 == p2)
            aRef =  (XWordCursor*)new SwXTextCursor(this , *aPam.GetPoint(), CURSOR_BODY, GetDoc(), aPam.GetMark());
    }
    if(!aRef.is())
        throw uno::RuntimeException();
    return aRef;
}
/*-- 10.12.98 11:17:30---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXBodyText::createEnumeration(void)
                            throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< container::XEnumeration >  aRef;
    if(IsValid())
    {
        SwNode& rNode = GetDoc()->GetNodes().GetEndOfContent();
        SwPosition aPos(rNode);
        SwUnoCrsr* pUnoCrsr = GetDoc()->CreateUnoCrsr(aPos, sal_False);
        pUnoCrsr->Move( fnMoveBackward, fnGoDoc );
        aRef = new SwXParagraphEnumeration(this, pUnoCrsr, CURSOR_BODY);
    }
    else
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    return aRef;

}
/* -----------------18.12.98 13:36-------------------
 *
 * --------------------------------------------------*/
uno::Type SwXBodyText::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((uno::Reference<XTextRange>*)0);
}
/* -----------------18.12.98 13:36-------------------
 *
 * --------------------------------------------------*/
sal_Bool SwXBodyText::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(IsValid())
        return sal_True;
    else
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    return sal_False;
}
/******************************************************************
 *	SwXHeadFootText
 ******************************************************************/
TYPEINIT1(SwXHeadFootText, SwClient);
/* -----------------------------06.04.00 16:40--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXHeadFootText::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXHeadFootText");
}
/* -----------------------------06.04.00 16:40--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXHeadFootText::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.Text") == rServiceName;
}
/* -----------------------------06.04.00 16:40--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXHeadFootText::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Text");
    return aRet;
}
/*-- 11.12.98 10:14:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXHeadFootText::SwXHeadFootText(SwFrmFmt& rHeadFootFmt, BOOL bHeader) :
    SwXText(rHeadFootFmt.GetDoc(), bHeader ? CURSOR_HEADER : CURSOR_FOOTER),
    SwClient(&rHeadFootFmt),
    bIsHeader(bHeader)
{

}
/*-- 11.12.98 10:14:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXHeadFootText::~SwXHeadFootText()
{

}
/*-- 11.12.98 10:14:49---------------------------------------------------

  -----------------------------------------------------------------------*/
const SwStartNode *SwXHeadFootText::GetStartNode() const
{
    const SwStartNode *pSttNd = 0;
    SwFrmFmt* pHeadFootFmt = GetFmt();
    if(pHeadFootFmt)
    {
        const SwFmtCntnt& rFlyCntnt = pHeadFootFmt->GetCntnt();
        if( rFlyCntnt.GetCntntIdx() )
            pSttNd = rFlyCntnt.GetCntntIdx()->GetNode().GetStartNode();
    }
    return pSttNd;
}

uno::Reference< XTextCursor >   SwXHeadFootText::createCursor() throw(uno::RuntimeException)
{
    return createTextCursor();
}
/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< ::com::sun::star::uno::Type > SwXHeadFootText::getTypes(  ) throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aHFTypes = SwXHeadFootTextBaseClass::getTypes();
    uno::Sequence< uno::Type > aTextTypes = SwXText::getTypes();

    long nIndex = aHFTypes.getLength();
    aHFTypes.realloc(
        aHFTypes.getLength() +
        aTextTypes.getLength());

    uno::Type* pHFTypes = aHFTypes.getArray();
    const uno::Type* pTextTypes = aTextTypes.getConstArray();
    for(long nPos = 0; nPos < aTextTypes.getLength(); nPos++)
        pHFTypes[nIndex++] = pTextTypes[nPos];

    return aHFTypes;
}

/* -----------------------------21.03.00 15:39--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< sal_Int8 > SwXHeadFootText::getImplementationId(  ) throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static Sequence< sal_Int8 > aId( 16 );
    static sal_Bool bInit = sal_False;
    if(!bInit)
    {
        rtl_createUuid( (sal_uInt8 *)(aId.getArray() ), 0, sal_True );
        bInit = sal_True;
    }
    return aId;
}
/* -----------------------------21.03.00 15:46--------------------------------

 ---------------------------------------------------------------------------*/
uno::Any SwXHeadFootText::queryInterface( const uno::Type& aType ) throw(uno::RuntimeException)
{
    uno::Any aRet = SwXHeadFootTextBaseClass::queryInterface(aType);
    if(aRet.getValueType() == ::getCppuVoidType() )
        aRet = SwXText::queryInterface(aType);
    return aRet;
}

/*-- 11.12.98 10:14:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextCursor >  SwXHeadFootText::createTextCursor(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  	xRet;
    SwFrmFmt* pHeadFootFmt = GetFmt();
    if(pHeadFootFmt)
    {
        const SwFmtCntnt& rFlyCntnt = pHeadFootFmt->GetCntnt();
        const SwNode& rNode = rFlyCntnt.GetCntntIdx()->GetNode();
        SwPosition aPos(rNode);
        SwXTextCursor* pCrsr = new SwXTextCursor(this, aPos, bIsHeader ? CURSOR_HEADER : CURSOR_FOOTER, GetDoc());
        SwUnoCrsr* pUnoCrsr = pCrsr->GetCrsr();
        pUnoCrsr->Move(fnMoveForward, fnGoNode);

        //save current start node to be able to check if there is content after the table -
        //otherwise the cursor would be in the body text!

        const SwStartNode* pOwnStartNode = rNode.FindSttNodeByType(
                        bIsHeader ? SwHeaderStartNode : SwFooterStartNode);
        //steht hier eine Tabelle?
        SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
        SwCntntNode* pCont = 0;
        while( pTblNode )
        {
            pUnoCrsr->GetPoint()->nNode = *pTblNode->EndOfSectionNode();
            pCont = GetDoc()->GetNodes().GoNext(&pUnoCrsr->GetPoint()->nNode);
            pTblNode = pCont->FindTableNode();
        }
        if(pCont)
            pUnoCrsr->GetPoint()->nContent.Assign(pCont, 0);
        const SwStartNode* pNewStartNode = pUnoCrsr->GetNode()->FindSttNodeByType(
                        bIsHeader ? SwHeaderStartNode : SwFooterStartNode);
        if(!pNewStartNode || pNewStartNode != pOwnStartNode)
        {
            pCrsr = NULL;
            uno::RuntimeException aExcept;
            aExcept.Message = S2U("no text available");
            throw aExcept;
        }
        xRet =  (XWordCursor*)pCrsr;
    }
    else
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }
    return xRet;
}
/*-- 11.12.98 10:14:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextCursor >  SwXHeadFootText::createTextCursorByRange(
                const uno::Reference< XTextRange > & aTextPosition) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextCursor >  xRet;
    SwFrmFmt* pHeadFootFmt = GetFmt();
    SwUnoInternalPaM aPam(*GetDoc());
    if(pHeadFootFmt && SwXTextRange::XTextRangeToSwPaM(aPam, aTextPosition))
    {
        SwNode& rNode = pHeadFootFmt->GetCntnt().GetCntntIdx()->GetNode();
        SwPosition aPos(rNode);
        SwPaM aHFPam(aPos);
        aHFPam.Move(fnMoveForward, fnGoNode);
        SwStartNode* pOwnStartNode = aHFPam.GetNode()->FindSttNodeByType(
                        bIsHeader ? SwHeaderStartNode : SwFooterStartNode);
        SwStartNode* p1 = aPam.GetNode()->FindSttNodeByType(
            bIsHeader ? SwHeaderStartNode : SwFooterStartNode);
        if(p1 == pOwnStartNode)
            xRet =  (XWordCursor*)new SwXTextCursor(this, *aPam.GetPoint(),
                            bIsHeader ? CURSOR_HEADER : CURSOR_FOOTER, GetDoc(), aPam.GetMark());
    }
    return xRet;
}
/* -----------------19.03.99 15:44-------------------
 *
 * --------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXHeadFootText::createEnumeration(void)
    throw( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< container::XEnumeration >  aRef;
    //wenn this ungueltig ist, dann kommt die uno::Exception aus createTextCursor()
    SwFrmFmt* pHeadFootFmt = GetFmt();
    if(pHeadFootFmt)
    {
        const SwFmtCntnt& rFlyCntnt = pHeadFootFmt->GetCntnt();
        const SwNode& rNode = rFlyCntnt.GetCntntIdx()->GetNode();
        SwPosition aPos(rNode);
        SwUnoCrsr* pUnoCrsr = GetDoc()->CreateUnoCrsr(aPos, sal_False);
        pUnoCrsr->Move(fnMoveForward, fnGoNode);
        aRef = new SwXParagraphEnumeration(this, pUnoCrsr, bIsHeader ? CURSOR_HEADER : CURSOR_FOOTER);
    }
    else
    {
        RuntimeException aRuntime;
        aRuntime.Message = C2U(cInvalidObject);
        throw aRuntime;
    }

    return aRef;
}
/* -----------------19.03.99 15:50-------------------
 *
 * --------------------------------------------------*/
uno::Type SwXHeadFootText::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((uno::Reference<XTextRange>*)0);
}
/* -----------------19.03.99 15:50-------------------
 *
 * --------------------------------------------------*/
sal_Bool SwXHeadFootText::hasElements(void) throw( uno::RuntimeException )
{
    return sal_True;
}

/*-- 11.12.98 10:14:51---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXHeadFootText::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify( this, pOld, pNew);
}

