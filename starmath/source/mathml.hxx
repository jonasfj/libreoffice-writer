/*************************************************************************
 *
 *  $RCSfile: mathml.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: cmc $ $Date: 2001-01-18 14:57:19 $
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
#ifndef MATHML_HXX
#define MATHML_HXX
#ifndef _SDXMLIMP_HXX
#include <xmloff/xmlimp.hxx>
#endif
#ifndef _SDXMLEXP_HXX
#include <xmloff/xmlexp.hxx>
#endif


#ifndef NODE_HXX
#include <node.hxx>
#endif

class SfxMedium;

class SmXMLWrapper
{
public:
    SmXMLWrapper(com::sun::star::uno::Reference<com::sun::star::frame::XModel> &rRef) : rModel(rRef) {};
    SmNode *SmXMLWrapper::Import(SfxMedium &rMedium);
    sal_Bool Export(SfxMedium &rMedium,SmNode *pTree);
private:
    com::sun::star::uno::Reference<com::sun::star::frame::XModel> &rModel;

};


class SmXMLImport : public SvXMLImport
{
public:
    SmXMLImport(
        com::sun::star::uno::Reference<com::sun::star::frame::XModel> &rModel,
        const rtl::OUString &rFileName) : SvXMLImport(rModel) ,
        pMathElemTokenMap(0), pPresLayoutElemTokenMap(0), pPresElemTokenMap(0),
        pPresScriptEmptyElemTokenMap(0), pPresTableElemTokenMap(0),
        pPresLayoutAttrTokenMap(0),pFencedAttrTokenMap(0),
        pOperatorAttrTokenMap(0),pColorTokenMap(0)
        {}
    SvXMLImportContext *CreateContext(sal_uInt16 nPrefix, 
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateMathContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateRowContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateFracContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateNumberContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateTextContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateStringContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateIdentifierContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateOperatorContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateSpaceContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateSqrtContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateRootContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateStyleContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreatePaddedContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreatePhantomContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateFencedContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateErrorContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateSubContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateSupContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateSubSupContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateUnderContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateOverContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateUnderOverContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateMultiScriptsContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateNoneContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreatePrescriptsContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateTableContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateTableRowContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateTableCellContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateAlignGroupContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    SvXMLImportContext *CreateActionContext(sal_uInt16 nPrefix,
        const rtl::OUString &rLocalName,
        const com::sun::star::uno::Reference <
        com::sun::star::xml::sax::XAttributeList> &xAttrList);
    const SvXMLTokenMap &GetMathElemTokenMap();
    const SvXMLTokenMap &GetPresLayoutElemTokenMap();
    const SvXMLTokenMap &GetPresLayoutAttrTokenMap();
    const SvXMLTokenMap &GetFencedAttrTokenMap();
    const SvXMLTokenMap &GetOperatorAttrTokenMap();
    const SvXMLTokenMap &GetPresElemTokenMap();
    const SvXMLTokenMap &GetPresScriptEmptyElemTokenMap();
    const SvXMLTokenMap &GetPresTableElemTokenMap();
    const SvXMLTokenMap &GetColorTokenMap();
    virtual ~SmXMLImport();
    SmNodeStack & GetNodeStack() {return aNodeStack;}
    SmNode *GetTree() { return aNodeStack.Pop();}
private:
        SvXMLTokenMap *pMathElemTokenMap;
        SvXMLTokenMap *pPresLayoutElemTokenMap;
        SvXMLTokenMap *pPresLayoutAttrTokenMap;
        SvXMLTokenMap *pFencedAttrTokenMap;
        SvXMLTokenMap *pOperatorAttrTokenMap;
        SvXMLTokenMap *pPresElemTokenMap;
        SvXMLTokenMap *pPresScriptEmptyElemTokenMap;
        SvXMLTokenMap *pPresTableElemTokenMap;
        SvXMLTokenMap *pColorTokenMap;

        SmNodeStack aNodeStack;
};

enum SmXMLMathElemTokenMap
{
    XML_TOK_MATH
};

enum SmXMLPresLayoutElemTokenMap
{
    XML_TOK_MSTYLE,
    XML_TOK_MERROR,
    XML_TOK_MPHANTOM,
    XML_TOK_MROW,
    XML_TOK_MFRAC,
    XML_TOK_MSQRT,
    XML_TOK_MROOT,
    XML_TOK_MSUB,
    XML_TOK_MSUP,
    XML_TOK_MSUBSUP,
    XML_TOK_MMULTISCRIPTS,
    XML_TOK_MUNDER,
    XML_TOK_MOVER,
    XML_TOK_MUNDEROVER,
    XML_TOK_MTABLE,
    XML_TOK_MACTION,
    XML_TOK_MFENCED,
    XML_TOK_MPADDED
};

enum SmXMLPresLayoutAttrTokenMap
{
    XML_TOK_FONTWEIGHT,
    XML_TOK_FONTSTYLE,
    XML_TOK_FONTSIZE,
    XML_TOK_FONTFAMILY,
    XML_TOK_COLOR
};


enum SmXMLFencedAttrTokenMap
{
    XML_TOK_OPEN,
    XML_TOK_CLOSE
};


enum SmXMLPresTableElemTokenMap
{
    XML_TOK_MTR,
    XML_TOK_MTD
};

enum SmXMLPresElemTokenMap
{
    XML_TOK_MI,
    XML_TOK_MN,
    XML_TOK_MO,
    XML_TOK_MTEXT,
    XML_TOK_MSPACE,
    XML_TOK_MS,
    XML_TOK_MALIGNGROUP
};

enum SmXMLPresScriptEmptyElemTokenMap
{
    XML_TOK_MPRESCRIPTS,
    XML_TOK_NONE
};

enum SmXMLOperatorAttrTokenMap
{
    XML_TOK_STRETCHY
};

class SmXMLExport : public SvXMLExport
{
public:
    SmXMLExport(const SmNode *pIn,const rtl::OUString &rFileName,
        com::sun::star::uno::Reference<
        com::sun::star::xml::sax::XDocumentHandler> &rHandler) :
        SvXMLExport(rFileName,rHandler), pTree(pIn) {}
    virtual ~SmXMLExport() {};

    void _ExportAutoStyles() {}
    void _ExportMasterStyles() {}
    void _ExportContent();
    sal_uInt32 exportDoc(const sal_Char *pClass);

protected:
    void ExportNodes(const SmNode *pIn,int nLevel);
    void ExportTable(const SmNode *pIn,int nLevel);
    void ExportLine(const SmNode *pNode,int nLevel);
    void ExportExpression(const SmNode *pNode,int nLevel);
    void ExportText(const SmNode *pNode, int nLevel);
    void ExportMath(const SmNode *pNode, int nLevel);
    void ExportBinaryHorizontal(const SmNode *pNode,int nLevel);
    void ExportUnaryHorizontal(const SmNode *pNode,int nLevel);
    void ExportBrace(const SmNode *pNode, int nLevel);
    void ExportBinaryVertical(const SmNode *pNode,int nLevel);
    void ExportSubSupScript(const SmNode *pNode,int nLevel);
    void ExportRoot(const SmNode *pNode, int nLevel);
    void ExportOperator(const SmNode *pNode, int nLevel);
    void ExportAttributes(const SmNode *pNode, int nLevel);
    void ExportFont(const SmNode *pNode, int nLevel);
    void ExportVerticalBrace(const SmNode *pNode, int nLevel);
    void ExportMatrix(const SmNode *pNode, int nLevel);
private:
    const SmNode *pTree;
};
#endif
