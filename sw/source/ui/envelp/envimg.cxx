/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: envimg.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 07:25:26 $
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


#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _TOOLS_RESID_HXX //autogen
#include <tools/resid.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif
#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _ENVIMG_HXX
#include <envimg.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _ENVELP_HRC
#include <envelp.hrc>
#endif

#ifdef WIN
#define NEXTLINE  UniString::CreateFromAscii("\r\n")
#else
#define NEXTLINE  '\n'
#endif

using namespace utl;
using namespace rtl;
using namespace com::sun::star::uno;

#define C2U(cChar) OUString::createFromAscii(cChar)

TYPEINIT1_AUTOFACTORY( SwEnvItem, SfxPoolItem );

// --------------------------------------------------------------------------
String MakeSender()
{
    SvtUserOptions& rUserOpt = SW_MOD()->GetUserOptions();

    String sRet;
    String sSenderToken(SW_RES(STR_SENDER_TOKENS));
    xub_StrLen nSttPos = 0, nTokenCount = sSenderToken.GetTokenCount(';');
    BOOL bLastLength = TRUE;
    for( xub_StrLen i = 0; i < nTokenCount; i++ )
    {
        String sToken = sSenderToken.GetToken( 0, ';', nSttPos );
        if(sToken.EqualsAscii("COMPANY"))
        {
            xub_StrLen nOldLen = sRet.Len();
            sRet += rUserOpt.GetCompany();
            bLastLength = sRet.Len() != nOldLen;
        }
        else if(sToken.EqualsAscii("CR"))
        {
            if(bLastLength)
                sRet +=NEXTLINE;
            bLastLength = TRUE;
        }
        else if(sToken.EqualsAscii("FIRSTNAME"))
            sRet += rUserOpt.GetFirstName();
        else if(sToken.EqualsAscii("LASTNAME"))
            sRet += rUserOpt.GetLastName();
        else if(sToken.EqualsAscii("ADDRESS"))
            sRet += rUserOpt.GetStreet();
        else if(sToken.EqualsAscii("COUNTRY"))
            sRet += rUserOpt.GetCountry();
        else if(sToken.EqualsAscii("POSTALCODE"))
            sRet += rUserOpt.GetZip();
        else if(sToken.EqualsAscii("CITY"))
            sRet += rUserOpt.GetCity();
        else if(sToken.EqualsAscii("STATEPROV"))
            sRet += rUserOpt.GetState();
        else if(sToken.Len()) //spaces
            sRet += sToken;
    }
    return sRet;
}
// --------------------------------------------------------------------------
SwEnvItem::SwEnvItem() :
    SfxPoolItem(FN_ENVELOP)
{
    aAddrText       = aEmptyStr;
    bSend           = TRUE;
    aSendText       = MakeSender();
    lSendFromLeft   = 566; // 1 cm
    lSendFromTop    = 566; // 1 cm
    lWidth          = lC65Width;
    lHeight         = lC65Height;
    eAlign          = ENV_HOR_LEFT;
    bPrintFromAbove = TRUE;
    lShiftRight     = 0;
    lShiftDown      = 0;

    lAddrFromLeft   = Max(lWidth, lHeight) / 2;
    lAddrFromTop    = Min(lWidth, lHeight) / 2;
}
// --------------------------------------------------------------------------
SwEnvItem::SwEnvItem(const SwEnvItem& rItem) :
    SfxPoolItem(FN_ENVELOP),
    aAddrText      (rItem.aAddrText),
    bSend          (rItem.bSend),
    aSendText      (rItem.aSendText),
    lSendFromLeft  (rItem.lSendFromLeft),
    lSendFromTop   (rItem.lSendFromTop),
    lAddrFromLeft  (rItem.lAddrFromLeft),
    lAddrFromTop   (rItem.lAddrFromTop),
    lWidth         (rItem.lWidth),
    lHeight        (rItem.lHeight),
    eAlign         (rItem.eAlign),
    bPrintFromAbove(rItem.bPrintFromAbove),
    lShiftRight    (rItem.lShiftRight),
    lShiftDown     (rItem.lShiftDown)
{
}

