/*************************************************************************
 *
 *  $RCSfile: paminit.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-20 13:05:33 $
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

#ifndef _PAMTYP_HXX
#include <pamtyp.hxx>
#endif


static SwMoveFnCollection aFwrd = {
    /* fnNd			*/  &GoNext,
    /* fnNds 		*/  &GoNextNds,
    /* fnDoc 		*/  &GoEndDoc,
    /* fnSections	*/  &GoEndSection,
    /* fnCmpOp		*/  &SwPosition::operator<,
    /* fnGetHint 	*/  &GetFrwrdTxtHint,
    /* fnSearch		*/  &utl::TextSearch::SearchFrwrd,
    /* fnSection 	*/  &SwNodes::GoStartOfSection
};

static SwMoveFnCollection aBwrd = {
    /* fnNd			*/  &GoPrevious,
    /* fnNds 		*/  &GoPreviousNds,
    /* fnDoc 		*/  &GoStartDoc,
    /* fnSections	*/  &GoStartSection,
    /* fnCmpOp		*/  &SwPosition::operator>,
    /* fnGetHint 	*/  &GetBkwrdTxtHint,
    /* fnSearch		*/  &utl::TextSearch::SearchBkwrd,
    /* fnSection 	*/  &SwNodes::GoEndOfSection
};

SwGoInDoc fnGoDoc 		= &GoInDoc;
SwGoInDoc fnGoSection	= &GoInSection;
SwGoInDoc fnGoNode		= &GoInNode;
SwGoInDoc fnGoCntnt		= &GoInCntnt;
SwGoInDoc fnGoCntntCells = &GoInCntntCells;
SwGoInDoc fnGoCntntSkipHidden      = &GoInCntntSkipHidden;
SwGoInDoc fnGoCntntCellsSkipHidden = &GoInCntntCellsSkipHidden;

SwWhichPara fnParaPrev	= &GoPrevPara;
SwWhichPara fnParaCurr	= &GoCurrPara;
SwWhichPara fnParaNext	= &GoNextPara;
SwPosPara fnParaStart	= &aFwrd;
SwPosPara fnParaEnd		= &aBwrd;

SwWhichSection fnSectionPrev	= &GoPrevSection;
SwWhichSection fnSectionCurr	= &GoCurrSection;
SwWhichSection fnSectionNext	= &GoNextSection;
SwPosSection fnSectionStart		= &aFwrd;
SwPosSection fnSectionEnd		= &aBwrd;

// Travelling in Tabellen
FASTBOOL GotoPrevTable( SwPaM&, SwPosTable, FASTBOOL bInReadOnly );
FASTBOOL GotoCurrTable( SwPaM&, SwPosTable, FASTBOOL bInReadOnly );
FASTBOOL GotoNextTable( SwPaM&, SwPosTable, FASTBOOL bInReadOnly );

SwWhichTable fnTablePrev = &GotoPrevTable;
SwWhichTable fnTableCurr = &GotoCurrTable;
SwWhichTable fnTableNext = &GotoNextTable;
SwPosTable fnTableStart = &aFwrd;
SwPosTable fnTableEnd = &aBwrd;

// Travelling in Bereichen
FASTBOOL GotoPrevRegion( SwPaM&, SwPosRegion, FASTBOOL bInReadOnly );
FASTBOOL GotoCurrRegion( SwPaM&, SwPosRegion, FASTBOOL bInReadOnly );
FASTBOOL GotoCurrRegionAndSkip( SwPaM&, SwPosRegion, FASTBOOL bInReadOnly );
FASTBOOL GotoNextRegion( SwPaM&, SwPosRegion, FASTBOOL bInReadOnly );

SwWhichRegion fnRegionPrev = &GotoPrevRegion;
SwWhichRegion fnRegionCurr = &GotoCurrRegion;
SwWhichRegion fnRegionCurrAndSkip = &GotoCurrRegionAndSkip;
SwWhichRegion fnRegionNext = &GotoNextRegion;
SwPosRegion fnRegionStart = &aFwrd;
SwPosRegion fnRegionEnd = &aBwrd;

SwMoveFn fnMoveBackward	= &aBwrd;
SwMoveFn fnMoveForward	= &aFwrd;

SwWhichPara GetfnParaCurr() //CHINA001 
{
    return fnParaCurr;
}
SwPosPara GetfnParaStart() //CHINA001 
{
     return  fnParaStart;
}
SwWhichTable GetfnTablePrev() //CHINA001 
{
    return fnTablePrev;
}
SwPosPara GetfnParaEnd() //CHINA001 
{
     return  fnParaEnd;
}
SwPosTable GetfnTableStart() //CHINA001 
{
    return fnTableStart;
}
SwWhichTable GetfnTableCurr() //CHINA001 
{
    return fnTableCurr;
}
SwPosTable GetfnTableEnd() //CHINA001 
{
    return fnTableEnd;
}
