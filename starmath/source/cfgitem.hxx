/*************************************************************************
 *
 *  $RCSfile: cfgitem.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: tl $ $Date: 2001-05-07 11:55:58 $
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

#pragma hdrstop

#ifndef _MATH_CFGITEM_HXX_
#define _MATH_CFGITEM_HXX_

#include <com/sun/star/beans/PropertyValues.hpp>
#include <com/sun/star/uno/Sequence.hxx>

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

#include <tools/solar.h>

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _SVARRAY_HXX
#include <svtools/svarray.hxx>
#endif
#ifndef _SV_TIMER_HXX
#include <vcl/timer.hxx>
#endif

#include <symbol.hxx>

using namespace com::sun::star;

class SmSym;
class SmFormat;
class Font;
struct SmCfgOther;

/////////////////////////////////////////////////////////////////

enum SmPrintSize { PRINT_SIZE_NORMAL, PRINT_SIZE_SCALED, PRINT_SIZE_ZOOMED };

/////////////////////////////////////////////////////////////////

class SmMathConfigItem : public utl::ConfigItem
{
    // disallow copy-constructor and assignment-operator for now
    SmMathConfigItem( const SmMathConfigItem & );
    SmMathConfigItem & operator = ( const SmMathConfigItem & );

public:
    inline SmMathConfigItem( const rtl::OUString &rPath, 
                      sal_Int16 nMode = CONFIG_MODE_IMMEDIATE_UPDATE );

    uno::Sequence< rtl::OUString > GetFormatPropertyNames();
    uno::Sequence< rtl::OUString > GetOtherPropertyNames();

    uno::Sequence< uno::Any >
        GetProperties( const uno::Sequence< rtl::OUString > &rNames )
        { 
            return ConfigItem::GetProperties(rNames); 
        }

    sal_Bool 
        PutProperties( const uno::Sequence< rtl::OUString > &rNames,
                       const uno::Sequence< uno::Any > &rValues)
        {
            return ConfigItem::PutProperties( rNames, rValues);
        }

    sal_Bool 
        SetSetProperties( const rtl::OUString &rNode, 
                          uno::Sequence< beans::PropertyValue > rValues )
        {
            return ConfigItem::SetSetProperties( rNode, rValues );
        }

    uno::Sequence< rtl::OUString >
        GetNodeNames( const rtl::OUString &rNode )
        {
            return ConfigItem::GetNodeNames( rNode );
        }
    
    sal_Bool
        ReplaceSetProperties( const rtl::OUString& rNode, 
                              uno::Sequence< beans::PropertyValue > rValues )
        {
            return ConfigItem::ReplaceSetProperties( rNode, rValues );
        }
};


inline SmMathConfigItem::SmMathConfigItem( 
        const rtl::OUString &rPath, 
        sal_Int16 nMode ) : 
    ConfigItem( rPath, nMode )
{
}


/////////////////////////////////////////////////////////////////


struct SmFontFormat
{
    String      aName;
    INT16       nCharSet;
    INT16       nFamily;
    INT16       nPitch;
    INT16       nWeight;
    INT16       nItalic;

    SmFontFormat();
    SmFontFormat( const Font &rFont );

    const Font      GetFont() const;
    BOOL            operator == ( const SmFontFormat &rFntFmt ) const;
};


struct SmFntFmtListEntry
{
    String          aId;
    SmFontFormat    aFntFmt;

    SmFntFmtListEntry( const String &rId, const SmFontFormat &rFntFmt );
};


SV_DECL_OBJARR( SmFntFmtListEntryArr, SmFntFmtListEntry, 8, 8 );


class SmFontFormatList
{
    SmFntFmtListEntryArr    aEntries;

    // disallow copy-constructor and assignment-operator for now
    SmFontFormatList( const SmFontFormatList & );
    SmFontFormatList & operator = ( const SmFontFormatList & );

public:
    SmFontFormatList();

    void    Clear();
    void    AddFontFormat( const String &rFntFmtId, const SmFontFormat &rFntFmt );
    
    const SmFontFormat *    GetFontFormat( const String &rFntFmtId ) const;
    const String            GetFontFormatId( const SmFontFormat &rFntFmt ) const;
    const String            GetNewFontFormatId() const;
    USHORT                  GetCount() const    { return aEntries.Count(); }
};


/////////////////////////////////////////////////////////////////

class SmMathConfig
{
    Timer               aSaveTimer;
    SmFormat *          pFormat;
    SmCfgOther *        pOther;
    SmFontFormatList *  pFontFormatList;
    SmSym *             pSymbols;
    USHORT              nSymbolCount;
    BOOL                bIsOtherModified;
    BOOL                bIsFormatModified;

    // disallow copy-constructor and assignment-operator for now
    SmMathConfig( const SmMathConfig & );
    SmMathConfig & operator = ( const SmMathConfig & );
 
    void	LoadSymbols();
    
    void    LoadOther();
    void    SaveOther();
    void    LoadFormat();
    void    SaveFormat();
    void    LoadFontFormatList();
    void    SaveFontFormatList();
    void    UpdateFontFormatList();

    void    Save();
    
    SmSym           ReadSymbol( SmMathConfigItem &rCfg,
                        const rtl::OUString &rSymbolName, 
                        const rtl::OUString &rBaseNode ) const;
    SmFontFormat    ReadFontFormat( SmMathConfigItem &rCfg,
                        const rtl::OUString &rSymbolName, 
                        const rtl::OUString &rBaseNode ) const;

    SetOtherIfNotEqual( BOOL &rbItem, BOOL bNewVal );

    DECL_LINK( TimeOut, Timer * );

protected:
    void        SetOtherModified( BOOL bVal );
    inline BOOL IsOtherModified() const     { return bIsOtherModified; }
    void        SetFormatModified( BOOL bVal );
    inline BOOL IsFormatModified() const    { return bIsFormatModified; }

    SmFontFormatList &          GetFontFormatList();
    const SmFontFormatList &    GetFontFormatList() const
    {
        return ((SmMathConfig *) this)->GetFontFormatList();
    }

public:
    SmMathConfig();
    ~SmMathConfig();

    USHORT			GetSymbolCount() const;
    const SmSym *	GetSymbol( USHORT nIndex ) const;
    void            ReplaceSymbols( const SmSym *pNewSymbols[], USHORT nCount );

    const SmFormat &    GetStandardFormat() const;
    void                SetStandardFormat( const SmFormat &rFormat );
    
    BOOL            IsPrintTitle() const;
    void            SetPrintTitle( BOOL bVal );
    BOOL            IsPrintFormulaText() const;
    void            SetPrintFormulaText( BOOL bVal );
    BOOL            IsPrintFrame() const;
    void            SetPrintFrame( BOOL bVal );
    SmPrintSize     GetPrintSize() const;
    void            SetPrintSize( SmPrintSize eSize );
    USHORT          GetPrintZoomFactor() const;
    void            SetPrintZoomFactor( USHORT nVal );

    BOOL            IsIgnoreSpacesRight() const;
    void            SetIgnoreSpacesRight( BOOL bVal );
    BOOL            IsToolboxVisible() const;
    void            SetToolboxVisible( BOOL bVal );
    BOOL            IsAutoRedraw() const;
    void            SetAutoRedraw( BOOL bVal );
    BOOL            IsShowFormulaCursor() const;
    void            SetShowFormulaCursor( BOOL bVal );
    BOOL            IsNoSymbolsWarning() const;
    void            SetNoSymbolsWarning( BOOL bVal );
};

/////////////////////////////////////////////////////////////////

#endif

