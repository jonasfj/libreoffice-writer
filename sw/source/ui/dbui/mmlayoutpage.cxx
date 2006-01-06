/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mmlayoutpage.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: kz $ $Date: 2006-01-06 13:03:17 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#pragma hdrstop
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _MAILMERGELAYOUTPAGE_HXX
#include <mmlayoutpage.hxx>
#endif
#ifndef _MAILMERGEWIZARD_HXX
#include <mailmergewizard.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif
#ifndef _MAILMERGEHELPER_HXX
#include <mailmergehelper.hxx>
#endif
#ifndef _UNOTOOLS_HXX
#include <unotools.hxx>
#endif
#ifndef _UNOTOOLS_TEMPFILE_HXX
#include <unotools/tempfile.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _SFXDISPATCH_HXX 
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXSTRITEM_HXX 
#include <svtools/stritem.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX
#include <sfx2/docfilt.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_XPARAGRAPHCURSOR_HPP_
#include <com/sun/star/text/XParagraphCursor.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_XVIEWSETTINGSSUPPLIER_HPP_
#include <com/sun/star/view/XViewSettingsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_DOCUMENTZOOMTYPE_HPP_
#include <com/sun/star/view/DocumentZoomType.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _FLDMGR_HXX
#include <fldmgr.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _UNOTXDOC_HXX 
#include <unotxdoc.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _FMTSRND_HXX
#include <fmtsrnd.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _FMTANCHR_HXX 
#include <fmtanchr.hxx>
#endif
#ifndef _FMTORNT_HXX 
#include <fmtornt.hxx>
#endif
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX
#include <svx/boxitem.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#include <osl/file.hxx>
#ifndef _UNOPRNMS_HXX
#include <unoprnms.hxx>
#endif


#include <mmlayoutpage.hrc>
#include <dbui.hrc>

using namespace osl;
using namespace svt;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::view;

#define DEFAULT_LEFT_DISTANCE (MM50*5)  // 2,5 cm
#define DEFAULT_TOP_DISTANCE  (MM50*11) // 5,5 cm
#define GREETING_TOP_DISTANCE (MM50*25) //12,5 cm
#define DEFAULT_ADDRESS_WIDTH  (MM50*15)// 7,5 cm
#define DEFAULT_ADDRESS_HEIGHT (MM50*7) // 3,5cm

