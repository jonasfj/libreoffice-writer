/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unofield.cxx,v $
 * $Revision: 1.106 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <swtypes.hxx>
#include <cmdid.h>
#include <doc.hxx>
#include <hints.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <ndtxt.hxx>
#include <unomap.hxx>
#include <unoprnms.hxx>
#include <unoobj.hxx>
#include <unocoll.hxx>
#include <svx/linkmgr.hxx>
#include <docstat.hxx>
#include <editsh.hxx>
#include <viewsh.hxx>
#include <comphelper/types.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/util/Time.hpp>
#include <com/sun/star/util/DateTimeRange.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <com/sun/star/beans/XPropertyStateChangeListener.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>

//undef to prevent error (from sfx2/docfile.cxx)
#undef SEQUENCE
#include <com/sun/star/text/SetVariableType.hpp>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>
#include <com/sun/star/text/PageNumberType.hpp>
#include <unofield.hxx>
#include <unocrsr.hxx>
#include <authfld.hxx>
#include <flddat.hxx>
#include <dbfld.hxx>
#include <usrfld.hxx>
#include <docufld.hxx>
#include <expfld.hxx>
#include <chpfld.hxx>
#include <flddropdown.hxx>
#include <poolfmt.hxx>
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif
#include <pagedesc.hxx>
#include <docary.hxx>
#include <reffld.hxx>
#include <ddefld.hxx>
#include <SwStyleNameMapper.hxx>
#include <swunohelper.hxx>
#include <unofldmid.h>
#include <scriptinfo.hxx>
#include <tools/datetime.hxx>
#include <tools/urlobj.hxx>
#include <svx/dataaccessdescriptor.hxx>
#define _SVSTDARR_STRINGS
#include <svtools/svstdarr.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <textapi.hxx>
#include <svx/outliner.hxx>
#include <docsh.hxx>

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace nsSwDocInfoSubType;

#define COM_TEXT_FLDMASTER		"com.sun.star.text.FieldMaster."

// case-corrected version of the first part for the service names (see #i67811)
#define COM_TEXT_FLDMASTER_CC   "com.sun.star.text.fieldmaster."

// note: this thing is indexed as an array, so do not insert/remove entries!
static const sal_uInt16 aDocInfoSubTypeFromService[] =
{
    DI_CHANGE | DI_SUB_AUTHOR,	//PROPERTY_MAP_FLDTYP_DOCINFO_CHANGE_AUTHOR
    DI_CHANGE | DI_SUB_DATE,	//PROPERTY_MAP_FLDTYP_DOCINFO_CHANGE_DATE_TIME
    DI_EDIT | DI_SUB_TIME,		//PROPERTY_MAP_FLDTYP_DOCINFO_EDIT_TIME
    DI_COMMENT,					//PROPERTY_MAP_FLDTYP_DOCINFO_DESCRIPTION
    DI_CREATE | DI_SUB_AUTHOR,	//PROPERTY_MAP_FLDTYP_DOCINFO_CREATE_AUTHOR
    DI_CREATE | DI_SUB_DATE,	//PROPERTY_MAP_FLDTYP_DOCINFO_CREATE_DATE_TIME
    0,       					//DUMMY
    0, 		        			//DUMMY
    0, 			        		//DUMMY
    0, 			        		//DUMMY
    DI_CUSTOM,					//PROPERTY_MAP_FLDTYP_DOCINFO_CUSTOM
    DI_PRINT | DI_SUB_AUTHOR,	//PROPERTY_MAP_FLDTYP_DOCINFO_PRINT_AUTHOR
    DI_PRINT | DI_SUB_DATE,		//PROPERTY_MAP_FLDTYP_DOCINFO_PRINT_DATE_TIME
    DI_KEYS,					//PROPERTY_MAP_FLDTYP_DOCINFO_KEY_WORDS
    DI_THEMA,					//PROPERTY_MAP_FLDTYP_DOCINFO_SUBJECT
    DI_TITEL,					//PROPERTY_MAP_FLDTYP_DOCINFO_TITLE
    DI_DOCNO					//PROPERTY_MAP_FLDTYP_DOCINFO_REVISION
};
struct ServiceIdResId
{
    USHORT nResId;
    USHORT nServiceId;
};
static const ServiceIdResId aServiceToRes[] =
{
    {RES_DATETIMEFLD, 	SW_SERVICE_FIELDTYPE_DATETIME				},
    {RES_USERFLD, 		SW_SERVICE_FIELDTYPE_USER                   },
    {RES_SETEXPFLD, 		SW_SERVICE_FIELDTYPE_SET_EXP            }    ,
    {RES_GETEXPFLD, 		SW_SERVICE_FIELDTYPE_GET_EXP            }    ,
    {RES_FILENAMEFLD, 	SW_SERVICE_FIELDTYPE_FILE_NAME              },
    {RES_PAGENUMBERFLD, 	SW_SERVICE_FIELDTYPE_PAGE_NUM           }    ,
    {RES_AUTHORFLD, 		SW_SERVICE_FIELDTYPE_AUTHOR             }    ,
    {RES_CHAPTERFLD, 	SW_SERVICE_FIELDTYPE_CHAPTER                },
    {RES_GETREFFLD, 		SW_SERVICE_FIELDTYPE_GET_REFERENCE      }    ,
    {RES_HIDDENTXTFLD, 	SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT       },
    {RES_POSTITFLD, 		SW_SERVICE_FIELDTYPE_ANNOTATION         }    ,
    {RES_INPUTFLD, 		SW_SERVICE_FIELDTYPE_INPUT                  },
    {RES_MACROFLD, 		SW_SERVICE_FIELDTYPE_MACRO                  },
    {RES_DDEFLD, 		SW_SERVICE_FIELDTYPE_DDE                    },
    {RES_HIDDENPARAFLD, 	SW_SERVICE_FIELDTYPE_HIDDEN_PARA        }    ,
    {RES_DOCINFOFLD, 	SW_SERVICE_FIELDTYPE_DOC_INFO               },
    {RES_TEMPLNAMEFLD, 	SW_SERVICE_FIELDTYPE_TEMPLATE_NAME          },
    {RES_EXTUSERFLD, 	SW_SERVICE_FIELDTYPE_USER_EXT               },
    {RES_REFPAGESETFLD, 	SW_SERVICE_FIELDTYPE_REF_PAGE_SET       }    ,
    {RES_REFPAGEGETFLD, 	SW_SERVICE_FIELDTYPE_REF_PAGE_GET       }    ,
    {RES_JUMPEDITFLD, 	SW_SERVICE_FIELDTYPE_JUMP_EDIT              },
    {RES_SCRIPTFLD, 		SW_SERVICE_FIELDTYPE_SCRIPT             }    ,
    {RES_DBNEXTSETFLD, 	SW_SERVICE_FIELDTYPE_DATABASE_NEXT_SET      },
    {RES_DBNUMSETFLD, 	SW_SERVICE_FIELDTYPE_DATABASE_NUM_SET       },
    {RES_DBSETNUMBERFLD, SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM      } ,
    {RES_DBFLD, 			SW_SERVICE_FIELDTYPE_DATABASE           }    ,
    {RES_DBNAMEFLD,		SW_SERVICE_FIELDTYPE_DATABASE_NAME          },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_PAGE_COUNT             },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT        },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_WORD_COUNT             },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_CHARACTER_COUNT        },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_TABLE_COUNT            },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT   },
    {RES_DOCSTATFLD, 	SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT  },
    {RES_DOCINFOFLD, 	SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR  },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME},
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME       },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION     },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR   },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME},
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM          },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR    },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS       },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT         },
    {RES_DOCINFOFLD,		SW_SERVICE_FIELDTYPE_DOCINFO_TITLE           },
    {RES_INPUTFLD, 		SW_SERVICE_FIELDTYPE_INPUT_USER                  },
    {RES_HIDDENTXTFLD, 	SW_SERVICE_FIELDTYPE_HIDDEN_TEXT                 },
    {RES_AUTHORITY, 	SW_SERVICE_FIELDTYPE_BIBLIOGRAPHY                },
    {RES_COMBINED_CHARS,	SW_SERVICE_FIELDTYPE_COMBINED_CHARACTERS	 },
    {RES_DROPDOWN,	SW_SERVICE_FIELDTYPE_DROPDOWN	                     },
    {RES_TABLEFLD,      SW_SERVICE_FIELDTYPE_TABLE_FORMULA              },
    {USHRT_MAX,			USHRT_MAX                                        }
};
//-----------------------------------------------------------------
sal_uInt16 lcl_ServiceIdToResId(sal_uInt16 nServiceId)
{
    const ServiceIdResId* pMap = aServiceToRes;
    while( USHRT_MAX != pMap->nServiceId && nServiceId != pMap->nServiceId )
            ++pMap;
#ifdef DBG_UTIL
    if( USHRT_MAX == pMap->nServiceId )
        DBG_ERROR("service id not found");
#endif
    return pMap->nResId;
}
//-----------------------------------------------------------------
sal_uInt16 lcl_GetServiceForField( const SwField& rFld )
{
    sal_uInt16 nWhich = rFld.Which(), nSrvId = USHRT_MAX;
    //special handling for some fields
    switch( nWhich )
    {
    case RES_INPUTFLD:
        if( INP_USR == (rFld.GetSubType() & 0x00ff) )
            nSrvId = SW_SERVICE_FIELDTYPE_INPUT_USER;
        break;

    case RES_DOCINFOFLD:
        {
            USHORT nSubType = rFld.GetSubType();
            switch( (nSubType & 0xff))
            {
            case DI_CHANGE:
                nSrvId = ((nSubType&0x300) == DI_SUB_AUTHOR)
                        ? SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR
                        : SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME;
                break;
            case DI_CREATE:
                nSrvId = ((nSubType&0x300) == DI_SUB_AUTHOR)
                        ? SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR
                        : SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME;
                break;
            case DI_PRINT:
                nSrvId = ((nSubType&0x300) == DI_SUB_AUTHOR)
                        ? SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR
                        : SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME;
                break;
            case DI_EDIT:	nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME;break;
            case DI_COMMENT:nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION;break;
            case DI_KEYS:   nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS;break;
            case DI_THEMA:  nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT;	break;
            case DI_TITEL:  nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_TITLE;	break;
            case DI_DOCNO:  nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_REVISION;	break;
            case DI_CUSTOM: nSrvId = SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM;	break;
            }
        }
        break;

    case RES_HIDDENTXTFLD:
        nSrvId = TYP_CONDTXTFLD == rFld.GetSubType()
                        ? SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT
                        : SW_SERVICE_FIELDTYPE_HIDDEN_TEXT;
        break;

    case RES_DOCSTATFLD:
        {
            switch( rFld.GetSubType() )
            {
            case DS_PAGE: nSrvId = SW_SERVICE_FIELDTYPE_PAGE_COUNT; break;
            case DS_PARA: nSrvId = SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT; break;
            case DS_WORD: nSrvId = SW_SERVICE_FIELDTYPE_WORD_COUNT     ; break;
            case DS_CHAR: nSrvId = SW_SERVICE_FIELDTYPE_CHARACTER_COUNT; break;
            case DS_TBL:  nSrvId = SW_SERVICE_FIELDTYPE_TABLE_COUNT    ; break;
            case DS_GRF:  nSrvId = SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT; break;
            case DS_OLE:  nSrvId = SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT; break;
            }
        }
        break;
    }
    if( USHRT_MAX == nSrvId )
    {
        for( const ServiceIdResId* pMap = aServiceToRes;
                USHRT_MAX != pMap->nResId; ++pMap )
            if( nWhich == pMap->nResId )
            {
                nSrvId = pMap->nServiceId;
                break;
            }
    }
#ifdef DBG_UTIL
    if( USHRT_MAX == nSrvId )
        DBG_ERROR("resid not found");
#endif
    return nSrvId;
}

sal_uInt16 lcl_GetPropMapIdForFieldType( USHORT nWhich )
{
    sal_uInt16 nId;
    switch( nWhich )
    {
    case RES_USERFLD:	nId = PROPERTY_MAP_FLDMSTR_USER;			break;
    case RES_DBFLD:		nId = PROPERTY_MAP_FLDMSTR_DATABASE;		break;
    case RES_SETEXPFLD:	nId = PROPERTY_MAP_FLDMSTR_SET_EXP;			break;
    case RES_DDEFLD:	nId = PROPERTY_MAP_FLDMSTR_DDE;				break;
    case RES_AUTHORITY:	nId = PROPERTY_MAP_FLDMSTR_BIBLIOGRAPHY;	break;
    default:			nId = PROPERTY_MAP_FLDMSTR_DUMMY0;
    }
    return nId;
}


BYTE GetFieldTypeMId( const OUString& rProperty, const SwFieldType& rTyp )
{
    USHORT nId = lcl_GetPropMapIdForFieldType( rTyp.Which() );
    const SfxItemPropertyMap* pMap = aSwMapProvider.GetPropertyMap( nId );
    if( !pMap )
        nId = USHRT_MAX;
    else
    {
        nId = USHRT_MAX;    // in case of property not found
        for( ; pMap->pName; ++pMap )
            if( rProperty.equalsAsciiL( pMap->pName, pMap->nNameLen ) )
            {
                nId = pMap->nWID;
                break;
            }
    }
    return (BYTE)nId;
}

