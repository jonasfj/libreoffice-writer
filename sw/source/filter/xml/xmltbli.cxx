/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmltbli.cxx,v $
 *
 *  $Revision: 1.58 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-18 14:29:48 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#include "hintids.hxx"

#include <limits.h>

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTTABLE_HPP_
#include <com/sun/star/text/XTextTable.hpp>
#endif

#ifndef _COM_SUN_STAR_TABLE_XCELLRANGE_HPP_
#include <com/sun/star/table/XCellRange.hpp>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif

#ifndef _XMLOFF_XMLTKMAP_HXX
#include <xmloff/xmltkmap.hxx>
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif

#ifndef _XMLOFF_FAMILIES_HXX
#include <xmloff/families.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _XMLOFF_I18NMAP_HXX
#include <xmloff/i18nmap.hxx>
#endif

#ifndef _SVX_PROTITEM_HXX
#include <svx/protitem.hxx>
#endif

#ifndef _POOLFMT_HXX
#include "poolfmt.hxx"
#endif
#ifndef _FMTFSIZE_HXX
#include "fmtfsize.hxx"
#endif
#ifndef _FMTORNT_HXX
#include "fmtornt.hxx"
#endif
#ifndef _FMTFORDR_HXX
#include "fmtfordr.hxx"
#endif
#ifndef _DOC_HXX
#include "doc.hxx"
#endif
#ifndef _SWTABLE_HXX
#include "swtable.hxx"
#endif
#ifndef _SWTBLFMT_HXX
#include "swtblfmt.hxx"
#endif
#ifndef _PAM_HXX
#include "pam.hxx"
#endif
#ifndef _UNOTBL_HXX
#include "unotbl.hxx"
#endif
#ifndef _UNOCRSR_HXX
#include "unocrsr.hxx"
#endif
#ifndef _CELLATR_HXX
#include "cellatr.hxx"
#endif
#ifndef _SWDDETBL_HXX
#include "swddetbl.hxx"
#endif
#ifndef _DDEFLD_HXX
#include "ddefld.hxx"
#endif

#ifndef _LINKMGR_HXX
#include <sfx2/linkmgr.hxx>  // for cTokenSeparator
#endif

#ifndef _XMLIMP_HXX
#include "xmlimp.hxx"
#endif
#ifndef _XMLTBLI_HXX
#include "xmltbli.hxx"
#endif

// for locking SolarMutex: svapp + mutex
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#ifndef _NDTXT_HXX
#include "ndtxt.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::table;
using namespace ::com::sun::star::xml::sax;
using namespace ::xmloff::token;
using ::std::hash_map;

enum SwXMLTableElemTokens
{
    XML_TOK_TABLE_HEADER_COLS,
    XML_TOK_TABLE_COLS,
    XML_TOK_TABLE_COL,
    XML_TOK_TABLE_HEADER_ROWS,
    XML_TOK_TABLE_ROWS,
    XML_TOK_TABLE_ROW,
    XML_TOK_OFFICE_DDE_SOURCE,
    XML_TOK_TABLE_ELEM_END=XML_TOK_UNKNOWN
};

enum SwXMLTableCellAttrTokens
{
    XML_TOK_TABLE_STYLE_NAME,
    XML_TOK_TABLE_NUM_COLS_SPANNED,
    XML_TOK_TABLE_NUM_ROWS_SPANNED,
    XML_TOK_TABLE_NUM_COLS_REPEATED,
    XML_TOK_TABLE_FORMULA,
    XML_TOK_TABLE_VALUE,
    XML_TOK_TABLE_TIME_VALUE,
    XML_TOK_TABLE_DATE_VALUE,
    XML_TOK_TABLE_BOOLEAN_VALUE,
    XML_TOK_TABLE_PROTECTED,
    XML_TOK_TABLE_CELL_ATTR_END=XML_TOK_UNKNOWN
};

static __FAR_DATA SvXMLTokenMapEntry aTableElemTokenMap[] =
{
    { XML_NAMESPACE_TABLE, XML_TABLE_HEADER_COLUMNS,
            XML_TOK_TABLE_HEADER_COLS },
    { XML_NAMESPACE_TABLE, XML_TABLE_COLUMNS,			XML_TOK_TABLE_COLS },
    { XML_NAMESPACE_TABLE, XML_TABLE_COLUMN,			XML_TOK_TABLE_COL },
    { XML_NAMESPACE_TABLE, XML_TABLE_HEADER_ROWS,
            XML_TOK_TABLE_HEADER_ROWS },
    { XML_NAMESPACE_TABLE, XML_TABLE_ROWS,				XML_TOK_TABLE_ROWS },
    { XML_NAMESPACE_TABLE, XML_TABLE_ROW,				XML_TOK_TABLE_ROW },
    { XML_NAMESPACE_OFFICE, XML_DDE_SOURCE,
            XML_TOK_OFFICE_DDE_SOURCE },

    // There are slight differences between <table:table-columns> and
    // <table:table-columns-groups>. However, none of these are
    // supported in Writer (they are Calc-only features), so we
    // support column groups by simply using the <table:table-columns>
    // token for column groups, too.
    { XML_NAMESPACE_TABLE, XML_TABLE_COLUMN_GROUP,		XML_TOK_TABLE_COLS },

    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aTableCellAttrTokenMap[] =
{
    { XML_NAMESPACE_TABLE, XML_STYLE_NAME, XML_TOK_TABLE_STYLE_NAME },
    { XML_NAMESPACE_TABLE, XML_NUMBER_COLUMNS_SPANNED, XML_TOK_TABLE_NUM_COLS_SPANNED },
    { XML_NAMESPACE_TABLE, XML_NUMBER_ROWS_SPANNED, XML_TOK_TABLE_NUM_ROWS_SPANNED },
    { XML_NAMESPACE_TABLE, XML_NUMBER_COLUMNS_REPEATED, XML_TOK_TABLE_NUM_COLS_REPEATED },
    { XML_NAMESPACE_TABLE, XML_FORMULA, XML_TOK_TABLE_FORMULA },
    { XML_NAMESPACE_OFFICE, XML_VALUE, XML_TOK_TABLE_VALUE },
    { XML_NAMESPACE_OFFICE, XML_TIME_VALUE, XML_TOK_TABLE_TIME_VALUE },
    { XML_NAMESPACE_OFFICE, XML_DATE_VALUE, XML_TOK_TABLE_DATE_VALUE },
    { XML_NAMESPACE_OFFICE, XML_BOOLEAN_VALUE, XML_TOK_TABLE_BOOLEAN_VALUE },
    { XML_NAMESPACE_TABLE, XML_PROTECTED, XML_TOK_TABLE_PROTECTED },
    { XML_NAMESPACE_TABLE, XML_PROTECT, XML_TOK_TABLE_PROTECTED }, // for backwards compatibility with SRC629 (and before)

    XML_TOKEN_MAP_END
};

const SvXMLTokenMap& SwXMLImport::GetTableElemTokenMap()
{
    if( !pTableElemTokenMap )
        pTableElemTokenMap = new SvXMLTokenMap( aTableElemTokenMap );

    return *pTableElemTokenMap;
}

const SvXMLTokenMap& SwXMLImport::GetTableCellAttrTokenMap()
{
    if( !pTableCellAttrTokenMap )
        pTableCellAttrTokenMap = new SvXMLTokenMap( aTableCellAttrTokenMap );

    return *pTableCellAttrTokenMap;
}

// ---------------------------------------------------------------------

class SwXMLTableCell_Impl
{
    OUString aStyleName;

    OUString sFormula;	// cell formula; valid if length > 0
    double dValue;		// formula value

    SvXMLImportContextRef	xSubTable;

    const SwStartNode *pStartNode;
    sal_uInt32 nRowSpan;
    sal_uInt32 nColSpan;

    sal_Bool bProtected : 1;
    sal_Bool bHasValue;	// determines whether dValue attribute is valid
    sal_Bool mbCovered;

public:

    SwXMLTableCell_Impl( sal_uInt32 nRSpan=1UL, sal_uInt32 nCSpan=1UL ) :
        nRowSpan( nRSpan ),
        nColSpan( nCSpan ),
        pStartNode( 0 ),
        bProtected( sal_False ),
        mbCovered( sal_False )
        {}

    inline void Set( const OUString& rStyleName,
                      sal_uInt32 nRSpan, sal_uInt32 nCSpan,
                     const SwStartNode *pStNd, SwXMLTableContext *pTable,
                     sal_Bool bProtect = sal_False,
                     const OUString* pFormula = NULL,
                     sal_Bool bHasValue = sal_False,
                     sal_Bool mbCovered = sal_False,
                     double dVal = 0.0 );

    sal_Bool IsUsed() const { return pStartNode!=0 ||
                                     xSubTable.Is() || bProtected;}

    sal_uInt32 GetRowSpan() const { return nRowSpan; }
    void SetRowSpan( sal_uInt32 nSet ) { nRowSpan = nSet; }
    sal_uInt32 GetColSpan() const { return nColSpan; }
    const OUString& GetStyleName() const { return aStyleName; }
    const OUString& GetFormula() const { return sFormula; }
    double GetValue() const { return dValue; }
    sal_Bool HasValue() const { return bHasValue; }
    sal_Bool IsProtected() const { return bProtected; }
    sal_Bool IsCovered() const { return mbCovered; }

    const SwStartNode *GetStartNode() const { return pStartNode; }
    inline void SetStartNode( const SwStartNode *pSttNd );

    inline SwXMLTableContext *GetSubTable() const;

    inline void Dispose();
};

inline void SwXMLTableCell_Impl::Set( const OUString& rStyleName,
                                      sal_uInt32 nRSpan, sal_uInt32 nCSpan,
                                      const SwStartNode *pStNd,
                                      SwXMLTableContext *pTable,
                                      sal_Bool bProtect,
                                      const OUString* pFormula,
                                      sal_Bool bHasVal,
                                      sal_Bool bCov,
                                      double dVal)
{
    aStyleName = rStyleName;
    nRowSpan = nRSpan;
    nColSpan = nCSpan;
    pStartNode = pStNd;
    xSubTable = pTable;
    dValue = dVal;
    bHasValue = bHasVal;
    mbCovered = bCov;
    bProtected = bProtect;

    // set formula, if valid
    if (pFormula != NULL)
    {
        sFormula = *pFormula;
    }
}

inline void SwXMLTableCell_Impl::SetStartNode( const SwStartNode *pSttNd )
{
    pStartNode = pSttNd;
    xSubTable = 0;
}

inline SwXMLTableContext *SwXMLTableCell_Impl::GetSubTable() const
{
    return (SwXMLTableContext *)&xSubTable;
}

inline void SwXMLTableCell_Impl::Dispose()
{
    if( xSubTable.Is() )
        xSubTable = 0;
}

// ---------------------------------------------------------------------

typedef SwXMLTableCell_Impl* SwXMLTableCellPtr;
SV_DECL_PTRARR_DEL(SwXMLTableCells_Impl,SwXMLTableCellPtr,5,5)
SV_IMPL_PTRARR(SwXMLTableCells_Impl,SwXMLTableCellPtr)

class SwXMLTableRow_Impl
{
    OUString aStyleName;
    OUString aDfltCellStyleName;

    SwXMLTableCells_Impl aCells;

    sal_Bool bSplitable;

public:

    SwXMLTableRow_Impl( const OUString& rStyleName, sal_uInt32 nCells,
                         const OUString *pDfltCellStyleName = 0 );
    ~SwXMLTableRow_Impl() {}

    inline SwXMLTableCell_Impl *GetCell( sal_uInt32 nCol ) const;

    inline void Set( const OUString& rStyleName,
                     const OUString& rDfltCellStyleName );

    void Expand( sal_uInt32 nCells, sal_Bool bOneCell );

    void SetSplitable( sal_Bool bSet ) { bSplitable = bSet; }
    sal_Bool IsSplitable() const { return bSplitable; }

    const OUString& GetStyleName() const { return aStyleName; }
    const OUString& GetDefaultCellStyleName() const { return aDfltCellStyleName; }

    void Dispose();
};

SwXMLTableRow_Impl::SwXMLTableRow_Impl( const OUString& rStyleName,
                                        sal_uInt32 nCells,
                                        const OUString *pDfltCellStyleName ) :
    aStyleName( rStyleName ),
    bSplitable( sal_False )
{
    if( pDfltCellStyleName  )
        aDfltCellStyleName = *pDfltCellStyleName;
    ASSERT( nCells <= USHRT_MAX,
            "SwXMLTableRow_Impl::SwXMLTableRow_Impl: too many cells" );
    if( nCells > USHRT_MAX )
        nCells = USHRT_MAX;

    for( sal_uInt16 i=0U; i<nCells; i++ )
    {
        aCells.Insert( new SwXMLTableCell_Impl, aCells.Count() );
    }
}

inline SwXMLTableCell_Impl *SwXMLTableRow_Impl::GetCell( sal_uInt32 nCol ) const
{
    ASSERT( nCol < USHRT_MAX,
            "SwXMLTableRow_Impl::GetCell: column number is to big" );
    return aCells[(sal_uInt16)nCol];
}

void SwXMLTableRow_Impl::Expand( sal_uInt32 nCells, sal_Bool bOneCell )
{
    ASSERT( nCells <= USHRT_MAX,
            "SwXMLTableRow_Impl::Expand: too many cells" );
    if( nCells > USHRT_MAX )
        nCells = USHRT_MAX;

    sal_uInt32 nColSpan = nCells - aCells.Count();
    for( sal_uInt16 i=aCells.Count(); i<nCells; i++ )
    {
        aCells.Insert( new SwXMLTableCell_Impl( 1UL,
                                                bOneCell ? nColSpan : 1UL ),
                       aCells.Count() );
        nColSpan--;
    }

    ASSERT( nCells<=aCells.Count(),
            "SwXMLTableRow_Impl::Expand: wrong number of cells" );
}

inline void SwXMLTableRow_Impl::Set( const OUString& rStyleName,
                                       const OUString& rDfltCellStyleName )
{
    aStyleName = rStyleName;
    aDfltCellStyleName = rDfltCellStyleName;
}

void SwXMLTableRow_Impl::Dispose()
{
    for( sal_uInt16 i=0; i < aCells.Count(); i++ )
        aCells[i]->Dispose();
}

// ---------------------------------------------------------------------

class SwXMLTableCellContext_Impl : public SvXMLImportContext
{
    OUString aStyleName;
    OUString sFormula;
    OUString sSaveParaDefault;

    SvXMLImportContextRef	xMyTable;

    double fValue;
    sal_Bool bHasValue;
    sal_Bool bProtect;

    sal_uInt32 					nRowSpan;
    sal_uInt32 					nColSpan;
    sal_uInt32 					nColRepeat;

    sal_Bool					bHasTextContent : 1;
    sal_Bool					bHasTableContent : 1;

    SwXMLTableContext *GetTable() { return (SwXMLTableContext *)&xMyTable; }

    sal_Bool HasContent() const { return bHasTextContent || bHasTableContent; }
    inline void _InsertContent();
    inline void InsertContent();
    inline void InsertContentIfNotThere();
    inline void InsertContent( SwXMLTableContext *pTable );

public:

    SwXMLTableCellContext_Impl(
            SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
            const Reference< xml::sax::XAttributeList > & xAttrList,
            SwXMLTableContext *pTable );

    virtual ~SwXMLTableCellContext_Impl();

    virtual SvXMLImportContext *CreateChildContext(
            sal_uInt16 nPrefix, const OUString& rLocalName,
            const Reference< xml::sax::XAttributeList > & xAttrList );
    virtual void EndElement();

    SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }
};