// --------------------------------------------------------------------------
SwEnvItem& SwEnvItem::operator =(const SwEnvItem& rItem)
{
    aAddrText       = rItem.aAddrText;
    bSend           = rItem.bSend;
    aSendText       = rItem.aSendText;
    lSendFromLeft   = rItem.lSendFromLeft;
    lSendFromTop    = rItem.lSendFromTop;
    lAddrFromLeft   = rItem.lAddrFromLeft;
    lAddrFromTop    = rItem.lAddrFromTop;
    lWidth          = rItem.lWidth;
    lHeight         = rItem.lHeight;
    eAlign          = rItem.eAlign;
    bPrintFromAbove = rItem.bPrintFromAbove;
    lShiftRight     = rItem.lShiftRight;
    lShiftDown      = rItem.lShiftDown;
    return *this;
}
// --------------------------------------------------------------------------
int SwEnvItem::operator ==(const SfxPoolItem& rItem) const
{
    const SwEnvItem& rEnv = (const SwEnvItem&) rItem;

    return aAddrText       == rEnv.aAddrText       &&
           bSend           == rEnv.bSend           &&
           aSendText       == rEnv.aSendText       &&
           lSendFromLeft   == rEnv.lSendFromLeft   &&
           lSendFromTop    == rEnv.lSendFromTop    &&
           lAddrFromLeft   == rEnv.lAddrFromLeft   &&
           lAddrFromTop    == rEnv.lAddrFromTop    &&
           lWidth          == rEnv.lWidth          &&
           lHeight         == rEnv.lHeight         &&
           eAlign          == rEnv.eAlign          &&
           bPrintFromAbove == rEnv.bPrintFromAbove &&
           lShiftRight     == rEnv.lShiftRight     &&
           lShiftDown      == rEnv.lShiftDown;
}

// --------------------------------------------------------------------------
SfxPoolItem* SwEnvItem::Clone(SfxItemPool*) const
{
    return new SwEnvItem(*this);
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
SwEnvCfgItem::SwEnvCfgItem() :
    ConfigItem(C2U("Office.Writer/Envelope"))
{
    Sequence<OUString> aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    EnableNotification(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed")
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case  0: pValues[nProp] >>= aEnvItem.aAddrText; break;// "Inscription/Addressee",
                    case  1: pValues[nProp] >>= aEnvItem.aSendText; break;// "Inscription/Sender",
                    case  2: aEnvItem.bSend = *(sal_Bool*)pValues[nProp].getValue(); break;// "Inscription/UseSender",
                    case  3:
                        pValues[nProp] >>= aEnvItem.lAddrFromLeft;// "Format/AddresseeFromLeft",
                        aEnvItem.lAddrFromLeft = MM100_TO_TWIP(aEnvItem.lAddrFromLeft);
                    break;
                    case  4:
                        pValues[nProp] >>= aEnvItem.lAddrFromTop;  // "Format/AddresseeFromTop",
                        aEnvItem.lAddrFromTop = MM100_TO_TWIP(aEnvItem.lAddrFromTop);
                    break;
                    case  5:
                        pValues[nProp] >>= aEnvItem.lSendFromLeft; // "Format/SenderFromLeft",
                        aEnvItem.lSendFromLeft = MM100_TO_TWIP(aEnvItem.lSendFromLeft);
                    break;
                    case  6:
                        pValues[nProp] >>= aEnvItem.lSendFromTop;// "Format/SenderFromTop",
                        aEnvItem.lSendFromTop = MM100_TO_TWIP(aEnvItem.lSendFromTop);
                    break;
                    case  7:
                        pValues[nProp] >>= aEnvItem.lWidth; // "Format/Width",
                        aEnvItem.lWidth = MM100_TO_TWIP(aEnvItem.lWidth);
                    break;
                    case  8:
                        pValues[nProp] >>= aEnvItem.lHeight; // "Format/Height",
                        aEnvItem.lHeight = MM100_TO_TWIP(aEnvItem.lHeight);
                    break;
                    case  9:
                    {
                        sal_Int32 nTemp;
                        pValues[nProp] >>= nTemp; aEnvItem.eAlign = (SwEnvAlign)nTemp; break;// "Print/Alignment",
                    }
                    case 10: aEnvItem.bPrintFromAbove = *(sal_Bool*)pValues[nProp].getValue(); break;// "Print/FromAbove",
                    case 11:
                        pValues[nProp] >>= aEnvItem.lShiftRight;
                        aEnvItem.lShiftRight = MM100_TO_TWIP(aEnvItem.lShiftRight);// "Print/Right",
                    break;
                    case 12:
                        pValues[nProp] >>= aEnvItem.lShiftDown;
                        aEnvItem.lShiftDown = MM100_TO_TWIP(aEnvItem.lShiftDown);
                    break;// "Print/Down"
                }
            }
        }
    }
}
/* -----------------------------26.09.00 14:04--------------------------------

 ---------------------------------------------------------------------------*/
SwEnvCfgItem::~SwEnvCfgItem()
{
}
/* -----------------------------26.09.00 14:05--------------------------------

 ---------------------------------------------------------------------------*/