USHORT lcl_GetPropertyMapOfService( USHORT nServiceId )
{
    USHORT nRet;
    switch ( nServiceId)
    {
    case SW_SERVICE_FIELDTYPE_DATETIME: nRet = PROPERTY_MAP_FLDTYP_DATETIME; break;
    case SW_SERVICE_FIELDTYPE_USER: nRet = PROPERTY_MAP_FLDTYP_USER; break;
    case SW_SERVICE_FIELDTYPE_SET_EXP: nRet = PROPERTY_MAP_FLDTYP_SET_EXP; break;
    case SW_SERVICE_FIELDTYPE_GET_EXP: nRet = PROPERTY_MAP_FLDTYP_GET_EXP; break;
    case SW_SERVICE_FIELDTYPE_FILE_NAME: nRet = PROPERTY_MAP_FLDTYP_FILE_NAME; break;
    case SW_SERVICE_FIELDTYPE_PAGE_NUM: nRet = PROPERTY_MAP_FLDTYP_PAGE_NUM; break;
    case SW_SERVICE_FIELDTYPE_AUTHOR: nRet = PROPERTY_MAP_FLDTYP_AUTHOR; break;
    case SW_SERVICE_FIELDTYPE_CHAPTER: nRet = PROPERTY_MAP_FLDTYP_CHAPTER; break;
    case SW_SERVICE_FIELDTYPE_GET_REFERENCE: nRet = PROPERTY_MAP_FLDTYP_GET_REFERENCE; break;
    case SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT: nRet = PROPERTY_MAP_FLDTYP_CONDITIONED_TEXT; break;
    case SW_SERVICE_FIELDTYPE_ANNOTATION: nRet = PROPERTY_MAP_FLDTYP_ANNOTATION; break;
    case SW_SERVICE_FIELDTYPE_INPUT_USER:
    case SW_SERVICE_FIELDTYPE_INPUT: nRet = PROPERTY_MAP_FLDTYP_INPUT; break;
    case SW_SERVICE_FIELDTYPE_MACRO: nRet = PROPERTY_MAP_FLDTYP_MACRO; break;
    case SW_SERVICE_FIELDTYPE_DDE: nRet = PROPERTY_MAP_FLDTYP_DDE; break;
    case SW_SERVICE_FIELDTYPE_HIDDEN_PARA: nRet = PROPERTY_MAP_FLDTYP_HIDDEN_PARA; break;
    case SW_SERVICE_FIELDTYPE_DOC_INFO: nRet = PROPERTY_MAP_FLDTYP_DOC_INFO; break;
    case SW_SERVICE_FIELDTYPE_TEMPLATE_NAME: nRet = PROPERTY_MAP_FLDTYP_TEMPLATE_NAME; break;
    case SW_SERVICE_FIELDTYPE_USER_EXT: nRet = PROPERTY_MAP_FLDTYP_USER_EXT; break;
    case SW_SERVICE_FIELDTYPE_REF_PAGE_SET: nRet = PROPERTY_MAP_FLDTYP_REF_PAGE_SET; break;
    case SW_SERVICE_FIELDTYPE_REF_PAGE_GET: nRet = PROPERTY_MAP_FLDTYP_REF_PAGE_GET; break;
    case SW_SERVICE_FIELDTYPE_JUMP_EDIT: nRet = PROPERTY_MAP_FLDTYP_JUMP_EDIT; break;
    case SW_SERVICE_FIELDTYPE_SCRIPT: nRet = PROPERTY_MAP_FLDTYP_SCRIPT; break;
    case SW_SERVICE_FIELDTYPE_DATABASE_NEXT_SET: nRet = PROPERTY_MAP_FLDTYP_DATABASE_NEXT_SET; break;
    case SW_SERVICE_FIELDTYPE_DATABASE_NUM_SET: nRet = PROPERTY_MAP_FLDTYP_DATABASE_NUM_SET; break;
    case SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM: nRet = PROPERTY_MAP_FLDTYP_DATABASE_SET_NUM; break;
    case SW_SERVICE_FIELDTYPE_DATABASE: nRet = PROPERTY_MAP_FLDTYP_DATABASE; break;
    case SW_SERVICE_FIELDTYPE_DATABASE_NAME: nRet = PROPERTY_MAP_FLDTYP_DATABASE_NAME; break;
    case SW_SERVICE_FIELDTYPE_TABLE_FORMULA: nRet = PROPERTY_MAP_FLDTYP_TABLE_FORMULA; break;
    case SW_SERVICE_FIELDTYPE_PAGE_COUNT:
    case SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT:
    case SW_SERVICE_FIELDTYPE_WORD_COUNT:
    case SW_SERVICE_FIELDTYPE_CHARACTER_COUNT:
    case SW_SERVICE_FIELDTYPE_TABLE_COUNT:
    case SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT:
    case SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT: nRet = PROPERTY_MAP_FLDTYP_DOCSTAT; break;
    case SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR:
    case SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR:
    case SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR: nRet = PROPERTY_MAP_FLDTYP_DOCINFO_AUTHOR; break;
    case SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME:
    case SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME:
    case SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME: nRet = PROPERTY_MAP_FLDTYP_DOCINFO_DATE_TIME; break;
    case SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME: nRet = PROPERTY_MAP_FLDTYP_DOCINFO_EDIT_TIME; break;
    case SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM: nRet = PROPERTY_MAP_FLDTYP_DOCINFO_CUSTOM; break;
    case SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION:
    case SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS:
    case SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT:
    case SW_SERVICE_FIELDTYPE_DOCINFO_TITLE: nRet = PROPERTY_MAP_FLDTYP_DOCINFO_MISC; break;
    case SW_SERVICE_FIELDTYPE_DOCINFO_REVISION: nRet = PROPERTY_MAP_FLDTYP_DOCINFO_REVISION; break;
    case SW_SERVICE_FIELDTYPE_BIBLIOGRAPHY: nRet = PROPERTY_MAP_FLDTYP_BIBLIOGRAPHY; break;
    case SW_SERVICE_FIELDTYPE_DUMMY_0:
    case SW_SERVICE_FIELDTYPE_COMBINED_CHARACTERS: nRet = PROPERTY_MAP_FLDTYP_COMBINED_CHARACTERS; break;
    case SW_SERVICE_FIELDTYPE_DROPDOWN: nRet = PROPERTY_MAP_FLDTYP_DROPDOWN; break;
    case SW_SERVICE_FIELDTYPE_DUMMY_4:
    case SW_SERVICE_FIELDTYPE_DUMMY_5:
    case SW_SERVICE_FIELDTYPE_DUMMY_6:
    case SW_SERVICE_FIELDTYPE_DUMMY_7:
    case SW_SERVICE_FIELDTYPE_DUMMY_8: nRet = PROPERTY_MAP_FLDTYP_DUMMY_0; break;
    case SW_SERVICE_FIELDMASTER_USER: nRet = PROPERTY_MAP_FLDMSTR_USER; break;
    case SW_SERVICE_FIELDMASTER_DDE: nRet = PROPERTY_MAP_FLDMSTR_DDE; break;
    case SW_SERVICE_FIELDMASTER_SET_EXP: nRet = PROPERTY_MAP_FLDMSTR_SET_EXP; break;
    case SW_SERVICE_FIELDMASTER_DATABASE: nRet = PROPERTY_MAP_FLDMSTR_DATABASE; break;
    case SW_SERVICE_FIELDMASTER_BIBLIOGRAPHY: nRet = PROPERTY_MAP_FLDMSTR_BIBLIOGRAPHY; break;
    case SW_SERVICE_FIELDMASTER_DUMMY2:
    case SW_SERVICE_FIELDMASTER_DUMMY3:
    case SW_SERVICE_FIELDMASTER_DUMMY4:
    case SW_SERVICE_FIELDMASTER_DUMMY5: nRet = PROPERTY_MAP_FLDMSTR_DUMMY0; break;
    case SW_SERVICE_FIELDTYPE_HIDDEN_TEXT: nRet = PROPERTY_MAP_FLDTYP_HIDDEN_TEXT; break;
    default:
        DBG_ERROR( "wrong service id" );
        nRet = USHRT_MAX;
    }
    return nRet;
}

/******************************************************************
 * SwXFieldMaster
 ******************************************************************/
TYPEINIT1(SwXFieldMaster, SwClient);
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXFieldMaster::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXFieldMaster::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(this) );
    }
    return 0;
}
/* -----------------------------06.04.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFieldMaster::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXFieldMaster");
}
/* -----------------------------06.04.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXFieldMaster::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    sal_Bool bRet = sal_False;
    if(rServiceName.equalsAsciiL(
            RTL_CONSTASCII_STRINGPARAM("com.sun.star.text.TextFieldMaster")))
        bRet = sal_True;
    else
    {
        const sal_Char* pEntry;
        switch( nResTypeId )
        {
        case RES_USERFLD:	pEntry = "User"; 			break;
        case RES_DBFLD:		pEntry = "Database"; 		break;
        case RES_SETEXPFLD:	pEntry = "SetExpression";	break;
        case RES_DDEFLD:	pEntry = "DDE";				break;
        case RES_AUTHORITY:	pEntry = "Bibliography"; 	break;
        default: pEntry = 0;
        }
        if( pEntry )
        {
            ByteString aTmp( RTL_CONSTASCII_STRINGPARAM(
                            "com.sun.star.text.fieldmaster."));
            aTmp.Append( pEntry );
            bRet = rServiceName.equalsAsciiL(aTmp.GetBuffer(), aTmp.Len());
        }
    }
    return bRet;
}
/* -----------------------------06.04.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXFieldMaster::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextFieldMaster");

    const sal_Char* pEntry1;
    switch( nResTypeId )
    {
    case RES_USERFLD: 	pEntry1 = "User"; 			break;
    case RES_DBFLD:	  	pEntry1 = "Database";		break;
    case RES_SETEXPFLD:	pEntry1 = "SetExpression";	break;
    case RES_DDEFLD:	pEntry1 = "DDE"; 			break;
    case RES_AUTHORITY:	pEntry1 = "Bibliography";	break;
    default: pEntry1 = 0;
    }
    if( pEntry1 )
    {
        String s;
        s.AppendAscii( "com.sun.star.text.fieldmaster." ).AppendAscii( pEntry1 );
        pArray[1] = s;
    }
    return aRet;
}
/*-- 14.12.98 11:08:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFieldMaster::SwXFieldMaster(SwDoc* pDoc, sal_uInt16 nResId) :
    aLstnrCntnr( (XPropertySet*)this),
    nResTypeId(nResId),
    m_pDoc(pDoc),
    m_bIsDescriptor(sal_True),
    fParam1(0.),
    nParam1(-1),
    bParam1(FALSE),
    nParam2(0)
{
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);
}
/*-- 14.12.98 11:08:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFieldMaster::SwXFieldMaster(SwFieldType& rType, SwDoc* pDoc) :
    SwClient(&rType),
    aLstnrCntnr( (XPropertySet*)this),
    nResTypeId(rType.Which()),
    m_pDoc(pDoc),
    m_bIsDescriptor(sal_False),
    fParam1(0.),
    nParam1(-1),
    bParam1(FALSE)
{

}
/*-- 14.12.98 11:08:34---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFieldMaster::~SwXFieldMaster()
{

}
/*-- 14.12.98 11:08:35---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo >  SwXFieldMaster::getPropertySetInfo(void)
                                            throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference< beans::XPropertySetInfo >  aRef = new SfxItemPropertySetInfo(
                        aSwMapProvider.GetPropertyMap(
                                lcl_GetPropMapIdForFieldType( nResTypeId ) ));
    return aRef;
}
/*-- 14.12.98 11:08:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::setPropertyValue( const OUString& rPropertyName,
                                    const uno::Any& rValue)
    throw( beans::UnknownPropertyException, beans::PropertyVetoException,
            lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFieldType* pType = GetFldType(sal_True);
    if(pType)
    {
        sal_Bool bSetValue = sal_True;
        if( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_SUB_TYPE)))
        {
            const SvStringsDtor& rExtraArr = SwStyleNameMapper::GetExtraUINameArray();
            String sTypeName = pType->GetName();
            static sal_uInt16 nIds[] =
            {
                RES_POOLCOLL_LABEL_DRAWING - RES_POOLCOLL_EXTRA_BEGIN,
                RES_POOLCOLL_LABEL_ABB - RES_POOLCOLL_EXTRA_BEGIN,
                RES_POOLCOLL_LABEL_TABLE - RES_POOLCOLL_EXTRA_BEGIN,
                RES_POOLCOLL_LABEL_FRAME- RES_POOLCOLL_EXTRA_BEGIN,
                0
            };
            for(const sal_uInt16 * pIds = nIds; *pIds; ++pIds)
            {
                if(sTypeName ==	*rExtraArr[ *pIds ] )
                {
                    bSetValue = sal_False;
                    break;
                }
            }
        }
        if( bSetValue )
        {
            // nothing special to be done here for the properties
            // UNO_NAME_DATA_BASE_NAME and UNO_NAME_DATA_BASE_URL.
            // We just call PutValue (empty string is allowed).
            // Thus the last property set will be used as Data Source.

            BYTE nMId = GetFieldTypeMId( rPropertyName, *pType  );
            if( UCHAR_MAX != nMId )
                pType->PutValue( rValue, nMId );
            else
                throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
        }
    }
    else if(!pType && m_pDoc &&
        ( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_NAME))) )
    {
        OUString uTmp;
        rValue >>= uTmp;
        String sTypeName(uTmp);
        SwFieldType* pType2 = m_pDoc->GetFldType(nResTypeId, sTypeName, sal_False);

        String sTable(SW_RES(STR_POOLCOLL_LABEL_TABLE));
        String sDrawing(SW_RES(STR_POOLCOLL_LABEL_DRAWING));
        String sFrame(SW_RES(STR_POOLCOLL_LABEL_FRAME));
        String sIllustration(SW_RES(STR_POOLCOLL_LABEL_ABB));

        if(pType2 ||
            (RES_SETEXPFLD == nResTypeId &&
            ( sTypeName == sTable || sTypeName == sDrawing ||
              sTypeName == sFrame || sTypeName == sIllustration )))
        {
            throw lang::IllegalArgumentException();
        }
        else
        {
            switch(nResTypeId)
            {
                case RES_USERFLD :
                {
                    SwUserFieldType aType(m_pDoc, sTypeName);
                    pType2 = m_pDoc->InsertFldType(aType);
                    ((SwUserFieldType*)pType2)->SetContent(sParam1);
                    ((SwUserFieldType*)pType2)->SetValue(fParam1);
                    ((SwUserFieldType*)pType2)->SetType(bParam1 ? nsSwGetSetExpType::GSE_EXPR : nsSwGetSetExpType::GSE_STRING);
                }
                break;
                case RES_DDEFLD :
                {
                    SwDDEFieldType aType(sTypeName, sParam1,
                        sal::static_int_cast< USHORT >(bParam1 ? sfx2::LINKUPDATE_ALWAYS : sfx2::LINKUPDATE_ONCALL));
                    pType2 = m_pDoc->InsertFldType(aType);
                }
                break;
                case RES_SETEXPFLD :
                {
                    SwSetExpFieldType aType(m_pDoc, sTypeName);
                    if(sParam1.Len())
                        aType.SetDelimiter( sParam1.GetChar(0));
                    if(nParam1 > -1 && nParam1 < MAXLEVEL)
                        aType.SetOutlineLvl(nParam1);
                    pType2 = m_pDoc->InsertFldType(aType);
                }
                break;
                case RES_DBFLD :
                {    
                    ::GetString( rValue, sParam3 );
                    pType = GetFldType();
                }
                break;
            }
            if(pType2)
            {
                pType2->Add(this);
                m_bIsDescriptor = sal_False;
            }
            else
                throw uno::RuntimeException();
        }

        DBG_ASSERT(pType2, "kein FieldType gefunden!" );
    }
    else
    {
        switch( nResTypeId )
        {
        case RES_USERFLD:
            if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_CONTENT)))
                ::GetString( rValue, sParam1 );
            else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_VALUE )))
            {
                if(rValue.getValueType() != ::getCppuType(static_cast<const double*>(0)))
                    throw lang::IllegalArgumentException();
                fParam1 = *(double*)rValue.getValue();
            }
            else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_EXPRESSION )))
            {
                if(rValue.getValueType() != ::getBooleanCppuType())
                    throw lang::IllegalArgumentException();
                bParam1 = *(sal_Bool*)rValue.getValue();
            }

            break;
        case RES_DBFLD:
            if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_NAME)))
                ::GetString( rValue, sParam1 );
            else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_TABLE_NAME)))
                ::GetString( rValue, sParam2 );
            else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_COLUMN_NAME)))
                ::GetString( rValue, sParam3 );
            else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_COMMAND_TYPE)))
                rValue >>= nParam2;
            if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_URL)))
                ::GetString( rValue, sParam5 );

            if((sParam1.Len() || sParam5.Len())
                    && sParam2.Len() && sParam3.Len())
                GetFldType();
            break;
        case  RES_SETEXPFLD:
            if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_NUMBERING_SEPARATOR)))
                ::GetString( rValue, sParam1 );
            else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_CHAPTER_NUMBERING_LEVEL)))
                rValue >>= nParam1;
            break;
        case RES_DDEFLD:
            {
                USHORT nPart = rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DDE_COMMAND_TYPE))  ? 0 :
                        rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DDE_COMMAND_FILE))  ? 1 :
                        rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DDE_COMMAND_ELEMENT))  ? 2 :
                        rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_AUTOMATIC_UPDATE)) ? 3 : USHRT_MAX;
                if(nPart  < 3 )
                {
                    String sTmp;
                    if(!sParam1.Len())
                        (sParam1 = sfx2::cTokenSeperator)
                                += sfx2::cTokenSeperator;

                    sParam1.SetToken( nPart, sfx2::cTokenSeperator,
                                ::GetString( rValue, sTmp ));
                }
                else if(3 == nPart)
                    bParam1 = *(sal_Bool*)rValue.getValue();
            }
            break;
        default:
            throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
        }
    }
}
/* -----------------------------30.03.01 14:40--------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwXFieldMaster::GetFldType(sal_Bool bDontCreate) const
{
    if(!bDontCreate && RES_DBFLD == nResTypeId && m_bIsDescriptor && m_pDoc)
    {
        SwDBData aData;

        // set DataSource
        svx::ODataAccessDescriptor aAcc;
        if( sParam1.Len() > 0 )
            aAcc[ svx::daDataSource ]       <<= OUString(sParam1); // DataBaseName
        else if( sParam5.Len() > 0 )
            aAcc[ svx::daDatabaseLocation]  <<= OUString(sParam5); // DataBaseURL
        aData.sDataSource = aAcc.getDataSource();

        aData.sCommand = sParam2;
        aData.nCommandType = nParam2;
        SwDBFieldType aType(m_pDoc, sParam3,  aData);
        SwFieldType* pType = m_pDoc->InsertFldType(aType);
        SwXFieldMaster* pThis = ((SwXFieldMaster*)this);
        pType->Add(pThis);
        pThis->m_bIsDescriptor = sal_False;
    }
    if(m_bIsDescriptor)
        return 0;
    else
        return (SwFieldType*)GetRegisteredIn();
}

/*-- 14.12.98 11:08:36---------------------------------------------------

  -----------------------------------------------------------------------*/
