/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: usrpref.hxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-07 16:33:48 $
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
#ifndef _USRPREF_HXX
#define _USRPREF_HXX



#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _FLDUPDE_HXX
#include <fldupde.hxx>
#endif
#include "viewopt.hxx"

#ifndef _VCL_FLDUNIT_HXX
#include <vcl/fldunit.hxx>
#endif

/* -----------------------------28.09.00 09:45--------------------------------

 ---------------------------------------------------------------------------*/
class SwMasterUsrPref;
class SwContentViewConfig : public utl::ConfigItem
{
    SwMasterUsrPref& 		rParent;
    BOOL 					bWeb;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
    public:
        SwContentViewConfig(BOOL bWeb, SwMasterUsrPref& rParent);
        ~SwContentViewConfig();

    // utl::ConfigItem
    virtual void    Notify( const com::sun::star::uno::Sequence< rtl::OUString > &rPropertyNames );
    virtual void    Commit();

    void                    Load();
    void 					SetModified(){ConfigItem::SetModified();}
};
/* -----------------------------28.09.00 09:45--------------------------------

 ---------------------------------------------------------------------------*/
class SwLayoutViewConfig : public utl::ConfigItem
{
    SwMasterUsrPref& 	rParent;
    BOOL 				bWeb;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
    public:
        SwLayoutViewConfig(BOOL bWeb, SwMasterUsrPref& rParent);
        ~SwLayoutViewConfig();

    virtual void			Commit();
    void					Load();
    void 					SetModified(){ConfigItem::SetModified();}
};
/* -----------------------------19.01.01 13:06--------------------------------

 ---------------------------------------------------------------------------*/
class SwGridConfig : public utl::ConfigItem
{
    SwMasterUsrPref& 	rParent;
    BOOL 				bWeb;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
    public:
        SwGridConfig(BOOL bWeb, SwMasterUsrPref& rParent);
        ~SwGridConfig();

    virtual void			Commit();
    void					Load();
    void 					SetModified(){ConfigItem::SetModified();}
};
/* -----------------------------19.01.01 13:06--------------------------------

 ---------------------------------------------------------------------------*/
class SwCursorConfig : public utl::ConfigItem
{
    SwMasterUsrPref& 	rParent;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
    public:
        SwCursorConfig(SwMasterUsrPref& rParent);
        ~SwCursorConfig();

    virtual void			Commit();
    void					Load();
    void 					SetModified(){ConfigItem::SetModified();}
};
/* -----------------------------28.09.00 09:45--------------------------------

 ---------------------------------------------------------------------------*/
class SwWebColorConfig : public utl::ConfigItem
{
    SwMasterUsrPref& 		rParent;
    com::sun::star::uno::Sequence<rtl::OUString> aPropNames;

    public:
        SwWebColorConfig(SwMasterUsrPref& rParent);
        ~SwWebColorConfig();

    virtual void			Commit();
    void					Load();
    void 					SetModified(){ConfigItem::SetModified();}
};
/* -----------------------------28.09.00 09:45--------------------------------

 ---------------------------------------------------------------------------*/
class SwMasterUsrPref : public SwViewOption
{
    friend class SwContentViewConfig;
    friend class SwLayoutViewConfig;
    friend class SwGridConfig;
    friend class SwCursorConfig;
    friend class SwWebColorConfig;

    SwFldUpdateFlags eFldUpdateFlags;    //udpate of fields and charts
    sal_Int32	nLinkUpdateMode;
    FieldUnit	eUserMetric;
    FieldUnit   eHScrollMetric;
    sal_Bool    bIsHScrollMetricSet;
    FieldUnit   eVScrollMetric;
    sal_Bool    bIsVScrollMetricSet;

    sal_Int32	nDefTab;			//default tab stop distance

    sal_Bool    bIsSquaredPageMode; //default page mode for text grid
    SwContentViewConfig aContentConfig;
    SwLayoutViewConfig  aLayoutConfig;
    SwGridConfig        aGridConfig;
    SwCursorConfig      aCursorConfig;
    SwWebColorConfig*   pWebColorConfig;

public:
    SwMasterUsrPref(BOOL bWeb);
    ~SwMasterUsrPref();

    void SetUsrPref(const SwViewOption &rCopy);