void	SwEnvCfgItem::Commit()
{
    Sequence<OUString> aNames = GetPropertyNames();
    OUString* pNames = aNames.getArray();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    const Type& rType = ::getBooleanCppuType();
    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case  0: pValues[nProp] <<= aEnvItem.aAddrText; break;// "Inscription/Addressee",
            case  1: pValues[nProp] <<= aEnvItem.aSendText; break;// "Inscription/Sender",
            case  2: pValues[nProp].setValue(&aEnvItem.bSend, rType);break;// "Inscription/UseSender",
            case  3: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lAddrFromLeft) ; break;// "Format/AddresseeFromLeft",
            case  4: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lAddrFromTop)  ; break;// "Format/AddresseeFromTop",
            case  5: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lSendFromLeft) ; break;// "Format/SenderFromLeft",
            case  6: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lSendFromTop)  ; break;// "Format/SenderFromTop",
            case  7: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lWidth)  ; break;// "Format/Width",
            case  8: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lHeight) ; break;// "Format/Height",
            case  9: pValues[nProp] <<= sal_Int32(aEnvItem.eAlign); break;// "Print/Alignment",
            case 10: pValues[nProp].setValue(&aEnvItem.bPrintFromAbove, rType); break;// "Print/FromAbove",
            case 11: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lShiftRight);break; // "Print/Right",
            case 12: pValues[nProp] <<= TWIP_TO_MM100(aEnvItem.lShiftDown); break;// "Print/Down"
        }
    }
    PutProperties(aNames, aValues);
}
/* -----------------------------26.09.00 14:04--------------------------------

 ---------------------------------------------------------------------------*/
Sequence<rtl::OUString> SwEnvCfgItem::GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "Inscription/Addressee",	//	0
        "Inscription/Sender",		//  1
        "Inscription/UseSender",	//  2
        "Format/AddresseeFromLeft",	//  3
        "Format/AddresseeFromTop",	//  4
        "Format/SenderFromLeft",	//  5
        "Format/SenderFromTop",		//  6
        "Format/Width",				//  7
        "Format/Height",			//  8
        "Print/Alignment",			//  9
        "Print/FromAbove",			// 10
        "Print/Right",				// 11
        "Print/Down"				// 12
    };
    const int nCount = 13;
    Sequence<OUString> aNames(nCount);
    OUString* pNames = aNames.getArray();
    for(int i = 0; i < nCount; i++)
        pNames[i] = OUString::createFromAscii(aPropNames[i]);
    return aNames;
}

BOOL SwEnvItem::QueryValue( Any& rVal, BYTE nMemberId ) const
{
    sal_Bool bRet = sal_True;
    switch(nMemberId & ~CONVERT_TWIPS)
    {
        case MID_ENV_ADDR_TEXT : rVal <<= aAddrText; break;
        case MID_ENV_SEND : rVal <<= bSend; break;
        case MID_SEND_TEXT : rVal <<= aSendText; break;
        case MID_ENV_ADDR_FROM_LEFT : rVal <<= lAddrFromLeft; break;
        case MID_ENV_ADDR_FROM_TOP : rVal <<= lAddrFromTop; break;
        case MID_ENV_SEND_FROM_LEFT : rVal <<= lSendFromLeft; break;
        case MID_ENV_SEND_FROM_TOP : rVal <<= lSendFromTop; break;
        case MID_ENV_WIDTH : rVal <<= lWidth; break;
        case MID_ENV_HEIGHT : rVal <<= lHeight; break;
        case MID_ENV_ALIGN            : rVal <<= (sal_Int16) eAlign; break;
        case MID_ENV_PRINT_FROM_ABOVE  : rVal <<= bPrintFromAbove; break;
        case MID_ENV_SHIFT_RIGHT      : rVal <<= lShiftRight; break;
        case MID_ENV_SHIFT_DOWN       : rVal <<= lShiftDown; break;
        default:
            DBG_ERROR("Wrong memberId");
            bRet = sal_False;
    }
    return bRet;
}
/* -----------------------------26.04.01 12:26--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwEnvItem::PutValue(const Any& rVal, BYTE nMemberId)
{
    BOOL bRet = FALSE;
    switch(nMemberId  & ~CONVERT_TWIPS)
    {
        case MID_ENV_ADDR_TEXT : bRet = (rVal >>= aAddrText); break;
        case MID_ENV_SEND : bRet = (rVal >>= bSend); break;
        case MID_SEND_TEXT : bRet = (rVal >>= aSendText); break;
        case MID_ENV_ADDR_FROM_LEFT : bRet = (rVal >>= lAddrFromLeft); break;
        case MID_ENV_ADDR_FROM_TOP : bRet = (rVal >>= lAddrFromTop); break;
        case MID_ENV_SEND_FROM_LEFT : bRet = (rVal >>= lSendFromLeft); break;
        case MID_ENV_SEND_FROM_TOP : bRet = (rVal >>= lSendFromTop); break;
        case MID_ENV_WIDTH : bRet = (rVal >>= lWidth); break;
        case MID_ENV_HEIGHT : bRet = (rVal >>= lHeight); break;
        case MID_ENV_ALIGN :
        {
            sal_Int16 nTemp;
            bRet = (rVal >>= nTemp);
            if (bRet)
                eAlign = SwEnvAlign(nTemp);
            break;
        }
        case MID_ENV_PRINT_FROM_ABOVE  : bRet = (rVal >>= bPrintFromAbove); break;
        case MID_ENV_SHIFT_RIGHT      : bRet = (rVal >>= lShiftRight); break;
        case MID_ENV_SHIFT_DOWN       : bRet = (rVal >>= lShiftDown); break;
        default:
            DBG_ERROR("Wrong memberId");
    }
    return bRet;
}