typedef SwFmtFld* SwFmtFldPtr;
SV_DECL_PTRARR(SwDependentFields, SwFmtFldPtr, 5, 5)
SV_IMPL_PTRARR(SwDependentFields, SwFmtFldPtr)

uno::Any SwXFieldMaster::getPropertyValue(const OUString& rPropertyName)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    SwFieldType* pType = GetFldType(sal_True);
    if( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_INSTANCE_NAME)) )
    {
        String sName;
        if(pType)
            SwXTextFieldMasters::getInstanceName(*pType, sName);
        aRet <<= OUString(sName);
    }
    else if(pType)
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_NAME) ))
        {
            aRet <<= SwXFieldMaster::GetProgrammaticName(*pType, *GetDoc());
        }
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DEPENDENT_TEXT_FIELDS)) )
        {
            //fill all text fields into a sequence
            SwClientIter aIter( *pType );
            SwDependentFields aFldArr;
            SwFmtFldPtr pFld = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
            while(pFld)
            {
                if(pFld->IsFldInDoc())
                    aFldArr.Insert(pFld, aFldArr.Count());
                pFld = (SwFmtFld*)aIter.Next();
            }
            uno::Sequence<uno::Reference <text::XDependentTextField> > aRetSeq(aFldArr.Count());
            uno::Reference<text::XDependentTextField>* pRetSeq = aRetSeq.getArray();
            SwXTextField* pInsert = 0;
            for(USHORT i = 0; i < aFldArr.Count(); i++)
            {
                pFld = aFldArr.GetObject(i);
                SwXTextField* pTemp = (SwXTextField*)aIter.First(TYPE(SwXTextField));
                while(pTemp)
                {
                    if(pTemp->GetFldFmt() == pFld)
                    {
                        pInsert = pTemp;
                        break;
                    }
                    pTemp = (SwXTextField*)aIter.Next();
                }
                if(!pInsert)
                    pInsert = new SwXTextField( *pFld, GetDoc());
                pRetSeq[i] = uno::Reference<text::XDependentTextField>(pInsert);
                pInsert = 0;
            }
            aRet <<= aRetSeq;
        }
        else if(pType)
        {
            //TODO: Properties fuer die uebrigen Feldtypen einbauen
            BYTE nMId = GetFieldTypeMId( rPropertyName, *pType );
            if( UCHAR_MAX != nMId )
            {
                pType->QueryValue( aRet, nMId );

                if( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_NAME)) ||
                    rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_URL)))
                {
                    OUString aDataSource;
                    aRet >>= aDataSource;
                    aRet <<= OUString();

                    OUString *pStr = 0;     // only one of this properties will return
                                            // a non-empty string.
                    INetURLObject aObj;
                    aObj.SetURL( aDataSource );
                    BOOL bIsURL = aObj.GetProtocol() != INET_PROT_NOT_VALID;
                    if (bIsURL && rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_URL)))
                        pStr = &aDataSource;        // DataBaseURL
                    else if (!bIsURL && rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_NAME)))
                        pStr = &aDataSource;        // DataBaseName

                    if (pStr)
                        aRet <<= *pStr;
                }
            }
            else
                throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
        }
        else
        {
            if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_COMMAND_TYPE)) )
                aRet <<= nParam2;
        }
    }
    else
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_COMMAND_TYPE)) )
            aRet <<= nParam2;
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DEPENDENT_TEXT_FIELDS)) )
        {
            uno::Sequence<uno::Reference <text::XDependentTextField> > aRetSeq(0);
            aRet <<= aRetSeq;
        }
        else
        {
            const String* pStr = 0;
            String sStr;
            switch ( nResTypeId )
            {
            case RES_USERFLD:
                if( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_CONTENT)) )
                    pStr = &sParam1;
                else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_VALUE )))
                    aRet <<= fParam1;
                else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_EXPRESSION )))
                    aRet.setValue(&bParam1, ::getBooleanCppuType());
                break;
            case RES_DBFLD:
                if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_NAME)) ||
                   rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_URL)))
                {
                    pStr = 0;   // only one of this properties will return
                                // a non-empty string.
                    INetURLObject aObj;
                    aObj.SetURL( sParam5 );  // SetSmartURL
                    BOOL bIsURL = aObj.GetProtocol() != INET_PROT_NOT_VALID;
                    if (bIsURL && rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_URL)))
                        pStr = &sParam5;        // DataBaseURL
                    else if ( rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_NAME)))
                        pStr = &sParam1;            // DataBaseName
                }
                else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_TABLE_NAME)))
                    pStr = &sParam2;
                else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_COLUMN_NAME)))
                    pStr = &sParam3;
                break;
            case RES_SETEXPFLD:
                if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_NUMBERING_SEPARATOR)))
                    pStr = &sParam1;
                else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_CHAPTER_NUMBERING_LEVEL)))
                    aRet <<= nParam1;
                break;
            case RES_DDEFLD:
                {
                    USHORT nPart = rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DDE_COMMAND_TYPE))  ? 0 :
                        rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DDE_COMMAND_FILE)) ? 1 :
                            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DDE_COMMAND_ELEMENT))  ? 2 :
                            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_AUTOMATIC_UPDATE)) ? 3 : USHRT_MAX;
                    if(nPart  < 3 )
                        pStr = &(sStr = sParam1.GetToken(nPart, sfx2::cTokenSeperator));
                    else if(3 == nPart)
                        aRet.setValue(&bParam1, ::getBooleanCppuType());
                }
                break;
            default:
                throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
            }

            if( pStr )
                aRet <<= OUString( *pStr );
        }
    }
    return aRet;
}
/*-- 14.12.98 11:08:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::addPropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 11:08:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::removePropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 11:08:37---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::addVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 11:08:37---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::removeVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}

/*-- 25.02.99 11:01:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::dispose(void) 			throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFieldType* pFldType = GetFldType(sal_True);
    if(pFldType)
    {
        sal_uInt16 nTypeIdx = USHRT_MAX;
        const SwFldTypes* pTypes = GetDoc()->GetFldTypes();
        for( sal_uInt16 i = 0; i < pTypes->Count(); i++ )
        {
            if((*pTypes)[i] == pFldType)
                nTypeIdx = i;
        }

        // zuerst alle Felder loeschen
        SwClientIter aIter( *pFldType );
        SwFmtFld* pFld = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
        while(pFld)
        {
            // Feld im Undo?
            SwTxtFld *pTxtFld = pFld->GetTxtFld();
            if(pTxtFld && pTxtFld->GetTxtNode().GetNodes().IsDocNodes() )
            {
                SwTxtNode& rTxtNode = (SwTxtNode&)*pTxtFld->GetpTxtNode();
                SwPaM aPam(rTxtNode, *pTxtFld->GetStart());
                aPam.SetMark();
                aPam.Move();
                GetDoc()->DeleteAndJoin(aPam);
            }
            pFld = (SwFmtFld*)aIter.Next();
        }
        // dann den FieldType loeschen
        GetDoc()->RemoveFldType(nTypeIdx);
    }
    else
        throw uno::RuntimeException();
}
/*-- 25.02.99 11:02:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::addEventListener(const uno::Reference< lang::XEventListener > & aListener)
                                            throw( uno::RuntimeException )
{
    if(!GetRegisteredIn())
        throw uno::RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 25.02.99 11:02:02---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::removeEventListener(const uno::Reference< lang::XEventListener > & aListener)
                                                    throw( uno::RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw uno::RuntimeException();
}

/*-- 14.12.98 11:08:38---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXFieldMaster::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
    {
        aLstnrCntnr.Disposing();
        m_pDoc = 0;
    }
}
/* -----------------------------06.11.00 09:44--------------------------------

const Programmatic2UIName_Impl* lcl_GetFieldNameTable()
{
    static BOOL bInitialized = FALSE;
    static Programmatic2UIName_Impl aFieldNames[5];
    if(!bInitialized)
    {
        bInitialized = TRUE;
        int nName = 0;
        aFieldNames[nName].sUIName = String				(SW_RES(STR_POOLCOLL_LABEL_ABB ));
        aFieldNames[nName++].sProgrammaticName = String	(SW_RES(STR_POCO_PRGM_LABEL_ABB));
        aFieldNames[nName].sUIName = String				(SW_RES(STR_POOLCOLL_LABEL_TABLE ));
        aFieldNames[nName++].sProgrammaticName = String	(SW_RES(STR_POCO_PRGM_LABEL_TABLE));
        aFieldNames[nName].sUIName = String				(SW_RES(STR_POOLCOLL_LABEL_FRAME));
        aFieldNames[nName++].sProgrammaticName = String	(SW_RES(STR_POCO_PRGM_LABEL_FRAME));
        aFieldNames[nName].sUIName = String				(SW_RES(STR_POOLCOLL_LABEL_DRAWING ));
        aFieldNames[nName++].sProgrammaticName = String	(SW_RES(STR_POCO_PRGM_LABEL_DRAWING));
    }
    return &aFieldNames[0];
}
 ---------------------------------------------------------------------------*/