SwXMLTableCellContext_Impl::SwXMLTableCellContext_Impl(
        SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
        const Reference< xml::sax::XAttributeList > & xAttrList,
        SwXMLTableContext *pTable ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    xMyTable( pTable ),
    nRowSpan( 1UL ),
    nColSpan( 1UL ),
    nColRepeat( 1UL ),
    bHasTextContent( sal_False ),
    bHasTableContent( sal_False ),
    sFormula(),
    fValue( 0.0 ),
    bHasValue( sal_False ),
    bProtect( sal_False )
{
    sSaveParaDefault = GetImport().GetTextImport()->sCellParaStyleDefault;
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );

        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        const OUString& rValue = xAttrList->getValueByIndex( i );
        const SvXMLTokenMap& rTokenMap =
            GetSwImport().GetTableCellAttrTokenMap();
        switch( rTokenMap.Get( nPrefix, aLocalName ) )
        {
        case XML_TOK_TABLE_STYLE_NAME:
            aStyleName = rValue;
            GetImport().GetTextImport()->sCellParaStyleDefault = rValue;
            break;
        case XML_TOK_TABLE_NUM_COLS_SPANNED:
            nColSpan = (sal_uInt32)rValue.toInt32();
            if( nColSpan < 1UL )
                nColSpan = 1UL;
            break;
        case XML_TOK_TABLE_NUM_ROWS_SPANNED:
            nRowSpan = (sal_uInt32)rValue.toInt32();
            if( nRowSpan < 1UL )
                nRowSpan = 1UL;
            break;
        case XML_TOK_TABLE_NUM_COLS_REPEATED:
            nColRepeat = (sal_uInt32)rValue.toInt32();
            if( nColRepeat < 1UL )
                nColRepeat = 1UL;
            break;
        case XML_TOK_TABLE_FORMULA:
            {
                OUString sTmp;
                sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
                        _GetKeyByAttrName( rValue, &sTmp, sal_False );
                sFormula = XML_NAMESPACE_OOOW == nPrefix ? sTmp : rValue;
            }
            break;
        case XML_TOK_TABLE_VALUE:
            {
                double fTmp;
                if (SvXMLUnitConverter::convertDouble(fTmp, rValue))
                {
                    fValue = fTmp;
                    bHasValue = sal_True;
                }
            }
            break;
        case XML_TOK_TABLE_TIME_VALUE:
            {
                double fTmp;
                if (SvXMLUnitConverter::convertTime(fTmp, rValue))
                {
                    fValue = fTmp;
                    bHasValue = sal_True;
                }
            }
            break;
        case XML_TOK_TABLE_DATE_VALUE:
            {
                double fTmp;
                if (GetImport().GetMM100UnitConverter().convertDateTime(fTmp,
                                                                      rValue))
                {
                    fValue = fTmp;
                    bHasValue = sal_True;
                }
            }
            break;
        case XML_TOK_TABLE_BOOLEAN_VALUE:
            {
                sal_Bool bTmp;
                if (SvXMLUnitConverter::convertBool(bTmp, rValue))
                {
                    fValue = (bTmp ? 1.0 : 0.0);
                    bHasValue = sal_True;
                }
            }
            break;
        case XML_TOK_TABLE_PROTECTED:
            {
                sal_Bool bTmp;
                if (SvXMLUnitConverter::convertBool(bTmp, rValue))
                {
                    bProtect = bTmp;
                }
            }
            break;
        }
    }
}

SwXMLTableCellContext_Impl::~SwXMLTableCellContext_Impl()
{
}

inline void SwXMLTableCellContext_Impl::_InsertContent()
{
    GetTable()->InsertCell( aStyleName, nRowSpan, nColSpan,
                            GetTable()->InsertTableSection(),
                            NULL, bProtect, &sFormula, bHasValue, fValue);
}

inline void SwXMLTableCellContext_Impl::InsertContent()
{
    ASSERT( !HasContent(), "content already there" );
    _InsertContent();
    bHasTextContent = sal_True;
}

inline void SwXMLTableCellContext_Impl::InsertContentIfNotThere()
{
    if( !HasContent() )
        InsertContent();
}

inline void SwXMLTableCellContext_Impl::InsertContent(
                                                SwXMLTableContext *pTable )
{
    GetTable()->InsertCell( aStyleName, nRowSpan, nColSpan, 0, pTable, bProtect );
    bHasTableContent = sal_True;
}

SvXMLImportContext *SwXMLTableCellContext_Impl::CreateChildContext(
        sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    sal_Bool bSubTable = sal_False;
    if( XML_NAMESPACE_TABLE == nPrefix &&
        IsXMLToken( rLocalName, XML_TABLE ) )
    {
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
        for( sal_Int16 i=0; i < nAttrCount; i++ )
        {
            const OUString& rAttrName = xAttrList->getNameByIndex( i );

            OUString aLocalName;
            sal_uInt16 nPrefix =
                GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                                &aLocalName );
            if( XML_NAMESPACE_TABLE == nPrefix &&
                 IsXMLToken( aLocalName, XML_IS_SUB_TABLE ) &&
                 IsXMLToken( xAttrList->getValueByIndex( i ), XML_TRUE ) )
                bSubTable = sal_True;
        }
    }

    if( bSubTable )
    {
        if( !HasContent() )
        {
            SwXMLTableContext *pTblContext =
                new SwXMLTableContext( GetSwImport(), nPrefix, rLocalName,
                                       xAttrList, GetTable() );
            pContext = pTblContext;
            if( GetTable()->IsValid() )
                InsertContent( pTblContext );

            GetTable()->SetHasSubTables( sal_True );
        }
    }
    else
    {
        if( GetTable()->IsValid() )
            InsertContentIfNotThere();
        pContext = GetImport().GetTextImport()->CreateTextChildContext(
                        GetImport(), nPrefix, rLocalName, xAttrList,
                        XML_TEXT_TYPE_CELL  );
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

    return pContext;
}

void SwXMLTableCellContext_Impl::EndElement()
{
    if( GetTable()->IsValid() )
    {
        if( bHasTextContent )
        {
            GetImport().GetTextImport()->DeleteParagraph();
            if( nColRepeat > 1 && nColSpan == 1 )
            {
                // The original text is is invalid after deleting the last
                // paragraph
                Reference < XTextCursor > xSrcTxtCursor =
                    GetImport().GetTextImport()->GetText()->createTextCursor();
                xSrcTxtCursor->gotoEnd( sal_True );

                // Until we have an API for copying we have to use the core.
                Reference<XUnoTunnel> xSrcCrsrTunnel( xSrcTxtCursor, UNO_QUERY);
                ASSERT( xSrcCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
                OTextCursorHelper *pSrcTxtCrsr =
                        (OTextCursorHelper*)xSrcCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
                ASSERT( pSrcTxtCrsr, "SwXTextCursor missing" );
                SwDoc *pDoc = pSrcTxtCrsr->GetDoc();
                const SwPaM *pSrcPaM = pSrcTxtCrsr->GetPaM();

                while( nColRepeat > 1 && GetTable()->IsInsertCellPossible() )
                {
                    _InsertContent();

                    Reference<XUnoTunnel> xDstCrsrTunnel(
                        GetImport().GetTextImport()->GetCursor(), UNO_QUERY);
                    ASSERT( xDstCrsrTunnel.is(),
                            "missing XUnoTunnel for Cursor" );
                    OTextCursorHelper *pDstTxtCrsr =
                            (OTextCursorHelper*)xDstCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
                    ASSERT( pDstTxtCrsr, "SwXTextCursor missing" );
                    SwPaM aSrcPaM( *pSrcPaM->GetPoint(),
                                   *pSrcPaM->GetMark() );
                    SwPosition aDstPos( *pDstTxtCrsr->GetPaM()->GetPoint() );
                    pDoc->Copy( aSrcPaM, aDstPos );

                    nColRepeat--;
                }
            }
        }
        else if( !bHasTableContent )
        {
            InsertContent();
            if( nColRepeat > 1 && nColSpan == 1 )
            {
                while( nColRepeat > 1 && GetTable()->IsInsertCellPossible() )
                {
                    _InsertContent();
                    nColRepeat--;
                }
            }
        }
    }
    GetImport().GetTextImport()->sCellParaStyleDefault = sSaveParaDefault;
}

// ---------------------------------------------------------------------

class SwXMLTableColContext_Impl : public SvXMLImportContext
{
    SvXMLImportContextRef	xMyTable;

    SwXMLTableContext *GetTable() { return (SwXMLTableContext *)&xMyTable; }

public:

    SwXMLTableColContext_Impl(
            SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
            const Reference< xml::sax::XAttributeList > & xAttrList,
            SwXMLTableContext *pTable );

    virtual ~SwXMLTableColContext_Impl();

    SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }
};