#define C2U(cChar) ::rtl::OUString::createFromAscii(cChar)
/*-- 15.04.2004 08:16:35---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeLayoutPage::SwMailMergeLayoutPage( SwMailMergeWizard* _pParent) :
    svt::OWizardPage( _pParent, SW_RES(DLG_MM_LAYOUT_PAGE)),
#pragma warning (disable : 4355)
    m_aHeaderFI( this, ResId(         FI_HEADER             )),
    m_aAlignToBodyCB( this, ResId(      CB_ALIGN              )),
    m_aPositionFL( this, ResId(       FL_POSITION           )),
    m_aLeftFT( this, ResId(           FT_LEFT               )),
    m_aLeftMF( this, ResId(           MF_LEFT               )),
    m_aTopFT( this, ResId(            FT_TOP                )),
    m_aTopMF( this, ResId(            MF_TOP                )),
    m_aGreetingLineFL( this, ResId(   FL_GREETINGLINE       )),
    m_aUpFT( this, ResId(             FT_UP                 )),
    m_aUpPB( this, ResId(             MF_UP                 )),
    m_aDownFT( this, ResId(           FT_DOWN               )),
    m_aDownPB( this, ResId(           PB_DOWN               )),
    m_aExampleContainerWIN( this, ResId(  WIN_EXAMPLECONTAINER      )),
    m_aExampleWIN( this, 0 ),
    m_aZoomFT( this, ResId(           FT_ZOOM               )),
    m_aZoomLB( this, ResId(           LB_ZOOM               )),
#pragma warning (default : 4355)
    m_pWizard(_pParent),
    m_pAddressBlockFormat(0),
    m_pExampleFrame(0),
    m_pExampleWrtShell(0),
    m_bIsGreetingInserted(false)
{
    FreeResource();
    m_aExampleWIN.SetPosSizePixel(m_aExampleContainerWIN.GetPosPixel(),
                                m_aExampleContainerWIN.GetSizePixel());

    
    const SfxFilter *pSfxFlt = SwIoSystem::GetFilterOfFormat(
            String::CreateFromAscii( GetFILTER_XML() ),
            SwDocShell::Factory().GetFilterContainer() );
    //save the current document into a temporary file
    {
        //temp file needs it's own block 
        //creating with extension is not supported by a static method :-(
        String sLeading;
        String sExt(pSfxFlt->GetDefaultExtension());
        sExt.EraseLeadingChars('*');
        utl::TempFile aTempFile( sLeading, &sExt );
        m_sExampleURL = aTempFile.GetURL();
        aTempFile.EnableKillingFile();
    }
    SwView* pView = m_pWizard->GetSwView();
    uno::Sequence< beans::PropertyValue > aValues(1);
    beans::PropertyValue* pValues = aValues.getArray();
    pValues[0].Name = C2U("FilterName");
    pValues[0].Value <<= ::rtl::OUString(pSfxFlt->GetFilterName());

    uno::Reference< frame::XStorable > xStore( pView->GetDocShell()->GetModel(), uno::UNO_QUERY);
    xStore->storeToURL( m_sExampleURL, aValues   );

    Link aLink(LINK(this, SwMailMergeLayoutPage, PreviewLoadedHdl_Impl));
    m_pExampleFrame = new SwOneExampleFrame( m_aExampleWIN,
                                    EX_SHOW_DEFAULT_PAGE, &aLink, &m_sExampleURL );

    m_aExampleWIN.Show( FALSE );
    m_aExampleContainerWIN.Show(TRUE);

    m_aLeftMF.SetValue(m_aLeftMF.Normalize(DEFAULT_LEFT_DISTANCE), FUNIT_TWIP);
    m_aTopMF.SetValue(m_aTopMF.Normalize(DEFAULT_TOP_DISTANCE), FUNIT_TWIP);

    m_aZoomLB.InsertEntry(String::CreateFromAscii("50 %"), 1);
    m_aZoomLB.InsertEntry(String::CreateFromAscii("75 %"), 2);
    m_aZoomLB.InsertEntry(String::CreateFromAscii("100 %"), 3);
    m_aZoomLB.SelectEntryPos(0); //page size
    m_aZoomLB.SetSelectHdl(LINK(this, SwMailMergeLayoutPage, ZoomHdl_Impl));

    Link aFrameHdl = LINK(this, SwMailMergeLayoutPage, ChangeAddressHdl_Impl);
    m_aLeftMF.SetUpHdl(aFrameHdl);
    m_aLeftMF.SetDownHdl(aFrameHdl);
    m_aLeftMF.SetLoseFocusHdl(aFrameHdl);
    m_aTopMF.SetUpHdl(aFrameHdl);
    m_aTopMF.SetDownHdl(aFrameHdl);
    m_aTopMF.SetLoseFocusHdl(aFrameHdl);

    Link aUpDownHdl = LINK(this, SwMailMergeLayoutPage, GreetingsHdl_Impl );
    m_aUpPB.SetClickHdl(aUpDownHdl);
    m_aDownPB.SetClickHdl(aUpDownHdl);
    m_aAlignToBodyCB.SetClickHdl(LINK(this, SwMailMergeLayoutPage, AlignToTextHdl_Impl));
    m_aAlignToBodyCB.Check();
}    
/*-- 15.04.2004 08:17:11---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeLayoutPage::~SwMailMergeLayoutPage()
{
    delete m_pExampleFrame;
    File::remove( m_sExampleURL );

}
/*-- 27.05.2004 13:41:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwMailMergeLayoutPage::ActivatePage()
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    sal_Bool bGreetingLine = rConfigItem.IsGreetingLine(sal_False) && !rConfigItem.IsGreetingInserted();
    sal_Bool bAddressBlock = rConfigItem.IsAddressBlock() && !rConfigItem.IsAddressInserted();
    
    m_aPositionFL.Enable(bAddressBlock);
    m_aLeftFT.Enable(bAddressBlock);
    m_aTopFT.Enable(bAddressBlock);
    m_aLeftMF.Enable(bAddressBlock);
    m_aTopMF.Enable(bAddressBlock);
    AlignToTextHdl_Impl( &m_aAlignToBodyCB );

    m_aGreetingLineFL.Enable(bGreetingLine);
    m_aUpPB.Enable(bGreetingLine);
    m_aDownPB.Enable(bGreetingLine);
    m_aUpFT.Enable(bGreetingLine);
    m_aDownFT.Enable(bGreetingLine);

    //check if greeting and/or address frame have to be inserted/removed
    if(m_pExampleWrtShell) // initially there's nothing to check
    {
        if(!rConfigItem.IsGreetingInserted() && 
                m_bIsGreetingInserted != (0 != bGreetingLine) )
        {
            if( m_bIsGreetingInserted )
            {
                m_pExampleWrtShell->DelFullPara();
                m_bIsGreetingInserted = false;
            }        
            else
            {
                InsertGreeting(*m_pExampleWrtShell, m_pWizard->GetConfigItem(), true);
                m_bIsGreetingInserted = true;
            }            
        }    
        if(!rConfigItem.IsAddressInserted() && 
                rConfigItem.IsAddressBlock() != ( 0 != m_pAddressBlockFormat ))
        {
            if( m_pAddressBlockFormat )
            {
                m_pExampleWrtShell->Push();
                m_pExampleWrtShell->GotoFly( m_pAddressBlockFormat->GetName() );
                m_pExampleWrtShell->DelRight();        
                m_pAddressBlockFormat = 0;
                m_pExampleWrtShell->Pop(FALSE);
            }
            else
            {
                long nLeft = m_aLeftMF.Denormalize(m_aLeftMF.GetValue(FUNIT_TWIP));
                long nTop = m_aTopMF.Denormalize(m_aTopMF.GetValue(FUNIT_TWIP));
                m_pAddressBlockFormat = InsertAddressFrame(
                        *m_pExampleWrtShell, m_pWizard->GetConfigItem(),
                        Point(nLeft, nTop), 
                        m_aAlignToBodyCB.IsChecked(), true);
            }    
        }    
                
    }            
}
/*-- 11.05.2004 10:41:26---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwMailMergeLayoutPage::commitPage(COMMIT_REASON _eReason)
{
    //now insert the frame and the greeting
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    if(CR_TRAVEL_NEXT == _eReason)
    {        
        long nLeft = m_aLeftMF.Denormalize(m_aLeftMF.GetValue(FUNIT_TWIP));
        long nTop = m_aTopMF.Denormalize(m_aTopMF.GetValue(FUNIT_TWIP));
        InsertAddressAndGreeting(
                    m_pWizard->GetSwView(), 
                    rConfigItem, 
                    Point(nLeft, nTop),
                    m_aAlignToBodyCB.IsChecked());
    }            
    return sal_True;
}
/*-- 24.06.2004 09:50:26---------------------------------------------------

  -----------------------------------------------------------------------*/