/* -----------------------------06.11.00 10:26--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFieldMaster::GetProgrammaticName(const SwFieldType& rType, SwDoc& rDoc)
{
    OUString sRet(rType.GetName());
    if(RES_SETEXPFLD == rType.Which())
    {
        const SwFldTypes* pTypes = rDoc.GetFldTypes();
        for( sal_uInt16 i = 0; i <= INIT_FLDTYPES; i++ )
        {
            if((*pTypes)[i] == &rType)
            {
                sRet = SwStyleNameMapper::GetProgName ( sRet, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL );
                break;
            }
        }
    }
    return sRet;
}
/* -----------------------------06.11.00 14:12--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFieldMaster::LocalizeFormula(
    const SwSetExpField& rFld,
    const OUString& rFormula,
    sal_Bool bQuery)
{
    const OUString sTypeName(rFld.GetTyp()->GetName());
    OUString sProgName = SwStyleNameMapper::GetProgName(sTypeName, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL );
    if(sProgName != sTypeName)
    {
        OUString sSource = bQuery ? sTypeName : sProgName;
        OUString sDest = bQuery ? sProgName : sTypeName;
        if(!rFormula.compareTo(sSource, sSource.getLength()))
        {
            OUString sTmpFormula = sDest;
            sTmpFormula += rFormula.copy(sSource.getLength());
            return sTmpFormula;
        }
    }
    return rFormula;
}
/******************************************************************
 *
 ******************************************************************/
struct SwFieldProperties_Impl
{
    String 		sPar1;
    String 		sPar2;
    String 		sPar3;
    String 		sPar4;
    String      sPar5;
    String      sPar6;
    Date 			aDate;
    double 			fDouble;
    uno::Sequence<beans::PropertyValue>	aPropSeq;
    uno::Sequence<OUString> aStrings;
    util::DateTime*	pDateTime;

    sal_Int32 		nSubType;
    sal_Int32 		nFormat;
    sal_uInt16 		nUSHORT1;
    sal_uInt16 		nUSHORT2;
    sal_Int16 		nSHORT1;
    sal_Int8 		nByte1;
    sal_Bool        bFormatIsDefault;
    sal_Bool 		bBool1;
    sal_Bool 		bBool2;
    sal_Bool 		bBool3;
    sal_Bool        bBool4;

    SwFieldProperties_Impl():
        fDouble(0.),
        pDateTime(0),
        nSubType(0),
        nFormat(0),
        nUSHORT1(0),
        nUSHORT2(0),
        nSHORT1(0),
        nByte1(0),
        bFormatIsDefault(sal_True),
        bBool1(sal_False),
        bBool2(sal_False),
        bBool3(sal_False),
        bBool4(sal_True) //Automatic language
        {}
    ~SwFieldProperties_Impl()
        {delete pDateTime;}

};

TYPEINIT1(SwXTextField, SwClient);
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextField::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXTextField::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >(this) );
    }
    return 0;
}
/*-- 14.12.98 11:37:14---------------------------------------------------

  -----------------------------------------------------------------------*/

SwXTextField::SwXTextField(sal_uInt16 nServiceId, SwDoc* pDoc) :
    aLstnrCntnr( (XTextContent*)this),
    pFmtFld(0),
    m_pDoc(pDoc),
    m_pTextObject(0),
    m_bIsDescriptor(nServiceId != USHRT_MAX),
    m_bCallUpdate(sal_False),
    m_nServiceId(nServiceId),
    m_pProps(new SwFieldProperties_Impl)
{
    //Set visible as default!
    if(SW_SERVICE_FIELDTYPE_SET_EXP == nServiceId ||
            SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM == nServiceId ||
            SW_SERVICE_FIELDTYPE_DATABASE == nServiceId ||
            SW_SERVICE_FIELDTYPE_DATABASE_NAME == nServiceId  )
        m_pProps->bBool2 = sal_True;
    else if(SW_SERVICE_FIELDTYPE_TABLE_FORMULA == nServiceId)
        m_pProps->bBool1 = sal_True;
    if(SW_SERVICE_FIELDTYPE_SET_EXP == nServiceId)
        m_pProps->nUSHORT2 = USHRT_MAX;

}
/*-- 14.12.98 11:37:15---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextField::SwXTextField(const SwFmtFld& rFmt, SwDoc* pDc) :
    aLstnrCntnr( (XTextContent*)this),
    pFmtFld(&rFmt),
    m_pDoc(pDc),
    m_pTextObject(0),
    m_bIsDescriptor(sal_False),
    m_bCallUpdate(sal_False),
    m_nServiceId( lcl_GetServiceForField( *pFmtFld->GetFld() ) ),
    m_pProps(0)
{
    pDc->GetUnoCallBack()->Add(this);
}
/*-- 14.12.98 11:37:15---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextField::~SwXTextField()
{
    if ( m_pTextObject )
    {
        m_pTextObject->DisposeEditSource();
        m_pTextObject->release();
    }

    delete m_pProps;
}
/*-- 14.12.98 11:37:16---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::attachTextFieldMaster(const uno::Reference< beans::XPropertySet > & xFieldMaster)
                    throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!m_bIsDescriptor)
        throw uno::RuntimeException();
    uno::Reference< lang::XUnoTunnel > xMasterTunnel(xFieldMaster, uno::UNO_QUERY);
    if (!xMasterTunnel.is())
        throw lang::IllegalArgumentException();
    SwXFieldMaster* pMaster = reinterpret_cast< SwXFieldMaster * >(
            sal::static_int_cast< sal_IntPtr >( xMasterTunnel->getSomething( SwXFieldMaster::getUnoTunnelId()) ));

    SwFieldType* pFieldType = pMaster ? pMaster->GetFldType() : 0;
    if(pFieldType && pFieldType->Which() == lcl_ServiceIdToResId(m_nServiceId))
    {
        m_sTypeName = pFieldType->GetName();
        pFieldType->Add( &m_aFieldTypeClient );
    }
    else
        throw lang::IllegalArgumentException();

}
/*-- 14.12.98 11:37:16---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySet >  SwXTextField::getTextFieldMaster(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwFieldType* pType = 0;
    if( m_bIsDescriptor && m_aFieldTypeClient.GetRegisteredIn() )
    {
        pType = (SwFieldType*)m_aFieldTypeClient.GetRegisteredIn();
    }            
    else
    {        
        if(!GetRegisteredIn())
            throw uno::RuntimeException();
        pType = pFmtFld->GetFld()->GetTyp();
    }
    SwXFieldMaster* pMaster = (SwXFieldMaster*)
                SwClientIter(*pType).First(TYPE(SwXFieldMaster));
    if(!pMaster)
        pMaster = new SwXFieldMaster(*pType, GetDoc());

    return pMaster;
}
/*-- 14.12.98 11:37:16---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXTextField::getPresentation(sal_Bool bShowCommand) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    OUString sRet;
    const SwField* pField = GetField();
    if(pField)
        sRet = pField->GetCntnt(bShowCommand);
    else
        throw uno::RuntimeException();
    return sRet;
}
/* -----------------18.02.99 13:39-------------------
 *
 * --------------------------------------------------*/
