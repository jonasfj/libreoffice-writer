/************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlimp.hxx,v $
 *
 *  $Revision: 1.31 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:22:13 $
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

#ifndef _XMLIMP_HXX
#define _XMLIMP_HXX

#include <sot/storage.hxx>

#ifndef _XMLOFF_XMLICTXT_HXX
#include <xmloff/xmlictxt.hxx>
#endif

#ifndef _XMLITMAP_HXX
#include "xmlitmap.hxx"
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include <xmloff/xmlimp.hxx>
#endif

class SwDoc;
class SwPaM;
class SvXMLUnitConverter;
class SvXMLTokenMap;
class SvXMLImportItemMapper;
class SfxItemSet;
class SwNodeIndex;
class XMLTextImportHelper;
class SvXMLGraphicHelper;
class SvXMLEmbeddedObjectHelper;

// define, how many steps ( = paragraphs ) the progress bar should advance
// for styles, autostyles and settings + meta
#define PROGRESS_BAR_STEP 20

class SwXMLImport: public SvXMLImport
{
    SwNodeIndex				*pSttNdIdx;

    SvXMLUnitConverter		*pTwipUnitConv;
    SvXMLImportItemMapper	*pTableItemMapper;// paragraph item import
    SvXMLTokenMap			*pDocElemTokenMap;
    SvXMLTokenMap			*pTableElemTokenMap;
    SvXMLTokenMap			*pTableCellAttrTokenMap;
    SvXMLGraphicHelper 		*pGraphicResolver;
    SvXMLEmbeddedObjectHelper	*pEmbeddedResolver;

    SvXMLItemMapEntriesRef 	xTableItemMap;
    SvXMLItemMapEntriesRef 	xTableColItemMap;
    SvXMLItemMapEntriesRef 	xTableRowItemMap;
    SvXMLItemMapEntriesRef 	xTableCellItemMap;
    SvStorageRef			xPackage;

    sal_uInt16				nStyleFamilyMask;// Mask of styles to load
    sal_Bool				bLoadDoc : 1;	// Load doc or styles only
    sal_Bool				bInsert : 1;	// Insert mode. If styles are
                                            // loaded only sal_False means that
                                            // existing styles will be
                                            // overwritten.
    sal_Bool				bBlock : 1;		// Load text block
    sal_Bool				bAutoStylesValid : 1;
    sal_Bool				bShowProgress : 1;
    sal_Bool				bOrganizerMode : 1;
    sal_Bool				bPreserveRedlineMode;

    void					_InitItemImport();
    void					_FinitItemImport();
    void 					UpdateTxtCollConditions( SwDoc *pDoc );

    void		 setTextInsertMode(
                     const ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XTextRange > & rInsertPos );
    void		 setStyleInsertMode( sal_uInt16 nFamilies,
                                     sal_Bool bOverwrite );
    void		 setBlockMode();
    void		 setOrganizerMode();


protected:

    // This method is called after the namespace map has been updated, but
    // before a context for the current element has been pushed.
    virtual SvXMLImportContext *CreateContext( sal_uInt16 nPrefix,
                  const ::rtl::OUString& rLocalName,
                  const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

    virtual XMLTextImportHelper* CreateTextImport();

    virtual XMLShapeImportHelper* CreateShapeImport();

public:

    // #110680#
    SwXMLImport(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
        sal_uInt16 nImportFlags = IMPORT_ALL);

#ifdef XML_CORE_API
    // #110680#
    SwXMLImport( 
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
        SwDoc& rDoc, 
        const SwPaM& rPaM, 
        sal_Bool bLoadDoc,
        sal_Bool bInsertMode, 
        sal_uInt16 nStyleFamMask,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > & rModel,
        const ::com::sun::star::uno::Reference< ::com::sun::star::document::XGraphicObjectResolver > &,
        SvStorage *pPkg );
#endif

    ~SwXMLImport() throw();

    // ::com::sun::star::xml::sax::XDocumentHandler
    virtual void SAL_CALL startDocument(void)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL endDocument(void)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );

    // XUnoTunnel
    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // XServiceInfo (override parent method)
    ::rtl::OUString SAL_CALL getImplementationName()
        throw( ::com::sun::star::uno::RuntimeException );

    void					InsertStyles( sal_Bool bAuto );
    void					FinishStyles();

    // namespace office
    SvXMLImportContext *CreateMetaContext( const ::rtl::OUString& rLocalName );
    SvXMLImportContext *CreateScriptContext( const ::rtl::OUString& rLocalName );
    SvXMLImportContext *CreateStylesContext(
                const ::rtl::OUString& rLocalName,
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
                sal_Bool bAuto );
    SvXMLImportContext *CreateMasterStylesContext(
                const ::rtl::OUString& rLocalName,
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    SvXMLImportContext *CreateFontDeclsContext(
            const ::rtl::OUString& rLocalName,
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    SvXMLImportContext *CreateBodyContentContext( const ::rtl::OUString& rLocalName );
    sal_uInt16 GetStyleFamilyMask() const { return nStyleFamilyMask; }
    sal_Bool IsInsertMode() const { return bInsert; }
    sal_Bool IsStylesOnlyMode() const { return !bLoadDoc; }
    sal_Bool IsBlockMode() const { return bBlock; }
    sal_Bool IsOrganizerMode() const { return bOrganizerMode; }

    inline const SvXMLUnitConverter& GetTwipUnitConverter() const;
    inline const SvXMLImportItemMapper& GetTableItemMapper() const;
    SvXMLImportContext *CreateTableItemImportContext( sal_uInt16 nPrefix,
                const ::rtl::OUString& rLocalName,
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
                sal_uInt16 nSubFamily, SfxItemSet& rItemSet );

    const SvXMLTokenMap& GetDocElemTokenMap();
    const SvXMLTokenMap& GetTableElemTokenMap();
    const SvXMLTokenMap& GetTableCellAttrTokenMap();

    sal_Bool FindAutomaticStyle( sal_uInt16 nFamily,
                             const ::rtl::OUString& rName,
                             const SfxItemSet **ppItemSet=0,
                             ::rtl::OUString *pParent=0 ) const;

    virtual void SetStatisticAttributes(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttribs);
    virtual void SetViewSettings(const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aViewProps);
    virtual void SetConfigurationSettings(const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aConfigProps);

    SvStorage *GetPackage() { return &xPackage; }

    void SetProgressValue( sal_Int32 nValue );

    // initialize XForms
    virtual void initXForms();
};

inline const SvXMLUnitConverter& SwXMLImport::GetTwipUnitConverter() const
{
    return *pTwipUnitConv;
}

inline const SvXMLImportItemMapper& SwXMLImport::GetTableItemMapper() const
{
    return *pTableItemMapper;
}

inline void SwXMLImport::SetProgressValue( sal_Int32 nValue )
{
    if ( bShowProgress ) 
        GetProgressBarHelper()->SetValue(nValue); 
}

#endif	//  _XMLIMP_HXX
