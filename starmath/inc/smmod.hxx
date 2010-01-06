/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: smmod.hxx,v $
 * $Revision: 1.18 $
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

#ifndef _SMMOD_HXX
#define _SMMOD_HXX

#ifndef _SV_RESARY_HXX
#include <tools/resary.hxx>
#endif
#include <svl/lstner.hxx>
#include <svtools/colorcfg.hxx>

#include <tools/shl.hxx>
#include <sfx2/module.hxx>

#ifndef _STARMATH_HRC
#include "starmath.hrc"
#endif

#include <unotools/options.hxx>

class SvxErrorHandler;
class SfxObjectFactory;
class SmConfig;
class SmModule;
class SmSymbolManager;

/*************************************************************************
|*
|* This subclass of <SfxModule> (which is a subclass of <SfxShell>) is
|* linked to the DLL. One instance of this class exists while the DLL is
|* loaded.
|*
|* SdModule is like to be compared with the <SfxApplication>-subclass.
|*
|* Remember: Don`t export this class! It uses DLL-internal symbols.
|*
\************************************************************************/

class SvtSysLocale;
class VirtualDevice;

/////////////////////////////////////////////////////////////////

class SmResId : public ResId
{
public:
    SmResId(USHORT nId);
};

class SmNamesArray : public Resource
{
    ResStringArray      aNamesAry;
    LanguageType        nLanguage;

public:
    SmNamesArray( LanguageType nLang, int nRID ) :
        Resource( SmResId(RID_LOCALIZED_NAMES) ),
        aNamesAry   (SmResId( static_cast < USHORT > ( nRID ))),
        nLanguage   (nLang)
    {
        FreeResource();
    }

    LanguageType            GetLanguage() const     { return nLanguage; }
    const ResStringArray&   GetNamesArray() const   { return aNamesAry; }
};

/////////////////////////////////////////////////////////////////

class SmLocalizedSymbolData : public Resource
{
    ResStringArray		aUiSymbolNamesAry;
    ResStringArray		aExportSymbolNamesAry;
    ResStringArray      aUiSymbolSetNamesAry;
    ResStringArray      aExportSymbolSetNamesAry;
    SmNamesArray       *p50NamesAry;
    SmNamesArray       *p60NamesAry;
    LanguageType        n50NamesLang;
    LanguageType        n60NamesLang;

public:
    SmLocalizedSymbolData();
    ~SmLocalizedSymbolData();

    const ResStringArray& GetUiSymbolNamesArray() const     { return aUiSymbolNamesAry; }
    const ResStringArray& GetExportSymbolNamesArray() const { return aExportSymbolNamesAry; }
    const String          GetUiSymbolName( const String &rExportName ) const;
    const String          GetExportSymbolName( const String &rUiName ) const;

    const ResStringArray& GetUiSymbolSetNamesArray() const     { return aUiSymbolSetNamesAry; }
    const ResStringArray& GetExportSymbolSetNamesArray() const { return aExportSymbolSetNamesAry; }
    const String          GetUiSymbolSetName( const String &rExportName ) const;
    const String          GetExportSymbolSetName( const String &rUiName ) const;

    const ResStringArray* Get50NamesArray( LanguageType nLang );
    const ResStringArray* Get60NamesArray( LanguageType nLang );
};

/////////////////////////////////////////////////////////////////

class SmModule : public SfxModule, utl::ConfigurationListener
{
    svtools::ColorConfig        *pColorConfig;
    SmConfig                *pConfig;
    SmLocalizedSymbolData   *pLocSymbolData;
    SvtSysLocale            *pSysLocale;
    VirtualDevice           *pVirtualDev;

    virtual void FillStatusBar(StatusBar &rBar);
    void _CreateSysLocale() const;
    void _CreateVirtualDev() const;

    void ApplyColorConfigValues( const svtools::ColorConfig &rColorCfg );

public:
    TYPEINFO();
    SFX_DECL_INTERFACE(SFX_INTERFACE_SMA_START + 0)

    SmModule(SfxObjectFactory* pObjFact);
    virtual ~SmModule();

    virtual void ConfigurationChanged( utl::ConfigurationBroadcaster*, sal_uInt32 );

    svtools::ColorConfig &  GetColorConfig();

    SmConfig *              GetConfig();
    SmSymbolManager &       GetSymbolManager();

    SmLocalizedSymbolData &   GetLocSymbolData() const;

    void GetState(SfxItemSet&);

    const SvtSysLocale& GetSysLocale() const
    {
        if( !pSysLocale )
            _CreateSysLocale();
        return *pSysLocale;
    }

    VirtualDevice &     GetDefaultVirtualDev()
    {
        if (!pVirtualDev)
            _CreateVirtualDev();
        return *pVirtualDev;
    }

    //virtuelle Methoden fuer den Optionendialog
    virtual SfxItemSet*	 CreateItemSet( USHORT nId );
    virtual void		 ApplyItemSet( USHORT nId, const SfxItemSet& rSet );
    virtual	SfxTabPage*	 CreateTabPage( USHORT nId, Window* pParent, const SfxItemSet& rSet );
};

#define SM_MOD() ( *(SmModule**) GetAppData(SHL_SM) )

#endif                                 // _SDMOD_HXX