void SwXTextField::attachToRange(
        const uno::Reference< text::XTextRange > & xTextRange)
    throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!m_bIsDescriptor)
        throw uno::RuntimeException();
    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    OTextCursorHelper* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange 	= reinterpret_cast< SwXTextRange * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( SwXTextRange::getUnoTunnelId()) ));
        pCursor	= reinterpret_cast< OTextCursorHelper * >(
                sal::static_int_cast< sal_IntPtr >( xRangeTunnel->getSomething( OTextCursorHelper::getUnoTunnelId()) ));
    }

    SwDoc* pDoc = pRange ? (SwDoc*)pRange->GetDoc() : pCursor ? (SwDoc*)pCursor->GetDoc() : 0;
    //wurde ein FieldMaster attached, dann ist das Dokument schon festgelegt!
    if(pDoc && (!m_pDoc || m_pDoc == pDoc))
    {
        SwUnoInternalPaM aPam(*pDoc);
        //das muss jetzt sal_True liefern
        SwXTextRange::XTextRangeToSwPaM(aPam, xTextRange);
        SwField* pFld = 0;
        switch(m_nServiceId)
        {
            case SW_SERVICE_FIELDTYPE_ANNOTATION:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_POSTITFLD);
                
                DateTime aDateTime;
                if (m_pProps->pDateTime)
                {
                    aDateTime.SetYear(m_pProps->pDateTime->Year);
                    aDateTime.SetMonth(m_pProps->pDateTime->Month);
                    aDateTime.SetDay(m_pProps->pDateTime->Day);
                    aDateTime.SetHour(m_pProps->pDateTime->Hours);
                    aDateTime.SetMin(m_pProps->pDateTime->Minutes);
                    aDateTime.SetSec(m_pProps->pDateTime->Seconds);
                }
                pFld = new SwPostItField((SwPostItFieldType*)pFldType,
                        m_pProps->sPar1, m_pProps->sPar2,aDateTime);
                if ( m_pTextObject )
                {
                    ((SwPostItField*)pFld)->SetTextObject( m_pTextObject->CreateText() );
                      ((SwPostItField*)pFld)->SetPar2(m_pTextObject->GetText());
                }
            }
            break;
            case SW_SERVICE_FIELDTYPE_SCRIPT:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_SCRIPTFLD);
                pFld = new SwScriptField((SwScriptFieldType*)pFldType,
                        m_pProps->sPar1, m_pProps->sPar2,
                        m_pProps->bBool1);
            }
            break;
            case SW_SERVICE_FIELDTYPE_DATETIME:
            {
                sal_uInt16 nSub = 0;
                if(m_pProps->bBool1)
                    nSub |= FIXEDFLD;
                if(m_pProps->bBool2)
                    nSub |= DATEFLD;
                else
                    nSub |= TIMEFLD;
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_DATETIMEFLD);
                pFld = new SwDateTimeField((SwDateTimeFieldType*)pFldType,
                nSub, m_pProps->nFormat);
                if(m_pProps->fDouble > 0.)
                    ((SwDateTimeField*)pFld)->SetValue( m_pProps->fDouble );
                if(m_pProps->pDateTime)
                {
                    uno::Any aVal; aVal <<= *m_pProps->pDateTime;
                    pFld->PutValue( aVal, FIELD_PROP_DATE_TIME );
                }
                ((SwDateTimeField*)pFld)->SetOffset(m_pProps->nSubType);
            }
            break;
            case SW_SERVICE_FIELDTYPE_FILE_NAME:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_FILENAMEFLD);
                sal_Int32 nFormat = m_pProps->nFormat;
                if(m_pProps->bBool2)
                    nFormat |= FF_FIXED;
                pFld = new SwFileNameField((SwFileNameFieldType*)pFldType, nFormat);
                if(m_pProps->sPar3.Len())
                    ((SwFileNameField*)pFld)->SetExpansion(m_pProps->sPar3);
                uno::Any aFormat(&m_pProps->nFormat, ::getCppuType(&m_pProps->nFormat));
                pFld->PutValue( aFormat, FIELD_PROP_FORMAT );
            }
            break;
            case SW_SERVICE_FIELDTYPE_TEMPLATE_NAME:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_TEMPLNAMEFLD);
                pFld = new SwTemplNameField((SwTemplNameFieldType*)pFldType,
                                                    m_pProps->nFormat);
                uno::Any aFormat(&m_pProps->nFormat, ::getCppuType(&m_pProps->nFormat));
                pFld->PutValue(aFormat, FIELD_PROP_FORMAT);
            }
            break;
            case SW_SERVICE_FIELDTYPE_CHAPTER:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_CHAPTERFLD);
                pFld = new SwChapterField((SwChapterFieldType*)pFldType, m_pProps->nUSHORT1);
                ((SwChapterField*)pFld)->SetLevel(m_pProps->nByte1);
                uno::Any aVal; aVal <<= (sal_Int16)m_pProps->nUSHORT1;
                pFld->PutValue(aVal, FIELD_PROP_USHORT1 );
            }
            break;
            case SW_SERVICE_FIELDTYPE_AUTHOR:
            {
                long nFormat = m_pProps->bBool1 ? AF_NAME : AF_SHORTCUT;
                if(m_pProps->bBool2)
                    nFormat |= AF_FIXED;

                SwFieldType* pFldType = pDoc->GetSysFldType(RES_AUTHORFLD);
                pFld = new SwAuthorField((SwAuthorFieldType*)pFldType, nFormat);
                ((SwAuthorField*)pFld)->SetExpansion(m_pProps->sPar1);
            }
            break;
            case SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT:
            case SW_SERVICE_FIELDTYPE_HIDDEN_TEXT:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_HIDDENTXTFLD);
                pFld = new SwHiddenTxtField(((SwHiddenTxtFieldType*)pFldType),
                        m_pProps->sPar1,
                        m_pProps->sPar2, m_pProps->sPar3,
                        static_cast< USHORT >(SW_SERVICE_FIELDTYPE_HIDDEN_TEXT == m_nServiceId ?
                             TYP_HIDDENTXTFLD : TYP_CONDTXTFLD));
                ((SwHiddenTxtField*)pFld)->SetValue(m_pProps->bBool1);
                uno::Any aVal; aVal <<= (OUString)m_pProps->sPar4;
                pFld->PutValue(aVal, FIELD_PROP_PAR4 );
            }
            break;
            case SW_SERVICE_FIELDTYPE_HIDDEN_PARA:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_HIDDENPARAFLD);
                pFld = new SwHiddenParaField((SwHiddenParaFieldType*)pFldType,
                                                m_pProps->sPar1);
                ((SwHiddenParaField*)pFld)->SetHidden(m_pProps->bBool1);
            }
            break;
            case SW_SERVICE_FIELDTYPE_GET_REFERENCE:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_GETREFFLD);
                pFld = new SwGetRefField((SwGetRefFieldType*)pFldType,
                            m_pProps->sPar1,
                            0,
                            0,
                            0);
                if(m_pProps->sPar3.Len())
                    ((SwGetRefField*)pFld)->SetExpand(m_pProps->sPar3);
                uno::Any aVal; aVal <<=(sal_Int16)m_pProps->nUSHORT1;
                pFld->PutValue(aVal, FIELD_PROP_USHORT1 );
                aVal <<=(sal_Int16)m_pProps->nUSHORT2;
                pFld->PutValue(aVal, FIELD_PROP_USHORT2 );
                aVal <<=(sal_Int16)m_pProps->nSHORT1;
                pFld->PutValue(aVal, FIELD_PROP_SHORT1 );
            }
            break;
            case SW_SERVICE_FIELDTYPE_JUMP_EDIT:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_JUMPEDITFLD);
                pFld = new SwJumpEditField((SwJumpEditFieldType*)pFldType,
                        m_pProps->nUSHORT1, m_pProps->sPar2, m_pProps->sPar1);
            }
            break;
            case SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR		:
            case SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME  :
            case SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME         :
            case SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION       :
            case SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR     :
            case SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME  :
            case SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM            :
            case SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR      :
            case SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME   :
            case SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS         :
            case SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT           :
            case SW_SERVICE_FIELDTYPE_DOCINFO_TITLE             :
            case SW_SERVICE_FIELDTYPE_DOCINFO_REVISION          :
            case SW_SERVICE_FIELDTYPE_DOC_INFO:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_DOCINFOFLD);
                sal_uInt16 nSubType = aDocInfoSubTypeFromService[
                        m_nServiceId - SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR];
                if( SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME == m_nServiceId ||
                    SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME == m_nServiceId ||
                    SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME == m_nServiceId ||
                    SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME == m_nServiceId )
                {
                    if(m_pProps->bBool2) //IsDate
                    {
                        nSubType &= 0xf0ff;
                        nSubType |= DI_SUB_DATE;
                    }
                    else
                    {
                        nSubType &= 0xf0ff;
                        nSubType |= DI_SUB_TIME;
                    }
                }
                if(m_pProps->bBool1)
                    nSubType |= DI_SUB_FIXED;
                pFld = new SwDocInfoField((SwDocInfoFieldType*)pFldType, nSubType, m_pProps->sPar4, m_pProps->nFormat);
                if(m_pProps->sPar3.Len())
                    ((SwDocInfoField*)pFld)->SetExpansion(m_pProps->sPar3);
            }
            break;
            case SW_SERVICE_FIELDTYPE_USER_EXT:
            {
                sal_Int32 nFormat = 0;
                if(m_pProps->bBool1)
                    nFormat = AF_FIXED;

                SwFieldType* pFldType = pDoc->GetSysFldType(RES_EXTUSERFLD);
                pFld = new SwExtUserField((SwExtUserFieldType*)pFldType, m_pProps->nUSHORT1, nFormat);
                ((SwExtUserField*)pFld)->SetExpansion(m_pProps->sPar1);
            }
            break;
            case SW_SERVICE_FIELDTYPE_USER:
            {
                SwFieldType* pFldType = pDoc->GetFldType(RES_USERFLD, m_sTypeName, sal_True);
                if(!pFldType)
                    throw uno::RuntimeException();
                USHORT nUserSubType = m_pProps->bBool1 ? nsSwExtendedSubType::SUB_INVISIBLE : 0;
                if(m_pProps->bBool2)
                    nUserSubType |= nsSwExtendedSubType::SUB_CMD;
                if(m_pProps->bFormatIsDefault &&
                    nsSwGetSetExpType::GSE_STRING == ((SwUserFieldType*)pFldType)->GetType())
                        m_pProps->nFormat = -1;
                pFld = new SwUserField((SwUserFieldType*)pFldType,
                                    nUserSubType,
                                    m_pProps->nFormat);
            }
            break;
            case SW_SERVICE_FIELDTYPE_REF_PAGE_SET:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_REFPAGESETFLD);
                pFld = new SwRefPageSetField( (SwRefPageSetFieldType*)pFldType,
                                    m_pProps->nUSHORT1,
                                    m_pProps->bBool1 );
            }
            break;
            case SW_SERVICE_FIELDTYPE_REF_PAGE_GET:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_REFPAGEGETFLD);
                pFld = new SwRefPageGetField( (SwRefPageGetFieldType*)pFldType,
                                                m_pProps->nUSHORT1 );
                ((SwRefPageGetField*)pFld)->SetText(m_pProps->sPar1);
            }
            break;
            case SW_SERVICE_FIELDTYPE_PAGE_NUM:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_PAGENUMBERFLD);
                pFld = new SwPageNumberField((SwPageNumberFieldType*)pFldType,
                                                PG_RANDOM,
                                                m_pProps->nFormat,
                                                m_pProps->nUSHORT1);
                ((SwPageNumberField*)pFld)->SetUserString(m_pProps->sPar1);
                uno::Any aVal; aVal <<= m_pProps->nSubType;
                pFld->PutValue( aVal, FIELD_PROP_SUBTYPE );
            }
            break;
            case SW_SERVICE_FIELDTYPE_DDE:
            {
                SwFieldType* pFldType = pDoc->GetFldType(RES_DDEFLD, m_sTypeName, sal_True);
                if(!pFldType)
                    throw uno::RuntimeException();
                pFld = new SwDDEField( (SwDDEFieldType*)pFldType );
            }
            break;
            case SW_SERVICE_FIELDTYPE_DATABASE_NAME:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_DBNAMEFLD);
                SwDBData aData;
                aData.sDataSource = m_pProps->sPar1;
                aData.sCommand = m_pProps->sPar2;
                aData.nCommandType = m_pProps->nSHORT1;
                pFld = new SwDBNameField((SwDBNameFieldType*)pFldType, aData);
                sal_uInt16  nSubType = pFld->GetSubType();
                if(m_pProps->bBool2)
                    nSubType &= ~nsSwExtendedSubType::SUB_INVISIBLE;
                else
                    nSubType |= nsSwExtendedSubType::SUB_INVISIBLE;
                pFld->SetSubType(nSubType);
            }
            break;
            case SW_SERVICE_FIELDTYPE_DATABASE_NEXT_SET:
            {
                SwDBData aData;
                aData.sDataSource = m_pProps->sPar1;
                aData.sCommand = m_pProps->sPar2;
                aData.nCommandType = m_pProps->nSHORT1;
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_DBNEXTSETFLD);
                pFld = new SwDBNextSetField((SwDBNextSetFieldType*)pFldType,
                        m_pProps->sPar3, aEmptyStr,
                        aData);
            }
            break;
            case SW_SERVICE_FIELDTYPE_DATABASE_NUM_SET:
            {
                SwDBData aData;
                aData.sDataSource = m_pProps->sPar1;
                aData.sCommand = m_pProps->sPar2;
                aData.nCommandType = m_pProps->nSHORT1;
                pFld = new SwDBNumSetField( (SwDBNumSetFieldType*)
                    pDoc->GetSysFldType(RES_DBNUMSETFLD),
                    m_pProps->sPar3,
                    String::CreateFromInt32(m_pProps->nFormat),
                    aData );
            }
            break;
            case SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM:
            {
                SwDBData aData;
                aData.sDataSource = m_pProps->sPar1;
                aData.sCommand = m_pProps->sPar2;
                aData.nCommandType = m_pProps->nSHORT1;
                pFld = new SwDBSetNumberField((SwDBSetNumberFieldType*)
                        pDoc->GetSysFldType(RES_DBSETNUMBERFLD),
                        aData,
                        m_pProps->nUSHORT1);
                ((SwDBSetNumberField*)pFld)->SetSetNumber(m_pProps->nFormat);
                sal_uInt16  nSubType = pFld->GetSubType();
                if(m_pProps->bBool2)
                    nSubType &= ~nsSwExtendedSubType::SUB_INVISIBLE;
                else
                    nSubType |= nsSwExtendedSubType::SUB_INVISIBLE;
                pFld->SetSubType(nSubType);
            }
            break;
            case SW_SERVICE_FIELDTYPE_DATABASE:
            {
                SwFieldType* pFldType = pDoc->GetFldType(RES_DBFLD, m_sTypeName, sal_False);
                if(!pFldType)
                    throw uno::RuntimeException();
                pFld = new SwDBField((SwDBFieldType*)pFldType, m_pProps->nFormat);
                ((SwDBField*)pFld)->InitContent(m_pProps->sPar1);
                sal_uInt16  nSubType = pFld->GetSubType();
                if(m_pProps->bBool2)
                    nSubType &= ~nsSwExtendedSubType::SUB_INVISIBLE;
                else
                    nSubType |= nsSwExtendedSubType::SUB_INVISIBLE;
                pFld->SetSubType(nSubType);
            }
            break;
            case SW_SERVICE_FIELDTYPE_SET_EXP:
            {
                SwFieldType* pFldType = pDoc->GetFldType(RES_SETEXPFLD, m_sTypeName, sal_True);
                if(!pFldType)
                    throw uno::RuntimeException();
                //#93192# detect the field type's sub type and set an appropriate number format
                if(m_pProps->bFormatIsDefault &&
                    nsSwGetSetExpType::GSE_STRING == ((SwSetExpFieldType*)pFldType)->GetType())
                        m_pProps->nFormat = -1;
                pFld = new SwSetExpField((SwSetExpFieldType*)pFldType,
                    m_pProps->sPar2,
                    m_pProps->nUSHORT2 != USHRT_MAX ?  //#i79471# the field can have a number format or a number_ing_ format
                    m_pProps->nUSHORT2 : m_pProps->nFormat);

                sal_uInt16	nSubType = pFld->GetSubType();
                if(m_pProps->bBool2)
                    nSubType &= ~nsSwExtendedSubType::SUB_INVISIBLE;
                else
                    nSubType |= nsSwExtendedSubType::SUB_INVISIBLE;
                if(m_pProps->bBool3)
                    nSubType |= nsSwExtendedSubType::SUB_CMD;
                else
                    nSubType &= ~nsSwExtendedSubType::SUB_CMD;
                pFld->SetSubType(nSubType);
                ((SwSetExpField*)pFld)->SetSeqNumber( m_pProps->nUSHORT1 );
                ((SwSetExpField*)pFld)->SetInputFlag(m_pProps->bBool1);
                ((SwSetExpField*)pFld)->SetPromptText(m_pProps->sPar3);
                if(m_pProps->sPar4.Len())
                    ((SwSetExpField*)pFld)->ChgExpStr(m_pProps->sPar4);

            }
            break;
            case SW_SERVICE_FIELDTYPE_GET_EXP:
            {
                sal_uInt16 nSubType;
                switch(m_pProps->nSubType)
                {
                    case text::SetVariableType::STRING: nSubType = nsSwGetSetExpType::GSE_STRING;   break;
                    case text::SetVariableType::VAR:        nSubType = nsSwGetSetExpType::GSE_EXPR;  break;
                    //case text::SetVariableType::SEQUENCE:   nSubType = nsSwGetSetExpType::GSE_SEQ;  break;
                    case text::SetVariableType::FORMULA:    nSubType = nsSwGetSetExpType::GSE_FORMULA; break;
                    default:
                        DBG_ERROR("wrong value");
                        nSubType = nsSwGetSetExpType::GSE_EXPR;
                }
                //make sure the SubType matches the field type
                SwFieldType* pSetExpFld = pDoc->GetFldType(RES_SETEXPFLD, m_pProps->sPar1, sal_False);
                if( pSetExpFld && nSubType != nsSwGetSetExpType::GSE_STRING &&
                    static_cast< SwSetExpFieldType* >(pSetExpFld)->GetType() == nsSwGetSetExpType::GSE_STRING
                    )
                    nSubType = nsSwGetSetExpType::GSE_STRING;

                if(m_pProps->bBool2)
                    nSubType |= nsSwExtendedSubType::SUB_CMD;
                else
                    nSubType &= ~nsSwExtendedSubType::SUB_CMD;
                pFld = new SwGetExpField((SwGetExpFieldType*)
                        pDoc->GetSysFldType(RES_GETEXPFLD),
                        m_pProps->sPar1, nSubType, m_pProps->nFormat);
                //TODO: SubType auswerten!
                if(m_pProps->sPar4.Len())
                    ((SwGetExpField*)pFld)->ChgExpStr(m_pProps->sPar4);
            }
            break;
            case SW_SERVICE_FIELDTYPE_INPUT_USER:
            case SW_SERVICE_FIELDTYPE_INPUT:
            {
                SwFieldType* pFldType = pDoc->GetFldType(RES_INPUTFLD, m_sTypeName, sal_True);
                if(!pFldType)
                    throw uno::RuntimeException();
                USHORT nInpSubType = sal::static_int_cast< USHORT >(SW_SERVICE_FIELDTYPE_INPUT_USER == m_nServiceId ? INP_USR : INP_TXT);
                SwInputField * pTxtField =
                    new SwInputField((SwInputFieldType*)pFldType,
                                     m_pProps->sPar1, m_pProps->sPar2,
                                     nInpSubType);
                pTxtField->SetHelp(m_pProps->sPar3);
                pTxtField->SetToolTip(m_pProps->sPar4);

                pFld = pTxtField;
            }
            break;
            case SW_SERVICE_FIELDTYPE_MACRO:
            {
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_MACROFLD);
                String aName;

                // support for Scripting Framework macros
                if (m_pProps->sPar4.Len() != 0)
                {
                    aName = m_pProps->sPar4;
                }
                else
                {
                    SwMacroField::CreateMacroString(
                        aName, m_pProps->sPar1, m_pProps->sPar3 );
                }
                pFld = new SwMacroField((SwMacroFieldType*)pFldType, aName,
                                        m_pProps->sPar2);
            }
            break;
            case SW_SERVICE_FIELDTYPE_PAGE_COUNT            :
            case SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT       :
            case SW_SERVICE_FIELDTYPE_WORD_COUNT            :
            case SW_SERVICE_FIELDTYPE_CHARACTER_COUNT       :
            case SW_SERVICE_FIELDTYPE_TABLE_COUNT           :
            case SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT  :
            case SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT :
            {
                sal_uInt16 nSubType = DS_PAGE;
                switch(m_nServiceId)
                {
//					case SW_SERVICE_FIELDTYPE_PAGE_COUNT            : break;
                    case SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT       : nSubType = DS_PARA;break;
                    case SW_SERVICE_FIELDTYPE_WORD_COUNT            : nSubType = DS_WORD;break;
                    case SW_SERVICE_FIELDTYPE_CHARACTER_COUNT       : nSubType = DS_CHAR;break;
                    case SW_SERVICE_FIELDTYPE_TABLE_COUNT           : nSubType = DS_TBL;break;
                    case SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT  : nSubType = DS_GRF;break;
                    case SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT : nSubType = DS_OLE;break;
                }
                SwFieldType* pFldType = pDoc->GetSysFldType(RES_DOCSTATFLD);
                pFld = new SwDocStatField((SwDocStatFieldType*)pFldType, nSubType, m_pProps->nUSHORT2);
            }
            break;
            case SW_SERVICE_FIELDTYPE_BIBLIOGRAPHY:
                pFld = new SwAuthorityField( (SwAuthorityFieldType*)
                        pDoc->InsertFldType(SwAuthorityFieldType(pDoc)),
                        aEmptyStr );
                if(m_pProps->aPropSeq.getLength())
                {
                    uno::Any aVal; aVal <<= m_pProps->aPropSeq;
                    pFld->PutValue( aVal, FIELD_PROP_PROP_SEQ );
                }
                break;
            case SW_SERVICE_FIELDTYPE_COMBINED_CHARACTERS:
                // create field
                pFld = new SwCombinedCharField( (SwCombinedCharFieldType*)
                            pDoc->GetSysFldType(RES_COMBINED_CHARS),
                            m_pProps->sPar1);
                break;
            case SW_SERVICE_FIELDTYPE_DROPDOWN:
                pFld = new SwDropDownField
                    ((SwDropDownFieldType *)
                     pDoc->GetSysFldType(RES_DROPDOWN));

                ((SwDropDownField *) pFld)->SetItems(m_pProps->aStrings);
                ((SwDropDownField *) pFld)->SetSelectedItem(m_pProps->sPar1);
                ((SwDropDownField *) pFld)->SetName(m_pProps->sPar2);
                ((SwDropDownField *) pFld)->SetHelp(m_pProps->sPar3);
                ((SwDropDownField *) pFld)->SetToolTip(m_pProps->sPar4);
                break;

            case SW_SERVICE_FIELDTYPE_TABLE_FORMULA :
            {

                // create field
                USHORT nType = nsSwGetSetExpType::GSE_FORMULA;
                if(m_pProps->bBool1)
                {
                    nType |= nsSwExtendedSubType::SUB_CMD;
                    if(m_pProps->bFormatIsDefault)
                        m_pProps->nFormat = -1;
                }
                pFld = new SwTblField( (SwTblFieldType*)
                    pDoc->GetSysFldType(RES_TABLEFLD),
                    m_pProps->sPar2,
                    nType,
                    m_pProps->nFormat);
               ((SwTblField*)pFld)->ChgExpStr(m_pProps->sPar1);
            }
            break;
            default: DBG_ERROR("was ist das fuer ein Typ?");
        }
        if(pFld)
        {
            pFld->SetAutomaticLanguage(!m_pProps->bBool4);
            SwFmtFld aFmt( *pFld );

            UnoActionContext aCont(pDoc);
            SwTxtAttr* pTxtAttr = 0;
            if(aPam.HasMark())
                pDoc->DeleteAndJoin(aPam);
            pDoc->Insert(aPam, aFmt, 10000);
            pTxtAttr = aPam.GetNode()->GetTxtNode()->GetTxtAttr(
                    aPam.GetPoint()->nContent.GetIndex()-1, RES_TXTATR_FIELD);

            // was passiert mit dem Update der Felder ? (siehe fldmgr.cxx)
            if(pTxtAttr)
            {
                const SwFmtFld& rFld = pTxtAttr->GetFld();
                pFmtFld = &rFld;
            }
        }
        delete pFld;

        m_pDoc = pDoc;
        m_pDoc->GetUnoCallBack()->Add(this);
        m_bIsDescriptor = sal_False;
        if(m_aFieldTypeClient.GetRegisteredIn())
            const_cast<SwModify*>(m_aFieldTypeClient.GetRegisteredIn())->Remove(&m_aFieldTypeClient);
        DELETEZ(m_pProps);
        if(m_bCallUpdate)
            update();
    }
    else
        throw lang::IllegalArgumentException();
}
/*-- 14.12.98 11:37:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::attach(const uno::Reference< text::XTextRange > & xTextRange)
    throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    attachToRange( xTextRange );
}
/*-- 14.12.98 11:37:18---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< text::XTextRange >  SwXTextField::getAnchor(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Reference< text::XTextRange >   aRef;
    SwField* pField = (SwField*)GetField();
    if(pField)
    {
        const SwTxtFld* pTxtFld = pFmtFld->GetTxtFld();
        if(!pTxtFld)
            throw uno::RuntimeException();
        const SwTxtNode& rTxtNode = pTxtFld->GetTxtNode();

        SwPaM aPam(rTxtNode, *pTxtFld->GetStart() + 1, rTxtNode, *pTxtFld->GetStart());

        aRef = SwXTextRange::CreateTextRangeFromPosition(m_pDoc,
                                *aPam.GetPoint(), aPam.GetMark());
    }
    return aRef;

}
/*-- 14.12.98 11:37:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::dispose(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwField* pField = (SwField*)GetField();
    if(pField)
    {
        UnoActionContext aContext(GetDoc());
        const SwTxtFld* pTxtFld = pFmtFld->GetTxtFld();
        SwTxtNode& rTxtNode = (SwTxtNode&)*pTxtFld->GetpTxtNode();
        SwPaM aPam(rTxtNode, *pTxtFld->GetStart());
        aPam.SetMark();
        aPam.Move();
        GetDoc()->DeleteAndJoin(aPam);
    }

    if ( m_pTextObject )
    {
        m_pTextObject->DisposeEditSource();
        m_pTextObject->release();
        m_pTextObject = 0;
    }
}
/*-- 14.12.98 11:37:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::addEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn())
        throw uno::RuntimeException();
    aLstnrCntnr.AddListener(aListener);
}
/*-- 14.12.98 11:37:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::removeEventListener(const uno::Reference< lang::XEventListener > & aListener) throw( uno::RuntimeException )
{
    if(!GetRegisteredIn() || !aLstnrCntnr.RemoveListener(aListener))
        throw uno::RuntimeException();
}
/*-- 14.12.98 11:37:19---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< beans::XPropertySetInfo >  SwXTextField::getPropertySetInfo(void)
        throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    //kein static
    uno::Reference< beans::XPropertySetInfo >  aRef;
    if(m_nServiceId != USHRT_MAX)
    {
        const SfxItemPropertyMap* pMap = aSwMapProvider.GetPropertyMap(
                        lcl_GetPropertyMapOfService( m_nServiceId ));
        uno::Reference< beans::XPropertySetInfo >  xInfo = new SfxItemPropertySetInfo(pMap);
        // extend PropertySetInfo!
        const uno::Sequence<beans::Property> aPropSeq = xInfo->getProperties();
        aRef = new SfxExtItemPropertySetInfo(
            aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARAGRAPH_EXTENSIONS),
            aPropSeq );
    }
    else
        throw uno::RuntimeException();
    return aRef;
}
/*-- 14.12.98 11:37:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::setPropertyValue(const OUString& rPropertyName, const uno::Any& rValue)
    throw( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException,
        lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    SwField* pField = (SwField*)GetField();
    const SfxItemPropertyMap* _pMap = aSwMapProvider.GetPropertyMap(
                                lcl_GetPropertyMapOfService( m_nServiceId));
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(_pMap, rPropertyName);

    if (!pMap)
        throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );
    if ( pMap->nFlags & beans::PropertyAttribute::READONLY)
        throw beans::PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );

    if(pField)
    {
        // Sonderbehandlung Serienbrieffeld
        sal_uInt16 nWhich = pField->Which();
        if( RES_DBFLD == nWhich &&
            (rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_NAME)) ||
            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_BASE_URL))||
            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_TABLE_NAME))||
            rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_DATA_COLUMN_NAME))))
        {
            // hier muss ein neuer Feldtyp angelegt werden und
            // das Feld an den neuen Typ umgehaengt werden
            DBG_WARNING("not implemented")
        }
        else
        {
            // -> #111840#
            SwDoc * pDoc = GetDoc();

            if (NULL != pDoc)
            {
                SwPosition * pPos = GetPosition();

                ASSERT(pPos, "no position");
                pDoc->PutValueToField( *pPos, rValue, pMap->nWID);

                delete pPos;
            }
            // <- #111840#
        }
        pField->PutValue( rValue, pMap->nWID );
        //#114571# changes of the expanded string have to be notified
        //#to the SwTxtFld
        if(RES_DBFLD == nWhich && pFmtFld->GetTxtFld())
        {
            pFmtFld->GetTxtFld()->Expand();
        }
    }
    else if(m_pProps)
    {
        String* pStr = 0;
        BOOL* pBool = 0;
        switch(pMap->nWID)
        {
        case FIELD_PROP_PAR1:
            pStr = &m_pProps->sPar1;
            break;
        case FIELD_PROP_PAR2:
            pStr = &m_pProps->sPar2;
            break;
        case FIELD_PROP_PAR3:
            pStr = &m_pProps->sPar3;
            break;
        case FIELD_PROP_PAR4:
            pStr = &m_pProps->sPar4;
            break;
        case FIELD_PROP_FORMAT:
            rValue >>= m_pProps->nFormat;
            m_pProps->bFormatIsDefault = sal_False;
            break;
        case FIELD_PROP_SUBTYPE:
            m_pProps->nSubType = SWUnoHelper::GetEnumAsInt32( rValue );
            break;
        case FIELD_PROP_BYTE1 :
            rValue >>= m_pProps->nByte1;
            break;
        case FIELD_PROP_BOOL1 :
            pBool = &m_pProps->bBool1;
            break;
        case FIELD_PROP_BOOL2 :
            pBool = &m_pProps->bBool2;
            break;
        case FIELD_PROP_BOOL3 :
            pBool = &m_pProps->bBool3;
            break;
        case FIELD_PROP_BOOL4:
            pBool = &m_pProps->bBool4;
        break;
        case FIELD_PROP_DATE :
        {
            if(rValue.getValueType() != ::getCppuType(static_cast<const util::Date*>(0)))
                throw lang::IllegalArgumentException();

            util::Date aTemp = *(const util::Date*)rValue.getValue();
            m_pProps->aDate = Date(aTemp.Day, aTemp.Month, aTemp.Year);
        }
        break;
        case FIELD_PROP_USHORT1:
        case FIELD_PROP_USHORT2:
            {
                 sal_Int16 nVal = 0;
                rValue >>= nVal;
                if( FIELD_PROP_USHORT1 == pMap->nWID)
                    m_pProps->nUSHORT1 = nVal;
                else
                    m_pProps->nUSHORT2 = nVal;
            }
            break;
        case FIELD_PROP_SHORT1:
            rValue >>= m_pProps->nSHORT1;
            break;
        case FIELD_PROP_DOUBLE:
            if(rValue.getValueType() != ::getCppuType(static_cast<const double*>(0)))
                throw lang::IllegalArgumentException();
            m_pProps->fDouble = *(double*)rValue.getValue();
            break;

        case FIELD_PROP_DATE_TIME :
            if(!m_pProps->pDateTime)
                m_pProps->pDateTime = new util::DateTime;
            rValue >>= (*m_pProps->pDateTime);
            break;
        case FIELD_PROP_PROP_SEQ:
            rValue >>= m_pProps->aPropSeq;
            break;
        case FIELD_PROP_STRINGS:
            rValue >>= m_pProps->aStrings;
            break;
        }
        if( pStr )
            ::GetString( rValue, *pStr );
        else if( pBool )
        {
            if( rValue.getValueType() == getCppuBooleanType() )
                *pBool = *(sal_Bool*)rValue.getValue();
            else
                throw lang::IllegalArgumentException();
        }
    }
    else
        throw uno::RuntimeException();
}
/*-- 14.12.98 11:37:19---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextField::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    const SwField* pField = GetField();
    const SfxItemPropertyMap* _pMap = aSwMapProvider.GetPropertyMap(
                                lcl_GetPropertyMapOfService( m_nServiceId));
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(_pMap, rPropertyName);
    if(!pMap )
    {
        _pMap = aSwMapProvider.GetPropertyMap(PROPERTY_MAP_PARAGRAPH_EXTENSIONS);
        pMap = SfxItemPropertyMap::GetByName(_pMap, rPropertyName);
    }
    if (!pMap)
        throw beans::UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( this ) );

    switch( pMap->nWID )
    {
    case FN_UNO_TEXT_WRAP:
        aRet <<= text::WrapTextMode_NONE;
        break;
    case FN_UNO_ANCHOR_TYPE:
        aRet <<= text::TextContentAnchorType_AS_CHARACTER;
        break;
    case FN_UNO_ANCHOR_TYPES:
        {
            uno::Sequence<text::TextContentAnchorType> aTypes(1);
            text::TextContentAnchorType* pArray = aTypes.getArray();
            pArray[0] = text::TextContentAnchorType_AS_CHARACTER;
            aRet.setValue(&aTypes, ::getCppuType(static_cast<uno::Sequence<text::TextContentAnchorType>*>(0)));
        }
        break;

    default:
        if( pField )
        {
            if (FIELD_PROP_IS_FIELD_USED      == pMap->nWID ||
                FIELD_PROP_IS_FIELD_DISPLAYED == pMap->nWID)
            {
                sal_Bool bIsFieldUsed       = sal_False;
                sal_Bool bIsFieldDisplayed  = sal_False;

                // in order to have the information about fields
                // correctly evaluated the document needs a layout
                // (has to be already formatted)
                SwDoc *pDoc = GetDoc();
                ViewShell *pViewShell = 0;
                SwEditShell *pEditShell = pDoc ? pDoc->GetEditShell( &pViewShell ) : 0;
                if (pEditShell)
                    pEditShell->CalcLayout();
                else if (pViewShell) // a page preview has no SwEditShell it should only have a view shell
                    pViewShell->CalcLayout();
                else
                    throw uno::RuntimeException();

                // get text node for the text field
                const SwFmtFld *pFldFmt = GetFldFmt();
                const SwTxtFld* pTxtFld = pFldFmt ? pFmtFld->GetTxtFld() : 0;
                if(!pTxtFld)
                    throw uno::RuntimeException();
                const SwTxtNode& rTxtNode = pTxtFld->GetTxtNode();

                // skip fields that are currently not in the document
                // e.g. fields in undo or redo array
                if (rTxtNode.GetNodes().IsDocNodes())
                {
                    sal_Bool bFrame = 0 != rTxtNode.FindLayoutRect().Width(); // oder so
                    sal_Bool bHidden = rTxtNode.IsHidden();
                    if ( !bHidden )
                    {
                        xub_StrLen nHiddenStart;
                        xub_StrLen nHiddenEnd;

                        SwPosition *pPos = pTxtFld->GetPosition();
                        if (!pPos)
                            throw uno::RuntimeException();

                        bHidden = SwScriptInfo::GetBoundsOfHiddenRange( rTxtNode,
                                        pPos->nContent.GetIndex(),
                                        nHiddenStart, nHiddenEnd );
                    }

                    // !bFrame && !bHidden: aller Wahrscheinlichkeit handelt es
                    // sich um ein Feld in einem unbenutzten Seitenstyle
                    //
                    // bHidden: Feld ist versteckt
                    // FME: Problem: Verstecktes Feld in unbenutzter Seitenvorlage =>
                    // bIsFieldUsed = true
                    // bIsFieldDisplayed = false
                    bIsFieldUsed       = bFrame || bHidden;
                    bIsFieldDisplayed  = bIsFieldUsed && !bHidden;
                }
                sal_Bool bRetVal = (FIELD_PROP_IS_FIELD_USED == pMap->nWID) ?
                                            bIsFieldUsed : bIsFieldDisplayed;
                aRet.setValue( &bRetVal, ::getCppuBooleanType() );
            }
            else
                pField->QueryValue( aRet, pMap->nWID );
        }
        else if( m_pProps )     // currently just a descriptor...
        {
            switch(pMap->nWID)
            {
            case FIELD_PROP_TEXT:
                {
                    if (!m_pTextObject)
                    {
                        SwTextAPIEditSource* pObj = new SwTextAPIEditSource( &m_pDoc->GetDocShell()->GetPool() );
                        m_pTextObject = new SwTextAPIObject( pObj );
                        m_pTextObject->acquire();
                    }
                    
                    uno::Reference < text::XText > xText( m_pTextObject  );
                    aRet <<= xText;
                    break;
                }
            case FIELD_PROP_PAR1:
                aRet <<= OUString(m_pProps->sPar1);
                break;
            case FIELD_PROP_PAR2:
                aRet <<= OUString(m_pProps->sPar2);
                break;
            case FIELD_PROP_PAR3:
                aRet <<= OUString(m_pProps->sPar3);
                break;
            case FIELD_PROP_PAR4:
                aRet <<= OUString(m_pProps->sPar4);
                break;
            case FIELD_PROP_FORMAT:
                aRet <<= m_pProps->nFormat;
                break;
            case FIELD_PROP_SUBTYPE:
                aRet <<= m_pProps->nSubType;
                break;
            case FIELD_PROP_BYTE1 :
                aRet <<= m_pProps->nByte1;
                break;
            case FIELD_PROP_BOOL1 :
                aRet.setValue(&m_pProps->bBool1, ::getCppuBooleanType());
                break;
            case FIELD_PROP_BOOL2 :
                aRet.setValue(&m_pProps->bBool2, ::getCppuBooleanType());
                break;
            case FIELD_PROP_BOOL3 :
                aRet.setValue(&m_pProps->bBool3, ::getCppuBooleanType());
                break;
            case FIELD_PROP_BOOL4 :
                aRet.setValue(&m_pProps->bBool4, ::getCppuBooleanType());
                break;
            case FIELD_PROP_DATE :
                aRet.setValue(&m_pProps->aDate, ::getCppuType(static_cast<const util::Date*>(0)));
                break;
            case FIELD_PROP_USHORT1:
                aRet <<= (sal_Int16)m_pProps->nUSHORT1;
                break;
            case FIELD_PROP_USHORT2:
                aRet <<= (sal_Int16)m_pProps->nUSHORT2;
                break;
            case FIELD_PROP_SHORT1:
                aRet <<= m_pProps->nSHORT1;
                break;
            case FIELD_PROP_DOUBLE:
                aRet <<= m_pProps->fDouble;
                break;
            case FIELD_PROP_DATE_TIME :
                if(m_pProps->pDateTime)
                    aRet <<= (*m_pProps->pDateTime);
                break;
            case FIELD_PROP_PROP_SEQ:
                aRet <<= m_pProps->aPropSeq;
                break;
            case FIELD_PROP_STRINGS:
                aRet <<= m_pProps->aStrings;
                break;
            case FIELD_PROP_IS_FIELD_USED:
            case FIELD_PROP_IS_FIELD_DISPLAYED:
                aRet.setValue( sal_False, ::getCppuBooleanType() );
                break;
            }
        }
        else
            throw uno::RuntimeException();
    }
    return aRet;
}
/*-- 14.12.98 11:37:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::addPropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 11:37:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::removePropertyChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 11:37:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::addVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/*-- 14.12.98 11:37:20---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextField::removeVetoableChangeListener(const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/) throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented")
}
/* -----------------------------23.03.01 13:15--------------------------------

 ---------------------------------------------------------------------------*/
