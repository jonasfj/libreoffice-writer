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
#ifndef _ACORRECT_HXX
#define _ACORRECT_HXX

#include <editeng/svxacorr.hxx>

#include <swundo.hxx>


class SwEditShell;
class SwPaM;
class SwNodeIndex;
struct SwPosition;
class SfxItemSet;

class SwDontExpandItem
{
    SfxItemSet* pDontExpItems;
public:
    SwDontExpandItem() :
        pDontExpItems(0){}
    ~SwDontExpandItem();

    void SaveDontExpandItems( const SwPosition& rPos );
    void RestoreDontExpandItems( const SwPosition& rPos );

};

class SwAutoCorrDoc : public SvxAutoCorrDoc
{
    SwEditShell& rEditSh;
    SwPaM& rCrsr;
    SwNodeIndex* pIdx;
    SwUndoId nUndoId;
    bool    bUndoIdInitialized;

    void DeleteSel( SwPaM& rDelPam );

public:
    SwAutoCorrDoc( SwEditShell& rEditShell, SwPaM& rPam, sal_Unicode cIns = 0 );
    ~SwAutoCorrDoc();

    virtual BOOL Delete( xub_StrLen nStt, xub_StrLen nEnd );
    virtual BOOL Insert( xub_StrLen nPos, const String& rTxt );
    virtual BOOL Replace( xub_StrLen nPos, const String& rTxt );

    virtual BOOL SetAttr( xub_StrLen nStt, xub_StrLen nEnd, USHORT nSlotId,
                            SfxPoolItem& );

    virtual BOOL SetINetAttr( xub_StrLen nStt, xub_StrLen nEnd, const String& rURL );

    // returne den Text eines vorherigen Absatzes.
    // Dieser darf nicht leer sein!
    // Gibt es diesen nicht oder gibt es davor nur Leere, dann returne 0
    // Das Flag gibt an:
    //		TRUE: den, vor der normalen Einfuegeposition (TRUE)
    // 		FALSE: den, in den das korrigierte Wort eingfuegt wurde.
    //				(Muss nicht der gleiche Absatz sein!!!!)
    virtual const String* GetPrevPara( BOOL bAtNormalPos );

    virtual BOOL ChgAutoCorrWord( xub_StrLen& rSttPos, xub_StrLen nEndPos,
                                  SvxAutoCorrect& rACorrect,
                                  const String** ppPara );

    // wird nach dem austauschen der Zeichen von den Funktionen
    //	- FnCptlSttWrd
    // 	- FnCptlSttSntnc
    // gerufen. Dann koennen die Worte ggfs. in die Ausnahmelisten
    // aufgenommen werden.
    virtual void SaveCpltSttWord( ULONG nFlag, xub_StrLen nPos,
                                    const String& rExceptWord, sal_Unicode cChar );
    virtual LanguageType GetLanguage( xub_StrLen nPos, BOOL bPrevPara ) const;
};

class SwAutoCorrExceptWord
{
    String sWord;
    ULONG nFlags, nNode;
    xub_StrLen nCntnt;
    sal_Unicode cChar;
    LanguageType eLanguage;
    BOOL bDeleted;
public:
    SwAutoCorrExceptWord( ULONG nAFlags, ULONG nNd, xub_StrLen nContent,
                                        const String& rWord, sal_Unicode cChr,
                                        LanguageType eLang )
        : sWord(rWord), nFlags(nAFlags), nNode(nNd), nCntnt(nContent),
        cChar(cChr), eLanguage(eLang), bDeleted(FALSE)
    {}

    BOOL IsDeleted() const							{ return bDeleted; }
    void CheckChar( const SwPosition& rPos, sal_Unicode cChar );
    BOOL CheckDelChar( const SwPosition& rPos );
};


#endif
