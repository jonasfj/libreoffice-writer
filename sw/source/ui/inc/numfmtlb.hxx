/*************************************************************************
 *
 *  $RCSfile: numfmtlb.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2004-08-12 13:05:57 $
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
#ifndef _SWNUMFMTLB_HXX
#define _SWNUMFMTLB_HXX

#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif

class SwView;

class NumFormatListBox : public ListBox
{
    short				nCurrFormatType;
    USHORT				nStdEntry;
    BOOL				bOneArea;
    ULONG				nDefFormat;
    SwView*				pVw;
    SvNumberFormatter*	pOwnFormatter;
    LanguageType		eCurLanguage;
    BOOL                bShowLanguageControl; //determine whether the language control has 
                                              //to be shown in the number format dialog
    BOOL                bUseAutomaticLanguage;//determine whether language is automatically assigned

    DECL_LINK( SelectHdl, ListBox * );

    double          GetDefValue(const short nFormatType) const;
    void            Init(short nFormatType, BOOL bUsrFmts);
    SwView*			GetView();

public:
    NumFormatListBox( Window* pWin, const ResId& rResId,
        short nFormatType = NUMBERFORMAT_NUMBER, ULONG nDefFmt = 0,
        BOOL bUsrFmts = TRUE );

    NumFormatListBox( Window* pWin, SwView* pView, const ResId& rResId,
        short nFormatType = NUMBERFORMAT_NUMBER, ULONG nDefFmt = 0,
        BOOL bUsrFmts = TRUE );

    ~NumFormatListBox();

    void            Clear();

    inline void		SetOneArea(BOOL bOnlyOne = TRUE) { bOneArea = bOnlyOne; }

    void			SetFormatType(const short nFormatType);
    inline short	GetFormatType() const { return nCurrFormatType; }
    void			SetDefFormat(const ULONG nDefFmt);
    ULONG			GetFormat() const;

    inline LanguageType	GetCurLanguage() const { return eCurLanguage;}
    void                SetLanguage(LanguageType eSet)  { eCurLanguage = eSet;}

    void            SetAutomaticLanguage(BOOL bSet){bUseAutomaticLanguage = bSet;}
    BOOL            IsAutomaticLanguage()const {return bUseAutomaticLanguage;}

    void            SetShowLanguageControl(BOOL bSet){bShowLanguageControl = bSet;}

};


#endif