void SwXTextField::update(  ) throw (uno::RuntimeException)
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    const SwField* pFld = GetField();
    if(pFld)
    {
        switch(pFld->Which())
        {
            case RES_DATETIMEFLD:
            ((SwDateTimeField*)pFld)->SetDateTime( ::DateTime() );
            break;

            case RES_EXTUSERFLD:
            {
                SwExtUserField* pExtUserFld = (SwExtUserField*)pFld;
                pExtUserFld->SetExpansion( ((SwExtUserFieldType*)pFld->GetTyp())->Expand(
                                            pExtUserFld->GetSubType(),
                                            pExtUserFld->GetFormat() ) );
            }
            break;

            case RES_AUTHORFLD:
            {
                SwAuthorField* pAuthorFld = (SwAuthorField*)pFld;
                pAuthorFld->SetExpansion( ((SwAuthorFieldType*)pFld->GetTyp())->Expand(
                                            pAuthorFld->GetFormat() ) );
            }
            break;

            case RES_FILENAMEFLD:
            {
                SwFileNameField* pFileNameFld = (SwFileNameField*)pFld;
                pFileNameFld->SetExpansion( ((SwFileNameFieldType*)pFld->GetTyp())->Expand(
                                            pFileNameFld->GetFormat() ) );
            }
            break;

            case RES_DOCINFOFLD:
            {
                    SwDocInfoField* pDocInfFld = (SwDocInfoField*)pFld;
                    pDocInfFld->SetExpansion( ((SwDocInfoFieldType*)pFld->GetTyp())->Expand(
                                                pDocInfFld->GetSubType(),
                                                pDocInfFld->GetFormat(),
                                                pDocInfFld->GetLanguage(),
                                                pDocInfFld->GetName() ) );
            }
            break;
        }
        // --> FME 2004-10-06 #116480#
        // Text formatting has to be triggered.
        const_cast<SwFmtFld*>(pFmtFld)->Modify( 0, 0 );
        // <--
    }
    else
        m_bCallUpdate = sal_True;
}
/* -----------------19.03.99 14:11-------------------
 *
 * --------------------------------------------------*/
