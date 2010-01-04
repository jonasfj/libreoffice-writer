/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fmtmeta.hxx,v $
 * $Revision: 1.1 $
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

#ifndef SW_FMTMETA_HXX
#define SW_FMTMETA_HXX

#include <svl/poolitem.hxx>
#include <sfx2/Metadatable.hxx>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <vector>


namespace com { namespace sun { namespace star {
    namespace text {
        class XTextField;
    }
}}}


/**
 * The classes that make up a meta entity are:
 * <dl>
 *   <dt>SwTxtMeta</dt><dd>the text hint</dd>
 *   <dt>SwFmtMeta</dt><dd>the pool item</dd>
 *   <dt>sw::Meta</dt><dd>the metadatable entity itself</dd>
 *   <dt>SwXMeta</dt><dd>the UNO wrapper object</dd>
 * </dl>
 *
 * The text hint contains the pool item (as usual) and has a pointer to the
 * text node at which it is attached.
 * The pool item has a shared pointer to the metadatable entity, and a reverse
 * pointer to the text attribute at which it is attached.
 * The pool item is non-poolable; it may only be attached to one text
 * attribute.
 * Of all the pool items that refer to a metadatable entity, only one may be
 * in the document content at any time. Others may be in the undo array, or in
 * undo objects.
 * The metadatable entity has a reverse pointer to the pool item that is
 * currently in the document. It also registers as a client at the text node
 * at which it is attached via this pool item and its text attribute.
 * The UNO wrapper object registers as a client at the metadatable entity.
 *
 * Copying the metadatable entity proceeds in the following way:
 * <ol>
 *   <li>The pool item is cloned (because it is non-poolable); the clone
 *       points to the same metadatable entity, but the metadatable entity's
 *       reverse pointer is unchanged.</li>
 *   <li>A new text hint is created, taking over the new pool item.
 *       Unfortunately, this also makes the metadatable entity point at the
 *       cloned pool item.</li>
 *   <li>The text hint is inserted into the hints array of some text node.</li>
 *   <li>The DoCopy() method must be called at the new pool item:
 *       it will clone the metadatable entity (using RegisterAsCopyOf),
 *       and fix the reverse pointer of the original to point at the
 *       original pool item.
 *       This is necessary, because first, a metadatable entity may
 *       only be inserted once into a document, and second, the copy may be
 *       inserted into a different document than the source document!</li>
 * </ol>
 */

class SwTxtMeta;
class SwXMeta;
class SwXMetaField;
namespace sw {
    class Meta;
}

class SwFmtMeta
    : public SfxPoolItem
{
private:
    friend class SwTxtMeta; // needs SetTxtAttr
    friend class ::sw::Meta; // needs m_pTxtAttr

    ::boost::shared_ptr< ::sw::Meta > m_pMeta;
    SwTxtMeta * m_pTxtAttr;

    SwTxtMeta * GetTxtAttr() { return m_pTxtAttr; }
    void SetTxtAttr(SwTxtMeta * const i_pTxtAttr);

    explicit SwFmtMeta( const USHORT i_nWhich );

public:
    // takes ownership
    explicit SwFmtMeta( ::boost::shared_ptr< ::sw::Meta > const & i_pMeta,
                        const USHORT i_nWhich );
    virtual ~SwFmtMeta();

    // SfxPoolItem
    virtual int              operator==( const SfxPoolItem & ) const;
    virtual SfxPoolItem *    Clone( SfxItemPool *pPool = 0 ) const;
//    TYPEINFO();

    // notify clients registered at m_pMeta that this meta is being removed
    void NotifyRemoval();
    static SwFmtMeta * CreatePoolDefault( const USHORT i_nWhich );
    ::sw::Meta * GetMeta() { return m_pMeta.get(); }
    /// this method <em>must</em> be called when the hint is actually copied
    void DoCopy( SwFmtMeta & rOriginalMeta );
};


namespace sw {

class MetaFieldManager;

class Meta
    : public ::sfx2::Metadatable
    , public SwModify
{
protected:
    friend class ::SwFmtMeta; // SetFmtMeta
    friend class ::SwXMeta; // GetTxtNode, GetTxtAttr

    SwFmtMeta * m_pFmt;

    SwTxtMeta * GetTxtAttr() const;
    SwTxtNode * GetTxtNode() const; // returns 0 if not in document (undo)

    SwFmtMeta * GetFmtMeta() const { return m_pFmt; }
    void SetFmtMeta( SwFmtMeta * const i_pFmt ) { m_pFmt = i_pFmt; };

public:
    explicit Meta(SwFmtMeta * const i_pFmt = 0);
    virtual ~Meta();

    // SwClient
    virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew );

    // sfx2::Metadatable
    virtual ::sfx2::IXmlIdRegistry& GetRegistry();
    virtual bool IsInClipboard() const;
    virtual bool IsInUndo() const;
    virtual bool IsInContent() const;
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::rdf::XMetadatable > MakeUnoObject();
};

class MetaField
    : public Meta
{
private:
    friend class ::SwFmtMeta;
    friend class ::SwXMetaField;
    friend class ::sw::MetaFieldManager;

    sal_uInt32 m_nNumberFormat;
    bool       m_bIsFixedLanguage;

    sal_uInt32 GetNumberFormat(::rtl::OUString const & rContent) const;
    void SetNumberFormat(sal_uInt32 nNumberFormat);
    bool IsFixedLanguage() const    { return m_bIsFixedLanguage; }
    void SetIsFixedLanguage(bool b) { m_bIsFixedLanguage = b; }

    explicit MetaField(SwFmtMeta * const i_pFmt = 0,
            const sal_uInt32 nNumberFormat = SAL_MAX_UINT32,
            const bool bIsFixedLanguage = false );

public:
    /// get prefix/suffix from the RDF repository. @throws RuntimeException
    void GetPrefixAndSuffix(
        ::rtl::OUString *const o_pPrefix, ::rtl::OUString *const o_pSuffix);
};

/** knows all meta-fields in the document. */
class MetaFieldManager
    : private ::boost::noncopyable
{
private:
    typedef ::std::vector< ::boost::weak_ptr<MetaField> > MetaFieldList_t;
    MetaFieldList_t m_MetaFields;

public:
    MetaFieldManager();
    ::boost::shared_ptr<MetaField> makeMetaField(
                SwFmtMeta * const i_pFmt = 0,
                const sal_uInt32 nNumberFormat = SAL_MAX_UINT32,
                const bool bIsFixedLanguage = false );
    /// get all meta fields
    ::std::vector< ::com::sun::star::uno::Reference<
        ::com::sun::star::text::XTextField> > getMetaFields();
};

} // namespace sw

#endif // SW_FMTMETA_HXX