SwXMLTableColContext_Impl::SwXMLTableColContext_Impl(
        SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
        const Reference< xml::sax::XAttributeList > & xAttrList,
        SwXMLTableContext *pTable ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    xMyTable( pTable )
{
    sal_uInt32 nColRep = 1UL;
    OUString aStyleName, aDfltCellStyleName;

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );

        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        const OUString& rValue = xAttrList->getValueByIndex( i );
        if( XML_NAMESPACE_TABLE == nPrefix )
        {
            if( IsXMLToken( aLocalName, XML_STYLE_NAME ) )
                aStyleName = rValue;
            else if( IsXMLToken( aLocalName, XML_NUMBER_COLUMNS_REPEATED ) )
                nColRep = (sal_uInt32)rValue.toInt32();
            else if( IsXMLToken( aLocalName, XML_DEFAULT_CELL_STYLE_NAME ) )
                aDfltCellStyleName = rValue;
        }
    }

    sal_Int32 nWidth = MINLAY;
    sal_Bool bRelWidth = sal_True;
    if( aStyleName.getLength() )
    {
        const SfxPoolItem *pItem;
        const SfxItemSet *pAutoItemSet = 0;
        if( GetSwImport().FindAutomaticStyle(
                    XML_STYLE_FAMILY_TABLE_COLUMN,
                                              aStyleName, &pAutoItemSet ) &&
            pAutoItemSet &&
            SFX_ITEM_SET == pAutoItemSet->GetItemState( RES_FRM_SIZE, sal_False,
                                                        &pItem ) )
        {
            const SwFmtFrmSize *pSize = ((const SwFmtFrmSize *)pItem);
            nWidth = pSize->GetWidth();
            bRelWidth = ATT_VAR_SIZE == pSize->GetHeightSizeType();
        }
    }

    if( nWidth )
    {
        while( nColRep-- && GetTable()->IsInsertColPossible() )
            GetTable()->InsertColumn( nWidth, bRelWidth, &aDfltCellStyleName );
    }
}

SwXMLTableColContext_Impl::~SwXMLTableColContext_Impl()
{
}

// ---------------------------------------------------------------------

class SwXMLTableColsContext_Impl : public SvXMLImportContext
{
    SvXMLImportContextRef	xMyTable;
    sal_Bool bHeader;

    SwXMLTableContext *GetTable() { return (SwXMLTableContext *)&xMyTable; }

public:

    SwXMLTableColsContext_Impl(
            SwXMLImport& rImport, sal_uInt16 nPrfx,
            const OUString& rLName,
            const Reference< xml::sax::XAttributeList > & xAttrList,
            SwXMLTableContext *pTable,
            sal_Bool bHead );

    virtual ~SwXMLTableColsContext_Impl();

    virtual SvXMLImportContext *CreateChildContext(
            sal_uInt16 nPrefix, const OUString& rLocalName,
            const Reference< xml::sax::XAttributeList > & xAttrList );

    SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }
};

SwXMLTableColsContext_Impl::SwXMLTableColsContext_Impl(
        SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
        const Reference< xml::sax::XAttributeList > &,
        SwXMLTableContext *pTable, sal_Bool bHead ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    xMyTable( pTable ),
    bHeader( bHead )
{
}

SwXMLTableColsContext_Impl::~SwXMLTableColsContext_Impl()
{
}

SvXMLImportContext *SwXMLTableColsContext_Impl::CreateChildContext(
        sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if( XML_NAMESPACE_TABLE == nPrefix &&
        IsXMLToken( rLocalName, XML_TABLE_COLUMN ) &&
        GetTable()->IsInsertColPossible() )
        pContext = new SwXMLTableColContext_Impl( GetSwImport(), nPrefix,
                                                  rLocalName, xAttrList,
                                                  GetTable() );

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

    return pContext;
}

// ---------------------------------------------------------------------

class SwXMLTableRowContext_Impl : public SvXMLImportContext
{
    SvXMLImportContextRef	xMyTable;

    sal_uInt32 					nRowRepeat;

    SwXMLTableContext *GetTable() { return (SwXMLTableContext *)&xMyTable; }

public:

    SwXMLTableRowContext_Impl(
            SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
            const Reference< xml::sax::XAttributeList > & xAttrList,
            SwXMLTableContext *pTable, sal_Bool bInHead=sal_False );

    virtual ~SwXMLTableRowContext_Impl();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
            const OUString& rLocalName,
            const Reference< xml::sax::XAttributeList > & xAttrList );

    virtual void EndElement();

    SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }
};

SwXMLTableRowContext_Impl::SwXMLTableRowContext_Impl( SwXMLImport& rImport,
        sal_uInt16 nPrfx,
        const OUString& rLName,
        const Reference< xml::sax::XAttributeList > & xAttrList,
        SwXMLTableContext *pTable,
        sal_Bool bInHead ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    xMyTable( pTable ),
    nRowRepeat( 1 )
{
    OUString aStyleName, aDfltCellStyleName;

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );

        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        const OUString& rValue = xAttrList->getValueByIndex( i );
        if( XML_NAMESPACE_TABLE == nPrefix )
        {
            if( IsXMLToken( aLocalName, XML_STYLE_NAME ) )
            {
                aStyleName = rValue;
            }
            else if( IsXMLToken( aLocalName, XML_NUMBER_ROWS_REPEATED ) )
            {
                nRowRepeat = (sal_uInt32)rValue.toInt32();
                if( nRowRepeat < 1UL )
                    nRowRepeat = 1UL;
            }
            else if( IsXMLToken( aLocalName, XML_DEFAULT_CELL_STYLE_NAME ) )
            {
                aDfltCellStyleName = rValue;
            }
        }
    }
    if( GetTable()->IsValid() )
        GetTable()->InsertRow( aStyleName, aDfltCellStyleName, bInHead );
}

void SwXMLTableRowContext_Impl::EndElement()
{
    if( GetTable()->IsValid() )
    {
        GetTable()->FinishRow();

        if( nRowRepeat > 1UL )
            GetTable()->InsertRepRows( nRowRepeat );
    }
}

SwXMLTableRowContext_Impl::~SwXMLTableRowContext_Impl()
{
}

SvXMLImportContext *SwXMLTableRowContext_Impl::CreateChildContext(
        sal_uInt16 nPrefix, const OUString& rLocalName,
        const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if( XML_NAMESPACE_TABLE == nPrefix )
    {
        if( IsXMLToken( rLocalName, XML_TABLE_CELL ) )
        {
            if( !GetTable()->IsValid() || GetTable()->IsInsertCellPossible() )
                pContext = new SwXMLTableCellContext_Impl( GetSwImport(),
                                                           nPrefix,
                                                           rLocalName,
                                                           xAttrList,
                                                           GetTable() );
        }
        else if( IsXMLToken( rLocalName, XML_COVERED_TABLE_CELL ) )
            pContext = new SvXMLImportContext( GetImport(), nPrefix,
                                               rLocalName );
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

    return pContext;
}

// ---------------------------------------------------------------------

class SwXMLTableRowsContext_Impl : public SvXMLImportContext
{
    SvXMLImportContextRef	xMyTable;

    sal_Bool bHeader;

    SwXMLTableContext *GetTable() { return (SwXMLTableContext *)&xMyTable; }

public:

    SwXMLTableRowsContext_Impl( SwXMLImport& rImport, sal_uInt16 nPrfx,
            const OUString& rLName,
            const Reference< xml::sax::XAttributeList > & xAttrList,
            SwXMLTableContext *pTable,
            sal_Bool bHead );

    virtual ~SwXMLTableRowsContext_Impl();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
            const OUString& rLocalName,
            const Reference< xml::sax::XAttributeList > & xAttrList );

    SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }
};

SwXMLTableRowsContext_Impl::SwXMLTableRowsContext_Impl( SwXMLImport& rImport,
        sal_uInt16 nPrfx,
        const OUString& rLName,
        const Reference< xml::sax::XAttributeList > &,
        SwXMLTableContext *pTable,
        sal_Bool bHead ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    xMyTable( pTable ),
    bHeader( bHead )
{
}

SwXMLTableRowsContext_Impl::~SwXMLTableRowsContext_Impl()
{
}

SvXMLImportContext *SwXMLTableRowsContext_Impl::CreateChildContext(
        sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if( XML_NAMESPACE_TABLE == nPrefix &&
        IsXMLToken( rLocalName, XML_TABLE_ROW ) &&
        GetTable()->IsInsertRowPossible() )
        pContext = new SwXMLTableRowContext_Impl( GetSwImport(), nPrefix,
                                                  rLocalName, xAttrList,
                                                  GetTable(),
                                                  bHeader );

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

    return pContext;
}

// ---------------------------------------------------------------------

class SwXMLDDETableContext_Impl : public SvXMLImportContext
{
    OUString sConnectionName;
    OUString sDDEApplication;
    OUString sDDEItem;
    OUString sDDETopic;
    sal_Bool bIsAutomaticUpdate;

public:

    TYPEINFO();

    SwXMLDDETableContext_Impl(
        SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName);

    ~SwXMLDDETableContext_Impl();

    virtual void StartElement(
        const Reference<xml::sax::XAttributeList> & xAttrList);

    OUString& GetConnectionName()	{ return sConnectionName; }
    OUString& GetDDEApplication()	{ return sDDEApplication; }
    OUString& GetDDEItem() 			{ return sDDEItem; }
    OUString& GetDDETopic()			{ return sDDETopic; }
    sal_Bool GetIsAutomaticUpdate()	{ return bIsAutomaticUpdate; }
};

TYPEINIT1( SwXMLDDETableContext_Impl, SvXMLImportContext );

SwXMLDDETableContext_Impl::SwXMLDDETableContext_Impl(
    SwXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName) :
        SvXMLImportContext(rImport, nPrfx, rLName),
        sConnectionName(),
        sDDEApplication(),
        sDDEItem(),
        sDDETopic(),
        bIsAutomaticUpdate(sal_False)
{
}

SwXMLDDETableContext_Impl::~SwXMLDDETableContext_Impl()
{
}

void SwXMLDDETableContext_Impl::StartElement(
    const Reference<xml::sax::XAttributeList> & xAttrList)
{
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i = 0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );

        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        const OUString& rValue = xAttrList->getValueByIndex( i );

        if (XML_NAMESPACE_OFFICE == nPrefix)
        {
            if ( IsXMLToken( aLocalName, XML_DDE_APPLICATION ) )
            {
                sDDEApplication = rValue;
            }
            else if ( IsXMLToken( aLocalName, XML_DDE_TOPIC ) )
            {
                sDDETopic = rValue;
            }
            else if ( IsXMLToken( aLocalName, XML_DDE_ITEM ) )
            {
                sDDEItem = rValue;
            }
            else if ( IsXMLToken( aLocalName, XML_NAME ) )
            {
                sConnectionName = rValue;
            }
            else if ( IsXMLToken( aLocalName, XML_AUTOMATIC_UPDATE ) )
            {
                sal_Bool bTmp;
                if (SvXMLUnitConverter::convertBool(bTmp, rValue))
                {
                    bIsAutomaticUpdate = bTmp;
                }
            }
            // else: unknown attribute
        }
        // else: unknown attribute namespace
    }
}

// generate a new name for DDE field type (called by lcl_GetDDEFieldType below)
String lcl_GenerateFldTypeName(OUString sPrefix, SwTableNode* pTableNode)
{
    String sPrefixStr(sPrefix);

    if (sPrefixStr.Len() == 0)
    {
        sPrefixStr = String('_');
    }
// 	else if (sPrefixStr.Copy(0, 1).IsAlphaAscii())
// 	{
// 		sPrefixStr.Insert('_', 0);
// 	}
    // else: name is OK.

    // increase count until we find a name that is not yet taken
    String sName;
    sal_Int32 nCount = 0;
    do
    {
        // this is crazy, but just in case all names are taken: exit gracefully
        if (nCount < 0)
            return sName;

        nCount++;
        sName = sPrefixStr;
        sName += String::CreateFromInt32(nCount);

    }
    while (NULL != pTableNode->GetDoc()->GetFldType(RES_DDEFLD, sName, false));

    return sName;
}