SwFrmFmt*  SwMailMergeLayoutPage::InsertAddressAndGreeting(SwView* pView, 
        SwMailMergeConfigItem& rConfigItem, 
        const Point& rAddressPosition, 
        bool bAlignToBody)
{        
    SwFrmFmt* pAddressBlockFormat = 0;
    pView->GetWrtShell().StartUndo(UNDO_INSERT);
    if(rConfigItem.IsAddressBlock() && !rConfigItem.IsAddressInserted())
    {        
        //insert the frame
        Point aAddressPosition(DEFAULT_LEFT_DISTANCE, DEFAULT_TOP_DISTANCE);
        if(rAddressPosition.X() > 0 && rAddressPosition.Y() > 0)
            aAddressPosition = rAddressPosition;
        pAddressBlockFormat = InsertAddressFrame( pView->GetWrtShell(), 
                                        rConfigItem,
                                        aAddressPosition, bAlignToBody, false);
        rConfigItem.SetAddressInserted(pAddressBlockFormat->GetName());
    }
    //now the greeting
    if(rConfigItem.IsGreetingLine(sal_False) && !rConfigItem.IsGreetingInserted())
    {        
        InsertGreeting( pView->GetWrtShell(), rConfigItem, false);
        rConfigItem.SetGreetingInserted();
    }
    pView->GetWrtShell().EndUndo(UNDO_INSERT);
    return pAddressBlockFormat;
}
/*-- 11.05.2004 12:49:04---------------------------------------------------

  -----------------------------------------------------------------------*/