OUString SwXTextField::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXTextField");
}
/* -----------------19.03.99 14:11-------------------
 *
 * --------------------------------------------------*/

static OUString OldNameToNewName_Impl( const OUString &rOld )
{
    static OUString aOldNamePart1( OUString::createFromAscii(".TextField.DocInfo.") );
    static OUString aOldNamePart2( OUString::createFromAscii(".TextField.") );
    static OUString aNewNamePart1( OUString::createFromAscii(".textfield.docinfo.") );
    static OUString aNewNamePart2( OUString::createFromAscii(".textfield.") );
    OUString sServiceNameCC( rOld );
    sal_Int32 nIdx = sServiceNameCC.indexOf( aOldNamePart1 );
    if (nIdx >= 0)
        sServiceNameCC = sServiceNameCC.replaceAt( nIdx, aOldNamePart1.getLength(), aNewNamePart1 );
    nIdx = sServiceNameCC.indexOf( aOldNamePart2 );
    if (nIdx >= 0)
        sServiceNameCC = sServiceNameCC.replaceAt( nIdx, aOldNamePart2.getLength(), aNewNamePart2 );
    return sServiceNameCC;
}

sal_Bool SwXTextField::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    OUString sServiceName = SwXServiceProvider::GetProviderName(m_nServiceId);

    // case-corected version of service-name (see #i67811)
    // (need to supply both because of compatibility to older versions)
    OUString sServiceNameCC(  OldNameToNewName_Impl( sServiceName ) );

    return sServiceName == rServiceName || sServiceNameCC == rServiceName ||
        rServiceName.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM("com.sun.star.text.TextContent"));
}
/* -----------------19.03.99 14:11-------------------
 *
 * --------------------------------------------------*/
uno::Sequence< OUString > SwXTextField::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    OUString sServiceName = SwXServiceProvider::GetProviderName(m_nServiceId);

    // case-corected version of service-name (see #i67811)
    // (need to supply both because of compatibility to older versions)
    OUString sServiceNameCC(  OldNameToNewName_Impl( sServiceName ) );
    sal_Int32 nLen = sServiceName == sServiceNameCC ? 2 : 3;

    uno::Sequence< OUString > aRet( nLen );
    OUString* pArray = aRet.getArray();
    *pArray++ = sServiceName;
    if (nLen == 3)
        *pArray++ = sServiceNameCC;
    *pArray++ = C2U("com.sun.star.text.TextContent");
    return aRet;
}

void SwXTextField::Invalidate()
{
    if (GetRegisteredIn())
    {
        ((SwModify*)GetRegisteredIn())->Remove(this);
        aLstnrCntnr.Disposing();
        pFmtFld = 0;
        m_pDoc = 0;
    }
}

/* -----------------14.12.98 12:00-------------------
 *
 * --------------------------------------------------*/
void SwXTextField::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    switch( pOld ? pOld->Which() : 0 )
    {
    case RES_REMOVE_UNO_OBJECT:
    case RES_OBJECTDYING:
        if( (void*)GetRegisteredIn() == ((SwPtrMsgPoolItem *)pOld)->pObject )
            Invalidate();
        break;

    case RES_FMT_CHG:
        // wurden wir an das neue umgehaengt und wird das alte geloscht?
        if( ((SwFmtChg*)pNew)->pChangedFmt == GetRegisteredIn() &&
            ((SwFmtChg*)pOld)->pChangedFmt->IsFmtInDTOR() )
            Invalidate();
        break;
    case RES_FIELD_DELETED:
        if( (void*)pFmtFld == ((SwPtrMsgPoolItem *)pOld)->pObject )
            Invalidate();
        break;
    }
}
/*-- 14.12.98 11:37:21---------------------------------------------------

  -----------------------------------------------------------------------*/
const SwField*  SwXTextField::GetField() const
{
    if(GetRegisteredIn() && pFmtFld)
        return  pFmtFld->GetFld();
    return 0;
}

// #111840#
SwPosition * SwXTextField::GetPosition()
{
    SwPosition * pResult = NULL;
    const SwFmtFld * pFmtFld2 = GetFldFmt();

    if (pFmtFld2)
    {
        const SwTxtFld * pTxtFld = pFmtFld2->GetTxtFld();

        if (pTxtFld)
            pResult = pTxtFld->GetPosition();
    }

    return pResult;
}