// set table properties
SwDDEFieldType* lcl_GetDDEFieldType(SwXMLDDETableContext_Impl* pContext,
                                    SwTableNode* pTableNode)
{
    // make command string
    String sCommand(pContext->GetDDEApplication());
    sCommand += sfx2::cTokenSeperator;
    sCommand += String(pContext->GetDDEItem());
    sCommand += sfx2::cTokenSeperator;
    sCommand += String(pContext->GetDDETopic());

    sal_uInt16 nType = pContext->GetIsAutomaticUpdate() ? sfx2::LINKUPDATE_ALWAYS
                                                        : sfx2::LINKUPDATE_ONCALL;

    String sName(pContext->GetConnectionName());

    // field type to be returned
    SwDDEFieldType* pType = NULL;

    // valid name?
    if (sName.Len() == 0)
    {
        sName = lcl_GenerateFldTypeName(pContext->GetDDEApplication(),
                                        pTableNode);
    }
    else
    {
        // check for existing DDE field type with the same name
        SwDDEFieldType* pOldType = (SwDDEFieldType*)pTableNode->GetDoc()->GetFldType(RES_DDEFLD, sName, false);
        if (NULL != pOldType)
        {
            // same values -> return old type
            if ( (pOldType->GetCmd() == sCommand) &&
                 (pOldType->GetType() == nType) )
            {
                // same name, same values -> return old type!
                pType = pOldType;
            }
            else
            {
                // same name, different values -> think of new name
                sName = lcl_GenerateFldTypeName(pContext->GetDDEApplication(),
                                                pTableNode);
            }
        }
        // no old type -> create new one
    }

    // create new field type (unless we already have one)
    if (NULL == pType)
    {
        // create new field type and return
        SwDDEFieldType aDDEFieldType(sName, sCommand, nType);
        pType = (SwDDEFieldType*)pTableNode->
            GetDoc()->InsertFldType(aDDEFieldType);
    }

    DBG_ASSERT(NULL != pType, "We really want a SwDDEFieldType here!");
    return pType;
}


// ---------------------------------------------------------------------

class TableBoxIndex
{
public:
    OUString msName;
    sal_Int32 mnWidth;
    sal_Bool mbProtected;

    TableBoxIndex( const OUString& rName, sal_Int32 nWidth,
                   sal_Bool bProtected ) :
        msName( rName ),
        mnWidth( nWidth ),
        mbProtected( bProtected )
    { }

    bool operator== ( const TableBoxIndex& rArg ) const
    {
        return (rArg.mnWidth == mnWidth) &&
            (rArg.mbProtected == mbProtected) &&
            (rArg.msName == msName);
    }
};

class TableBoxIndexHasher
{
public:
    size_t operator() (const TableBoxIndex& rArg) const
    {
        return rArg.msName.hashCode() + rArg.mnWidth + rArg.mbProtected;
    }
};




typedef SwXMLTableRow_Impl* SwXMLTableRowPtr;
SV_DECL_PTRARR_DEL(SwXMLTableRows_Impl,SwXMLTableRowPtr,5,5)
SV_IMPL_PTRARR(SwXMLTableRows_Impl,SwXMLTableRowPtr)

SwXMLTableCell_Impl *SwXMLTableContext::GetCell( sal_uInt32 nRow,
                                                 sal_uInt32 nCol ) const
{
    return (*pRows)[(sal_uInt16)nRow]->GetCell( (sal_uInt16)nCol );
}

TYPEINIT1( SwXMLTableContext, XMLTextTableContext );

SwXMLTableContext::SwXMLTableContext( SwXMLImport& rImport,
        sal_uInt16 nPrfx,
        const OUString& rLName,
        const Reference< xml::sax::XAttributeList > & xAttrList ) :
    XMLTextTableContext( rImport, nPrfx, rLName ),
    pRows( new SwXMLTableRows_Impl ),
    pColumnDefaultCellStyleNames( 0 ),
    pTableNode( 0 ),
    pBox1( 0 ),
    pSttNd1( 0 ),
    pBoxFmt( 0 ),
    pLineFmt( 0 ),
    nCurRow( 0UL ),
    nCurCol( 0UL ),
    nWidth( 0UL ),
    bFirstSection( sal_True ),
    bRelWidth( sal_True ),
    bHasSubTables( sal_False ),
    nHeaderRows( 0 ),
    pDDESource(NULL),
    pSharedBoxFormats(NULL)
{
    OUString aName;

    // this method will modify the document directly -> lock SolarMutex
    vos::OGuard aGuard(Application::GetSolarMutex());

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );

        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        const OUString& rValue = xAttrList->getValueByIndex( i );
        if( XML_NAMESPACE_TABLE == nPrefix )
        {
            if( IsXMLToken( aLocalName, XML_STYLE_NAME ) )
                aStyleName = rValue;
            else if( IsXMLToken( aLocalName, XML_NAME ) )
                aName = rValue;
            else if( IsXMLToken( aLocalName, XML_DEFAULT_CELL_STYLE_NAME ) )
                aDfltCellStyleName = rValue;
        }
    }

    SwXMLImport& rSwImport = GetSwImport();
    Reference<XUnoTunnel> xCrsrTunnel( GetImport().GetTextImport()->GetCursor(),
                                       UNO_QUERY);
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
                (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );
    SwDoc *pDoc = pTxtCrsr->GetDoc();

    String sTblName;
    if( aName.getLength() )
    {
        const SwTableFmt *pTblFmt = pDoc->FindTblFmtByName( aName );
        if( !pTblFmt )
            sTblName = aName;
    }
    if( !sTblName.Len() )
    {
        sTblName = pDoc->GetUniqueTblName();
        GetImport().GetTextImport()
            ->GetRenameMap().Add( XML_TEXT_RENAME_TYPE_TABLE, aName, sTblName );
    }

    Reference< XTextTable > xTable;
    const SwXTextTable *pXTable = 0;
    Reference<XMultiServiceFactory> xFactory( GetImport().GetModel(),
                                              UNO_QUERY );
    ASSERT( xFactory.is(), "factory missing" );
    if( xFactory.is() )
    {
        OUString sService(
                RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.TextTable" ) );
        Reference<XInterface> xIfc = xFactory->createInstance( sService );
        ASSERT( xIfc.is(), "Couldn't create a table" );

        if( xIfc.is() )
            xTable = Reference< XTextTable > ( xIfc, UNO_QUERY );
    }

    if( xTable.is() )
    {
        xTable->initialize( 1, 1 );

        try
        {
            xTextContent = Reference< XTextContent >( xTable, UNO_QUERY );
            GetImport().GetTextImport()->InsertTextContent( xTextContent );
        }
        catch( IllegalArgumentException& )
        {
            xTable = 0;
        }
    }

    if( xTable.is() )
    {
        Reference<XUnoTunnel> xTableTunnel( xTable, UNO_QUERY);
        if( xTableTunnel.is() )
        {
            pXTable = (SwXTextTable*)xTableTunnel->getSomething(
                                            SwXTextTable::getUnoTunnelId() );
            ASSERT( pXTable, "SwXTextTable missing" );
        }

        Reference < XCellRange > xCellRange( xTable, UNO_QUERY );
        Reference < XCell > xCell = xCellRange->getCellByPosition( 0, 0 );
        Reference < XText> xText( xCell, UNO_QUERY );
        xOldCursor = GetImport().GetTextImport()->GetCursor();
        GetImport().GetTextImport()->SetCursor( xText->createTextCursor() );

        // take care of open redlines for tables
        GetImport().GetTextImport()->RedlineAdjustStartNodeCursor(sal_True);
    }
    if( pXTable )
    {
        SwFrmFmt *pTblFrmFmt = pXTable->GetFrmFmt();
        ASSERT( pTblFrmFmt, "table format missing" );
        SwTable *pTbl = SwTable::FindTable( pTblFrmFmt );
        ASSERT( pTbl, "table missing" );
        pTableNode = pTbl->GetTableNode();
        ASSERT( pTableNode, "table node missing" );

        pTblFrmFmt->SetName( sTblName );

        SwTableLine *pLine1 = pTableNode->GetTable().GetTabLines()[0U];
        pBox1 = pLine1->GetTabBoxes()[0U];
        pSttNd1 = pBox1->GetSttNd();
    }
}

SwXMLTableContext::SwXMLTableContext( SwXMLImport& rImport,
        sal_uInt16 nPrfx,
        const OUString& rLName,
        const Reference< xml::sax::XAttributeList > &,
        SwXMLTableContext *pTable ) :
    XMLTextTableContext( rImport, nPrfx, rLName ),
    xParentTable( pTable ),
    pRows( new SwXMLTableRows_Impl ),
    pColumnDefaultCellStyleNames( 0 ),
    pTableNode( pTable->pTableNode ),
    pBox1( 0 ),
    pSttNd1( 0 ),
    pBoxFmt( 0 ),
    pLineFmt( 0 ),
    nCurRow( 0UL ),
    nCurCol( 0UL ),
    nWidth( 0UL ),
    bFirstSection( sal_False ),
    bRelWidth( sal_True ),
    nHeaderRows( 0 ),
    pDDESource(NULL),
    pSharedBoxFormats(NULL)
{
}

SwXMLTableContext::~SwXMLTableContext()
{
    delete pColumnDefaultCellStyleNames;
    delete pSharedBoxFormats;
    delete pRows;

    // close redlines on table end nodes
    GetImport().GetTextImport()->RedlineAdjustStartNodeCursor(sal_False);
}

