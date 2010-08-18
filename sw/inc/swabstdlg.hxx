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
#ifndef _SW_ABSTDLG_HXX
#define _SW_ABSTDLG_HXX

// include ---------------------------------------------------------------

#include <tools/solar.h>
#include <tools/string.hxx>
#include <sfx2/sfxdlg.hxx>
#include <svx/svxdlg.hxx>
#include <vcl/syswin.hxx>
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include "swunodef.hxx"
#include "itabenum.hxx"

class SfxViewFrame;
class SfxBindings;
class SfxItemSet;
class ResId;
class Window;
class String;
class SfxItemPool;
class SfxStyleSheetBase;
class SwGlossaryHdl;
class SwField;

class SwLabFmtPage;
class SwLabRec;
class SwAsciiOptions;
class SwDocShell;
class SvStream;
class SwWrtShell;
class SfxRequest;
class SwView;
class SwTableAutoFmt;
class SwTOXMgr;
class SwForm;
struct CurTOXType;
class SwTOXDescription;
class SwTOXBase;
class SwSectionData;
struct SwDBData;
class SwField;
class Printer;
class SwLabItem;
class SwNewDBMgr;
class SwTableFUNC;
class SwChildWinWrapper;
struct SfxChildWinInfo;
class SwTOXMark;
struct SwDocStat;
#include <cnttab.hxx> //add for struct CurTOXType

namespace com{namespace sun{namespace star{
    namespace frame{
        class XFrame;
    }
    namespace sdbcx{
    class XColumnsSupplier;
    }
    namespace sdbc{
    class XDataSource;
    class XConnection;
    class XResultSet;
    }
}}}

typedef   void (*SwLabDlgMethod) (::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel>& xModel,	const SwLabItem& rItem);

typedef String		(*GlossaryGetCurrGroup)();
typedef void   		(*GlossarySetActGroup)(const String& rNewGroup);

class AbstractGlossaryDlg : public VclAbstractDialog  //add for SwGlossaryDlg
{
public:
    virtual	String			GetCurrGrpName() const = 0;
    virtual String			GetCurrShortName() const = 0;
};

class AbstractFldInputDlg : public VclAbstractDialog  //add for SwFldInputDlg
{
public:
    //from class SalFrame
    virtual void            SetWindowState( const ByteString& rStr ) = 0;
    virtual ByteString      GetWindowState( ULONG nMask = WINDOWSTATE_MASK_ALL ) const = 0;
};

class AbstractInsFootNoteDlg : public VclAbstractDialog  //add for SwInsFootNoteDlg
{
public:
    virtual String          GetFontName() = 0;
    virtual BOOL            IsEndNote() = 0;
    virtual String          GetStr() = 0;
    //from class Window
    virtual void    SetHelpId( ULONG nHelpId ) = 0;
    virtual void    SetText( const XubString& rStr ) = 0;
};

class AbstractInsertGrfRulerDlg : public VclAbstractDialog  //add for SwInsertGrfRulerDlg
{
public:
    virtual String 			GetGraphicName() = 0;
    virtual BOOL			IsSimpleLine() = 0;
    virtual BOOL 			HasImages() const  = 0;
};

class AbstractInsTableDlg : public VclAbstractDialog  //add for SwInsTableDlg
{
public:
    virtual void			GetValues( String& rName, USHORT& rRow, USHORT& rCol,
                                SwInsertTableOptions& rInsTblFlags, String& rTableAutoFmtName,
                                SwTableAutoFmt *& prTAFmt ) = 0;
};

class AbstractJavaEditDialog : public VclAbstractDialog  //add for SwJavaEditDialog
{
public:
    virtual String              GetText() = 0;
    virtual String              GetType() = 0;
    virtual BOOL                IsUrl() = 0;
    virtual BOOL                IsNew() = 0;
    virtual BOOL                IsUpdate() = 0;
};