SwFrmFmt* SwMailMergeLayoutPage::InsertAddressFrame(
        SwWrtShell& rShell, 
        SwMailMergeConfigItem& rConfigItem,
        const Point& rDestination,
        bool bAlignLeft,
        bool bExample)
{
    // insert the address block and the greeting line
    SfxItemSet aSet(rShell.GetAttrPool(), RES_ANCHOR, RES_ANCHOR,
                        RES_VERT_ORIENT, RES_VERT_ORIENT, 
                        RES_HORI_ORIENT, RES_HORI_ORIENT, 
                        RES_BOX, RES_BOX,
                        RES_FRM_SIZE, RES_FRM_SIZE, 
                        RES_SURROUND, RES_SURROUND, 
                        0 );
    aSet.Put(SwFmtAnchor(FLY_PAGE, 1));
    if(bAlignLeft)
        aSet.Put(SwFmtHoriOrient( 0, HORI_NONE, REL_PG_PRTAREA ));
    else
        aSet.Put(SwFmtHoriOrient( rDestination.X(), HORI_NONE, REL_PG_FRAME ));
    aSet.Put(SwFmtVertOrient( rDestination.Y(), VERT_NONE, REL_PG_FRAME ));
    aSet.Put(SwFmtFrmSize( ATT_FIX_SIZE, DEFAULT_ADDRESS_WIDTH, DEFAULT_ADDRESS_HEIGHT ));
    // the example gets a border around the frame, the real document doesn't get one
    if(!bExample)
        aSet.Put(SvxBoxItem( RES_BOX ));
    aSet.Put(SwFmtSurround( SURROUND_NONE ));
    
    rShell.NewFlyFrm(aSet, sal_True );  
    SwFrmFmt* pRet = rShell.GetFlyFrmFmt();
    ASSERT( pRet, "Fly not inserted" );

    rShell.UnSelectFrm();  
    const Sequence< ::rtl::OUString> aBlocks = rConfigItem.GetAddressBlocks();
    if(bExample)
    {
        rShell.Insert(aBlocks[0]);
    }
    else
    {
        //the placeholders should be replaced by the appropriate fields
        SwFldMgr aFldMgr(&rShell);
        //create a database string source.command.commandtype.column
        const SwDBData& rData = rConfigItem.GetCurrentDBData();
        String sDBName(rData.sDataSource);
        sDBName += DB_DELIM;
        sDBName += String(rData.sCommand);
        sDBName += DB_DELIM;
        String sDatabaseConditionPrefix(sDBName);
        sDatabaseConditionPrefix.SearchAndReplaceAll(DB_DELIM, '.');
        sDBName += String::CreateFromInt32(rData.nCommandType);
        sDBName += DB_DELIM;
        
        // if only the country is in an address line the 
        // paragraph has to be hidden depending on the 
        // IsIncludeCountry()/GetExcludeCountry() settings

        sal_Bool bIncludeCountry = rConfigItem.IsIncludeCountry();
        sal_Bool bHideEmptyParagraphs = rConfigItem.IsHideEmptyParagraphs();
        const ::rtl::OUString rExcludeCountry = rConfigItem.GetExcludeCountry();
        bool bSpecialReplacementForCountry = (!bIncludeCountry || rExcludeCountry.getLength());

        const ResStringArray& rHeaders = rConfigItem.GetDefaultAddressHeaders();
        String sCountryColumn = rHeaders.GetString(MM_PART_COUNTRY);
        Sequence< ::rtl::OUString> aAssignment = 
                        rConfigItem.GetColumnAssignment( rConfigItem.GetCurrentDBData() );
        const ::rtl::OUString* pAssignment = aAssignment.getConstArray();
        if(aAssignment.getLength() > MM_PART_COUNTRY && aAssignment[MM_PART_COUNTRY].getLength())
            sCountryColumn = aAssignment[MM_PART_COUNTRY];
        //
        String sHideParagraphsExpression;
        SwAddressIterator aIter(aBlocks[0]);
        while(aIter.HasMore())
        {        
            SwMergeAddressItem aItem = aIter.Next();
            if(aItem.bIsColumn)
            {
                String sConvertedColumn = aItem.sText;
                for(USHORT nColumn = 0; 
                        nColumn < rHeaders.Count() && nColumn < aAssignment.getLength(); 
                                                                                    ++nColumn)
                {
                    if(rHeaders.GetString(nColumn) == aItem.sText && 
                        pAssignment[nColumn].getLength())
                    {
                        sConvertedColumn = pAssignment[nColumn];
                        break;
                    }            
                }
                String sDB(sDBName);
                sDB += sConvertedColumn;

                if(sHideParagraphsExpression.Len())
                   sHideParagraphsExpression.AppendAscii(" AND ");
                sHideParagraphsExpression += '!';
                sHideParagraphsExpression += '[';
                sHideParagraphsExpression += sDatabaseConditionPrefix;
                sHideParagraphsExpression += sConvertedColumn;
                sHideParagraphsExpression += ']';

                if( bSpecialReplacementForCountry && sCountryColumn == sConvertedColumn )
                {
                    // now insert a hidden paragraph field
                    String sExpression;
                    if( rExcludeCountry.getLength() )
                    {
                        sExpression = sDatabaseConditionPrefix;
                        sExpression.Insert('[', 0);
                        sExpression += sCountryColumn;
                        sExpression.AppendAscii("]");

                        String sCondition(sExpression);
                        sCondition.AppendAscii(" != \"");
                        sCondition += String(rExcludeCountry);
                        sCondition += '\"';

                        SwInsertFld_Data aData(TYP_CONDTXTFLD, 0, sCondition, sExpression, 0, &rShell );
                        aFldMgr.InsertFld( aData );
                    }    
                    else
                    {
                        SwInsertFld_Data aData(TYP_HIDDENPARAFLD, 0, sExpression, aEmptyStr, 0, &rShell );
                        aFldMgr.InsertFld( aData );
                    }
                }
                else
                {
                    SwInsertFld_Data aData(TYP_DBFLD, 0, sDB, aEmptyStr, 0, &rShell );
                    aFldMgr.InsertFld( aData );
                }
            }
            else if(!aItem.bIsReturn)
            {
                rShell.Insert(aItem.sText);
            }    
            else
            {
                if(bHideEmptyParagraphs)
                {
                    SwInsertFld_Data aData(TYP_HIDDENPARAFLD, 0, sHideParagraphsExpression, aEmptyStr, 0, &rShell );
                    aFldMgr.InsertFld( aData );
                }            
                sHideParagraphsExpression.Erase();
                //now add a new paragraph
                rShell.SplitNode();
            }                                                             
        }
        if(bHideEmptyParagraphs && sHideParagraphsExpression.Len())
        {
            SwInsertFld_Data aData(TYP_HIDDENPARAFLD, 0, sHideParagraphsExpression, aEmptyStr, 0, &rShell );
            aFldMgr.InsertFld( aData );
        }            
    }            
    return pRet;
}