    void Commit()
        {
            aContentConfig.Commit();
            aLayoutConfig.Commit();
            aGridConfig.Commit();
            aCursorConfig.Commit();
            if(pWebColorConfig)
                pWebColorConfig->Commit();
        }
    void SetModified()
        {
            aContentConfig.SetModified();
            aLayoutConfig.SetModified();
            aGridConfig.SetModified();
            aCursorConfig.SetModified();
            if(pWebColorConfig)
                pWebColorConfig->SetModified();
        }

    void SetUpdateLinkMode(sal_Int32 nSet, sal_Bool bNoModify = sal_False)
        {
            nLinkUpdateMode = nSet;
            if(!bNoModify)
                aContentConfig.SetModified();
        }
    sal_Int32 GetUpdateLinkMode() const {return nLinkUpdateMode; }

    void SetUpdateFields(BOOL bSet, sal_Bool bNoModify = sal_False)
        {
            if(bSet && eFldUpdateFlags == AUTOUPD_OFF)
            {
                eFldUpdateFlags = AUTOUPD_FIELD_ONLY;
                if(!bNoModify)
                    aContentConfig.SetModified();
             }
            else if(!bSet)
            {
                eFldUpdateFlags = AUTOUPD_OFF;
                if(!bNoModify)
                    aContentConfig.SetModified();
            }
        };
    sal_Bool IsUpdateFields()const {return eFldUpdateFlags != AUTOUPD_OFF; }

    SwFldUpdateFlags   GetFldUpdateFlags()const {return eFldUpdateFlags;}
    void        SetFldUpdateFlags(SwFldUpdateFlags eSet, sal_Bool bNoModify = sal_False)
        {
            eFldUpdateFlags = eSet;
            if(!bNoModify)
                aContentConfig.SetModified();
        }

    void SetUpdateCharts(BOOL bSet, sal_Bool bNoModify = sal_False)
        {
            if(bSet)
            {
                eFldUpdateFlags = AUTOUPD_FIELD_AND_CHARTS;
                if(!bNoModify)
                    aContentConfig.SetModified();
             }
             else if(eFldUpdateFlags == AUTOUPD_FIELD_AND_CHARTS)
             {
                eFldUpdateFlags = AUTOUPD_FIELD_ONLY;
                if(!bNoModify)
                    aContentConfig.SetModified();
             }
        };
    sal_Bool IsUpdateCharts()const {return eFldUpdateFlags == AUTOUPD_FIELD_AND_CHARTS; }

    FieldUnit	GetMetric() const { return eUserMetric;}
    void        SetMetric(FieldUnit eSet, sal_Bool bNoModify = sal_False)
                {
                    eUserMetric = eSet;
                    if(!bNoModify)
                        aLayoutConfig.SetModified();
                }

    sal_Bool    IsHScrollMetric()const {return bIsHScrollMetricSet;}
    FieldUnit   GetHScrollMetric() const { return bIsHScrollMetricSet ? eHScrollMetric : eUserMetric;}
    void        SetHScrollMetric(FieldUnit eSet, sal_Bool bNoModify = sal_False)
                {
                    eHScrollMetric = eSet; bIsHScrollMetricSet = sal_True;
                    if(!bNoModify)
                        aLayoutConfig.SetModified();
                }

    sal_Bool    IsVScrollMetric()const {return bIsVScrollMetricSet;}
    FieldUnit   GetVScrollMetric() const { return bIsVScrollMetricSet ? eVScrollMetric : eUserMetric;}
    void        SetVScrollMetric(FieldUnit eSet, sal_Bool bNoModify = sal_False)
                {
                    eVScrollMetric = eSet; bIsVScrollMetricSet = sal_True;
                    if(!bNoModify)
                        aLayoutConfig.SetModified();
                }

    sal_Int32   GetDefTab() const { return nDefTab;}
    void        SetDefTab( sal_Int32  nSet, sal_Bool bNoModify = sal_False )
                {
                    nDefTab = nSet;
                    if(!bNoModify)
                        aLayoutConfig.SetModified();
                }
    
    //default page mode for text grid
    sal_Bool    IsSquaredPageMode() const {return bIsSquaredPageMode;}
    void        SetDefaultPageMode( sal_Bool bVal, sal_Bool bNoModify = sal_False ) 
                { 
                    bIsSquaredPageMode = bVal;
                    if(!bNoModify)
                        aLayoutConfig.SetModified(); 
                }

};

#endif