class AbstractMailMergeDlg : public VclAbstractDialog  //add for SwMailMergeDlg
{
public:
    virtual USHORT	GetMergeType() = 0;
    virtual const ::rtl::OUString& GetSaveFilter() const = 0;
    virtual const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > GetSelection() const = 0;
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet> GetResultSet() const = 0;
};
class AbstractMailMergeCreateFromDlg : public VclAbstractDialog  //add for SwMailMergeCreateFromDlg
{
public:
    virtual BOOL    IsThisDocument() const = 0;
};
class AbstractMailMergeFieldConnectionsDlg : public VclAbstractDialog  //add for SwMailMergeFieldConnectionsDlg
{
public:
    virtual BOOL    IsUseExistingConnections() const = 0;
};

class AbstractMultiTOXTabDialog : public VclAbstractDialog  //add for SwMultiTOXTabDialog
{
public:
    virtual SwForm*				GetForm(CurTOXType eType) = 0;
    virtual CurTOXType			GetCurrentTOXType() const = 0;
    virtual SwTOXDescription&	GetTOXDescription(CurTOXType eTOXTypes) = 0;
    //from SfxTabDialog
    virtual const SfxItemSet* 	GetOutputItemSet() const = 0;
};

class AbstractEditRegionDlg : public VclAbstractDialog  //add for SwEditRegionDlg
{
public:
    virtual void    SelectSection(const String& rSectionName) = 0;
};
class AbstractInsertSectionTabDialog : public VclAbstractDialog  //add for SwInsertSectionTabDialog
{
public:
    virtual void     SetSectionData(SwSectionData const& rSect) = 0;
};

class AbstractSwWordCountDialog : public VclAbstractDialog
{
public:
    virtual void    SetValues(const SwDocStat& rCurrent, const SwDocStat& rDoc) = 0;
};

class AbstractSwInsertAbstractDlg : public VclAbstractDialog    // add for SwInsertAbstractDlg
{
public:
    virtual BYTE	GetLevel() const = 0;
    virtual BYTE	GetPara() const = 0;
};

class AbstractSwAsciiFilterDlg : public VclAbstractDialog // add for SwAsciiFilterDlg
{
public:
    virtual void FillOptions( SwAsciiOptions& rOptions ) = 0;

};

class AbstractSwBreakDlg : public VclAbstractDialog // add for SwBreakDlg
{
public:
    virtual String  GetTemplateName() = 0;
    virtual USHORT  GetKind() = 0;
    virtual USHORT  GetPageNumber() = 0;

};

class AbstractSplitTableDialog : public VclAbstractDialog // add for 
{
public:
    virtual sal_uInt16 GetSplitMode() = 0;
};

class AbstractSwConvertTableDlg :  public VclAbstractDialog // add for SwConvertTableDlg
{
public:
    virtual void GetValues( sal_Unicode& rDelim,
                    SwInsertTableOptions& rInsTblFlags,
                    SwTableAutoFmt *& prTAFmt ) = 0;
};

class AbstractSwInsertDBColAutoPilot :  public VclAbstractDialog // add for SwInsertDBColAutoPilot
{
public:

    virtual void DataToDoc( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& rSelection,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource> rxSource,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> xConnection,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > xResultSet) = 0;
};

class AbstractDropDownFieldDialog : public VclAbstractDialog //add for DropDownFieldDialog
{
public:
     virtual ByteString      GetWindowState( ULONG nMask = WINDOWSTATE_MASK_ALL ) const = 0; //this method inherit from SystemWindow
     virtual void            SetWindowState( const ByteString& rStr ) =0;//this method inherit from SystemWindow
};

class AbstarctSwLabDlg  : public SfxAbstractTabDialog //add for SwLabDlg
{
public:
     virtual const String& GetBusinessCardStr() const = 0;
     virtual Printer *GetPrt() =0;
};

class AbstarctSwSelGlossaryDlg : public VclAbstractDialog //add for SwSelGlossaryDlg
{
public:
    virtual void InsertGlos(const String &rRegion, const String &rGlosName) = 0;	// inline
    virtual USHORT GetSelectedIdx() const = 0;	// inline
    virtual void SelectEntryPos(USHORT nIdx) = 0; 	// inline
};

