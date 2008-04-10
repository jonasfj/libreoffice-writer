/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: textcontrolcombo.cxx,v $
 * $Revision: 1.6 $
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


#include "textcontrolcombo.hxx"


TextControlCombo::TextControlCombo( Window* _pParent, const ResId& _rResId,
    Control& _rCtrl, FixedText& _rFTbefore, FixedText& _rFTafter )
    :Window			( _pParent, _rResId )
    ,mrCtrl			( _rCtrl )
    ,mrFTbefore		( _rFTbefore )
    ,mrFTafter		( _rFTafter )
{
}

TextControlCombo::~TextControlCombo()
{
}

void TextControlCombo::Arrange( FixedText& _rFTcomplete, BOOL /*bShow*/ )
{
    Point			aBasePos( GetPosPixel() );
    Size			aMetricVals( GetSizePixel() );

    long			nTextHeight = _rFTcomplete.GetSizePixel().Height();
    long			nCtrlHeight = mrCtrl.GetSizePixel().Height();

    // calc y positions / center vertical
    long			nYFT = aBasePos.Y();
    long			nYCtrl = nYFT;
    if( nCtrlHeight > nTextHeight )
        nYFT += aMetricVals.Height();
    else
        nYCtrl += aMetricVals.Height();

    // separate text parts
    const String	aReplStr( RTL_CONSTASCII_STRINGPARAM( "%POSITION_OF_CONTROL" ) );
    String			aTxtBefore( _rFTcomplete.GetText() );
    String			aTxtAfter;
    xub_StrLen		nReplPos = aTxtBefore.Search( aReplStr );
    if( nReplPos != STRING_NOTFOUND )
    {
        xub_StrLen	nStrStartAfter = nReplPos + aReplStr.Len();
        aTxtAfter = String( aTxtBefore, nStrStartAfter, aTxtBefore.Len() - nStrStartAfter );
        aTxtBefore.Erase( nReplPos );
    }

    // arrange and fill Fixed Texts
    long			nX = aBasePos.X();
    long			nWidth = GetTextWidth( aTxtBefore );

    mrFTbefore.SetText( aTxtBefore );
    mrFTbefore.SetPosSizePixel( nX, nYFT, nWidth, nTextHeight );

    nX += nWidth;
    nX += aMetricVals.Width();
    mrCtrl.SetPosPixel( Point( nX, nYCtrl ) );

    nX += mrCtrl.GetSizePixel().Width();
    nX += aMetricVals.Width();
    mrFTafter.SetText( aTxtAfter );
    mrFTafter.SetPosSizePixel( nX, nYFT, GetTextWidth( aTxtAfter ), nTextHeight );

    _rFTcomplete.Hide();

    Show();

    Window::Hide();
}

void TextControlCombo::Show( BOOL _bVisible, USHORT _nFlags )
{
    mrCtrl.Show( _bVisible, _nFlags );
    mrFTbefore.Show( _bVisible, _nFlags );
    mrFTafter.Show( _bVisible, _nFlags );
}

void TextControlCombo::Enable( BOOL _bEnable, BOOL _bChild )
{
    mrCtrl.Enable( _bEnable, _bChild );
    mrFTbefore.Enable( _bEnable, _bChild );
    mrFTafter.Enable( _bEnable, _bChild );
}