/*-- 12.05.2004 12:20:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwMailMergeLayoutPage::InsertGreeting(SwWrtShell& rShell, SwMailMergeConfigItem& rConfigItem, bool bExample)
{
    //set the cursor to the desired position - if no text content is here then 
    //new paragraphs are inserted
    sal_Bool bRet = rShell.SetShadowCrsrPos( 
            Point(DEFAULT_LEFT_DISTANCE + DOCUMENTBORDER, GREETING_TOP_DISTANCE), FILL_SPACE );
    if(!bRet)
    {
        //there's already text at the desired position 
        //go to start of the doc, directly!
        rShell.SwCrsrShell::SttDoc();
        //and go by paragraph until the position is reached
        long nYPos = rShell.GetCharRect().Top();
        while(nYPos < GREETING_TOP_DISTANCE)
        {        
            if(!rShell.FwdPara())
                break;
            nYPos = rShell.GetCharRect().Top();
        }
        //text needs to be appended 
        while(nYPos < GREETING_TOP_DISTANCE)
        {
            if(!rShell.AppendTxtNode())
                break;
            nYPos = rShell.GetCharRect().Top();
        }            
    }
    else
    {
        //we may end up inside of a paragraph if the left margin is not at DEFAULT_LEFT_DISTANCE
        rShell.MovePara(GetfnParaCurr(), GetfnParaStart());
    }
    bool bSplitNode = rShell.GetText().Len() > 0;
//    rShell.SetTxtFmtColl( rShell.GetTxtCollFromPool( RES_POOLCOLL_GREETING ) );
    sal_Int32 nMoves = rConfigItem.GetGreetingMoves();
    if( !bExample && 0 != nMoves )
    {
        if(nMoves < 0)
        {        
            rShell.MoveParagraph( nMoves );
        }
        else 
            while(nMoves)
            {        
                sal_Bool bMoved = rShell.MoveParagraph( 1 );
                if(!bMoved)
                {
                    //insert a new paragraph before the greeting line
                    rShell.SplitNode();
                }            
                --nMoves;
            }
    }            
    //now insert the greeting text - if we have any? 
    sal_Bool bIndividual = rConfigItem.IsIndividualGreeting(sal_False);
    String sGreeting;
    if(bIndividual)
    {        
        //lock expression fields - prevents hiding of the paragraph to insert into
        rShell.LockExpFlds();
        if(bExample)
        {
            for(sal_Int8 eGender = SwMailMergeConfigItem::FEMALE;
                eGender <= SwMailMergeConfigItem::NEUTRAL; ++eGender)
            {
                Sequence< ::rtl::OUString > aEntries = 
                            rConfigItem.GetGreetings((SwMailMergeConfigItem::Gender)eGender);
                sal_Int32 nCurrent = rConfigItem.GetCurrentGreeting((SwMailMergeConfigItem::Gender)eGender);
                if( nCurrent >= 0 && nCurrent < aEntries.getLength())
                {        
                    sGreeting = aEntries[nCurrent];
                    rShell.Insert(sGreeting);
                    break;
                }
            }
        }
        else
        {
            SwFldMgr aFldMgr(&rShell);
            //three paragraphs, each with an appropriate hidden paragraph field 
            //are to be inserted 
            
            //name of the gender column
            String sGenderColumn = rConfigItem.GetAssignedColumn(MM_PART_GENDER);
            String sNameColumn = rConfigItem.GetAssignedColumn(MM_PART_LASTNAME);
            
            const ::rtl::OUString& rFemaleGenderValue = rConfigItem.GetFemaleGenderValue();
            sal_Bool bHideEmptyParagraphs = rConfigItem.IsHideEmptyParagraphs();
            const SwDBData& rData = rConfigItem.GetCurrentDBData();
            String sConditionBase(rData.sDataSource);
            sConditionBase += '.';
            sConditionBase += String(rData.sCommand);
            sConditionBase += '.';
            //split the name column from here
            String sNameColumnBase(sConditionBase);

            sConditionBase += String(sGenderColumn);
            sConditionBase += ']';
            sConditionBase.Insert('[', 0);

            sNameColumnBase += String(sNameColumn);
            sNameColumnBase += ']';
            sNameColumnBase.Insert('[', 0);

            String sDBName(rData.sDataSource);
            sDBName += DB_DELIM;
            sDBName += String(rData.sCommand);
            sDBName += DB_DELIM;
            sDBName += String::CreateFromInt32(rData.nCommandType);
            sDBName += DB_DELIM;

//          Female:  [database.sGenderColumn] != "rFemaleGenderValue" && [database.NameColumn]
//          Male:    [database.sGenderColumn] == "rFemaleGenderValue" && [database.rGenderColumn]
//          Neutral: [database.sNameColumn] 
            DBG_ASSERT(sGenderColumn.Len() && rFemaleGenderValue.getLength(),
                    "gender settings not available - how to form the condition?")
            //column used as lastname
            for(sal_Int8 eGender = SwMailMergeConfigItem::FEMALE;
                eGender <= SwMailMergeConfigItem::NEUTRAL; ++eGender)
            {
                Sequence< ::rtl::OUString> aEntries = rConfigItem.GetGreetings((SwMailMergeConfigItem::Gender)eGender);
                sal_Int32 nCurrent = rConfigItem.GetCurrentGreeting((SwMailMergeConfigItem::Gender)eGender);
                if( nCurrent >= 0 && nCurrent < aEntries.getLength())
                {        
                    sGreeting = aEntries[nCurrent];
                    String sCondition(sConditionBase);
                    String sHideParagraphsExpression;
                    switch(eGender)
                    {
                        case  SwMailMergeConfigItem::FEMALE:    
                            sCondition.AppendAscii(" != \"");
                            sCondition += String(rFemaleGenderValue);
                            sCondition.AppendAscii("\" OR NOT ");
                            sCondition += String(sNameColumnBase);
                            
                            sHideParagraphsExpression += '!';
                            sHideParagraphsExpression += sNameColumnBase;
                        break;
                        case  SwMailMergeConfigItem::MALE:      
                            sCondition.AppendAscii(" == \"");
                            sCondition += String(rFemaleGenderValue);
                            sCondition.AppendAscii("\" OR NOT ");
                            sCondition += String(sNameColumnBase);
                            sHideParagraphsExpression += '!';
                            sHideParagraphsExpression += sNameColumnBase;
                        break;
                        case  SwMailMergeConfigItem::NEUTRAL: 
                            sCondition = sNameColumnBase;
                        break;
                    }
                    SwInsertFld_Data aData(TYP_HIDDENPARAFLD, 0, sCondition, aEmptyStr, 0, &rShell );
                    aFldMgr.InsertFld( aData );
                    if(bHideEmptyParagraphs && sHideParagraphsExpression.Len())
                    {        
                        SwInsertFld_Data aData(TYP_HIDDENPARAFLD, 0, sHideParagraphsExpression, aEmptyStr, 0, &rShell );
                        aFldMgr.InsertFld( aData );
                    }
                    //now the text has to be inserted
                    const ResStringArray& rHeaders = rConfigItem.GetDefaultAddressHeaders();
                    Sequence< ::rtl::OUString> aAssignment = 
                                    rConfigItem.GetColumnAssignment( rConfigItem.GetCurrentDBData() );
                    const ::rtl::OUString* pAssignment = aAssignment.getConstArray();
                    SwAddressIterator aIter(sGreeting);
                    while(aIter.HasMore())
                    {        
                        SwMergeAddressItem aItem = aIter.Next();
                        if(aItem.bIsColumn)
                        {
                            String sDB(sDBName);
                            String sConvertedColumn = aItem.sText;
                            for(USHORT nColumn = 0; 
                                    nColumn < rHeaders.Count() && nColumn < aAssignment.getLength(); 
                                                                                                ++nColumn)
                            {
                                if(rHeaders.GetString(nColumn) == aItem.sText && 
                                    pAssignment[nColumn].getLength())
                                {
                                    sConvertedColumn = pAssignment[nColumn];
                                    break;
                                }            
                            }
                            sDB += sConvertedColumn;
                            SwInsertFld_Data aData(TYP_DBFLD, 0, sDB, aEmptyStr, 0, &rShell );
                            aFldMgr.InsertFld( aData );
                        }
                        else
                        {
                            rShell.Insert(aItem.sText);
                        }    
                    }
                    //now add a new paragraph
                    rShell.SplitNode();
                }
            }
            
        }
        rShell.UnlockExpFlds();
    }
    else
    {
        Sequence< ::rtl::OUString> aEntries = rConfigItem.GetGreetings(SwMailMergeConfigItem::NEUTRAL);
        sal_Int32 nCurrent = rConfigItem.GetCurrentGreeting(SwMailMergeConfigItem::NEUTRAL);
        if( nCurrent >= 0 && nCurrent < aEntries.getLength())
            sGreeting = aEntries[nCurrent];
        rShell.Insert(sGreeting);
    }            
    // now insert a new paragraph here if necessary
    if(bSplitNode)
    {        
        rShell.Push();
        rShell.SplitNode();
        rShell.Pop(FALSE);
    }
    //put the cursor to the start of the paragraph
    rShell.SttPara();
    
    DBG_ASSERT(0 == rShell.GetTableFmt(), "What to do with a table here?")
}
/*-- 10.05.2004 09:34:25---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeLayoutPage, PreviewLoadedHdl_Impl, void*, EMPTYARG)
{
    m_aExampleWIN.Show( TRUE );
    m_aExampleContainerWIN.Show(FALSE);
    
    Reference< XModel > & xModel = m_pExampleFrame->GetModel();
    //now the ViewOptions should be set properly
    Reference< XViewSettingsSupplier >  xSettings(xModel->getCurrentController(), UNO_QUERY);
    m_xViewProperties = xSettings->getViewSettings();
    Reference< XUnoTunnel > xDocTunnel(xModel, UNO_QUERY);
    SwXTextDocument* pXDoc = (SwXTextDocument*)xDocTunnel->getSomething(SwXTextDocument::getUnoTunnelId());
    SwDocShell* pDocShell = pXDoc->GetDocShell();
    m_pExampleWrtShell = pDocShell->GetWrtShell();
    DBG_ASSERT(m_pExampleWrtShell, "No SwWrtShell found!");
    if(!m_pExampleWrtShell)
        return 0;

    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    if(rConfigItem.IsAddressBlock())
    {
        m_pAddressBlockFormat = InsertAddressFrame(
                *m_pExampleWrtShell, rConfigItem,
                Point(DEFAULT_LEFT_DISTANCE, DEFAULT_TOP_DISTANCE), 
                m_aAlignToBodyCB.IsChecked(), true);
    }
    if(rConfigItem.IsGreetingLine(sal_False))
    {       
        InsertGreeting(*m_pExampleWrtShell, rConfigItem, true);
        m_bIsGreetingInserted = true;
    }

    Any aZoom;
    aZoom <<= (sal_Int16)DocumentZoomType::ENTIRE_PAGE;
    m_xViewProperties->setPropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_ZOOM_TYPE)), aZoom);
    

//    m_pExampleWrtShell->SetTxtFmtColl( rSh.GetTxtCollFromPool( RES_POOLCOLL_STANDARD ) );
    const SwFmtFrmSize& rPageSize = m_pExampleWrtShell->GetPageDesc( 
                                     m_pExampleWrtShell->GetCurPageDesc()).GetMaster().GetFrmSize();
    m_aLeftMF.SetMax(rPageSize.GetWidth() - DEFAULT_LEFT_DISTANCE);
    m_aTopMF.SetMax(rPageSize.GetHeight() - DEFAULT_TOP_DISTANCE);
    return 0;
}
/*-- 10.05.2004 14:05:24---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeLayoutPage, ZoomHdl_Impl, ListBox*, pBox)
{
    if(m_pExampleWrtShell)
    {        
        sal_Int16 eType = DocumentZoomType::BY_VALUE;
        short nZoom = 50;
        switch(pBox->GetSelectEntryPos())
        {
            case 0 : eType = DocumentZoomType::ENTIRE_PAGE; break;
            case 1 : nZoom = 50; break;
            case 2 : nZoom = 75; break;
            case 3 : nZoom = 100; break;
        }
        Any aZoom;
        aZoom <<= eType;
        m_xViewProperties->setPropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_ZOOM_TYPE)), aZoom);
        aZoom <<= nZoom;
        m_xViewProperties->setPropertyValue(C2U(SW_PROP_NAME_STR(UNO_NAME_ZOOM_VALUE)), aZoom);

    }
    return 0;
}
    

/*-- 10.05.2004 15:56:51---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeLayoutPage, ChangeAddressHdl_Impl, MetricField*, pField)
{
    if(m_pExampleWrtShell && m_pAddressBlockFormat)
    {        
        long nLeft = m_aLeftMF.Denormalize(m_aLeftMF.GetValue(FUNIT_TWIP));
        long nTop = m_aTopMF.Denormalize(m_aTopMF.GetValue(FUNIT_TWIP));

        SfxItemSet aSet(m_pExampleWrtShell->GetAttrPool(), RES_ANCHOR, RES_ANCHOR,
                            RES_VERT_ORIENT, RES_VERT_ORIENT, 
                            RES_HORI_ORIENT, RES_HORI_ORIENT, 
                            0 );
        if(m_aAlignToBodyCB.IsChecked())
            aSet.Put(SwFmtHoriOrient( 0, HORI_NONE, REL_PG_PRTAREA ));
        else
            aSet.Put(SwFmtHoriOrient( nLeft, HORI_NONE, REL_PG_FRAME ));
        aSet.Put(SwFmtVertOrient( nTop, VERT_NONE, REL_PG_FRAME ));
        m_pExampleWrtShell->GetDoc()->SetFlyFrmAttr( *m_pAddressBlockFormat, aSet );
    }
    return 0;
}

/*-- 10.05.2004 16:13:36---------------------------------------------------
    
  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeLayoutPage, GreetingsHdl_Impl, PushButton*, pButton)
{
    bool bDown = pButton == &m_aDownPB;
    BOOL bMoved = m_pExampleWrtShell->MoveParagraph( bDown ? 1 : -1 );
    m_pWizard->GetConfigItem().MoveGreeting(bDown ? 1 : -1 );
    if(!bMoved && bDown)
    {
        //insert a new paragraph before the greeting line
        m_pExampleWrtShell->SplitNode();
    }            
    
    return 0;
}
/*-- 15.07.2004 16:05:30---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeLayoutPage, AlignToTextHdl_Impl, CheckBox*, pBox)
{
    sal_Bool bCheck = pBox->IsChecked() && pBox->IsEnabled();
    m_aLeftFT.Enable(!bCheck);
    m_aLeftMF.Enable(!bCheck);
    ChangeAddressHdl_Impl( 0 );
    return 0;
}
