/*************************************************************************
 *
 *  $RCSfile: SwUndoPageDesc.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2005-05-03 14:39:42 $
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

#include <tools/resid.hxx>
#include <doc.hxx>
#include <swundo.hxx>
#include <pagedesc.hxx>
#include <SwUndoPageDesc.hxx>
#include <SwRewriter.hxx>
#include <undobj.hxx>
#include <comcore.hrc>
#include <headerfooterhelper.hxx>


SwUndoPageDesc::SwUndoPageDesc(const SwPageDesc & _aOld, 
                               const SwPageDesc & _aNew,
                               SwDoc * _pDoc)
    : SwUndo(_aOld.GetName() != _aNew.GetName() ? 
             UNDO_RENAME_PAGEDESC 
             :UNDO_CHANGE_PAGEDESC), 
      aOld(_aOld, _pDoc), aNew(_aNew, _pDoc), pDoc(_pDoc)
{   
    ASSERT(0 != pDoc, "no document?");

    // The headers/footers from the two SwPageDesc saved by this undo
    // are still in the main document array. In order to preserve the
    // absolute indices, we need to move them into the undo nodes
    // array.
    // #i46909# no undo if header or footer changed
    //    SwNodes& rNodes = * const_cast<SwNodes*>( pDoc->GetUndoNds() );
    //    saveHeaderFooterNodes( (SwPageDesc&)aOld, rNodes );
    //    saveHeaderFooterNodes( (SwPageDesc&)aNew, rNodes );
}

SwUndoPageDesc::~SwUndoPageDesc()
{
}

void SwUndoPageDesc::Undo(SwUndoIter & rIt)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    pDoc->ChgPageDesc(aOld.GetName(), aOld);
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDesc::Redo(SwUndoIter & rIt)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    pDoc->ChgPageDesc(aNew.GetName(), aNew);
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDesc::Repeat(SwUndoIter & rIt)
{
    Redo(rIt);
}

SwRewriter SwUndoPageDesc::GetRewriter() const
{
    SwRewriter aResult;

    aResult.AddRule(UNDO_ARG1, aOld.GetName());
    aResult.AddRule(UNDO_ARG2, SW_RES(STR_YIELDS));
    aResult.AddRule(UNDO_ARG3, aNew.GetName());

    return aResult;
}

// #116530#
SwUndoPageDescCreate::SwUndoPageDescCreate(const SwPageDesc * pNew, 
                                           SwDoc * _pDoc)
    : SwUndo(UNDO_CREATE_PAGEDESC), pDesc(pNew), aNew(*pNew, _pDoc), 
      pDoc(_pDoc)
{
    ASSERT(0 != pDoc, "no document?");
}

SwUndoPageDescCreate::~SwUndoPageDescCreate()
{
}

void SwUndoPageDescCreate::Undo(SwUndoIter & rIt)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);

    // -> #116530#
    if (pDesc)
    {
        aNew = *pDesc;
        pDesc = NULL;
    }
    // <- #116530#

    pDoc->DelPageDesc(aNew.GetName(), TRUE);
    pDoc->DoUndo(bUndo);
}


void SwUndoPageDescCreate::Redo(SwUndoIter & rIt)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    
    SwPageDesc aPageDesc = aNew;
    pDoc->MakePageDesc(aNew.GetName(), &aPageDesc, FALSE, TRUE); // #116530#

    pDoc->DoUndo(bUndo);
}

void SwUndoPageDescCreate::Repeat(SwUndoIter & rIt)
{
    Redo(rIt);
}

SwRewriter SwUndoPageDescCreate::GetRewriter() const
{
    SwRewriter aResult;

    if (pDesc)
        aResult.AddRule(UNDO_ARG1, pDesc->GetName());
    else
        aResult.AddRule(UNDO_ARG1, aNew.GetName());
        

    return aResult;
}

SwUndoPageDescDelete::SwUndoPageDescDelete(const SwPageDesc & _aOld,
                                           SwDoc * _pDoc)
    : SwUndo(UNDO_DELETE_PAGEDESC), aOld(_aOld, _pDoc), pDoc(_pDoc)
{
    ASSERT(0 != pDoc, "no document?");
}
                                           
SwUndoPageDescDelete::~SwUndoPageDescDelete()
{
}

void SwUndoPageDescDelete::Undo(SwUndoIter & rIt)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);

    SwPageDesc aPageDesc = aOld;
    pDoc->MakePageDesc(aOld.GetName(), &aPageDesc, FALSE, TRUE); // #116530#
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDescDelete::Redo(SwUndoIter & rIt)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    pDoc->DelPageDesc(aOld.GetName(), TRUE); // #116530#
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDescDelete::Repeat(SwUndoIter & rIt)
{
    Redo(rIt);
}

SwRewriter SwUndoPageDescDelete::GetRewriter() const
{
    SwRewriter aResult;

    aResult.AddRule(UNDO_ARG1, aOld.GetName());

    return aResult;
}