SvXMLImportContext *SwXMLTableContext::CreateChildContext( sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetSwImport().GetTableElemTokenMap();
    sal_Bool bHeader = sal_False;
    switch( rTokenMap.Get( nPrefix, rLocalName ) )
    {
    case XML_TOK_TABLE_HEADER_COLS:
        bHeader = sal_True;
    case XML_TOK_TABLE_COLS:
        if( IsValid() )
            pContext = new SwXMLTableColsContext_Impl( GetSwImport(), nPrefix,
                                                       rLocalName, xAttrList,
                                                       this, bHeader );
        break;
    case XML_TOK_TABLE_COL:
        if( IsValid() && IsInsertColPossible() )
            pContext = new SwXMLTableColContext_Impl( GetSwImport(), nPrefix,
                                                      rLocalName, xAttrList,
                                                      this );
        break;
    case XML_TOK_TABLE_HEADER_ROWS:
        bHeader = sal_True;
    case XML_TOK_TABLE_ROWS:
        pContext = new SwXMLTableRowsContext_Impl( GetSwImport(), nPrefix,
                                                   rLocalName, xAttrList,
                                                   this, bHeader );
        break;
    case XML_TOK_TABLE_ROW:
        if( IsInsertRowPossible() )
            pContext = new SwXMLTableRowContext_Impl( GetSwImport(), nPrefix,
                                                      rLocalName, xAttrList,
                                                      this );
        break;
    case XML_TOK_OFFICE_DDE_SOURCE:
        // save context for later processing (discard old context, if approp.)
        if( IsValid() )
        {
            if (pDDESource != NULL)
            {
                pDDESource->ReleaseRef();
            }
            pDDESource = new SwXMLDDETableContext_Impl( GetSwImport(), nPrefix,
                                                        rLocalName );
            pDDESource->AddRef();
            pContext = pDDESource;
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

    return pContext;
}

void SwXMLTableContext::InsertColumn( sal_Int32 nWidth, sal_Bool bRelWidth,
                                         const OUString *pDfltCellStyleName )
{
    ASSERT( nCurCol < USHRT_MAX,
            "SwXMLTableContext::InsertColumn: no space left" );
    if( nCurCol >= USHRT_MAX )
        return;

    if( nWidth < MINLAY )
        nWidth = MINLAY;
    else if( nWidth > USHRT_MAX )
        nWidth = USHRT_MAX;
    aColumnWidths.Insert( (sal_uInt16)nWidth, aColumnWidths.Count() );
    aColumnRelWidths.Insert( bRelWidth, aColumnRelWidths.Count() );
    if( (pDfltCellStyleName && pDfltCellStyleName->getLength() > 0) ||
        pColumnDefaultCellStyleNames )
    {
        if( !pColumnDefaultCellStyleNames )
        {
            pColumnDefaultCellStyleNames = new SvStringsDtor;
            sal_uInt16 nCount = aColumnRelWidths.Count() - 1;
            while( nCount-- )
                pColumnDefaultCellStyleNames->Insert( new String,
                    pColumnDefaultCellStyleNames->Count() );
        }

        pColumnDefaultCellStyleNames->Insert(
            pDfltCellStyleName ? new String( *pDfltCellStyleName ) : new String,
            pColumnDefaultCellStyleNames->Count() );
    }
}

sal_Int32 SwXMLTableContext::GetColumnWidth( sal_uInt32 nCol,
                                             sal_uInt32 nColSpan ) const
{
    sal_uInt32 nLast = nCol+nColSpan;
    if( nLast > aColumnWidths.Count() )
        nLast = aColumnWidths.Count();

    sal_Int32 nWidth = 0L;
    for( sal_uInt16 i=(sal_uInt16)nCol; i < nLast; i++ )
        nWidth += aColumnWidths[i];

    return nWidth;
}

OUString SwXMLTableContext::GetColumnDefaultCellStyleName( sal_uInt32 nCol ) const
{
    OUString sRet;
    if( pColumnDefaultCellStyleNames )
        sRet =  *(*pColumnDefaultCellStyleNames)[(sal_uInt16)nCol];

    return sRet;
}

void SwXMLTableContext::InsertCell( const OUString& rStyleName,
                                    sal_uInt32 nRowSpan, sal_uInt32 nColSpan,
                                    const SwStartNode *pStartNode,
                                    SwXMLTableContext *pTable,
                                    sal_Bool bProtect,
                                    const OUString* pFormula,
                                    sal_Bool bHasValue,
                                    double fValue)
{
    ASSERT( nCurCol < GetColumnCount(),
            "SwXMLTableContext::InsertCell: row is full" );
    ASSERT( nCurRow < USHRT_MAX,
            "SwXMLTableContext::InsertCell: table is full" );
    if( nCurCol >= USHRT_MAX || nCurRow > USHRT_MAX )
        return;

    ASSERT( nRowSpan >=1UL, "SwXMLTableContext::InsertCell: row span is 0" );
    if( 0UL == nRowSpan )
        nRowSpan = 1UL;
    ASSERT( nColSpan >=1UL, "SwXMLTableContext::InsertCell: col span is 0" );
    if( 0UL == nColSpan )
        nColSpan = 1UL;

    sal_uInt32 i, j;

    // Until it is possible to add columns here, fix the column span.
    sal_uInt32 nColsReq = nCurCol + nColSpan;
    if( nColsReq > GetColumnCount() )
    {
        nColSpan = GetColumnCount() - nCurCol;
        nColsReq = GetColumnCount();
    }

    // Check whether there are cells from a previous line already that reach
    // into the current row.
    if( nCurRow > 0UL && nColSpan > 1UL )
    {
        SwXMLTableRow_Impl *pCurRow = (*pRows)[(sal_uInt16)nCurRow];
        sal_uInt32 nLastCol = GetColumnCount() < nColsReq ? GetColumnCount()
                                                     : nColsReq;
        for( i=nCurCol+1UL; i<nLastCol; i++ )
        {
            if( pCurRow->GetCell(i)->IsUsed() )
            {
                // If this cell is used, the column span is truncated
                nColSpan = i - nCurCol;
                nColsReq = i;
                break;
            }
        }
    }

    sal_uInt32 nRowsReq = nCurRow + nRowSpan;
    if( nRowsReq > USHRT_MAX )
    {
        nRowSpan = USHRT_MAX - nCurRow;
        nRowsReq = USHRT_MAX;
    }

    // Add columns (if # required columns greater than # columns):
    // This should never happen, since we require column definitions!
    if ( nColsReq > GetColumnCount() )
    {
        for( i=GetColumnCount(); i<nColsReq; i++ )
        {
            aColumnWidths.Insert( MINLAY, aColumnWidths.Count() );
            aColumnRelWidths.Insert( sal_True, aColumnRelWidths.Count() );
        }
        // adjust columns in *all* rows, if columns must be inserted
        for( i=0; i<pRows->Count(); i++ )
            (*pRows)[(sal_uInt16)i]->Expand( nColsReq, i<nCurRow );
    }

    // Add rows
    if( pRows->Count() < nRowsReq )
    {
        OUString aStyleName;
        for( i=pRows->Count(); i<nRowsReq; i++ )
            pRows->Insert( new SwXMLTableRow_Impl(aStyleName,GetColumnCount()),
                           pRows->Count() );
    }

    OUString sStyleName( rStyleName );
    if( !sStyleName.getLength() )
    {
        sStyleName = ((*pRows)[(sal_uInt16)nCurRow])->GetDefaultCellStyleName();
        if( !sStyleName.getLength() && HasColumnDefaultCellStyleNames() )
        {
            sStyleName = GetColumnDefaultCellStyleName( nCurCol );
            if( !sStyleName.getLength() )
                sStyleName = aDfltCellStyleName;
        }
    }

    // Fill the cells
    for( i=nColSpan; i>0UL; i-- )
    {
        for( j=nRowSpan; j>0UL; j-- )
        {
            const bool bCovered = i != nColSpan || j != nRowSpan;
            GetCell( nRowsReq-j, nColsReq-i )
                ->Set( sStyleName, j, i, pStartNode,
                       pTable, bProtect, pFormula, bHasValue, bCovered, fValue);
        }
    }

    // Set current col to the next (free) column
    nCurCol = nColsReq;
    while( nCurCol<GetColumnCount() && GetCell(nCurRow,nCurCol)->IsUsed() )
        nCurCol++;
}

void SwXMLTableContext::InsertRow( const OUString& rStyleName,
                                   const OUString& rDfltCellStyleName,
                                   sal_Bool bInHead )
{
    ASSERT( nCurRow < USHRT_MAX,
            "SwXMLTableContext::InsertRow: no space left" );
    if( nCurRow >= USHRT_MAX )
        return;

    // Make sure there is at least one column.
    if( 0==nCurRow && 0UL == GetColumnCount()  )
        InsertColumn( USHRT_MAX, sal_True );

    if( nCurRow < pRows->Count() )
    {
        // The current row has already been inserted because of a row span
        // of a previous row.
        (*pRows)[(sal_uInt16)nCurRow]->Set( rStyleName, rDfltCellStyleName );
    }
    else
    {
        // add a new row
        pRows->Insert( new SwXMLTableRow_Impl( rStyleName, GetColumnCount(),
                                               &rDfltCellStyleName ),
                       pRows->Count() );
    }

    // We start at the first column ...
    nCurCol=0UL;

    // ... but this cell may be occupied already.
    while( nCurCol<GetColumnCount() && GetCell(nCurRow,nCurCol)->IsUsed() )
        nCurCol++;

    if( bInHead  &&  nHeaderRows == nCurRow )
        nHeaderRows++;
}

void SwXMLTableContext::InsertRepRows( sal_uInt32 nCount )
{
    const SwXMLTableRow_Impl *pSrcRow = (*pRows)[(sal_uInt16)nCurRow-1];
    while( nCount > 1 && IsInsertRowPossible() )
    {
        InsertRow( pSrcRow->GetStyleName(), pSrcRow->GetDefaultCellStyleName(),
                   sal_False );
        while( nCurCol < GetColumnCount() )
        {
            if( !GetCell(nCurRow,nCurCol)->IsUsed() )
            {
                const SwXMLTableCell_Impl *pSrcCell =
                    GetCell( nCurRow-1, nCurCol );
                InsertCell( pSrcCell->GetStyleName(), 1U,
                            pSrcCell->GetColSpan(),
                            InsertTableSection(), 0, pSrcCell->IsProtected(),
                            &pSrcCell->GetFormula(),
                            pSrcCell->HasValue(), pSrcCell->GetValue() );
            }
        }
        FinishRow();
        nCount--;
    }
}

void SwXMLTableContext::FinishRow()
{
    // Insert an empty cell at the end of the line if the row is not complete
    if( nCurCol < GetColumnCount() )
    {
        OUString aStyleName;
        InsertCell( aStyleName, 1U, GetColumnCount() - nCurCol,
                    InsertTableSection() );
    }

    // Move to the next row.
    nCurRow++;
}

const SwStartNode *SwXMLTableContext::GetPrevStartNode( sal_uInt32 nRow,
                                                        sal_uInt32 nCol ) const
{
    const SwXMLTableCell_Impl *pPrevCell = 0;
    if( GetColumnCount() == nCol )
    {
        // The last cell is the right one here.
        pPrevCell = GetCell( pRows->Count()-1U, GetColumnCount()-1UL );
    }
    else if( 0UL == nRow )
    {
        // There are no vertically merged cells within the first row, so the
        // previous cell is the right one always.
        if( nCol > 0UL )
            pPrevCell = GetCell( nRow, nCol-1UL );
    }
    else
    {
        // If there is a previous cell in the current row that is not spanned
        // from the previous row, its the right one.
        const SwXMLTableRow_Impl *pPrevRow = (*pRows)[(sal_uInt16)nRow-1U];
        sal_uInt32 i = nCol;
        while( !pPrevCell &&  i > 0UL )
        {
            i--;
            if( 1UL == pPrevRow->GetCell( i )->GetRowSpan() )
                pPrevCell = GetCell( nRow, i );
        }

        // Otherwise, the last cell from the previous row is the right one.
        if( !pPrevCell )
            pPrevCell = pPrevRow->GetCell( GetColumnCount()-1UL );
    }

    const SwStartNode *pSttNd = 0;
    if( pPrevCell )
    {
        if( pPrevCell->GetStartNode() )
            pSttNd = pPrevCell->GetStartNode();
        else
            pSttNd = pPrevCell->GetSubTable()->GetLastStartNode();
    }

    return pSttNd;
}

void SwXMLTableContext::FixRowSpan( sal_uInt32 nRow, sal_uInt32 nCol,
                                    sal_uInt32 nColSpan )
{
    sal_uInt32 nLastCol = nCol + nColSpan;
    for( sal_uInt16 i = (sal_uInt16)nCol; i < nLastCol; i++ )
    {
        sal_uInt32 j = nRow;
        sal_uInt32 nRowSpan = 1UL;
        SwXMLTableCell_Impl *pCell = GetCell( j, i );
        while( pCell && pCell->GetRowSpan() > 1UL )
        {
            pCell->SetRowSpan( nRowSpan++ );
            pCell = j > 0UL ? GetCell( --j, i ) : 0;
        }
    }
}

void SwXMLTableContext::ReplaceWithEmptyCell( sal_uInt32 nRow, sal_uInt32 nCol, bool bRows )
{
    const SwStartNode *pPrevSttNd = GetPrevStartNode( nRow, nCol );
    const SwStartNode *pSttNd = InsertTableSection( pPrevSttNd );

    const SwXMLTableCell_Impl *pCell = GetCell( nRow, nCol );
    sal_uInt32 nLastRow = bRows ? nRow + pCell->GetRowSpan() : nRow + 1;
    sal_uInt32 nLastCol = nCol + pCell->GetColSpan();

    for( sal_uInt32 i=nRow; i<nLastRow; i++ )
    {
        SwXMLTableRow_Impl *pRow = (*pRows)[(sal_uInt16)i];
        for( sal_uInt32 j=nCol; j<nLastCol; j++ )
            pRow->GetCell( j )->SetStartNode( pSttNd );
    }

}

SwTableBox *SwXMLTableContext::NewTableBox( const SwStartNode *pStNd,
                                             SwTableLine *pUpper )
{
    // The topmost table is the only table that maintains the two members
    // pBox1 and bFirstSection.
    if( xParentTable.Is() )
        return ((SwXMLTableContext *)&xParentTable)->NewTableBox( pStNd,
                                                                  pUpper );

    SwTableBox *pBox;

    if( pBox1 &&
        pBox1->GetSttNd() == pStNd )
    {
        // wenn der StartNode dem StartNode der initial angelegten Box
        // entspricht nehmen wir diese Box
        pBox = pBox1;
        pBox->SetUpper( pUpper );
        pBox1 = 0;
    }
    else
        pBox = new SwTableBox( pBoxFmt, *pStNd, pUpper );

    return pBox;
}

SwTableBoxFmt* SwXMLTableContext::GetSharedBoxFormat(
    SwTableBox* pBox,
    const OUString& rStyleName,
    sal_Int32 nColumnWidth,
    sal_Bool bProtected,
    sal_Bool bMayShare,
    sal_Bool& bNew,
    sal_Bool* pModifyLocked )
{
    if ( pSharedBoxFormats == NULL )
        pSharedBoxFormats = new map_BoxFmt();

    SwTableBoxFmt* pBoxFmt;

    TableBoxIndex aKey( rStyleName, nColumnWidth, bProtected );
    map_BoxFmt::iterator aIter = pSharedBoxFormats->find( aKey );
    if ( aIter == pSharedBoxFormats->end() )
    {
        // unknown format so far -> construct a new one

        // get the old format, and reset all attributes
        // (but preserve FillOrder)
        pBoxFmt = (SwTableBoxFmt*)pBox->ClaimFrmFmt();
        SwFmtFillOrder aFillOrder( pBoxFmt->GetFillOrder() );
        // --> OD 2007-01-25 #i73790# - method renamed
        pBoxFmt->ResetAllFmtAttr();
        // <--
        pBoxFmt->SetAttr( aFillOrder );
        bNew = sal_True;    // it's a new format now

        // share this format, if allowed
        if ( bMayShare )
            (*pSharedBoxFormats)[ aKey ] = pBoxFmt;
    }
    else
    {
        // set the shared format
        pBoxFmt = aIter->second;
        pBox->ChgFrmFmt( pBoxFmt );
        bNew = sal_False;   // copied from an existing format

        // claim it, if we are not allowed to share
        if ( !bMayShare )
            pBoxFmt = (SwTableBoxFmt*)pBox->ClaimFrmFmt();
    }

    // lock format (if so desired)
    if ( pModifyLocked != NULL )
    {
        (*pModifyLocked) = pBoxFmt->IsModifyLocked();
        pBoxFmt->LockModify();
    }

    return pBoxFmt;
}

SwTableBox *SwXMLTableContext::MakeTableBox( SwTableLine *pUpper,
                                             sal_uInt32 nTopRow,
                                             sal_uInt32 nLeftCol,
                                             sal_uInt32 nBottomRow,
                                             sal_uInt32 nRightCol )
{
    SwTableBox *pBox = new SwTableBox( pBoxFmt, 0, pUpper );

    sal_uInt32 nColSpan = nRightCol - nLeftCol;
    sal_Int32 nColWidth = GetColumnWidth( nLeftCol, nColSpan );

    // TODO: Share formats!
    SwFrmFmt *pFrmFmt = pBox->ClaimFrmFmt();
    SwFmtFillOrder aFillOrder( pFrmFmt->GetFillOrder() );
    // --> OD 2007-01-25 #i73790# - method renamed
    pFrmFmt->ResetAllFmtAttr();
    // <--
    pFrmFmt->SetAttr( aFillOrder );

    pFrmFmt->SetAttr( SwFmtFrmSize( ATT_VAR_SIZE, nColWidth ) );

    SwTableLines& rLines = pBox->GetTabLines();
    sal_Bool bSplitted = sal_False;

    while( !bSplitted )
    {
        sal_uInt32 nStartRow = nTopRow;
        sal_uInt32 i;

        for( i = nTopRow; i < nBottomRow; i++ )
        {
            // Could the table be splitted behind the current row?
            sal_Bool bSplit = sal_True;
            SwXMLTableRow_Impl *pRow = (*pRows)[(sal_uInt16)i];
            for( sal_uInt32 j=nLeftCol; j<nRightCol; j++ )
            {
                bSplit = ( 1UL == pRow->GetCell(j)->GetRowSpan() );
                if( !bSplit )
                    break;
            }
            if( bSplit && (nStartRow>nTopRow || i+1UL<nBottomRow) )
            {
                SwTableLine *pLine =
                    MakeTableLine( pBox, nStartRow, nLeftCol, i+1UL,
                                   nRightCol );

                rLines.C40_INSERT( SwTableLine, pLine, rLines.Count() );

                nStartRow = i+1UL;
                bSplitted = sal_True;
            }
        }
        if( !bSplitted )
        {
            // No splitting was possible. That for, we have to force it.
            // Ruthless!

            nStartRow = nTopRow;
            while( nStartRow < nBottomRow )
            {
                sal_uInt32 nMaxRowSpan = 0UL;
                SwXMLTableRow_Impl *pStartRow = (*pRows)[(sal_uInt16)nStartRow];
                SwXMLTableCell_Impl *pCell;
                for( i=nLeftCol; i<nRightCol; i++ )
                    if( ( pCell=pStartRow->GetCell(i),
                          pCell->GetRowSpan() > nMaxRowSpan ) )
                        nMaxRowSpan = pCell->GetRowSpan();

                nStartRow += nMaxRowSpan;
                if( nStartRow<nBottomRow )
                {
                    SwXMLTableRow_Impl *pPrevRow =
                                        (*pRows)[(sal_uInt16)nStartRow-1U];
                    i = nLeftCol;
                    while( i < nRightCol )
                    {
                        if( pPrevRow->GetCell(i)->GetRowSpan() > 1UL )
                        {
                            const SwXMLTableCell_Impl *pCell =
                                GetCell( nStartRow, i );
                            sal_uInt32 nColSpan = pCell->GetColSpan();
                            FixRowSpan( nStartRow-1UL, i, nColSpan );
                            ReplaceWithEmptyCell( nStartRow, i, true );
                            i += nColSpan;
                        }
                        else
                        {
                            i++;
                        }
                    }
                }
            }
            // und jetzt nochmal von vorne ...
        }
    }

    return pBox;
}

SwTableBox *SwXMLTableContext::MakeTableBox(
        SwTableLine *pUpper, const SwXMLTableCell_Impl *pCell,
        sal_uInt32 nTopRow, sal_uInt32 nLeftCol, sal_uInt32 nBottomRow,
        sal_uInt32 nRightCol )
{
    SwTableBox *pBox;
    sal_uInt32 nColSpan = nRightCol - nLeftCol;
    sal_uInt32 nRowSpan = nBottomRow - nTopRow;
    sal_Int32 nColWidth = GetColumnWidth( nLeftCol, nColSpan );

    if( pCell->GetStartNode() )
    {
        pBox = NewTableBox( pCell->GetStartNode(), pUpper );
    }
    else
    {
        // und die ist eine Tabelle: dann bauen wir eine neue
        // Box und fuegen die Zeilen der Tabelle in die Zeilen
        // der Box ein
        pBox = new SwTableBox( pBoxFmt, 0, pUpper );
        pCell->GetSubTable()->MakeTable( pBox, nColWidth );
    }

    // Share formats!
    OUString sStyleName = pCell->GetStyleName();
    sal_Bool bModifyLocked;
    sal_Bool bNew;
    SwTableBoxFmt *pBoxFmt = GetSharedBoxFormat(
        pBox, sStyleName, nColWidth, pCell->IsProtected(),
        pCell->GetStartNode() && pCell->GetFormula().getLength() == 0 &&
            ! pCell->HasValue(),
        bNew, &bModifyLocked  );

    // if a new format was created, then we need to set the style
    if ( bNew )
    {
        // set style
        const SfxItemSet *pAutoItemSet = 0;
        if( pCell->GetStartNode() && sStyleName &&
            GetSwImport().FindAutomaticStyle(
                XML_STYLE_FAMILY_TABLE_CELL, sStyleName, &pAutoItemSet ) )
        {
            if( pAutoItemSet )
                pBoxFmt->SetAttr( *pAutoItemSet );
        }
    }

    if( pCell->GetStartNode() )
    {

        // #104801# try to rescue broken documents with a certain pattern
        // if: 1) the cell has a default number format (number 0)
        //     2) the call has no formula
        //     3) the value is 0.0
        //     4) the text doesn't look anything like 0.0
        //        [read: length > 10, or length smaller 10 and no 0 in it]
        // then make it a text cell!
        bool bSuppressNumericContent = false;
        if( pCell->HasValue() && (pCell->GetValue() == 0.0) &&
            (pCell->GetFormula().getLength() == 0) &&
            (sStyleName.getLength() != 0) )
        {
            // default num format?
            const SfxPoolItem* pItem = NULL;
            if( pBoxFmt->GetItemState( RES_BOXATR_FORMAT, FALSE, &pItem )
                            == SFX_ITEM_SET )
            {
                const SwTblBoxNumFormat* pNumFormat =
                    static_cast<const SwTblBoxNumFormat*>( pItem );
                if( ( pNumFormat != NULL ) && ( pNumFormat->GetValue() == 0 ) )
                {
                    // only one text node?
                    SwNodeIndex aNodeIndex( *(pCell->GetStartNode()), 1 );
                    if( ( aNodeIndex.GetNode().EndOfSectionIndex() -
                          aNodeIndex.GetNode().StartOfSectionIndex() ) == 2 )
                    {
                        SwTxtNode* pTxtNode= aNodeIndex.GetNode().GetTxtNode();
                        if( pTxtNode != NULL )
                        {
                            // check text: does it look like some form of 0.0?
                            const String& rText = pTxtNode->GetTxt();
                            if( ( rText.Len() > 10 ) ||
                                ( rText.Search( '0' ) == STRING_NOTFOUND ) )
                            {
                                bSuppressNumericContent = true;
                            }
                        }
                    }
                    else
                        bSuppressNumericContent = true; // several nodes
                }
            }
        }

        if( bSuppressNumericContent )
        {
            // suppress numeric content? Then reset number format!
            pBoxFmt->ResetAttr( RES_BOXATR_FORMULA );
            pBoxFmt->ResetAttr( RES_BOXATR_FORMAT );
            pBoxFmt->ResetAttr( RES_BOXATR_VALUE );
        }
        else
        {
            // the normal case: set formula and value (if available)

            const OUString& rFormula = pCell->GetFormula();
            if (rFormula.getLength() > 0)
            {
                // formula cell: insert formula if valid
                SwTblBoxFormula aFormulaItem( rFormula );
                pBoxFmt->SetAttr( aFormulaItem );
            }

            // always insert value, even if default
            if( pCell->HasValue() )
            {
                SwTblBoxValue aValueItem( pCell->GetValue() );
                pBoxFmt->SetAttr( aValueItem );
            }
        }

        // update cell content depend on the default language
        pBox->ActualiseValueBox();
    }

    // table cell protection
    if( pCell->IsProtected() )
    {
        SvxProtectItem aProtectItem( RES_PROTECT );
        aProtectItem.SetCntntProtect( sal_True );
        pBoxFmt->SetAttr( aProtectItem );
    }

    // restore old modify-lock state
    if (! bModifyLocked)
        pBoxFmt->UnlockModify();

    pBoxFmt->SetAttr( SwFmtFrmSize( ATT_VAR_SIZE, nColWidth ) );

    return pBox;
}

SwTableLine *SwXMLTableContext::MakeTableLine( SwTableBox *pUpper,
                                               sal_uInt32 nTopRow,
                                               sal_uInt32 nLeftCol,
                                               sal_uInt32 nBottomRow,
                                               sal_uInt32 nRightCol )
{
    SwTableLine *pLine;
    if( !pUpper && 0UL==nTopRow )
    {
        pLine = pTableNode->GetTable().GetTabLines()[0U];
    }
    else
    {
        pLine = new SwTableLine( pLineFmt, 0, pUpper );
    }

    // TODO: Share formats!
    SwFrmFmt *pFrmFmt = pLine->ClaimFrmFmt();
    SwFmtFillOrder aFillOrder( pFrmFmt->GetFillOrder() );
    // --> OD 2007-01-25 #i73790# - method renamed
    pFrmFmt->ResetAllFmtAttr();
    // <--
    pFrmFmt->SetAttr( aFillOrder );

    const SfxItemSet *pAutoItemSet = 0;
    const OUString& rStyleName = (*pRows)[(sal_uInt16)nTopRow]->GetStyleName();
    if( 1UL == (nBottomRow - nTopRow) &&
        rStyleName.getLength() &&
        GetSwImport().FindAutomaticStyle(
            XML_STYLE_FAMILY_TABLE_ROW, rStyleName, &pAutoItemSet ) )
    {
        if( pAutoItemSet )
            pFrmFmt->SetAttr( *pAutoItemSet );
    }

    SwTableBoxes& rBoxes = pLine->GetTabBoxes();

    sal_uInt32 nStartCol = nLeftCol;
    while( nStartCol < nRightCol )
    {
        for( sal_uInt32 nRow=nTopRow; nRow<nBottomRow; nRow++ )
            (*pRows)[(sal_uInt16)nRow]->SetSplitable( sal_True );

        sal_uInt32 nCol = nStartCol;
        sal_uInt32 nSplitCol = nRightCol;
        sal_Bool bSplitted = sal_False;
        while( !bSplitted )
        {
            ASSERT( nCol < nRightCol, "Zu weit gelaufen" );

            // Kann hinter der aktuellen HTML-Tabellen-Spalte gesplittet
            // werden? Wenn ja, koennte der enstehende Bereich auch noch
            // in Zeilen zerlegt werden, wenn man die naechste Spalte
            // hinzunimmt?
            sal_Bool bSplit = sal_True;
            sal_Bool bHoriSplitMayContinue = sal_False;
            sal_Bool bHoriSplitPossible = sal_False;

            if ( bHasSubTables )
            {
                // Convert row spans if the table has subtables:
                for( sal_uInt32 nRow=nTopRow; nRow<nBottomRow; nRow++ )
                {
                    SwXMLTableCell_Impl *pCell = GetCell(nRow,nCol);
                    // Could the table fragment be splitted horizontally behind
                    // the current line?
                    sal_Bool bHoriSplit = (*pRows)[(sal_uInt16)nRow]->IsSplitable() &&
                                      nRow+1UL < nBottomRow &&
                                      1UL == pCell->GetRowSpan();
                    (*pRows)[(sal_uInt16)nRow]->SetSplitable( bHoriSplit );

                    // Could the table fragment be splitted vertically behind the
                    // current column (uptp the current line?
                    bSplit &= ( 1UL == pCell->GetColSpan() );
                    if( bSplit )
                    {
                        bHoriSplitPossible |= bHoriSplit;

                        // Could the current table fragment be splitted
                        // horizontally behind the next collumn, too?
                        bHoriSplit &= (nCol+1UL < nRightCol &&
                                       1UL == GetCell(nRow,nCol+1UL)->GetRowSpan());
                        bHoriSplitMayContinue |= bHoriSplit;
                    }
                }
            }
            else
            {
                // No subtabels: We use the new table model.
                SwXMLTableCell_Impl *pCell = GetCell(nTopRow,nCol);

                // Could the table fragment be splitted vertically behind the
                // current column (uptp the current line?
                bSplit = 1UL == pCell->GetColSpan();
            }

#ifndef PRODUCT
            if( nCol == nRightCol-1UL )
            {
                ASSERT( bSplit, "Split-Flag falsch" );
                if ( bHasSubTables )
                {
                    ASSERT( !bHoriSplitMayContinue,
                            "HoriSplitMayContinue-Flag falsch" );
                    SwXMLTableCell_Impl *pTmpCell = GetCell( nTopRow, nStartCol );
                    ASSERT( pTmpCell->GetRowSpan() != (nBottomRow-nTopRow) ||
                            !bHoriSplitPossible, "HoriSplitPossible-Flag falsch" );
                }
            }
#endif

            ASSERT( !bHasSubTables || !bHoriSplitMayContinue || bHoriSplitPossible,
                    "bHoriSplitMayContinue, aber nicht bHoriSplitPossible" );

            if( bSplit )
            {
                SwTableBox* pBox = 0;
                SwXMLTableCell_Impl *pCell = GetCell( nTopRow, nStartCol );
                if( ( !bHasSubTables || ( pCell->GetRowSpan() == (nBottomRow-nTopRow) ) ) &&
                    pCell->GetColSpan() == (nCol+1UL-nStartCol) )
                {
                    // insert new empty cell for covered cells:
                    long nBoxRowSpan = 1;
                    if ( !bHasSubTables )
                    {
                        nBoxRowSpan = pCell->GetRowSpan();
                        if ( pCell->IsCovered() )
                        {
                            nBoxRowSpan = -1 * nBoxRowSpan;
                            ReplaceWithEmptyCell( nTopRow, nStartCol, false );
                        }
                    }

                    // The remaining box neither contains lines nor rows (i.e.
                    // is a content box
                    nSplitCol = nCol + 1UL;

                    pBox = MakeTableBox( pLine, pCell,
                                         nTopRow, nStartCol,
                                         nBottomRow, nSplitCol );

                    if ( 1 != nBoxRowSpan )
                        pBox->setRowSpan( nBoxRowSpan );

                    bSplitted = sal_True;
                }
                else if( bHasSubTables && bHoriSplitPossible && bHoriSplitMayContinue )
                {
                    // The table fragment could be splitted behind the current
                    // column, and the remaining fragment could be divided
                    // into lines. Anyway, it could be that this applies to
                    // the next column, too. That for, we check the next
                    // column but rememeber the current one as a good place to
                    // split.
                    nSplitCol = nCol + 1UL;
                }
                else if ( bHasSubTables )
                {
                    // If the table resulting table fragment could be divided
                    // into lines if spllitting behind the current column, but
                    // this doesn't apply for thr next column, we split begind
                    // the current column. This applies for the last column,
                    // too.
                    // If the resulting box cannot be splitted into rows,
                    // the split at the last split position we remembered.
                    if( bHoriSplitPossible || nSplitCol > nCol+1 )
                    {
                        ASSERT( !bHoriSplitMayContinue,
                                "bHoriSplitMayContinue==sal_True" );
                        ASSERT( bHoriSplitPossible || nSplitCol == nRightCol,
                                "bHoriSplitPossible-Flag sollte gesetzt sein" );

                        nSplitCol = nCol + 1UL;
                    }

                    pBox = MakeTableBox( pLine, nTopRow, nStartCol,
                                         nBottomRow, nSplitCol );
                    bSplitted = sal_True;
                }

                ASSERT( bHasSubTables || pBox, "Colspan trouble" )

                if( pBox )
                    rBoxes.C40_INSERT( SwTableBox, pBox, rBoxes.Count() );
            }
            nCol++;
        }
        nStartCol = nSplitCol;
    }

    return pLine;
}

void SwXMLTableContext::_MakeTable( SwTableBox *pBox )
{
    // fix column widths
    sal_uInt32 i;
    sal_uInt32 nCols = GetColumnCount();

    // If there are empty rows (because of some row span of previous rows)
    // the have to be deleted. The previous rows have to be truncated.

    if( pRows->Count() > nCurRow )
    {
        SwXMLTableRow_Impl *pPrevRow = (*pRows)[(sal_uInt16)nCurRow-1U];
        SwXMLTableCell_Impl *pCell;
        for( i=0UL; i<nCols; i++ )
        {
            if( ( pCell=pPrevRow->GetCell(i), pCell->GetRowSpan() > 1UL ) )
            {
                FixRowSpan( nCurRow-1UL, i, 1UL );
            }
        }
        for( i=(sal_uInt32)pRows->Count()-1UL; i>=nCurRow; i-- )
            pRows->DeleteAndDestroy( (sal_uInt16)i );
    }

    if( 0UL == pRows->Count() )
    {
        OUString aStyleName;
        InsertCell( aStyleName, 1U, nCols, InsertTableSection() );
    }

    // TODO: Do we have to keep both values, the realtive and the absolute
    // width?
    sal_Int32 nAbsWidth = 0L;
    sal_Int32 nMinAbsColWidth = 0L;
    sal_Int32 nRelWidth = 0L;
    sal_Int32 nMinRelColWidth = 0L;
    sal_uInt32 nRelCols = 0UL;
    for( i=0U; i < nCols; i++ )
    {
        sal_Int32 nColWidth = aColumnWidths[(sal_uInt16)i];
        if( aColumnRelWidths[(sal_uInt16)i] )
        {
            nRelWidth += nColWidth;
            if( 0L == nMinRelColWidth || nColWidth < nMinRelColWidth )
                nMinRelColWidth = nColWidth;
            nRelCols++;
        }
        else
        {
            nAbsWidth += nColWidth;
            if( 0L == nMinAbsColWidth || nColWidth < nMinAbsColWidth )
                nMinAbsColWidth = nColWidth;
        }
    }
    sal_uInt32 nAbsCols = nCols - nRelCols;

    if( bRelWidth )
    {
        // If there a columns that have an absolute width, we have to
        // calculate a relative one for them.
        if( nAbsCols > 0UL )
        {
            // All column that have absolute widths get relative widths;
            // these widths relate to each over like the original absolute
            // widths. The smallest column gets a width that hat the same
            // value as the smallest column that has an relative width
            // already.
            if( 0L == nMinRelColWidth )
                nMinRelColWidth = nMinAbsColWidth;

            for( i=0UL; nAbsCols > 0UL && i < nCols; i++ )
            {
                if( !aColumnRelWidths[(sal_uInt16)i] )
                {
                    sal_Int32 nRelCol = (aColumnWidths[(sal_uInt16)i] * nMinRelColWidth) /
                                   nMinAbsColWidth;
                    aColumnWidths.Replace( (sal_uInt16)nRelCol, (sal_uInt16)i );
                    nRelWidth += nRelCol;
                    nAbsCols--;
                }
            }
        }

        if( !nWidth )
        {
            // This happens only for percentage values for the table itself.
            // In this case, the columns get the correct width even if the
            // the sum of the relative withs is smaller than the available
            // width in TWIP. Therfore, we can use the relative width.
            //
            nWidth = nRelWidth > USHRT_MAX ? USHRT_MAX : nRelWidth;
        }
        if( nRelWidth != nWidth )
        {
            double n = (double)nWidth / (double)nRelWidth;
            nRelWidth = 0L;
            for( i=0U; i < nCols-1UL; i++ )
            {
                sal_Int32 nW = (sal_Int32)(aColumnWidths[(sal_uInt16)i] * n);
                aColumnWidths.Replace( (sal_uInt16)nW, (sal_uInt16)i );
                nRelWidth += nW;
            }
            aColumnWidths.Replace( (sal_uInt16)(nWidth-nRelWidth),
                                   (sal_uInt16)nCols-1U );
        }
    }
    else
    {
        // If there are columns that have relative widths, we have to
        // calculate a absolute widths for them.
        if( nRelCols > 0UL )
        {
            // The absolute space that is available for all columns with a
            // relative width.
            sal_Int32 nAbsForRelWidth =
                    nWidth > nAbsWidth ? nWidth - nAbsWidth : (sal_Int32)0L;

            // The relative width that has to be distributed in addition to
            // equally widthed columns.
            sal_Int32 nExtraRel = nRelWidth - (nRelCols * nMinRelColWidth);

            // The absolute space that may be distributed in addition to
            // minumum widthed columns.
            sal_Int32 nMinAbs = nRelCols * MINLAY;
            sal_Int32 nExtraAbs =
                    nAbsForRelWidth > nMinAbs ? nAbsForRelWidth - nMinAbs : (sal_Int32)0L;

            sal_Bool bMin = sal_False;		// Do all columns get the mininum width?
            sal_Bool bMinExtra = sal_False;	// Do all columns get the minimum width plus
                                    // some extra space?

            if( nAbsForRelWidth <= nMinAbs )
            {
                // If there is not enough space left for all columns to
                // get the minimum width, they get the minimum width, anyway.
                nAbsForRelWidth = nMinAbs;
                bMin = sal_True;
            }
            else if( nAbsForRelWidth <= (nRelWidth * MINLAY) /
                                        nMinRelColWidth )
            {
                // If there is enougth space for all columns to get the
                // minimum width, but not to get a width that takes the
                // relative width into account, each column gets the minimum
                // width plus some extra space that is based on the additional
                // space that is available.
                bMinExtra = sal_True;
            }
            // Otherwise, if there is enouth space for every column, every
            // column gets this space.

            for( i=0UL; nRelCols > 0UL && i < nCols; i++ )
            {
                if( aColumnRelWidths[(sal_uInt16)i] )
                {
                    sal_Int32 nAbsCol;
                    if( 1UL == nRelCols )
                    {
                        // The last column that has a relative width gets
                        // all absolute space that is left.
                        nAbsCol = nAbsForRelWidth;
                    }
                    else
                    {
                        if( bMin )
                        {
                            nAbsCol = MINLAY;
                        }
                        else if( bMinExtra )
                        {
                            sal_Int32 nExtraRelCol =
                                aColumnWidths[(sal_uInt16)i] - nMinRelColWidth;
                            nAbsCol = MINLAY + (nExtraRelCol * nExtraAbs) /
                                                 nExtraRel;
                        }
                        else
                        {
                            nAbsCol = (aColumnWidths[(sal_uInt16)i] * nAbsForRelWidth) /
                                      nRelWidth;
                        }
                    }
                    aColumnWidths.Replace( (sal_uInt16)nAbsCol, (sal_uInt16)i );
                    nAbsForRelWidth -= nAbsCol;
                    nAbsWidth += nAbsCol;
                    nRelCols--;
                }
            }
        }

        if( nAbsWidth < nWidth )
        {
            // If the table's width is larger than the absolute column widths,
            // every column get some extra width.
            sal_Int32 nExtraAbs = nWidth - nAbsWidth;
            sal_Int32 nAbsLastCol =
                    aColumnWidths[(sal_uInt16)nCols-1U] + nExtraAbs;
            for( i=0UL; i < nCols-1UL; i++ )
            {
                sal_Int32 nAbsCol = aColumnWidths[(sal_uInt16)i];
                sal_Int32 nExtraAbsCol = (nAbsCol * nExtraAbs) /
                                         nAbsWidth;
                nAbsCol += nExtraAbsCol;
                aColumnWidths.Replace( (sal_uInt16)nAbsCol, (sal_uInt16)i );
                nAbsLastCol -= nExtraAbsCol;
            }
            aColumnWidths.Replace( (sal_uInt16)nAbsLastCol, (sal_uInt16)nCols-1U );
        }
        else if( nAbsWidth > nWidth )
        {
            // If the table's width is smaller than the absolute column
            // widths, every column gets the minimum width plus some extra
            // width.
            sal_Int32 nExtraAbs = nWidth - (nCols * MINLAY);
            sal_Int32 nAbsLastCol = MINLAY + nExtraAbs;
            for( i=0UL; i < nCols-1UL; i++ )
            {
                sal_Int32 nAbsCol = aColumnWidths[(sal_uInt16)i];
                sal_Int32 nExtraAbsCol = (nAbsCol * nExtraAbs) /
                                         nAbsWidth;
                nAbsCol = MINLAY + nExtraAbsCol;
                aColumnWidths.Replace( (sal_uInt16)nAbsCol, (sal_uInt16)i );
                nAbsLastCol -= nExtraAbsCol;
            }
            aColumnWidths.Replace( (sal_uInt16)nAbsLastCol, (sal_uInt16)nCols-1U );
        }
    }

    SwTableLines& rLines =
        pBox ? pBox->GetTabLines()
             : pTableNode->GetTable().GetTabLines();

    sal_uInt32 nStartRow = 0UL;
    sal_uInt32 nRows = pRows->Count();
    for( i=0UL; i<nRows; i++ )
    {
        // Could we split the table behind the current line?
        sal_Bool bSplit = sal_True;
        if ( bHasSubTables )
        {
            SwXMLTableRow_Impl *pRow = (*pRows)[(sal_uInt16)i];
            for( sal_uInt32 j=0UL; j<nCols; j++ )
            {
                bSplit = ( 1UL == pRow->GetCell(j)->GetRowSpan() );
                if( !bSplit )
                    break;
            }
        }

        if( bSplit )
        {
            SwTableLine *pLine =
                MakeTableLine( pBox, nStartRow, 0UL, i+1UL, nCols );
            if( pBox || nStartRow>0UL )
                rLines.C40_INSERT( SwTableLine, pLine, rLines.Count() );
            nStartRow = i+1UL;
        }
    }
}

void SwXMLTableContext::MakeTable()
{
    // this method will modify the document directly -> lock SolarMutex
    // This will call all other MakeTable*(..) methods, so
    // those don't need to be locked separately.
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwXMLImport& rSwImport = GetSwImport();

    SwFrmFmt *pFrmFmt = pTableNode->GetTable().GetFrmFmt();

    SwHoriOrient eHoriOrient = HORI_FULL;
    sal_Bool bSetHoriOrient = sal_False;

    sal_uInt16 nPrcWidth = 0U;

    pTableNode->GetTable().SetRowsToRepeat( nHeaderRows );
    pTableNode->GetTable().SetTableModel( !bHasSubTables );

    const SfxItemSet *pAutoItemSet = 0;
    if( aStyleName.getLength() &&
        rSwImport.FindAutomaticStyle(
            XML_STYLE_FAMILY_TABLE_TABLE, aStyleName, &pAutoItemSet ) &&
         pAutoItemSet )
    {
        const SfxPoolItem *pItem;
        const SvxLRSpaceItem *pLRSpace = 0;
        if( SFX_ITEM_SET == pAutoItemSet->GetItemState( RES_LR_SPACE, sal_False,
                                                        &pItem ) )
            pLRSpace = (const SvxLRSpaceItem *)pItem;

        if( SFX_ITEM_SET == pAutoItemSet->GetItemState( RES_HORI_ORIENT, sal_False,
                                                        &pItem ) )
        {
            eHoriOrient = ((const SwFmtHoriOrient *)pItem)->GetHoriOrient();
            switch( eHoriOrient )
            {
            case HORI_FULL:
                if( pLRSpace )
                {
                    eHoriOrient = HORI_NONE;
                    bSetHoriOrient = sal_True;
                }
                break;
            case HORI_LEFT:
                if( pLRSpace )
                {
                    eHoriOrient = HORI_LEFT_AND_WIDTH;
                    bSetHoriOrient = sal_True;
                }
                break;
            }
        }
        else
        {
            bSetHoriOrient = sal_True;
        }

        const SwFmtFrmSize *pSize = 0;
        if( SFX_ITEM_SET == pAutoItemSet->GetItemState( RES_FRM_SIZE, sal_False,
                                                        &pItem ) )
            pSize = (const SwFmtFrmSize *)pItem;

        switch( eHoriOrient )
        {
        case HORI_FULL:
        case HORI_NONE:
            // #78246#: For HORI_NONE we would prefere to use the sum
            // of the relative column widths as reference width.
            // Unfortunately this works only if this sum interpreted as
            // twip value is larger than the space that is avaialable.
            // We don't know that space, so we have to use USHRT_MAX, too.
            // Even if a size is speczified, it will be ignored!
            nWidth = USHRT_MAX;
            break;
            break;
        default:
            if( pSize )
            {
                if( pSize->GetWidthPercent() )
                {
                    // The width will be set in _MakeTable
                    nPrcWidth = pSize->GetWidthPercent();
                }
                else
                {
                    nWidth = pSize->GetWidth();
                    if( nWidth < (sal_Int32)GetColumnCount() * MINLAY )
                    {
                        nWidth = GetColumnCount() * MINLAY;
                    }
                    else if( nWidth > USHRT_MAX )
                    {
                        nWidth = USHRT_MAX;
                    }
                    bRelWidth = sal_False;
                }
            }
            else
            {
                eHoriOrient = HORI_LEFT_AND_WIDTH == eHoriOrient
                                    ? HORI_NONE : HORI_FULL;
                bSetHoriOrient = sal_True;
                nWidth = USHRT_MAX;
            }
            break;
        }

        pFrmFmt->SetAttr( *pAutoItemSet );
    }
    else
    {
        bSetHoriOrient = sal_True;
        nWidth = USHRT_MAX;
    }

    SwTableLine *pLine1 = pTableNode->GetTable().GetTabLines()[0U];
    DBG_ASSERT( pBox1 == pLine1->GetTabBoxes()[0U],
                "Why is box 1 change?" );
    pBox1->pSttNd = pSttNd1;
    pLine1->GetTabBoxes().Remove(0U);

    pLineFmt = (SwTableLineFmt*)pLine1->GetFrmFmt();
    pBoxFmt = (SwTableBoxFmt*)pBox1->GetFrmFmt();

    _MakeTable( 0 );

    if( bSetHoriOrient )
        pFrmFmt->SetAttr( SwFmtHoriOrient( 0, eHoriOrient ) );

    // This must be bahind the call to _MakeTable, because nWidth might be
    // changed there.
    pFrmFmt->LockModify();
    SwFmtFrmSize aSize( ATT_VAR_SIZE, nWidth );
    aSize.SetWidthPercent( (sal_Int8)nPrcWidth );
    pFrmFmt->SetAttr( aSize );
    pFrmFmt->UnlockModify();


    for( sal_uInt16 i=0; i<pRows->Count(); i++ )
        (*pRows)[i]->Dispose();

    // now that table is complete, change into DDE table (if appropriate)
    if (NULL != pDDESource)
    {
        // change existing table into DDE table:
        // 1) Get DDE field type (get data from dde-source context),
        SwDDEFieldType* pFldType = lcl_GetDDEFieldType( pDDESource,
                                                        pTableNode );

        // 2) release the DDE source context,
        pDDESource->ReleaseRef();

        // 3) create new DDE table, and
        SwDDETable* pDDETable = new SwDDETable( pTableNode->GetTable(),
                                                pFldType, FALSE );

        // 4) set new (DDE)table at node.
        pTableNode->SetNewTable(pDDETable, FALSE);
    }

    if( pTableNode->GetDoc()->GetRootFrm() )
    {
        pTableNode->DelFrms();
        SwNodeIndex aIdx( *pTableNode->EndOfSectionNode(), 1 );
        pTableNode->MakeFrms( &aIdx );
    }
}

void SwXMLTableContext::MakeTable( SwTableBox *pBox, sal_Int32 nW )
{
    pLineFmt = GetParentTable()->pLineFmt;
    pBoxFmt = GetParentTable()->pBoxFmt;
    nWidth = nW;
    bRelWidth = GetParentTable()->bRelWidth;

    _MakeTable( pBox );
}

const SwStartNode *SwXMLTableContext::InsertTableSection(
                                            const SwStartNode *pPrevSttNd )
{
    // The topmost table is the only table that maintains the two members
    // pBox1 and bFirstSection.
    if( xParentTable.Is() )
        return ((SwXMLTableContext *)&xParentTable)->InsertTableSection( pPrevSttNd );

    const SwStartNode *pStNd;
    Reference<XUnoTunnel> xCrsrTunnel( GetImport().GetTextImport()->GetCursor(),
                                       UNO_QUERY);
    ASSERT( xCrsrTunnel.is(), "missing XUnoTunnel for Cursor" );
    OTextCursorHelper *pTxtCrsr =
            (OTextCursorHelper*)xCrsrTunnel->getSomething(
                                            OTextCursorHelper::getUnoTunnelId() );
    ASSERT( pTxtCrsr, "SwXTextCursor missing" );

    if( bFirstSection )
    {
        // The Cursor already is in the first section
        pStNd = pTxtCrsr->GetPaM()->GetNode()->FindTableBoxStartNode();
        bFirstSection = sal_False;
        OUString sStyleName( RTL_CONSTASCII_USTRINGPARAM("Standard") );
        GetImport().GetTextImport()->SetStyleAndAttrs( GetImport(),
            GetImport().GetTextImport()->GetCursor(), sStyleName, sal_True );
    }
    else
    {
        const SwEndNode *pEndNd = pPrevSttNd ? pPrevSttNd->EndOfSectionNode()
                                             : pTableNode->EndOfSectionNode();
        SwDoc* pDoc = pTxtCrsr->GetDoc();
        // --> OD 2007-07-02 #i78921# - make code robust
#if OSL_DEBUG_LEVEL > 1
        ASSERT( pDoc, "<SwXMLTableContext::InsertTableSection(..)> - no <pDoc> at <SwXTextCursor> instance - <SwXTextCurosr> doesn't seem to be registered at a <SwUnoCrsr> instance." );
#endif
        if ( !pDoc )
        {
            pDoc = const_cast<SwDoc*>(pEndNd->GetDoc());
        }
        // <--
        sal_uInt32 nOffset = pPrevSttNd ? 1UL : 0UL;
        SwNodeIndex aIdx( *pEndNd, nOffset );
        SwTxtFmtColl *pColl =
            pDoc->GetTxtCollFromPool( RES_POOLCOLL_STANDARD, false );
        pStNd = pDoc->GetNodes().MakeTextSection( aIdx, SwTableBoxStartNode,
                                                 pColl );
        // --> FLR 2005-08-30 #125369#
        // Consider the case that a table is defined without a row.
        if( !pPrevSttNd && pBox1 != NULL )
        // <--
        {
            pBox1->pSttNd = pStNd;
            SwCntntNode *pCNd = pDoc->GetNodes()[ pStNd->GetIndex() + 1 ]
                                                            ->GetCntntNode();
            SwPosition aPos( *pCNd );
            aPos.nContent.Assign( pCNd, 0U );

            Reference < XTextRange > xTextRange =
                SwXTextRange::CreateTextRangeFromPosition( pDoc, aPos, 0 );
            Reference < XText > xText = xTextRange->getText();
            Reference < XTextCursor > xTextCursor =
                xText->createTextCursorByRange( xTextRange );
            GetImport().GetTextImport()->SetCursor( xTextCursor );
        }
    }

    return pStNd;
}

void SwXMLTableContext::EndElement()
{
    if( IsValid() && !xParentTable.Is() )
    {
        MakeTable();
        GetImport().GetTextImport()->SetCursor( xOldCursor );
    }
}

Reference < XTextContent > SwXMLTableContext::GetXTextContent() const
{
    return xTextContent;
}
