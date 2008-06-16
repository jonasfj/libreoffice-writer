/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: postithelper.cxx,v $
 * $Revision: 1.2 $
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

#include <tools/gen.hxx>
#include <postithelper.hxx>
#include <PostItMgr.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <ndtxt.hxx>
#include <cntfrm.hxx>
#include <pagefrm.hxx>
#include <rootfrm.hxx>
#include <txtfrm.hxx>
#include <IDocumentRedlineAccess.hxx>
#include <redline.hxx>


SwPostItHelper::SwLayoutStatus SwPostItHelper::getLayoutInfos( std::vector< SwLayoutInfo >& rInfo, SwPosition& rPos )
{
    SwLayoutStatus aRet = INVISIBLE;
    SwCntntNode* pNode = rPos.nNode.GetNode().GetCntntNode();
    if( !pNode )
        return aRet;
    SwClientIter aIter( *pNode );
    SwTxtFrm *pTxtFrm;
    for( pTxtFrm = (SwTxtFrm*)aIter.First( TYPE( SwTxtFrm )); pTxtFrm; pTxtFrm = (SwTxtFrm*)aIter.Next() )
    {
        if( !pTxtFrm->IsFollow() )
        {
            pTxtFrm = ((SwTxtFrm*)pTxtFrm)->GetFrmAtPos( rPos );
            SwPageFrm *pPage = pTxtFrm ? pTxtFrm->FindPageFrm() : 0;
            if( pPage )
            {
                SwLayoutInfo aInfo;
                pTxtFrm->GetCharRect( aInfo.mPosition, rPos, 0 );
                aInfo.mPageFrame = pPage->Frm();
                aInfo.mPagePrtArea = pPage->Prt();
                aInfo.mPagePrtArea.Pos() += aInfo.mPageFrame.Pos();
                aInfo.mnPageNumber = pPage->GetPhyPageNum();
                aInfo.mbMarginSide = pPage->MarginSide();

                if( aRet == INVISIBLE )
                {
                    aRet = VISIBLE;
                    const IDocumentRedlineAccess* pIDRA = pNode->getIDocumentRedlineAccess();
                    if( IDocumentRedlineAccess::IsShowChanges( pIDRA->GetRedlineMode() ) )
                    {
                        const SwRedline* pRedline = pIDRA->GetRedline( rPos, 0 );
                        if( pRedline )
                        {
                            if( nsRedlineType_t::REDLINE_INSERT == pRedline->GetType() )
                                aRet = INSERTED;
                            else if( nsRedlineType_t::REDLINE_DELETE == pRedline->GetType() )
                                aRet = DELETED;
                            aInfo.mRedlineAuthor = pRedline->GetAuthor();
                        }
                    }
                }

                 rInfo.push_back( aInfo );
            }
        }
    }
    return aRet;
}

long SwPostItHelper::getLayoutHeight( const SwRootFrm* pRoot )
{
    long nRet = pRoot ? pRoot->Frm().Height() : 0;
    return nRet;
}

void SwPostItHelper::setSidebarChanged( SwRootFrm* pRoot, bool bBrowseMode )
{
    if( pRoot )
    {
        pRoot->SetSidebarChanged();
        if( bBrowseMode )
            pRoot->InvalidateBrowseWidth();
    }
}

unsigned long SwPostItHelper::getPageInfo( SwRect& rPageFrm, const SwRootFrm* pRoot, const Point& rPoint )
{
    unsigned long nRet = 0;
    const SwFrm* pPage = pRoot->GetPageAtPos( rPoint, 0, true );
    if( pPage )
    {
        nRet = pPage->GetPhyPageNum();
        rPageFrm = pPage->Frm();
    }
    return nRet;
}

SwPostItHelper::SwLayoutStatus SwPostItHelper::getLayoutInfos( std::vector< SwLayoutInfo >& rInfo, SwTxtFld* pFld )
{
    SwLayoutStatus aRet = SwPostItHelper::INVISIBLE;
    if( pFld )
    {
        SwTxtNode* pTNd = pFld->GetpTxtNode();
        if( pTNd )
        {
            SwPosition aPos( *pTNd );
            aPos.nContent.Assign( pTNd, *pFld->GetStart() );
            aRet = getLayoutInfos( rInfo, aPos );
        }
    }
    return aRet;
}


