/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef _TEXTCONTROLCOMBO_HXX
#define _TEXTCONTROLCOMBO_HXX

#ifndef _FIXED_HXX
#include <vcl/fixed.hxx>
#endif

#include <vcl/field.hxx>
#include "swdllapi.h"

class SW_DLLPUBLIC TextControlCombo : public Window
{
protected:

    Control&	mrCtrl;
    FixedText&	mrFTbefore;
    FixedText&	mrFTafter;

public:

    using Window::Enable;
    using Window::Disable;

                TextControlCombo( Window* _pParent, const ResId& _rResId,
                            Control& _rCtrl, FixedText& _rFTbefore, FixedText& _rFTafter );
    virtual		~TextControlCombo();

    void		Arrange( FixedText& _rOrg, BOOL bShow = true );

    // identical to window functionality
    void		Show( BOOL bVisible = TRUE, USHORT nFlags = 0 );
    void		Hide( USHORT nFlags = 0 ) { Show( FALSE, nFlags ); }
    
    void		Enable( BOOL bEnable = TRUE, BOOL bChild = TRUE );
    void		Disable( BOOL bChild = TRUE ) { Enable( FALSE, bChild ); }
};


#endif
