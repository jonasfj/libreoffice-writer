/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: node.hxx,v $
 * $Revision: 1.19 $
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


#ifndef NODE_HXX
#define NODE_HXX


#include <vector>

#include "parse.hxx"
#include "types.hxx"
#include "rect.hxx"
#include "format.hxx"


#define ATTR_BOLD		0x0001
#define ATTR_ITALIC		0x0002

#define FNTSIZ_ABSOLUT	1
#define FNTSIZ_PLUS 	2
#define FNTSIZ_MINUS	3
#define FNTSIZ_MULTIPLY 4
#define FNTSIZ_DIVIDE	5

// flags to interdict respective status changes
#define FLG_FONT		0x0001
#define FLG_SIZE		0x0002
#define FLG_BOLD   		0x0004
#define FLG_ITALIC		0x0008
#define FLG_COLOR		0x0010
#define FLG_VISIBLE		0x0020
#define FLG_HORALIGN	0x0040


extern SmFormat	*pActiveFormat;

class SmDocShell;
class SmNode;
class SmStructureNode;

typedef std::vector< SmNode * > SmNodeArray;
typedef std::vector< SmStructureNode * > SmStructureNodeArray;


////////////////////////////////////////////////////////////////////////////////

enum SmScaleMode	{ SCALE_NONE, SCALE_WIDTH, SCALE_HEIGHT };

enum SmNodeType
{
    NTABLE,			NBRACE,			NBRACEBODY,		NOPER,			NALIGN,
    NATTRIBUT,		NFONT,			NUNHOR,			NBINHOR,		NBINVER,
    NBINDIAGONAL,	NSUBSUP,		NMATRIX,		NPLACE,			NTEXT,
    NSPECIAL,		NGLYPH_SPECIAL,	NMATH,			NBLANK,			NERROR,
    NLINE,          NEXPRESSION,    NPOLYLINE,      NROOT,          NROOTSYMBOL,    
    NRECTANGLE,     NVERTICAL_BRACE
};


////////////////////////////////////////////////////////////////////////////////


class SmNode : public SmRect
{
    SmFace          aFace;

    SmToken			aNodeToken;
    SmNodeType		eType;
    SmScaleMode		eScaleMode;
    RectHorAlign	eRectHorAlign;
    USHORT			nFlags,
                    nAttributes;
    BOOL			bIsPhantom,
                    bIsDebug;
protected:
    SmNode(SmNodeType eNodeType, const SmToken &rNodeToken);

    // index in accessible text -1 if not (yet) applicable
    sal_Int32       nAccIndex;

public:
    virtual 			~SmNode();

    virtual BOOL        IsVisible() const;

    virtual USHORT      GetNumSubNodes() const;
    virtual SmNode *    GetSubNode(USHORT nIndex);
            const SmNode * GetSubNode(USHORT nIndex) const
            {
                return ((SmNode *) this)->GetSubNode(nIndex);
            }

    virtual SmNode * 	   GetLeftMost();
            const SmNode * GetLeftMost() const
            {
                return ((SmNode *) this)->GetLeftMost();
            }

            USHORT &	Flags() { return nFlags; }
            USHORT &	Attributes() { return nAttributes; }

            BOOL IsDebug() const { return bIsDebug; }
            BOOL IsPhantom() const { return bIsPhantom; }
            void SetPhantom(BOOL bIsPhantom);
            void SetColor(const Color &rColor);

            void SetAttribut(USHORT nAttrib);
            void ClearAttribut(USHORT nAttrib);

            const SmFace & GetFont() const { return aFace; };
                  SmFace & GetFont()  	   { return aFace; };

            void SetFont(const SmFace &rFace);
            void SetFontSize(const Fraction &rRelSize, USHORT nType);
            void SetSize(const Fraction &rScale);

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void PrepareAttributes();

#if OSL_DEBUG_LEVEL
            void ToggleDebug() const;
#endif

    void         SetRectHorAlign(RectHorAlign eHorAlign, BOOL bApplyToSubTree = TRUE );
    RectHorAlign GetRectHorAlign() const { return eRectHorAlign; }

