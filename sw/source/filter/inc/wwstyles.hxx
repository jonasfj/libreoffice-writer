/*************************************************************************
 *
 *  $RCSfile: wwstyles.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2003-12-09 11:41:27 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/// @HTML
#ifndef WW_WWSTYLES_HXX
#define WW_WWSTYLES_HXX

class String;       //yuck...

namespace ww
{
    enum sti
    {
        stiNormal = 0,                  // 0x0000
        stiLev1 = 1,                    // 0x0001
        stiLev2 = 2,                    // 0x0002
        stiLev3 = 3,                    // 0x0003
        stiLev4 = 4,                    // 0x0004
        stiLev5 = 5,                    // 0x0005
        stiLev6 = 6,                    // 0x0006
        stiLev7 = 7,                    // 0x0007
        stiLev8 = 8,                    // 0x0008
        stiLev9 = 9,                    // 0x0009
        stiLevFirst = stiLev1,
        stiLevLast = stiLev9,
        stiIndex1 = 10,                 // 0x000A
        stiIndex2 = 11,                 // 0x000B
        stiIndex3 = 12,                 // 0x000C
        stiIndex4 = 13,                 // 0x000D
        stiIndex5 = 14,                 // 0x000E
        stiIndex6 = 15,                 // 0x000F
        stiIndex7 = 16,                 // 0x0010
        stiIndex8 = 17,                 // 0x0011
        stiIndex9 = 18,                 // 0x0012
        stiIndexFirst = stiIndex1,
        stiIndexLast = stiIndex9,
        stiToc1 = 19,                   // 0x0013
        stiToc2 = 20,                   // 0x0014
        stiToc3 = 21,                   // 0x0015
        stiToc4 = 22,                   // 0x0016
        stiToc5 = 23,                   // 0x0017
        stiToc6 = 24,                   // 0x0018
        stiToc7 = 25,                   // 0x0019
        stiToc8 = 26,                   // 0x001A
        stiToc9 = 27,                   // 0x001B
        stiTocFirst = stiToc1,
        stiTocLast = stiToc9,
        stiNormIndent = 28,             // 0x001C
        stiFtnText = 29,                // 0x001D
        stiAtnText = 30,                // 0x001E
        stiHeader = 31,                 // 0x001F
        stiFooter = 32,                 // 0x0020
        stiIndexHeading = 33,           // 0x0021
        stiCaption = 34,                // 0x0022
        stiToCaption = 35,              // 0x0023
        stiEnvAddr = 36,                // 0x0024
        stiEnvRet = 37,                 // 0x0025
        stiFtnRef = 38,                 // 0x0026  char style
        stiAtnRef = 39,                 // 0x0027  char style
        stiLnn = 40,                    // 0x0028  char style
        stiPgn = 41,                    // 0x0029  char style
        stiEdnRef = 42,                 // 0x002A  char style
        stiEdnText = 43,                // 0x002B
        stiToa = 44,                    // 0x002C
        stiMacro = 45,                  // 0x002D
        stiToaHeading = 46,             // 0x002E
        stiList = 47,                   // 0x002F
        stiListBullet = 48,             // 0x0030
        stiListNumber = 49,             // 0x0031
        stiList2 = 50,                  // 0x0032
        stiList3 = 51,                  // 0x0033
        stiList4 = 52,                  // 0x0034
        stiList5 = 53,                  // 0x0035
        stiListBullet2 = 54,            // 0x0036
        stiListBullet3 = 55,            // 0x0037
        stiListBullet4 = 56,            // 0x0038
        stiListBullet5 = 57,            // 0x0039
        stiListNumber2 = 58,            // 0x003A
        stiListNumber3 = 59,            // 0x003B
        stiListNumber4 = 60,            // 0x003C
        stiListNumber5 = 61,            // 0x003D
        stiTitle = 62,                  // 0x003E
        stiClosing = 63,                // 0x003F
        stiSignature = 64,              // 0x0040
        stiNormalChar = 65,             // 0x0041  char style
        stiBodyText = 66,               // 0x0042
        /*
         stiBodyTextInd1 was orig stiBodyText2 in documentation, but that
         collides with the other stiBodyText2 and this seems more reasonable.
         cmc@openoffice.org
        */
        stiBodyTextInd1 = 67,           // 0x0043
        stiListCont = 68,               // 0x0044
        stiListCont2 = 69,              // 0x0045
        stiListCont3 = 70,              // 0x0046
        stiListCont4 = 71,              // 0x0047
        stiListCont5 = 72,              // 0x0048
        stiMsgHeader = 73,              // 0x0049
        stiSubtitle = 74,               // 0x004A
        stiSalutation = 75,             // 0x004B
        stiDate = 76,                   // 0X004C
        stiBodyText1I = 77,             // 0x004D
        stiBodyText1I2 = 78,            // 0x004E
        stiNoteHeading = 79,            // 0x004F
        stiBodyText2 = 80,              // 0x0050
        stiBodyText3 = 81,              // 0x0051
        stiBodyTextInd2 = 82,           // 0x0052
        stiBodyTextInd3 = 83,           // 0x0053
        stiBlockQuote = 84,             // 0x0054
        stiHyperlink = 85,              // 0x0055  char style
        stiHyperlinkFollowed = 86,      // 0x0056  char style
        stiStrong = 87,                 // 0x0057  char style
        stiEmphasis = 88,               // 0x0058  char style
        stiNavPane = 89,                // 0x0059  char style
        stiPlainText = 90,              // 0x005A
        stiMax = 91,                    // number of defined sti's
        stiUser = 0x0ffe,               // user styles are distinguished by name
        stiNil = 0x0fff                 // max for 12 bits
    };
   
    /** Find the WinWord sti index of an english style name.
    
        When importing a RTF document we would like to treat styles as similiar
        to how word does as possible, to this end word will treat some styles
        with special names as inbuilt styles, and some as user defined styles.

        See OpenOffice.org issue 21881
        (http://www.openoffice.org/issues/show_bug.cgi?id=21881)
        and OpenOffice.org issue 17503
        (http://www.openoffice.org/issues/show_bug.cgi?id=17503)

        @param
        rSting the Style name to test to see what winword sti word would give
        such a name if it has no other information to work with (e.g. importing
        a RTF document)

        @return the sti that word would give it. stiUser if word would treat
        it as a user defined style.

        @author
        <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        <a href="mailto:tono@openoffice.org">Takashi Ono</a>
    */
    sti GetCanonicalStiFromEnglishName(const String &rString) throw();
};

#endif
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
