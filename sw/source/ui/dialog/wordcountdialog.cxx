/*************************************************************************
 *
 *  $RCSfile: wordcountdialog.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-04-27 13:43:55 $
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
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef SW_WORDCOUNTDIALOG_HXX
#include <wordcountdialog.hxx>
#endif
#ifndef _DOCSTAT_HXX
#include <docstat.hxx>
#endif

#include <dialog.hrc>
#include <wordcountdialog.hrc>

/*-- 06.04.2004 16:05:55---------------------------------------------------

  -----------------------------------------------------------------------*/
SwWordCountDialog::SwWordCountDialog(Window* pParent) :
    SfxModalDialog(pParent, SW_RES(DLG_WORDCOUNT)), 
#pragma warning (disable : 4355)
    aCurrentFL( this, ResId(              FL_CURRENT            )),
    aCurrentWordFT( this, ResId(          FT_CURRENTWORD        )),
    aCurrentWordFI( this, ResId(          FI_CURRENTWORD        )),
    aCurrentCharacterFT( this, ResId(     FT_CURRENTCHARACTER   )),
    aCurrentCharacterFI( this, ResId(     FI_CURRENTCHARACTER   )),

    aDocFL( this, ResId(                  FL_DOC                )),
    aDocWordFT( this, ResId(              FT_DOCWORD            )),
    aDocWordFI( this, ResId(              FI_DOCWORD            )),
    aDocCharacterFT( this, ResId(         FT_DOCCHARACTER       )),
    aDocCharacterFI( this, ResId(         FI_DOCCHARACTER       )),
    aBottomFL(this, ResId(                FL_BOTTOM             )),
    aOK( this, ResId(                     PB_OK                 )),
    aHelp( this, ResId(                   PB_HELP               ))
#pragma warning (default : 4355)
{
    FreeResource();
}
/*-- 06.04.2004 16:05:56---------------------------------------------------

  -----------------------------------------------------------------------*/
SwWordCountDialog::~SwWordCountDialog()
{
}
/*-- 06.04.2004 16:05:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void  SwWordCountDialog::SetValues(const SwDocStat& rCurrent, const SwDocStat& rDoc)
{
    aCurrentWordFI.SetText(     String::CreateFromInt32(rCurrent.nWord ));
    aCurrentCharacterFI.SetText(String::CreateFromInt32(rCurrent.nChar ));
    aDocWordFI.SetText(         String::CreateFromInt32(rDoc.nWord ));
    aDocCharacterFI.SetText(    String::CreateFromInt32(rDoc.nChar ));
}