class AbstractSwSplitTableDlg :public VclAbstractDialog //add for SwSplitTableDlg
{
public:
    virtual BOOL                IsHorizontal() const = 0;
    virtual BOOL                IsProportional() const = 0;
    virtual long                GetCount() const = 0;
};

class AbstractSwAutoFormatDlg : public VclAbstractDialog //add for SwAutoFormatDlg
{
public:
    virtual void FillAutoFmtOfIndex( SwTableAutoFmt*& rToFill ) const = 0;
};

class AbstractSwFldDlg : public SfxAbstractTabDialog //add for SwFldDlg
{
public:
    virtual void				Start( BOOL bShow = TRUE ) = 0;  //this method from sfxtabdialog
    virtual void				Initialize(SfxChildWinInfo *pInfo) = 0;
    virtual void				ReInitDlg() = 0;
    virtual void                ActivateDatabasePage() = 0;
    virtual void				ShowPage( USHORT nId ) = 0;// this method from SfxTabDialog
    virtual Window *			GetWindow() = 0; //this method is added for return a Window type pointer
};

class AbstractSwRenameXNamedDlg : public VclAbstractDialog //add for SwRenameXNamedDlg
{
public:
    virtual void	SetForbiddenChars( const String& rSet ) = 0;
    virtual void SetAlternativeAccess(
             STAR_REFERENCE( container::XNameAccess ) & xSecond,
             STAR_REFERENCE( container::XNameAccess ) & xThird ) = 0;
};

class AbstractSwModalRedlineAcceptDlg : public VclAbstractDialog //add for SwModalRedlineAcceptDlg
{
public:
    virtual void			AcceptAll( BOOL bAccept ) = 0;
};

class AbstractMarkFloatDlg : public VclAbstractDialog     //add for SwIndexMarkFloatDlg & SwAuthMarkFloatDlg
{
public:
    virtual void    ReInitDlg(SwWrtShell& rWrtShell) = 0;
    virtual Window *	GetWindow() = 0; //this method is added for return a Window type pointer
};

#define RET_LOAD_DOC            100
#define RET_EDIT_DOC            101
#define RET_EDIT_RESULT_DOC     102
#define RET_TARGET_CREATED      103
#define RET_REMOVE_TARGET       104

class SwView;
class SwMailMergeConfigItem;

class AbstractMailMergeWizard : public VclAbstractDialog2
{
public:
    virtual void                SetReloadDocument(const String& rURL) = 0;
    virtual const String&       GetReloadDocument() const = 0;
    virtual BOOL                ShowPage( USHORT nLevel ) = 0;
    virtual sal_uInt16          GetRestartPage() const = 0;
};


//-------Swabstract fractory
class SwAbstractDialogFactory
{
public:
    static SwAbstractDialogFactory* 	Create();

    virtual SfxAbstractDialog*				CreateSfxDialog( Window* pParent, //add for SvxMeasureDialog & SvxConnectionDialog
                                                                        const SfxItemSet& rAttr,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& _rxFrame,
                                                                        sal_uInt32 nResId
                                                                        ) = 0;
    virtual AbstractSwWordCountDialog* CreateSwWordCountDialog( Window* pWindow ) = 0;
    virtual AbstractSwInsertAbstractDlg * CreateSwInsertAbstractDlg ( Window* pParent, int nResId) = 0; // add for SwInsertAbstractDlg
    virtual AbstractSwAsciiFilterDlg*  CreateSwAsciiFilterDlg ( Window* pParent, SwDocShell& rDocSh,
                                                                SvStream* pStream, int nResId ) = 0;// add for SwAsciiFilterDlg
    virtual VclAbstractDialog * CreateSwInsertBookmarkDlg( Window *pParent, SwWrtShell &rSh, SfxRequest& rReq, int nResId ) = 0;// add for SwInsertBookmarkDlg