    const SmRect & GetRect() const { return *this; }
          SmRect & GetRect() 	   { return *this; }

    virtual void Move(const Point &rPosition);
    void MoveTo(const Point &rPosition) { Move(rPosition - GetTopLeft()); }
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    virtual void CreateTextFromNode(String &rText);

#ifdef SM_RECT_DEBUG
    using   SmRect::Draw;
#endif
    virtual void Draw(OutputDevice &rDev, const Point &rPosition) const;

    virtual void    GetAccessibleText( String &rText ) const;
    sal_Int32       GetAccessibleIndex() const { return nAccIndex; }
    const SmNode *  FindNodeWithAccessibleIndex(xub_StrLen nAccIndex) const;

    USHORT	GetRow() const	  { return (USHORT)aNodeToken.nRow; }
    USHORT	GetColumn() const { return (USHORT)aNodeToken.nCol; }

    SmScaleMode		GetScaleMode() const { return eScaleMode; }
    void			SetScaleMode(SmScaleMode eMode) { eScaleMode = eMode; }

    virtual void AdaptToX(const OutputDevice &rDev, ULONG nWidth);
    virtual void AdaptToY(const OutputDevice &rDev, ULONG nHeight);

    SmNodeType		GetType() const  { return eType; }
    const SmToken &	GetToken() const { return aNodeToken; }