/******************************************************************
 *
 ******************************************************************/
/******************************************************************
 * SwXTextFieldMasters
 ******************************************************************/
/* -----------------------------06.04.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextFieldMasters::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXTextFieldMasters");
}
/* -----------------------------06.04.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextFieldMasters::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return rServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                            "com.sun.star.text.TextFieldMasters" ));
}
/* -----------------------------06.04.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXTextFieldMasters::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextFieldMasters");
    return aRet;
}
/*-- 21.12.98 10:37:14---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextFieldMasters::SwXTextFieldMasters(SwDoc* _pDoc) :
    SwUnoCollection(_pDoc)
{
}
/*-- 21.12.98 10:37:32---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextFieldMasters::~SwXTextFieldMasters()
{

}
/*-- 21.12.98 10:37:33---------------------------------------------------
    Iteration ueber nicht-Standard Feldtypen
    USER/SETEXP/DDE/DATABASE
    Der Name ist demnach:
    "com.sun.star.text.fieldmaster.User" + <Feltypname>
    "com.sun.star.text.fieldmaster.DDE" + <Feltypname>
    "com.sun.star.text.fieldmaster.SetExpression" + <Feltypname>
    "com.sun.star.text.fieldmaster.DataBase" + <Feltypname>

    Falls wir grosszuegig werden wollen, dann koennte man com.sun.star.text
    auch optional weglassen
  -----------------------------------------------------------------------*/

sal_uInt16 lcl_GetIdByName( String& rName, String& rTypeName )
{
    if( rName.EqualsAscii( COM_TEXT_FLDMASTER, 0, RTL_CONSTASCII_LENGTH(COM_TEXT_FLDMASTER ))
        ||  rName.EqualsAscii( COM_TEXT_FLDMASTER_CC, 0, RTL_CONSTASCII_LENGTH(COM_TEXT_FLDMASTER_CC )))
        rName.Erase(0, 30);

    sal_uInt16 nResId = USHRT_MAX;
    xub_StrLen nFound = 0;
    rTypeName = rName.GetToken( 0, '.', nFound );
    if(rTypeName.EqualsAscii("User"))
        nResId = RES_USERFLD;
    else if(rTypeName.EqualsAscii("DDE"))
        nResId = RES_DDEFLD;
    else if(rTypeName.EqualsAscii("SetExpression"))
    {
        nResId = RES_SETEXPFLD;

        String sFldTypName( rName.GetToken( 1, '.' ));
        String sUIName( SwStyleNameMapper::GetSpecialExtraUIName( sFldTypName ) );

        if( sUIName != sFldTypName )
            rName.SetToken( 1, '.', sUIName );
    }
    else if(rTypeName.EqualsAscii("DataBase"))
    {
        rName.Erase( 0, RTL_CONSTASCII_LENGTH( "DataBase." ));
        USHORT nDotCount = rName.GetTokenCount('.');
        if( 2 <= nDotCount )
        {
            // #i51815#
            //rName.SearchAndReplace('.', DB_DELIM);
            //rName.SetChar( rName.SearchBackward( '.' ), DB_DELIM );

            rName.InsertAscii( "DataBase.", 0 );
            nResId = RES_DBFLD;
        }
    }
    else if( rTypeName.EqualsAscii("Bibliography"))
        nResId = RES_AUTHORITY;
    return nResId;
}

//-----------------------------------------------------------------------------
uno::Any SwXTextFieldMasters::getByName(const OUString& rName)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!GetDoc())
        throw uno::RuntimeException();

    String sName(rName), sTypeName;
    sal_uInt16 nResId = lcl_GetIdByName( sName, sTypeName );
    if( USHRT_MAX == nResId )
        throw container::NoSuchElementException();

    sName.Erase(0, sTypeName.Len()+1);
    SwFieldType* pType = GetDoc()->GetFldType(nResId, sName, sal_True);
    if(!pType)
        throw container::NoSuchElementException();
    SwXFieldMaster* pMaster = (SwXFieldMaster*)
                SwClientIter(*pType).First(TYPE(SwXFieldMaster));
    if(!pMaster)
        pMaster = new SwXFieldMaster(*pType, GetDoc());
    uno::Reference< beans::XPropertySet >  aRef = pMaster;
    uno::Any aRet(&aRef, ::getCppuType( static_cast<const uno::Reference<beans::XPropertySet>* >(0)));
    return aRet;
}
/*-- 06.03.2001 11:29:34,5-------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextFieldMasters::getInstanceName(
    const SwFieldType& rFldType, String& rName)
{
    sal_Bool bRet = sal_True;
    switch( rFldType.Which() )
    {
    case RES_USERFLD:
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( COM_TEXT_FLDMASTER_CC ));
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "User."));
        rName += rFldType.GetName();
        break;
    case RES_DDEFLD:
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( COM_TEXT_FLDMASTER_CC ));
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "DDE."));
        rName += rFldType.GetName();
        break;

    case RES_SETEXPFLD:
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( COM_TEXT_FLDMASTER_CC ));
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "SetExpression."));
        rName += String( SwStyleNameMapper::GetSpecialExtraProgName( rFldType.GetName() ) );
        break;

    case RES_DBFLD:
        {
            rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( COM_TEXT_FLDMASTER_CC ));
            rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "DataBase."));
            String sDBName(rFldType.GetName());
            sDBName.SearchAndReplaceAll(DB_DELIM, '.');
            rName += sDBName;
        }
        break;

    case RES_AUTHORITY:
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( COM_TEXT_FLDMASTER_CC ));
        rName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "Bibliography"));
        break;

    default:
        bRet = sal_False;
    }

    return bRet;
}


/*-- 21.12.98 10:37:33---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXTextFieldMasters::getElementNames(void)
    throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!GetDoc())
        throw uno::RuntimeException();

    const SwFldTypes* pFldTypes = GetDoc()->GetFldTypes();
    sal_uInt16 nCount = pFldTypes->Count();

    SvStrings aFldNames;
    String* pString = new String();
    sal_uInt16 i;

    for( i = 0; i < nCount; i++)
    {
        SwFieldType& rFldType = *((*pFldTypes)[i]);

        if (SwXTextFieldMasters::getInstanceName(rFldType, *pString))
        {
            aFldNames.Insert(pString, aFldNames.Count());
            pString = new String();
        }
    }
    delete pString;

    uno::Sequence< OUString > aSeq(aFldNames.Count());
    OUString* pArray = aSeq.getArray();
    for(i = 0; i < aFldNames.Count();i++)
    {
        pArray[i] = *aFldNames.GetObject(i);
    }
    aFldNames.DeleteAndDestroy(0, aFldNames.Count());

    return aSeq;

}
/*-- 21.12.98 10:37:33---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextFieldMasters::hasByName(const OUString& rName) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!GetDoc())
        throw uno::RuntimeException();

    String sName(rName), sTypeName;
    sal_uInt16 nResId = lcl_GetIdByName( sName, sTypeName );
    sal_Bool bRet = sal_False;
    if( USHRT_MAX != nResId )
    {
        sName.Erase(0, sTypeName.Len()+1);
        bRet = USHRT_MAX != nResId && 0 != GetDoc()->GetFldType(nResId, sName, sal_True);
    }
    return bRet;
}
/*-- 21.12.98 10:37:34---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type  SwXTextFieldMasters::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType(static_cast<const uno::Reference<beans::XPropertySet>*>(0));

}
/*-- 21.12.98 10:37:34---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextFieldMasters::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return sal_True;
}

/******************************************************************
 *
 ******************************************************************/
/* -----------------------------06.04.00 13:24--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextFieldTypes::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXTextFieldTypes");
}
/* -----------------------------06.04.00 13:24--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextFieldTypes::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return rServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                            "com.sun.star.text.TextFields" ));
}
/* -----------------------------06.04.00 13:24--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXTextFieldTypes::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextFields");
    return aRet;
}
/*-- 21.12.98 10:35:15---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextFieldTypes::SwXTextFieldTypes(SwDoc* _pDoc) :
    SwUnoCollection (_pDoc),
    aRefreshCont    ( static_cast< XEnumerationAccess * >(this) )
{
}
/*-- 21.12.98 10:35:16---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextFieldTypes::~SwXTextFieldTypes()
{
}
/*-- 11.07.02 14:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXTextFieldTypes::Invalidate()
{
    SwUnoCollection::Invalidate();
    aRefreshCont.Disposing();
}
/*-- 21.12.98 10:35:17---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< container::XEnumeration >  SwXTextFieldTypes::createEnumeration(void)
    throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return new SwXFieldEnumeration(GetDoc());
}

/*-- 21.12.98 10:35:17---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type  SwXTextFieldTypes::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType(static_cast<const uno::Reference<text::XDependentTextField>*>(0));
}
/*-- 21.12.98 10:35:17---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextFieldTypes::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    //es gibt sie immer
    return sal_True;
}
/* -----------------24.02.99 16:19-------------------
 *
 * --------------------------------------------------*/
void SwXTextFieldTypes::refresh(void)  throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    UnoActionContext aContext(GetDoc());
    SwDocStat aDocStat;
    GetDoc()->UpdateDocStat(aDocStat);
    GetDoc()->UpdateFlds(0, sal_False);

    // call refresh listeners
    aRefreshCont.Refreshed();
}
/* -----------------24.02.99 16:19-------------------
 *
 * --------------------------------------------------*/
void SwXTextFieldTypes::addRefreshListener(const uno::Reference< util::XRefreshListener > & l)
    throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if ( !IsValid() )
        throw uno::RuntimeException();
    aRefreshCont.AddListener ( reinterpret_cast < const uno::Reference < lang::XEventListener > &> ( l ));
}
/* -----------------24.02.99 16:19-------------------
 *
 * --------------------------------------------------*/
void SwXTextFieldTypes::removeRefreshListener(const uno::Reference< util::XRefreshListener > & l)
     throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if ( !IsValid() || !aRefreshCont.RemoveListener ( reinterpret_cast < const uno::Reference < lang::XEventListener > &> ( l ) ) )
        throw uno::RuntimeException();
}
/******************************************************************
 * SwXFieldEnumeration
 ******************************************************************/
/* -----------------------------06.04.00 13:25--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFieldEnumeration::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXFieldEnumeration");
}
/* -----------------------------06.04.00 13:25--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXFieldEnumeration::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return rServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                            "com.sun.star.text.FieldEnumeration" ));
}
/* -----------------------------06.04.00 13:25--------------------------------

 ---------------------------------------------------------------------------*/
uno::Sequence< OUString > SwXFieldEnumeration::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.FieldEnumeration");
    return aRet;
}
/* -----------------21.12.98 14:57-------------------
 *
 * --------------------------------------------------*/
SwXFieldEnumeration::SwXFieldEnumeration(SwDoc* pDc) :
    nNextIndex(0),
    pDoc(pDc)
{
    pDoc->GetPageDescFromPool(RES_POOLPAGE_STANDARD)->Add(this);

    // build sequence
    sal_Int32 nSize = 32;
    aItems.realloc( nSize );
    uno::Reference< text::XTextField > *pItems = aItems.getArray();
    sal_Int32 nFillPos = 0;
    //
    const SwFldTypes* pFldTypes = pDoc->GetFldTypes();
    sal_uInt16 nCount = pFldTypes->Count();
    for(sal_uInt16 nType = 0;  nType < nCount;  ++nType)
    {
        const SwFieldType *pCurType = pFldTypes->GetObject(nType);

        SwClientIter aIter( *(SwFieldType*)pCurType );
        const SwFmtFld* pCurFldFmt = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
        while (pCurFldFmt)
        {
            const SwTxtFld *pTxtFld = pCurFldFmt->GetTxtFld();
            // skip fields that are currently not in the document
            // e.g. fields in undo or redo array
            BOOL bSkip = !pTxtFld ||
                         !pTxtFld->GetpTxtNode()->GetNodes().IsDocNodes();
            if (!bSkip)
                pItems[ nFillPos++ ] = new SwXTextField(*pCurFldFmt, pDoc);
            pCurFldFmt = (SwFmtFld*)aIter.Next();

            // enlarge sequence if necessary
            if (aItems.getLength() == nFillPos)
            {
                aItems.realloc( 2 * aItems.getLength() );
                pItems = aItems.getArray();
            }
        }
    }
    // resize sequence to actual used size
    aItems.realloc( nFillPos );
}
/*-- 21.12.98 14:57:23---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFieldEnumeration::~SwXFieldEnumeration()
{

}
/*-- 21.12.98 14:57:42---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXFieldEnumeration::hasMoreElements(void)
    throw( uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());
    return nNextIndex < aItems.getLength();
}
/*-- 21.12.98 14:57:42---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXFieldEnumeration::nextElement(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard  aGuard(Application::GetSolarMutex());

    if (!(nNextIndex < aItems.getLength()))
        throw container::NoSuchElementException();

#if OSL_DEBUG_LEVEL > 1
    uno::Reference< text::XTextField > *pItems = aItems.getArray();
    (void)pItems;
#endif
    uno::Reference< text::XTextField >  &rxFld = aItems.getArray()[ nNextIndex++ ];
    uno::Any aRet(&rxFld, ::getCppuType(static_cast<const uno::Reference<text::XTextField>*>(0)));
    rxFld = 0;  // free memory for item that is not longer used
    return aRet;
}
/* -----------------21.12.98 15:08-------------------
 *
 * --------------------------------------------------*/
void SwXFieldEnumeration::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
    if(!GetRegisteredIn())
        pDoc = 0;
}


String& GetString( const uno::Any& rAny, String& rStr )
{
    OUString aStr;
    rAny >>= aStr;
    rStr = String( aStr );
    return rStr;
}