    virtual	AbstractSwBreakDlg * CreateSwBreakDlg( Window *pParent, SwWrtShell &rSh,int nResId ) = 0; // add for SwBreakDlg
    virtual VclAbstractDialog	* CreateSwChangeDBDlg( SwView& rVw, int nResId ) = 0; //add for SwChangeDBDlg
    virtual SfxAbstractTabDialog *  CreateSwCharDlg( Window* pParent, SwView& pVw, const SfxItemSet& rCoreSet, int nResId, // add for SwCharDlg
                                                const String* pFmtStr = 0, BOOL bIsDrwTxtDlg = FALSE) = 0;
    virtual AbstractSwConvertTableDlg* CreateSwConvertTableDlg ( SwView& rView , int nResId, bool bToTable) = 0; //add for SwConvertTableDlg
    virtual VclAbstractDialog * CreateSwCaptionDialog ( Window *pParent, SwView &rV,int nResId) = 0; //add for SwCaptionDialog

    virtual AbstractSwInsertDBColAutoPilot* CreateSwInsertDBColAutoPilot( SwView& rView, // add for SwInsertDBColAutoPilot
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource> rxSource,
        com::sun::star::uno::Reference<com::sun::star::sdbcx::XColumnsSupplier> xColSupp,
        const SwDBData& rData,  int nResId) = 0;
    virtual SfxAbstractTabDialog * CreateSwFootNoteOptionDlg( Window *pParent, SwWrtShell &rSh,int nResId) = 0;//add for  SwFootNoteOptionDlg

    virtual AbstractDropDownFieldDialog * CreateDropDownFieldDialog ( Window *pParent, SwWrtShell &rSh, //add for DropDownFieldDialog
                                SwField* pField,int nResId, BOOL bNextButton = FALSE ) = 0;
    virtual SfxAbstractTabDialog* CreateSwEnvDlg ( Window* pParent, const SfxItemSet& rSet, SwWrtShell* pWrtSh, Printer* pPrt, BOOL bInsert,int nResId ) = 0; //add for SwEnvDlg


    virtual AbstarctSwLabDlg* CreateSwLabDlg ( Window* pParent, const SfxItemSet& rSet, //add for SwLabDlg
                                                     SwNewDBMgr* pNewDBMgr, BOOL bLabel,int nResId  ) = 0;

    virtual SwLabDlgMethod GetSwLabDlgStaticMethod () =0;//add for SwLabDlg

    virtual SfxAbstractTabDialog* CreateSwParaDlg ( Window *pParent, //add for SwParaDlg
                                                    SwView& rVw,
                                                    const SfxItemSet& rCoreSet,
                                                    BYTE nDialogMode,
                                                    int nResId,
                                                    const String *pCollName = 0,
                                                    BOOL bDraw = FALSE,
                                                    UINT16 nDefPage = 0) = 0;

    virtual AbstarctSwSelGlossaryDlg * CreateSwSelGlossaryDlg ( Window * pParent, const String &rShortName, int nResId ) = 0; //add for SwSelGlossaryDlg

    virtual VclAbstractDialog * CreateVclAbstractDialog ( Window * pParent, SwWrtShell &rSh, int nResId ) = 0; //add for  SwTableHeightDlg SwSortDlg 
    virtual AbstractSplitTableDialog * CreateSplitTblDialog ( Window * pParent, SwWrtShell &rSh ) = 0; //add for  SwSplitTblDlg

    virtual AbstractSwAutoFormatDlg * CreateSwAutoFormatDlg( Window* pParent, SwWrtShell* pShell, //add for SwAutoFormatDlg
                                                            int nResId,
                                                            BOOL bSetAutoFmt = TRUE,
                                                            const SwTableAutoFmt* pSelFmt = 0 ) = 0;
    virtual SfxAbstractDialog * CreateSwBorderDlg ( Window* pParent, SfxItemSet& rSet, USHORT nType, int nResId ) = 0;//add for SwBorderDlg
    virtual SfxAbstractDialog * CreateSwWrapDlg ( Window* pParent, SfxItemSet& rSet, SwWrtShell* pSh, BOOL bDrawMode, int nResId ) = 0; //add for SwWrapDlg