    const SmNode *	FindTokenAt(USHORT nRow, USHORT nCol) const;
    const SmNode *	FindRectClosestTo(const Point &rPoint) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmStructureNode : public SmNode
{
    SmNodeArray  aSubNodes;

protected:
    SmStructureNode(SmNodeType eNodeType, const SmToken &rNodeToken)
    :	SmNode(eNodeType, rNodeToken)
    {}

public:
            SmStructureNode( const SmStructureNode &rNode );
    virtual ~SmStructureNode();

    virtual BOOL 		IsVisible() const;

    virtual USHORT		GetNumSubNodes() const;
            void        SetNumSubNodes(USHORT nSize) { aSubNodes.resize(nSize); }

    using   SmNode::GetSubNode;
    virtual	SmNode * 	GetSubNode(USHORT nIndex);
            void SetSubNodes(SmNode *pFirst, SmNode *pSecond, SmNode *pThird = NULL);
            void SetSubNodes(const SmNodeArray &rNodeArray);

    virtual SmStructureNode & operator = ( const SmStructureNode &rNode );

    virtual void  GetAccessibleText( String &rText ) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmVisibleNode : public SmNode
{
protected:
    SmVisibleNode(SmNodeType eNodeType, const SmToken &rNodeToken)
    :	SmNode(eNodeType, rNodeToken)
    {}

public:

    virtual BOOL 		IsVisible() const;
    virtual	USHORT		GetNumSubNodes() const;
    using   SmNode::GetSubNode;
    virtual	SmNode * 	GetSubNode(USHORT nIndex);
};


////////////////////////////////////////////////////////////////////////////////


class SmGraphicNode : public SmVisibleNode
{
protected:
    SmGraphicNode(SmNodeType eNodeType, const SmToken &rNodeToken)
    :	SmVisibleNode(eNodeType, rNodeToken)
    {}

public:

    virtual void  GetAccessibleText( String &rText ) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmRectangleNode : public SmGraphicNode
{
    Size  aToSize;

public:
    SmRectangleNode(const SmToken &rNodeToken)
    :	SmGraphicNode(NRECTANGLE, rNodeToken)
    {}

    virtual void AdaptToX(const OutputDevice &rDev, ULONG nWidth);
    virtual void AdaptToY(const OutputDevice &rDev, ULONG nHeight);

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);

#ifdef SM_RECT_DEBUG
    using   SmRect::Draw;
#endif
    virtual void Draw(OutputDevice &rDev, const Point &rPosition) const;
    
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmPolyLineNode : public SmGraphicNode
{
    Polygon		aPoly;
    Size		aToSize;
    long		nWidth;

public:
    SmPolyLineNode(const SmToken &rNodeToken);

    long		 GetWidth() const { return nWidth; }

    virtual void AdaptToX(const OutputDevice &rDev, ULONG nWidth);
    virtual void AdaptToY(const OutputDevice &rDev, ULONG nHeight);

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);

#ifdef SM_RECT_DEBUG
    using   SmRect::Draw;
#endif
    virtual void Draw(OutputDevice &rDev, const Point &rPosition) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmTextNode : public SmVisibleNode
{
    XubString   aText;
    USHORT      nFontDesc;

protected:
    SmTextNode(SmNodeType eNodeType, const SmToken &rNodeToken, USHORT nFontDescP)
    :	SmVisibleNode(eNodeType, rNodeToken)
    {
        nFontDesc = nFontDescP;
    }

public:
    SmTextNode(const SmToken &rNodeToken, USHORT nFontDescP)
    :	SmVisibleNode(NTEXT, rNodeToken)
    {
        nFontDesc = nFontDescP;
    }


    USHORT				GetFontDesc() const { return nFontDesc; }
    void				SetText(const XubString &rText) { aText = rText; }
    const XubString &	GetText() const { return aText; }

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    virtual void CreateTextFromNode(String &rText);

#ifdef SM_RECT_DEBUG
    using   SmRect::Draw;
#endif
    virtual void Draw(OutputDevice &rDev, const Point &rPosition) const;

    virtual void  GetAccessibleText( String &rText ) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmSpecialNode : public SmTextNode
{
protected:
    SmSpecialNode(SmNodeType eNodeType, const SmToken &rNodeToken, USHORT _nFontDesc)
    :   SmTextNode(eNodeType, rNodeToken, _nFontDesc)
    {}

public:
    SmSpecialNode(const SmToken &rNodeToken)
    :	SmTextNode(NSPECIAL, rNodeToken, FNT_MATH)	//! default Font nicht immer richtig
    {}

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);

#ifdef SM_RECT_DEBUG
    using   SmRect::Draw;
#endif
    virtual void Draw(OutputDevice &rDev, const Point &rPosition) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmGlyphSpecialNode : public SmSpecialNode
{
public:
    SmGlyphSpecialNode(const SmToken &rNodeToken)
    :	SmSpecialNode(NGLYPH_SPECIAL, rNodeToken, FNT_MATH)
    {}

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmMathSymbolNode : public SmSpecialNode
{
protected:
    SmMathSymbolNode(SmNodeType eNodeType, const SmToken &rNodeToken)
    :	SmSpecialNode(eNodeType, rNodeToken, FNT_MATH)
    {
        xub_Unicode cChar = GetToken().cMathChar;
        if ((xub_Unicode) '\0' != cChar)
            SetText( cChar );
    }

public:
    SmMathSymbolNode(const SmToken &rNodeToken);

    virtual void AdaptToX(const OutputDevice &rDev, ULONG nWidth);
    virtual void AdaptToY(const OutputDevice &rDev, ULONG nHeight);

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmRootSymbolNode : public SmMathSymbolNode
{
    ULONG  nBodyWidth;  // width of body (argument) of root sign

public:
    SmRootSymbolNode(const SmToken &rNodeToken)
    :   SmMathSymbolNode(NROOTSYMBOL, rNodeToken)
    {}

    virtual void AdaptToX(const OutputDevice &rDev, ULONG nWidth);
    virtual void AdaptToY(const OutputDevice &rDev, ULONG nHeight);

#ifdef SM_RECT_DEBUG
    using   SmRect::Draw;
#endif
    virtual void Draw(OutputDevice &rDev, const Point &rPosition) const;
};


////////////////////////////////////////////////////////////////////////////////


class SmPlaceNode : public SmMathSymbolNode
{
public:
    SmPlaceNode(const SmToken &rNodeToken)
    :	SmMathSymbolNode(NPLACE, rNodeToken)
    {
    }

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmErrorNode : public SmMathSymbolNode
{
public:
    SmErrorNode(SmParseError /*eError*/, const SmToken &rNodeToken)
    :	SmMathSymbolNode(NERROR, rNodeToken)
    {
        SetText((xub_Unicode) MS_ERROR);
    }

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmTableNode : public SmStructureNode
{
public:
    SmTableNode(const SmToken &rNodeToken)
    :	SmStructureNode(NTABLE, rNodeToken)
    {}

    using   SmNode::GetLeftMost;
    virtual SmNode * GetLeftMost();

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmLineNode : public SmStructureNode
{
protected:
    SmLineNode(SmNodeType eNodeType, const SmToken &rNodeToken)
    :	SmStructureNode(eNodeType, rNodeToken)
    {}

public:
    SmLineNode(const SmToken &rNodeToken)
    :	SmStructureNode(NLINE, rNodeToken)
    {}

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmExpressionNode : public SmLineNode
{
public:
    SmExpressionNode(const SmToken &rNodeToken)
    :	SmLineNode(NEXPRESSION, rNodeToken)
    {}

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmUnHorNode : public SmStructureNode
{
public:
    SmUnHorNode(const SmToken &rNodeToken)
    :	SmStructureNode(NUNHOR, rNodeToken)
    {
        SetNumSubNodes(2);
    }

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmRootNode : public SmStructureNode
{
protected:
    void   GetHeightVerOffset(const SmRect &rRect,
                              long &rHeight, long &rVerOffset) const;
    Point  GetExtraPos(const SmRect &rRootSymbol, const SmRect &rExtra) const;

public:
    SmRootNode(const SmToken &rNodeToken)
    :	SmStructureNode(NROOT, rNodeToken)
    {
        SetNumSubNodes(3);
    }

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmBinHorNode : public SmStructureNode
{
public:
    SmBinHorNode(const SmToken &rNodeToken)
    :	SmStructureNode(NBINHOR, rNodeToken)
    {
        SetNumSubNodes(3);
    }

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmBinVerNode : public SmStructureNode
{
public:
    SmBinVerNode(const SmToken &rNodeToken)
    :	SmStructureNode(NBINVER, rNodeToken)
    {
        SetNumSubNodes(3);
    }

    using   SmNode::GetLeftMost;
    virtual SmNode * GetLeftMost();

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmBinDiagonalNode : public SmStructureNode
{
    BOOL	bAscending;

    void 	GetOperPosSize(Point &rPos, Size &rSize,
                           const Point &rDiagPoint, double fAngleDeg) const;

public:
    SmBinDiagonalNode(const SmToken &rNodeToken);

    BOOL 	IsAscending() const { return bAscending; }
    void 	SetAscending(BOOL bVal)  { bAscending = bVal; }

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


// enums used to index sub-/supscripts in the 'aSubNodes' array
// in 'SmSubSupNode'
// See graphic for positions at char:
//
//		CSUP
//
// LSUP H  H RSUP
//		H  H
//		HHHH
//		H  H
// LSUB H  H RSUB
//
//		CSUB
//
enum SmSubSup
{	CSUB, CSUP, RSUB, RSUP, LSUB, LSUP
};

// numbers of entries in the above enum (that is: the number of possible
// sub-/supscripts)
#define SUBSUP_NUM_ENTRIES 6


class SmSubSupNode : public SmStructureNode
{
    BOOL  bUseLimits;

public:
    SmSubSupNode(const SmToken &rNodeToken)
    :	SmStructureNode(NSUBSUP, rNodeToken)
    {
        SetNumSubNodes(1 + SUBSUP_NUM_ENTRIES);
        bUseLimits = FALSE;
    }

    SmNode *	   GetBody()	{ return GetSubNode(0); }
    const SmNode * GetBody() const
    {
        return ((SmSubSupNode *) this)->GetBody();
    }

    void  SetUseLimits(BOOL bVal) { bUseLimits = bVal; }
    BOOL  IsUseLimits() const { return bUseLimits; };

    SmNode * GetSubSup(SmSubSup eSubSup) { return GetSubNode( sal::static_int_cast< USHORT >(1 + eSubSup) ); };

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);

};


////////////////////////////////////////////////////////////////////////////////


class SmBraceNode : public SmStructureNode
{
public:
    SmBraceNode(const SmToken &rNodeToken)
    :	SmStructureNode(NBRACE, rNodeToken)
    {
        SetNumSubNodes(3);
    }

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmBracebodyNode : public SmStructureNode
{
    long  nBodyHeight;

public:
    inline SmBracebodyNode(const SmToken &rNodeToken);

    virtual void	Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    long		    GetBodyHeight() const { return nBodyHeight; }
};


inline SmBracebodyNode::SmBracebodyNode(const SmToken &rNodeToken) :
    SmStructureNode(NBRACEBODY, rNodeToken)
{
    nBodyHeight = 0;
}


////////////////////////////////////////////////////////////////////////////////


class SmVerticalBraceNode : public SmStructureNode
{
public:
    inline SmVerticalBraceNode(const SmToken &rNodeToken);

    virtual void	Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


inline SmVerticalBraceNode::SmVerticalBraceNode(const SmToken &rNodeToken) :
    SmStructureNode(NVERTICAL_BRACE, rNodeToken)
{
    SetNumSubNodes(3);
}


////////////////////////////////////////////////////////////////////////////////


class SmOperNode : public SmStructureNode
{
public:
    SmOperNode(const SmToken &rNodeToken)
    :	SmStructureNode(NOPER, rNodeToken)
    {
        SetNumSubNodes(2);
    }

    SmNode *	   GetSymbol();
    const SmNode * GetSymbol() const
    {
        return ((SmOperNode *) this)->GetSymbol();
    }

    long CalcSymbolHeight(const SmNode &rSymbol, const SmFormat &rFormat) const;

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmAlignNode : public SmStructureNode
{
public:
    SmAlignNode(const SmToken &rNodeToken)
    :	SmStructureNode(NALIGN, rNodeToken)
    {}

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////


class SmAttributNode : public SmStructureNode
{
public:
    SmAttributNode(const SmToken &rNodeToken)
    :	SmStructureNode(NATTRIBUT, rNodeToken)
    {}

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmFontNode : public SmStructureNode
{
    USHORT		nSizeType;
    Fraction	aFontSize;

public:
    SmFontNode(const SmToken &rNodeToken)
    :	SmStructureNode(NFONT, rNodeToken)
    {
        nSizeType = FNTSIZ_MULTIPLY;
        aFontSize = Fraction(1L);
    }

    void SetSizeParameter(const Fraction &rValue, USHORT nType);
    const Fraction & GetSizeParameter() const {return aFontSize;}
    const USHORT& GetSizeType() const {return nSizeType;}

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmMatrixNode : public SmStructureNode
{
    USHORT	nNumRows,
            nNumCols;

public:
    SmMatrixNode(const SmToken &rNodeToken)
    :	SmStructureNode(NMATRIX, rNodeToken)
    {
        nNumRows = nNumCols = 0;
    }

    USHORT GetNumRows() const {return nNumRows;}
    USHORT GetNumCols() const {return nNumCols;}
    void SetRowCol(USHORT nMatrixRows, USHORT nMatrixCols);

    using   SmNode::GetLeftMost;
    virtual SmNode * GetLeftMost();

    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
    void CreateTextFromNode(String &rText);
};


////////////////////////////////////////////////////////////////////////////////


class SmBlankNode : public SmGraphicNode
{
    USHORT	nNum;

public:
    SmBlankNode(const SmToken &rNodeToken)
    :	SmGraphicNode(NBLANK, rNodeToken)
    {
        nNum = 0;
    }

    void         IncreaseBy(const SmToken &rToken);
    void		 Clear() { nNum = 0; }

    virtual void Prepare(const SmFormat &rFormat, const SmDocShell &rDocShell);
    virtual void Arrange(const OutputDevice &rDev, const SmFormat &rFormat);
};


////////////////////////////////////////////////////////////////////////////////

#endif