    virtual VclAbstractDialog * CreateSwTableWidthDlg ( Window *pParent, SwTableFUNC &rFnc , int nResId ) = 0; //add for SwTableWidthDlg
    virtual SfxAbstractTabDialog* CreateSwTableTabDlg( Window* pParent, SfxItemPool& Pool,
                                                        const SfxItemSet* pItemSet, SwWrtShell* pSh,int nResId ) = 0; //add for SwTableTabDlg

    virtual AbstractSwFldDlg * CreateSwFldDlg ( SfxBindings* pB, SwChildWinWrapper* pCW, Window *pParent, int nResId ) = 0; //add for SwFldDlg
    virtual SfxAbstractDialog*	 CreateSwFldEditDlg ( SwView& rVw, int nResId ) = 0; //add for SwFldEditDlg
    virtual AbstractSwRenameXNamedDlg * CreateSwRenameXNamedDlg( Window* pParent, //add for SwRenameXNamedDlg
                                                                STAR_REFERENCE( container::XNamed ) & xNamed,
                                                                STAR_REFERENCE( container::XNameAccess ) & xNameAccess, int nResId ) = 0;
    virtual AbstractSwModalRedlineAcceptDlg * CreateSwModalRedlineAcceptDlg ( Window *pParent, int nResId ) = 0; //add for SwModalRedlineAcceptDlg

    virtual VclAbstractDialog*          CreateSwVclDialog( int nResId, Window* pParent, BOOL& rWithPrev ) = 0; //add for SwMergeTblDlg
    virtual SfxAbstractTabDialog*       CreateFrmTabDialog( int nResId,
                                                SfxViewFrame *pFrame, Window *pParent,
                                                const SfxItemSet& rCoreSet,
                                                BOOL			bNewFrm  = TRUE,
                                                USHORT			nResType = DLG_FRM_STD,
                                                BOOL			bFmt 	 = FALSE,
                                                UINT16			nDefPage = 0,
                                                const String* 	pFmtStr  = 0) = 0;  //add for SwFrmDlg
    virtual SfxAbstractTabDialog*		CreateTemplateDialog( int nResId,
                                                Window*             pParent,
                                                SfxStyleSheetBase&  rBase,
                                                USHORT              nRegion,
                                                BOOL 				bColumn = FALSE,
                                                SwWrtShell* 		pActShell = 0,
                                                BOOL 				bNew = FALSE ) = 0; //add for SwTemplateDlg
    virtual AbstractGlossaryDlg*		CreateGlossaryDlg( int nResId,
                                                SfxViewFrame* pViewFrame,
                                                SwGlossaryHdl* pGlosHdl,
                                                SwWrtShell *pWrtShell) = 0; //add for SwGlossaryDlg
    virtual AbstractFldInputDlg*		CreateFldInputDlg( int nResId,
                                                Window *pParent, SwWrtShell &rSh,
                                                SwField* pField, BOOL bNextButton = FALSE ) = 0; //add for SwFldInputDlg
    virtual AbstractInsFootNoteDlg*		CreateInsFootNoteDlg( int nResId,
                                                Window * pParent, SwWrtShell &rSh, BOOL bEd = FALSE) = 0; //add for SwInsFootNoteDlg
    virtual VclAbstractDialog *			CreateVclSwViewDialog( int nResId,
                                                SwView& rView, BOOL bCol = FALSE ) = 0; //add for SwInsRowColDlg, SwLineNumberingDlg
    virtual AbstractInsertGrfRulerDlg*		CreateInsertGrfRulerDlg( int nResId,
                                                Window * pParent ) = 0; //add for SwInsertGrfRulerDlg
    virtual AbstractInsTableDlg*		CreateInsTableDlg( int nResId,
                                                SwView& rView ) = 0; //add for SwInsTableDlg
    virtual AbstractJavaEditDialog*		CreateJavaEditDialog( int nResId,
                                                Window* pParent, SwWrtShell* pWrtSh ) = 0; //add for SwJavaEditDialog
    virtual AbstractMailMergeDlg*		CreateMailMergeDlg( int nResId,
                                                Window* pParent, SwWrtShell& rSh,
                                                 const String& rSourceName,
                                                const String& rTblName,
                                                sal_Int32 nCommandType,
                                                const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>& xConnection,
                                                ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >* pSelection = 0 ) = 0; //add for SwMailMergeDlg
    virtual AbstractMailMergeCreateFromDlg*		CreateMailMergeCreateFromDlg( int nResId,
                                                    Window* pParent ) = 0; //add for SwMailMergeCreateFromDlg
    virtual AbstractMailMergeFieldConnectionsDlg*		CreateMailMergeFieldConnectionsDlg( int nResId,
                                                            Window* pParent ) = 0; //add for SwMailMergeFieldConnectionsDlg
    virtual VclAbstractDialog* 			CreateMultiTOXMarkDlg( int nResId,
                                                Window* pParent, SwTOXMgr &rTOXMgr ) = 0; //add for SwMultiTOXMarkDlg
    virtual SfxAbstractTabDialog*		CreateSwTabDialog( int nResId,
                                                Window* pParent,
                                                const SfxItemSet* pSwItemSet,
                                                SwWrtShell &) = 0; //add for SwSvxNumBulletTabDialog, SwOutlineTabDialog
    virtual AbstractMultiTOXTabDialog*		CreateMultiTOXTabDialog( int nResId,
                                                Window* pParent, const SfxItemSet& rSet,
                                                SwWrtShell &rShell,
                                                SwTOXBase* pCurTOX, USHORT nToxType = USHRT_MAX,
                                                BOOL bGlobal = FALSE) = 0; //add for SwMultiTOXTabDialog
    virtual AbstractEditRegionDlg*		CreateEditRegionDlg( int nResId,
                                                Window* pParent, SwWrtShell& rWrtSh ) = 0; //add for SwEditRegionDlg
    virtual AbstractInsertSectionTabDialog*		CreateInsertSectionTabDialog( int nResId,
                                                    Window* pParent, const SfxItemSet& rSet, SwWrtShell& rSh ) = 0; //add for SwInsertSectionTabDialog
    virtual AbstractMarkFloatDlg*		CreateIndexMarkFloatDlg( int nResId,
                                                    SfxBindings* pBindings,
                                                       SfxChildWindow* pChild,
                                                       Window *pParent,
                                                    SfxChildWinInfo* pInfo,
                                                       sal_Bool bNew=sal_True) = 0; //add for SwIndexMarkFloatDlg
    virtual AbstractMarkFloatDlg*		CreateAuthMarkFloatDlg( int nResId,
                                                    SfxBindings* pBindings,
                                                       SfxChildWindow* pChild,
                                                       Window *pParent,
                                                    SfxChildWinInfo* pInfo,
                                                       sal_Bool bNew=sal_True) = 0; //add for SwAuthMarkFloatDlg
    virtual VclAbstractDialog *			CreateIndexMarkModalDlg( int nResId,
                                                Window *pParent, SwWrtShell& rSh, SwTOXMark* pCurTOXMark ) = 0; //add for SwIndexMarkModalDlg

    virtual AbstractMailMergeWizard*    CreateMailMergeWizard(SwView& rView, SwMailMergeConfigItem& rConfigItem) = 0;

    //add for static func in SwGlossaryDlg
    virtual GlossaryGetCurrGroup		GetGlossaryCurrGroupFunc( USHORT nId ) = 0;
    virtual GlossarySetActGroup			SetGlossaryActGroupFunc( USHORT nId ) = 0;

    // for tabpage
    virtual CreateTabPage				GetTabPageCreatorFunc( USHORT nId ) = 0;
    virtual GetTabPageRanges			GetTabPageRangesFunc( USHORT nId ) = 0;
};
#endif

