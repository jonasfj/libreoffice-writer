
// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
#include <hintids.hxx>
#include <tools/stream.hxx>
#include <svtools/rtftoken.h>
#include <svtools/rtfkeywd.hxx>
#include <svl/intitem.hxx>
#include <svtools/rtfout.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/cmapitem.hxx>
#include <editeng/crsditem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/wrlmitem.hxx>
#include <editeng/numitem.hxx>
#include <shellio.hxx>
#include <fltini.hxx>
#include <swtypes.hxx>
#include <swparrtf.hxx>
#include <wrtrtf.hxx>
#include <ndtxt.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <pam.hxx>
#include <charfmt.hxx>
#include <charatr.hxx>
#include <paratr.hxx>
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#include <numrule.hxx>

#define RTF_NUMRULE_NAME "RTF_Num"

SV_IMPL_VARARR( SwListArr, SwListEntry )


void lcl_ExpandNumFmts( SwNumRule& rRule )
{
    // dann noch das NumFormat in alle Ebenen setzen
    for( BYTE n = 1; n < MAXLEVEL; ++n )
        if( !rRule.GetNumFmt( n ) )
        {
            SwNumFmt aNumFmt( rRule.Get( 0 ));
            aNumFmt.SetIndentAt( aNumFmt.GetIndentAt() * ( n + 1 ) );
            rRule.Set( n, aNumFmt );
        }
}

SfxItemSet& GetNumChrFmt( SwDoc& rDoc, SwNumRule& rRule, BYTE nNumLvl )
{
    SwCharFmt* pChrFmt = rRule.Get( nNumLvl ).GetCharFmt();
    if( !pChrFmt )
    {
        String sNm( rRule.GetName() );
        ( sNm += ' ' ) += String::CreateFromInt32( nNumLvl + 1 );
        pChrFmt = rDoc.MakeCharFmt( sNm, rDoc.GetDfltCharFmt() );
        if( !rRule.GetNumFmt( nNumLvl ))
            rRule.Set( nNumLvl, rRule.Get( nNumLvl ) );
        ((SwNumFmt*)rRule.GetNumFmt( nNumLvl ))->SetCharFmt( pChrFmt );
    }
    return (SfxItemSet&)pChrFmt->GetAttrSet();
}

void SwRTFParser::ReadListLevel( SwNumRule& rRule, BYTE nNumLvl )
{
    int nToken;
    int nNumOpenBrakets = 1;		// die erste wurde schon vorher erkannt !!
    int nLvlTxtLevel = 0, nLvlNumberLevel = 0;
    String sLvlText, sLvlNumber;
    SwNumFmt* pCurNumFmt;
    SvxNumberFormat::SvxNumLabelFollowedBy eFollowedBy = SvxNumberFormat::NOTHING;

    if( MAXLEVEL >= nNumLvl )
    {
        pCurNumFmt = (SwNumFmt*)rRule.GetNumFmt( nNumLvl );
        pCurNumFmt->SetIndentAt( 0 );
        pCurNumFmt->SetFirstLineIndent( 0 );
    }
    else
        pCurNumFmt = 0;

    while( nNumOpenBrakets && IsParserWorking() )
    {
        switch( ( nToken = GetNextToken() ))
        {
        case '}':
            if( nNumOpenBrakets )
            {
                if( nLvlTxtLevel == nNumOpenBrakets )
                {
                    if( DelCharAtEnd( sLvlText, ';' ).Len() &&
                        sLvlText.Len() && sLvlText.Len() ==
                        (USHORT)(sLvlText.GetChar( 0 )) + 1 )
                        sLvlText.Erase( 0, 1 );
                    nLvlTxtLevel = 0;
                }
                if( nLvlNumberLevel == nNumOpenBrakets )
                {
                    DelCharAtEnd( sLvlNumber, ';' );
                    nLvlNumberLevel = 0;
                }
            }
            --nNumOpenBrakets;
            break;

        case '{':
            {
                if( RTF_IGNOREFLAG != GetNextToken() )
                    nToken = SkipToken( -1 );
                // Unknown und alle bekannten nicht ausgewerteten Gruppen
                // sofort ueberspringen
                else if( RTF_UNKNOWNCONTROL != ( nToken = GetNextToken() ))
//						RTF_PANOSE != nToken && RTF_FALT != nToken &&
//						RTF_FALT != nToken && RTF_FNAME != nToken &&
//						RTF_FONTEMB != nToken && RTF_FONTFILE != nToken )
                    nToken = SkipToken( -2 );
                else
                {
                    // gleich herausfiltern
                    ReadUnknownData();
                    nToken = GetNextToken();
                    if( '}' != nToken )
                        eState = SVPAR_ERROR;
                    break;
                }
                ++nNumOpenBrakets;
            }
            break;

        case RTF_LEVELNFC:
            {
                sal_Int16 eType = SVX_NUM_ARABIC;
                switch( nTokenValue )
                {
                case 1:		eType = SVX_NUM_ROMAN_UPPER;			break;
                case 2:		eType = SVX_NUM_ROMAN_LOWER;			break;
                case 3:		eType = SVX_NUM_CHARS_UPPER_LETTER_N;	break;
                case 4:		eType = SVX_NUM_CHARS_LOWER_LETTER_N;	break;
                case 255:
                case 23:	eType = SVX_NUM_CHAR_SPECIAL;			break;
                }
                if( pCurNumFmt )
                    pCurNumFmt->SetNumberingType(eType);
            }
            break;

        case RTF_LEVELJC:
            {
                SvxAdjust eAdj = SVX_ADJUST_LEFT;
                switch( nTokenValue )
                {
                case 1:		eAdj = SVX_ADJUST_CENTER;	break;
                case 2:		eAdj = SVX_ADJUST_RIGHT;	break;
                }
                if( pCurNumFmt )
                    pCurNumFmt->SetNumAdjust( eAdj );
            }
            break;

        case RTF_LEVELSTARTAT:
            if( pCurNumFmt && -1 != nTokenValue )
                pCurNumFmt->SetStart( USHORT( nTokenValue ));
            break;

        case RTF_LEVELTEXT:
            nLvlTxtLevel = nNumOpenBrakets;
            break;

        case RTF_LEVELNUMBERS:
            nLvlNumberLevel = nNumOpenBrakets;
            break;


        case RTF_TEXTTOKEN:
            if( nLvlTxtLevel == nNumOpenBrakets )
                sLvlText += aToken;
            else if( nLvlNumberLevel == nNumOpenBrakets )
                sLvlNumber += aToken;
            break;

        case RTF_LEVELFOLLOW:
          /* removed; waiting for swnum02 to be integrated! */
            switch (nTokenValue)
            {
            case 0:
                eFollowedBy = SvxNumberFormat::LISTTAB;
                break;
            case 1:
                eFollowedBy = SvxNumberFormat::SPACE;
                break;
            }
            break;

        case RTF_LEVELOLD:
        case RTF_LEVELPREV:
        case RTF_LEVELPREVSPACE:
        case RTF_LEVELINDENT:
        case RTF_LEVELSPACE:
        case RTF_LEVELLEGAL:
        case RTF_LEVELNORESTART:
            break;

        default:
            if( pCurNumFmt && (
                RTF_CHRFMT == (nToken & ~(0xff | RTF_SWGDEFS) ) ||
                RTF_PARFMT == (nToken & ~(0xff | RTF_SWGDEFS) ) ))
            {
                SfxItemSet aSet( pDoc->GetAttrPool(), aTxtNodeSetRange );
                // put the current CharFmtAttr into the set
                SfxItemSet& rCFmtSet = GetNumChrFmt( *pDoc, rRule, nNumLvl );
                aSet.Put( rCFmtSet );
                // and put the current "LRSpace" into the set
                {
                    SvxLRSpaceItem aLR( RES_LR_SPACE );
                    aLR.SetTxtLeft( pCurNumFmt->GetIndentAt() );
                    aLR.SetTxtFirstLineOfst(pCurNumFmt->GetFirstLineIndent());
                    aSet.Put( aLR );
                }

                ReadAttr( nToken, &aSet );

                //#i24880# Word appears to ignore char background for numbering
                aSet.ClearItem(RES_CHRATR_BACKGROUND);

                // put all CharFmt Items into the charfmt
                rCFmtSet.Put( aSet );

                // test for LRSpace Item. If exist then set all relevant
                // values on the numrule format
                const SfxPoolItem* pItem;
                if( SFX_ITEM_SET == aSet.GetItemState( RES_LR_SPACE,
                        FALSE, &pItem ))
                {
                    const SvxLRSpaceItem& rLR = *(SvxLRSpaceItem*)pItem;
                    pCurNumFmt->SetListtabPos( rLR.GetTxtLeft( ) );
                    pCurNumFmt->SetIndentAt( rLR.GetTxtLeft() );
                    pCurNumFmt->SetFirstLineIndent( rLR.GetTxtFirstLineOfst());
                }

                // dann aus der Vorlage den Font holen
                if( SVX_NUM_CHAR_SPECIAL == pCurNumFmt->GetNumberingType() )
                    pCurNumFmt->SetBulletFont( FindFontOfItem(
                                pCurNumFmt->GetCharFmt()->GetFont() ) );
            }
            break;
        }
    }

    if( IsParserWorking() && pCurNumFmt )
    {
        // dann erzeuge mal die Pre/Postfix-Strings
        if( sLvlText.Len() &&
            SVX_NUM_CHAR_SPECIAL == pCurNumFmt->GetNumberingType() )
        {
            pCurNumFmt->SetBulletChar( sLvlText.GetChar( 0 ) );
            // dann aus der Vorlage den Font holen
            if( pCurNumFmt->GetCharFmt() )
                pCurNumFmt->SetBulletFont( FindFontOfItem(
                        pCurNumFmt->GetCharFmt()->GetFont() ) );
        }
        else if( sLvlNumber.Len() && sLvlText.Len() )
        {
            // in sLvlText steht der Text, in sLvlNumber die Position
            // der Ebenen in sLvlText
            pCurNumFmt->SetPrefix(
                sLvlText.Copy( 0, USHORT( sLvlNumber.GetChar( 0 ))-1 ));
            pCurNumFmt->SetSuffix( sLvlText.Copy(
                    USHORT( sLvlNumber.GetChar( sLvlNumber.Len()-1 )) ));
            // wieviele Levels stehen im String?
            pCurNumFmt->SetIncludeUpperLevels( (BYTE)sLvlNumber.Len() );
        }
        else
        {
            pCurNumFmt->SetNumberingType(SVX_NUM_NUMBER_NONE);
            pCurNumFmt->SetSuffix( sLvlText );
        }

        /* removed; waiting for swnum02 to be integrated!*/
        pCurNumFmt->SetLabelFollowedBy( eFollowedBy );
        if (eFollowedBy == SvxNumberFormat::LISTTAB && !pCurNumFmt->IsItemize())
        {
            pCurNumFmt->SetIndentAt(0);
            pCurNumFmt->SetFirstLineIndent(0);
        }
    }

    SkipToken( -1 );
}

void SwRTFParser::ReadListTable()
{
    int nToken;
    int nNumOpenBrakets = 1;		// die erste wurde schon vorher erkannt !!
    bNewNumList = TRUE;

    BYTE nNumLvl = 0;
    SwNumRule* pCurRule = 0;
    SwListEntry aEntry;

    while( nNumOpenBrakets && IsParserWorking() )
    {
        switch( ( nToken = GetNextToken() ))
        {
        case '}':		if( --nNumOpenBrakets && IsParserWorking() )
                        {
                            // Style konnte vollstaendig gelesen werden,
                            // also ist das noch ein stabiler Status
                            SaveState( RTF_LISTTABLE );
                            if( 1 == nNumOpenBrakets )
                            {
                                if( aEntry.nListId )
                                    aListArr.Insert( aEntry, aListArr.Count() );
                                aEntry.Clear();
                            }
                        }
                        break;

        case '{':
            {
                if( RTF_IGNOREFLAG != GetNextToken() )
                    nToken = SkipToken( -1 );
                // Unknown und alle bekannten nicht ausgewerteten Gruppen
                // sofort ueberspringen
                else if( RTF_UNKNOWNCONTROL != ( nToken = GetNextToken() ))
//						RTF_PANOSE != nToken && RTF_FALT != nToken &&
//						RTF_FALT != nToken && RTF_FNAME != nToken &&
//						RTF_FONTEMB != nToken && RTF_FONTFILE != nToken )
                    nToken = SkipToken( -2 );
                else
                {
                    // gleich herausfiltern
                    ReadUnknownData();
                    nToken = GetNextToken();
                    if( '}' != nToken )
                        eState = SVPAR_ERROR;
                    break;
                }
                ++nNumOpenBrakets;
            }
            break;

        case RTF_LIST:
            {
                if( pCurRule && pCurRule->IsContinusNum() )
                    lcl_ExpandNumFmts( *pCurRule );

                String sTmp( String::CreateFromAscii(
                    RTL_CONSTASCII_STRINGPARAM( RTF_NUMRULE_NAME " 1" )));
                aEntry.nListDocPos = pDoc->MakeNumRule( sTmp, 0, FALSE, SvxNumberFormat::LABEL_ALIGNMENT );
                pCurRule = pDoc->GetNumRuleTbl()[ aEntry.nListDocPos ];
                // --> OD 2008-07-08 #i91400#
                pCurRule->SetName( pDoc->GetUniqueNumRuleName( &sTmp, FALSE ),
                                   *pDoc );
                // <--
                pCurRule->SetAutoRule( FALSE );
                nNumLvl = (BYTE)-1;
            }
            break;

        case RTF_LISTID:			aEntry.nListId = nTokenValue;		break;
        case RTF_LISTTEMPLATEID:	aEntry.nListTemplateId = nTokenValue; break;

        case RTF_LISTRESTARTHDN:
            break;
        case RTF_LISTNAME:
            if (nNextCh=='}') break;  // #118989# empty listnames
            if( RTF_TEXTTOKEN == GetNextToken() )
            {
                String sTmp( DelCharAtEnd( aToken, ';' ));
                if( sTmp.Len() && !pDoc->FindNumRulePtr( sTmp ))
                {
                    // --> OD 2008-07-08 #i91400#
                    pCurRule->SetName( sTmp, *pDoc );
                    // <--
                }
            }
            SkipGroup();
            break;

        case RTF_LISTSIMPLE:
            pCurRule->SetContinusNum( TRUE );
            break;

        case RTF_LISTLEVEL:
            {
                if( ++nNumLvl < MAXLEVEL )
                    pCurRule->Set( nNumLvl, pCurRule->Get( nNumLvl ));
                ReadListLevel( *pCurRule, nNumLvl );
            }
            break;
        }
    }

    if( pCurRule && pCurRule->IsContinusNum() )
        lcl_ExpandNumFmts( *pCurRule );

    SkipToken( -1 );		// die schliesende Klammer wird "oben" ausgewertet
}

BOOL lcl_IsEqual( SwNumRule* pOrigRule, SwNumRule* pRule )
{
    BOOL bRet = 0;
    if( pOrigRule && pRule )
    {
        bRet =  pOrigRule->GetRuleType() == pRule->GetRuleType() &&
                pOrigRule->IsContinusNum() == pRule->IsContinusNum() &&
                pOrigRule->IsAbsSpaces() == pRule->IsAbsSpaces();
        if( bRet )
            for( BYTE n = 0; bRet && n < MAXLEVEL; ++n )
            {
                const SwNumFmt* pOFmt = pOrigRule->GetNumFmt( n ),
                              *	pFmt = pRule->GetNumFmt( n );
                if( pFmt && pOFmt )
                {
                    SwCharFmt* pOCFmt = pOFmt->GetCharFmt(),
                             * pCFmt = pFmt->GetCharFmt();
                    if( pOCFmt && pCFmt )
                    {
                        bRet = 0 != (pCFmt->GetAttrSet() == pOCFmt->GetAttrSet());
                    }
                    else
                        bRet = !pCFmt && !pOCFmt;
                    if( bRet )
                    {
                        ((SwNumFmt*)pOFmt)->SetCharFmt( 0 );
                        ((SwNumFmt*)pFmt)->SetCharFmt( 0 );
                        bRet = *pOFmt == *pFmt;
                        ((SwNumFmt*)pOFmt)->SetCharFmt( pOCFmt );
                        ((SwNumFmt*)pFmt)->SetCharFmt( pCFmt );
                    }
                }
                else
                    bRet = !pFmt && !pOFmt;
            }
    }
    return bRet;
}

void SwRTFParser::ReadListOverrideTable()
{
    int nToken;
    int nNumOpenBrakets = 1;		// die erste wurde schon vorher erkannt !!
    SwListEntry aEntry;
    SwNumRule* pRule = 0, *pOrigRule = 0;
    BYTE nNumLvl = 0;
    BOOL bOverrideFormat = FALSE, bOverrideStart = FALSE;

    while( nNumOpenBrakets && IsParserWorking() )
    {
        switch( ( nToken = GetNextToken() ))
        {
        case '}':
            if( --nNumOpenBrakets && IsParserWorking() )
            {
                // Style konnte vollstaendig gelesen werden,
                // also ist das noch ein stabiler Status
                SaveState( RTF_LISTOVERRIDETABLE );

                if( 1 == nNumOpenBrakets )
                {
                    bOverrideFormat = FALSE, bOverrideStart = FALSE;
                    if( pRule )
                    {
                        if( lcl_IsEqual( pOrigRule, pRule ))
                        {
                            // no changes on the rule -> use the original rule
                            aEntry.nListDocPos = pDoc->FindNumRule(
                                                    pOrigRule->GetName() );
                            // delete the temp Rule
                            RemoveUnusedNumRule( pRule );
                        }
                        else if( pRule->IsContinusNum() )
                            lcl_ExpandNumFmts( *pRule );
                    }

                    if( aEntry.nListId && aEntry.nListNo )
                    {
                        int nMatch=-1;
                        for( USHORT n = aListArr.Count(); n; )
                        {
                            if( aListArr[ --n ].nListId == aEntry.nListId)
                            {
                                nMatch=n;
                                break;
                            }
                        }
                        if(nMatch>=0)
                        {
                            USHORT nMatch2 = static_cast< USHORT >(nMatch);
                            if (!aListArr[nMatch2].nListNo )
                            {
                                aListArr[nMatch2].nListNo = aEntry.nListNo;
                            }
                            else
                            {
                                aEntry.nListDocPos=aListArr[nMatch2].nListDocPos;
                                aEntry.nListTemplateId=aListArr[nMatch2].nListTemplateId;
                                aListArr.Insert(aEntry, aListArr.Count());
                            }
                            if(pOrigRule)
                                aListArr[nMatch2].nListDocPos = aEntry.nListDocPos;
                        }
                    }
                    aEntry.Clear();
                    pOrigRule = 0;
                    pRule = 0;
                }
            }
            break;

        case '{':
            {
                if( RTF_IGNOREFLAG != GetNextToken() )
                    nToken = SkipToken( -1 );
                // Unknown und alle bekannten nicht ausgewerteten Gruppen
                // sofort ueberspringen
                else if( RTF_UNKNOWNCONTROL != ( nToken = GetNextToken() ))
                    nToken = SkipToken( -2 );
                else
                {
                    // gleich herausfiltern
                    ReadUnknownData();
                    nToken = GetNextToken();
                    if( '}' != nToken )
                        eState = SVPAR_ERROR;
                    break;
                }
                ++nNumOpenBrakets;
            }
            break;

        case RTF_LISTOVERRIDE:		aEntry.Clear();					break;
        case RTF_LISTID:			aEntry.nListId = nTokenValue;	break;
        case RTF_LS:				aEntry.nListNo = nTokenValue;	break;
        case RTF_LISTOVERRIDECOUNT:
            if( nTokenValue )
            {
                pRule = 0;
                // dann erzeugen wir mal schnell eine Kopie von der NumRule,
                // denn diese wird jetzt mal kurz veraendert.
                if( aEntry.nListId )
                    for( USHORT n = 0; n < aListArr.Count(); ++n )
                        if( aListArr[ n ].nListId == aEntry.nListId )
                        {
                            pRule = pDoc->GetNumRuleTbl()[
                                                aListArr[ n ].nListDocPos ];
                            pOrigRule = pRule;

                            String sTmp( String::CreateFromAscii(
                                RTL_CONSTASCII_STRINGPARAM( RTF_NUMRULE_NAME " 1" )));
                            aEntry.nListDocPos = pDoc->MakeNumRule( sTmp, pRule );
                            pRule = pDoc->GetNumRuleTbl()[ aEntry.nListDocPos ];
                            // --> OD 2008-07-08 #i91400#
                            pRule->SetName( pDoc->GetUniqueNumRuleName( &sTmp, FALSE ),
                                            *pDoc );
                            // <--
                            pRule->SetAutoRule( FALSE );
                            nNumLvl = (BYTE)-1;
                            aListArr.Insert( aEntry, aListArr.Count() );
                            break;
                        }

            }
            break;

        case RTF_LISTLEVEL:
            if( pRule && bOverrideFormat )
            {
                if( ++nNumLvl < MAXLEVEL )
                    pRule->Set( nNumLvl, pRule->Get( nNumLvl ));
                ReadListLevel( *pRule, nNumLvl );
            }
            break;

        case RTF_LEVELSTARTAT:
            if( pRule && bOverrideStart )
            {
            }
            break;

        case RTF_LISTOVERRIDESTART:
            bOverrideStart = TRUE;
            break;

        case RTF_LISTOVERRIDEFORMAT:
            bOverrideFormat = TRUE;
            break;

        case RTF_LFOLEVEL:
            // was fehlt noch?
            break;
        }
    }

    // search the outline numrule and set it into the doc
    if( GetStyleTbl().Count() )
    {
        if( !bStyleTabValid )
            MakeStyleTab();

        const SfxPoolItem* pItem( 0 );
        const SwTxtFmtColl* pColl( 0 );
        USHORT nRulePos( USHRT_MAX );
        const SwNumRule *pNumRule = 0;
        SvxRTFStyleType* pStyle = GetStyleTbl().First();
        do {
            // --> OD 2007-12-17 #151213#
            // suppress deletion of outline list style.
            // refactoring of code: no assignments in if-condition
//            if( MAXLEVEL > pStyle->nOutlineNo &&
//                0 != ( pColl = aTxtCollTbl.Get( (USHORT)GetStyleTbl().
//                                                        GetCurKey() )) &&
//                SFX_ITEM_SET == pColl->GetItemState( RES_PARATR_NUMRULE,
//                                                    FALSE, &pItem ) &&
//                USHRT_MAX != (nRulePos = pDoc->FindNumRule(
//                                ((SwNumRuleItem*)pItem)->GetValue() )) &&
//                (pNumRule = pDoc->GetNumRuleTbl()[ nRulePos ])->IsAutoRule() )
            if ( MAXLEVEL > pStyle->nOutlineNo )
            {
                pColl = aTxtCollTbl.Get( (USHORT)GetStyleTbl().GetCurKey() );
                if ( pColl )
                {
                    const SfxItemState eItemState =
                        pColl->GetItemState( RES_PARATR_NUMRULE, FALSE, &pItem );
                    if ( eItemState == SFX_ITEM_SET )
                    {
                        nRulePos = pDoc->FindNumRule( ((SwNumRuleItem*)pItem)->GetValue() );
                        if ( nRulePos != USHRT_MAX )
                        {
                            pNumRule = pDoc->GetNumRuleTbl()[ nRulePos ];
                            if ( pNumRule->IsAutoRule() &&
                                 pNumRule != pDoc->GetOutlineNumRule() )
                            {
                                pDoc->SetOutlineNumRule( *pNumRule );
                                pDoc->DelNumRule( pNumRule->GetName() );
                                // now pNumRule pointer is invalid !!!

                                // now decrement all position in the listtable, which will
                                // behind the doc-rule position
                                for( USHORT n = aListArr.Count(); n; )
                                {
                                    SwListEntry& rEntry = aListArr[ --n ];
                                    if( rEntry.nListDocPos == nRulePos )
                                        aListArr.Remove( n );
                                    else if( rEntry.nListDocPos > nRulePos )
                                        --rEntry.nListDocPos;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            // <--

            pStyle->aAttrSet.ClearItem( FN_PARAM_NUM_LEVEL );

        } while( 0 != (pStyle = GetStyleTbl().Next()) );
    }

    SkipToken( -1 );		// die schliesende Klammer wird "oben" ausgewertet
}

SwNumRule* SwRTFParser::GetNumRuleOfListNo( long nListNo, BOOL bRemoveFromList )
{
    SwNumRule* pRet = 0;
    SwListEntry* pEntry;
    for( USHORT n = aListArr.Count(); n; )
        if( ( pEntry = &aListArr[ --n ])->nListNo == nListNo )
        {
            if( bRemoveFromList )
                aListArr.Remove( n );
            else
            {
                pEntry->bRuleUsed = TRUE;
                pRet = pDoc->GetNumRuleTbl()[ pEntry->nListDocPos ];
            }
            break;
        }
    return pRet;
}

void SwRTFParser::RemoveUnusedNumRule( SwNumRule* pRule )
{
    if( pRule )
    {
        for ( BYTE nLvl = 0; nLvl < MAXLEVEL; ++nLvl )
        {
            SwNumFmt& rNFmt = (SwNumFmt&)pRule->Get( nLvl );
            SwCharFmt* pCFmt = rNFmt.GetCharFmt();
            if( pCFmt )
            {
                pCFmt->Remove( &rNFmt );
                if( !pCFmt->GetDepends() )
                    pDoc->DelCharFmt( pCFmt );
            }
        }
        pDoc->DelNumRule( pRule->GetName() );
    }
#ifdef DBG_UTIL
    else
    {
        ASSERT( pRule, "NumRulePointer 0 kann nicht geloescht werden" );
    }
#endif
}

void SwRTFParser::RemoveUnusedNumRules()
{
    SwListEntry* pEntry;
    SvPtrarr aDelArr;
    USHORT n;
    for( n = aListArr.Count(); n; )
    {
        if( !( pEntry = &aListArr[ --n ])->bRuleUsed )
        {
            // really *NOT* used by anyone else?
            BOOL unused=TRUE;
            for(USHORT j = 0;  j < aListArr.Count();  ++j)
            {
                if (aListArr[n].nListNo==aListArr[j].nListNo)
                    unused&=!aListArr[j].bRuleUsed;
            }
            if (unused)
            {
                void * p = pDoc->GetNumRuleTbl()[pEntry->nListDocPos];
                // dont delete named char formats
                if( USHRT_MAX == aDelArr.GetPos( p ) &&
                    ((SwNumRule*)p)->GetName().EqualsAscii( RTF_NUMRULE_NAME, 0,
                                    sizeof( RTF_NUMRULE_NAME )) )
                    aDelArr.Insert( p, aDelArr.Count() );
            }
        }
    }

    for( n = aDelArr.Count(); n; )
    {
        SwNumRule* pDel = (SwNumRule*)aDelArr[ --n ];
        RemoveUnusedNumRule( pDel );
    }
}

const Font* SwRTFParser::FindFontOfItem( const SvxFontItem& rItem ) const
{
    SvxRTFFontTbl& rFntTbl = ((SwRTFParser*)this)->GetFontTbl();
    const Font* pFnt = rFntTbl.First();
    while( pFnt )
    {
        if( pFnt->GetFamily() == rItem.GetFamily() &&
            pFnt->GetName() == rItem.GetFamilyName() &&
            pFnt->GetStyleName() == rItem.GetStyleName() &&
            pFnt->GetPitch() == rItem.GetPitch() &&
            pFnt->GetCharSet() == rItem.GetCharSet() )
            return pFnt;

        pFnt = rFntTbl.Next();
    }
    return 0;
}


SwNumRule *SwRTFParser::ReadNumSecLevel( int nToken )
{
    // lese die \pnseclvl - Gruppe
    // nTokenValue gibt schon den richtigen Level vor 1 - 9!
    BYTE nLevel = 0;
    long nListNo = 0;
    BOOL bContinus = TRUE;

    if( RTF_PNSECLVL == nToken )
    {
        // suche die Rule - steht unter Nummer 3
        nListNo = 3;
        bContinus = FALSE;
        nLevel = MAXLEVEL <= nTokenValue ? MAXLEVEL - 1
                                         : BYTE( nTokenValue - 1 );
    }
    else
    {
        switch( nToken = GetNextToken() )
        {
        case RTF_PNLVL:			nListNo = 3;
                                bContinus = FALSE;
                                nLevel = MAXLEVEL <= nTokenValue
                                                    ? MAXLEVEL - 1
                                                    : BYTE( nTokenValue-1 );
                                break;

        case RTF_PNLVLBODY:
            nListNo = 2;
            break;
        case RTF_PNLVLBLT:
            nListNo = 1;
            break;
        case RTF_PNLVLCONT:
            SkipGroup();
            return 0;
        default:
            SkipGroup();
            return 0;
        }
    }

    // suche die Rule - steht unter Nummer 3
    USHORT nNewFlag = static_cast< USHORT >(1 << nListNo);
    SwNumRule* pCurRule = GetNumRuleOfListNo( nListNo,
                                        0 != ( nNewNumSectDef & nNewFlag ) );
    if( !pCurRule )
    {
        // dann muessen wir die mal anlegen
        nNewNumSectDef &= ~nNewFlag;
        String sTmp( String::CreateFromAscii(
                        RTL_CONSTASCII_STRINGPARAM( RTF_NUMRULE_NAME " 1" )));
        SwListEntry aEntry( nListNo, 0, pDoc->MakeNumRule( sTmp ));
        aEntry.nListNo = nListNo;
        aListArr.Insert( aEntry, aListArr.Count() );
        pCurRule = pDoc->GetNumRuleTbl()[ aEntry.nListDocPos ];
        // --> OD 2008-07-08 #i91400#
        pCurRule->SetName( pDoc->GetUniqueNumRuleName( &sTmp, FALSE ), *pDoc );
        // <--
        pCurRule->SetAutoRule( FALSE );
        pCurRule->SetContinusNum( bContinus );
    }

    if( !pCurRule->GetNumFmt( nLevel ))
        pCurRule->Set( nLevel, pCurRule->Get( nLevel ));
    SwNumFmt* pCurNumFmt = (SwNumFmt*)pCurRule->GetNumFmt( nLevel );
    if( RTF_PNLVLBLT == nToken )
        pCurNumFmt->SetNumberingType(SVX_NUM_CHAR_SPECIAL);
    pCurNumFmt->SetSuffix( aEmptyStr );
    pCurNumFmt->SetPrefix( aEmptyStr );
    pCurNumFmt->SetNumberingType(SVX_NUM_NUMBER_NONE);

    if( bStyleTabValid && RTF_PNSECLVL != nToken )
    {
        // dann den akt. Lvl und Rule am Absatz setzen.
        // Dieses muss aber in den vorherigen "Kontext", sprich in den vor
        // der Klammer offenen Attrset. Darum das SetNewGroup davor und dahinter
        SetNewGroup( FALSE );
        GetAttrSet().Put( SfxUInt16Item( FN_PARAM_NUM_LEVEL, nLevel ));
        GetAttrSet().Put( SwNumRuleItem( pCurRule->GetName() ));
        SetNewGroup( TRUE );
    }

    FontUnderline eUnderline;
    int nNumOpenBrakets = 1;		// die erste wurde schon vorher erkannt !!
    while( nNumOpenBrakets && IsParserWorking() )
    {
        switch( ( nToken = GetNextToken() ))
        {
        case '}':
            if( --nNumOpenBrakets && IsParserWorking() )
            {
                // Style konnte vollstaendig gelesen werden,
                // also ist das noch ein stabiler Status
                SaveState( RTF_PNSECLVL );
            }
            break;

        case '{':
            {
                if( RTF_IGNOREFLAG != GetNextToken() )
                    nToken = SkipToken( -1 );
                // Unknown und alle bekannten nicht ausgewerteten Gruppen
                // sofort ueberspringen
                else if( RTF_UNKNOWNCONTROL != ( nToken = GetNextToken() ))
                    nToken = SkipToken( -2 );
                else
                {
                    // gleich herausfiltern
                    ReadUnknownData();
                    nToken = GetNextToken();
                    if( '}' != nToken )
                        eState = SVPAR_ERROR;
                    break;
                }
                ++nNumOpenBrakets;
            }
            break;

        case RTF_PNCARD:
        case RTF_PNORD:
        case RTF_PNORDT:
        case RTF_PNDEC: 	pCurNumFmt->SetNumberingType(SVX_NUM_ARABIC);				break;
        case RTF_PNUCLTR:	pCurNumFmt->SetNumberingType(SVX_NUM_CHARS_UPPER_LETTER_N);	break;
        case RTF_PNUCRM:    pCurNumFmt->SetNumberingType(SVX_NUM_ROMAN_UPPER);			break;
        case RTF_PNLCLTR:   pCurNumFmt->SetNumberingType(SVX_NUM_CHARS_LOWER_LETTER_N);	break;
        case RTF_PNLCRM:    pCurNumFmt->SetNumberingType(SVX_NUM_ROMAN_LOWER);			break;

        case RTF_PNF:
            {
                const Font& rSVFont = GetFont( USHORT(nTokenValue) );
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put(
                            SvxFontItem( rSVFont.GetFamily(),
                                rSVFont.GetName(), rSVFont.GetStyleName(),
                                rSVFont.GetPitch(), rSVFont.GetCharSet(),
                                            RES_CHRATR_FONT ));
                if( SVX_NUM_CHAR_SPECIAL == pCurNumFmt->GetNumberingType() )
                    pCurNumFmt->SetBulletFont( &rSVFont );
            }
            break;
        case RTF_PNFS:
            {
                if( -1 == nTokenValue )
                    nTokenValue = 240;
                else
                    nTokenValue *= 10;
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put(
                            SvxFontHeightItem( (const USHORT)nTokenValue, 100, RES_CHRATR_FONTSIZE ));
            }
            break;

        case RTF_PNB:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put( SvxWeightItem(
                                nTokenValue ? WEIGHT_BOLD : WEIGHT_NORMAL, RES_CHRATR_WEIGHT ));
            }
            break;

        case RTF_PNI:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put( SvxPostureItem(
                            nTokenValue ? ITALIC_NORMAL : ITALIC_NONE, RES_CHRATR_POSTURE ));
            }
            break;

        case RTF_PNCAPS:
        case RTF_PNSCAPS:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put( SvxCaseMapItem(
                                nTokenValue ? SVX_CASEMAP_KAPITAELCHEN
                                            : SVX_CASEMAP_NOT_MAPPED, RES_CHRATR_CASEMAP ));
            }
            break;
        case RTF_PNSTRIKE:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put( SvxCrossedOutItem(
                        nTokenValue ? STRIKEOUT_SINGLE : STRIKEOUT_NONE, RES_CHRATR_CROSSEDOUT ));
            }
            break;

        case RTF_PNCF:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put( SvxColorItem(
                            GetColor( USHORT(nTokenValue) ), RES_CHRATR_COLOR ));
            }
            break;


        case RTF_PNUL:
            eUnderline = nTokenValue ? UNDERLINE_SINGLE : UNDERLINE_NONE;
            goto NUMATTR_SETUNDERLINE;
        case RTF_PNULD:
            eUnderline = UNDERLINE_DOTTED;
            goto NUMATTR_SETUNDERLINE;
        case RTF_PNULDB:
            eUnderline = UNDERLINE_DOUBLE;
            goto NUMATTR_SETUNDERLINE;
        case RTF_PNULNONE:
            eUnderline = UNDERLINE_NONE;
            goto NUMATTR_SETUNDERLINE;
        case RTF_PNULW:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put(
                                    SvxWordLineModeItem( TRUE, RES_CHRATR_WORDLINEMODE ));
            }
            eUnderline = UNDERLINE_SINGLE;
            goto NUMATTR_SETUNDERLINE;

NUMATTR_SETUNDERLINE:
            {
                GetNumChrFmt( *pDoc, *pCurRule, nLevel ).Put(
                        SvxUnderlineItem( eUnderline, RES_CHRATR_UNDERLINE ));
            }
            break;

        case RTF_PNINDENT:
            if( 0 > short( nTokenValue ) )
                nTokenValue = - (short)nTokenValue;
            pCurNumFmt->SetFirstLineIndent( - nTokenValue );
            pCurNumFmt->SetIndentAt( (nLevel + 1 ) * nTokenValue );
            break;
        case RTF_PNSP:
            pCurNumFmt->SetCharTextDistance( USHORT( nTokenValue ));
            break;

        case RTF_PNPREV:
            if( nLevel )
            {
                BYTE nPrev = 2, nLast = nLevel;
                while( nLast && 1 < pCurRule->Get( --nLast ).GetIncludeUpperLevels() )
                    ++nPrev;
                pCurNumFmt->SetIncludeUpperLevels( nPrev );
            }
            break;

        case RTF_PNQC:	pCurNumFmt->SetNumAdjust( SVX_ADJUST_CENTER ); 	break;
        case RTF_PNQL:	pCurNumFmt->SetNumAdjust( SVX_ADJUST_LEFT ); 		break;
        case RTF_PNQR:	pCurNumFmt->SetNumAdjust( SVX_ADJUST_RIGHT );		break;

        case RTF_PNSTART:
            pCurNumFmt->SetStart( USHORT( nTokenValue ));
            break;

        case RTF_PNNUMONCE:
        case RTF_PNACROSS:
        case RTF_PNHANG:
        case RTF_PNRESTART:		break;

        case RTF_PNTXTA:
            {
                String sTmp;
                GetTextToEndGroup( sTmp );
                if( SVX_NUM_CHAR_SPECIAL == pCurNumFmt->GetNumberingType() )
                {
                    pCurNumFmt->SetBulletChar( sTmp.GetChar( 0 ) );
                    if( pCurNumFmt->GetCharFmt() )
                        pCurNumFmt->SetBulletFont( FindFontOfItem(
                                    pCurNumFmt->GetCharFmt()->GetFont() ) );
                    sTmp.Erase();
                }
                pCurNumFmt->SetSuffix( sTmp );
            }
            break;

        case RTF_PNTXTB:
            {
                String sTmp;
                pCurNumFmt->SetPrefix( GetTextToEndGroup( sTmp ) );
            }
            break;
        }
    }

    // falls vollstaendige Numerierung an ist und das Zeichen davor ein
    // Punkt ist, dann will RTF den Punkt als Trenner zwischen den Ebenen
    // haben - das haben wir aber schon als default
    if( 1 < pCurNumFmt->GetIncludeUpperLevels() &&
        1 == pCurNumFmt->GetPrefix().Len() &&
        '.' == pCurNumFmt->GetPrefix().GetChar( 0 ) &&
        SVX_NUM_CHAR_SPECIAL != pCurNumFmt->GetNumberingType() )
        pCurNumFmt->SetPrefix( aEmptyStr );

    // falls das ein nicht numerierter Absatz mit ein Prefix-Text mit
    // einem Zeichen ist, dann setze den als Bulletzeichen
    if( pCurNumFmt->GetCharFmt() && SVX_NUM_NUMBER_NONE == pCurNumFmt->GetNumberingType() &&
        3 == nListNo && 1 == pCurNumFmt->GetPrefix().Len() )
    {
        SwCharFmt* pChFmt = pCurNumFmt->GetCharFmt();
        pCurNumFmt->SetNumberingType(SVX_NUM_CHAR_SPECIAL);
        pCurNumFmt->SetBulletFont( FindFontOfItem( pChFmt->GetFont() ) );

        pCurNumFmt->SetBulletChar( pCurNumFmt->GetPrefix().GetChar( 0 ) );
        pCurNumFmt->SetPrefix( aEmptyStr );

        // den Font oder sogar das gesamte CharFormat loeschen?
        if( SFX_ITEM_SET == pChFmt->GetItemState( RES_CHRATR_FONT, FALSE ))
        {
            if( 1 == pChFmt->GetAttrSet().Count() )
            {
                pCurNumFmt->SetCharFmt( 0 );
                pDoc->DelCharFmt( pChFmt );
            }
            else
                pChFmt->ResetFmtAttr( RES_CHRATR_FONT );
        }
    }

    SkipToken( -1 );		// die schliesende Klammer wird "oben" ausgewertet
    return pCurRule;
}


/*  */

// dann noch die Ausgabe-Funktionen (nur fuer WinWord 97)

BOOL lcl_IsExportNumRule( const SwNumRule& rRule, BYTE* pEnd = 0 )
{
    BYTE nEnd = MAXLEVEL;
    while( nEnd-- && !rRule.GetNumFmt( nEnd ))
        ;
    ++nEnd;

    const SwNumFmt* pNFmt;
    BYTE nLvl;

    for( nLvl = 0; nLvl < nEnd; ++nLvl )
        if( SVX_NUM_NUMBER_NONE != ( pNFmt = &rRule.Get( nLvl ))
            ->GetNumberingType() || pNFmt->GetPrefix().Len() ||
            (pNFmt->GetSuffix().Len() && pNFmt->GetSuffix() != aDotStr ))
            break;

    if( pEnd )
        *pEnd = nEnd;
    return nLvl != nEnd;
}

void SwRTFWriter::OutRTFListTab()
{
    ByteString sOverrideList;
    USHORT nId = 1, nTmplId = 1, n;

    // prepare the NodeNum to generate the NumString
    SwNumberTree::tNumberVector aNumVector;
    for( n = 0; n < MAXLEVEL; ++n )
        aNumVector.push_back(n);
    BYTE aNumLvlPos[ MAXLEVEL ];

    if( !pNumRuleTbl )
        BuildNumRuleTbl();

    for( n = 0; n < pNumRuleTbl->Count(); ++n )
    {
        const SwNumRule* pRule = (*pNumRuleTbl)[ n ];

        // mit der ersten Rule wird auch die ListTable geschrieben
        if( !sOverrideList.Len() )
            OutComment( *this, OOO_STRING_SVTOOLS_RTF_LISTTABLE );

        Strm() << '{' << OOO_STRING_SVTOOLS_RTF_LIST << OOO_STRING_SVTOOLS_RTF_LISTTEMPLATEID;
        OutULong( nTmplId );
        ByteString sTmp;

        if( pRule->IsContinusNum() )
            Strm() << OOO_STRING_SVTOOLS_RTF_LISTSIMPLE;

        BYTE nLvl, nEnd;
        lcl_IsExportNumRule( *pRule, &nEnd );

        for( nLvl = 0; nLvl < nEnd; ++nLvl )
        {
            const SwNumFmt& rFmt = pRule->Get( nLvl );
            Strm() << sNewLine;
            if( nLvl > 8 )			// RTF-kennt nur 9 Ebenen
                OutComment( *this, OOO_STRING_SVTOOLS_RTF_SOUTLVL );

            Strm() << '{' << OOO_STRING_SVTOOLS_RTF_LISTLEVEL << OOO_STRING_SVTOOLS_RTF_LEVELNFC;

            USHORT nVal = 0;
            switch( rFmt.GetNumberingType() )
            {
            case SVX_NUM_ROMAN_UPPER:			nVal = 1;	break;
            case SVX_NUM_ROMAN_LOWER:			nVal = 2;	break;
            case SVX_NUM_CHARS_UPPER_LETTER:
            case SVX_NUM_CHARS_UPPER_LETTER_N:	nVal = 3;	break;
            case SVX_NUM_CHARS_LOWER_LETTER:
            case SVX_NUM_CHARS_LOWER_LETTER_N:	nVal = 4;	break;

            case SVX_NUM_BITMAP:
            case SVX_NUM_CHAR_SPECIAL:			nVal = 23;	break;
            }
            OutLong( nVal ) << OOO_STRING_SVTOOLS_RTF_LEVELJC;

            switch( rFmt.GetNumAdjust() )
            {
            case SVX_ADJUST_CENTER:		nVal = 1;	break;
            case SVX_ADJUST_RIGHT:		nVal = 2;	break;
            default:					nVal = 0;	break;
            }
            OutLong( nVal ) << OOO_STRING_SVTOOLS_RTF_LEVELSTARTAT;
            OutLong( rFmt.GetStart() )
                << OOO_STRING_SVTOOLS_RTF_LEVELFOLLOW << "0{" << OOO_STRING_SVTOOLS_RTF_LEVELTEXT << ' ';

            BOOL bWriteBulletFont = FALSE;
            memset( aNumLvlPos, 0, MAXLEVEL );
            if( SVX_NUM_CHAR_SPECIAL == rFmt.GetNumberingType() ||
                SVX_NUM_BITMAP == rFmt.GetNumberingType() )
            {
                Strm() << "\\'01";
                ByteString sNo( ByteString::CreateFromInt32( rFmt.GetBulletChar()));
                Strm() << "\\u" << sNo.GetBuffer() << " ?";
                bWriteBulletFont = TRUE;
            }
            else if( SVX_NUM_NUMBER_NONE == rFmt.GetNumberingType() )
            {
                String sOut( rFmt.GetPrefix() ); sOut += rFmt.GetSuffix();
                if( sOut.Len() )
                {
                    Strm() << "\\'";
                    OutHex( sOut.Len() );
                    RTFOutFuncs::Out_String( Strm(), sOut,
                                            eDefaultEncoding, bWriteHelpFmt );
                }
            }
            else
            {
                memset( aNumLvlPos, 0, MAXLEVEL );
                BYTE* pLvlPos = aNumLvlPos;
                // --> OD 2005-11-18 #128056#
                // correction of refactoring done by cws swnumtree:
                // - the numbering string has to be restrict to the level
                //   currently working on.
                String sNumStr( pRule->MakeNumString( aNumVector, FALSE, TRUE, nLvl ));
                // <--

                // now search the nums in the string
                for( BYTE i = 0; i <= nLvl; ++i )
                {
                    String sSrch( String::CreateFromInt32( i ));
                    xub_StrLen nFnd = sNumStr.Search( sSrch );
                    if( STRING_NOTFOUND != nFnd )
                    {
                        *pLvlPos = (BYTE)(nFnd + rFmt.GetPrefix().Len() + 1 );
                        ++pLvlPos;
                        sNumStr.SetChar( nFnd, (sal_Unicode)i );
                    }
                }

                Strm() << "\\'";
                OutHex( sNumStr.Len() + rFmt.GetPrefix().Len() +
                                        rFmt.GetSuffix().Len() );

                if( rFmt.GetPrefix().Len() )
                    RTFOutFuncs::Out_String( Strm(), rFmt.GetPrefix(),
                                            eDefaultEncoding, bWriteHelpFmt );


                for( xub_StrLen x = 0; x < sNumStr.Len(); ++x )
                    if( sNumStr.GetChar( x ) < 0x20 ||
                        sNumStr.GetChar( x ) > 0xFF )
                    {
                        Strm() << "\\'"; OutHex( sNumStr.GetChar( x ) );
                    }
                    else
                        Strm() << (sal_Char)sNumStr.GetChar( x );

                if( rFmt.GetSuffix().Len() )
                    RTFOutFuncs::Out_String( Strm(), rFmt.GetSuffix(),
                                            eDefaultEncoding, bWriteHelpFmt );
            }

            // write the levelnumbers
            Strm() << ";}{" << OOO_STRING_SVTOOLS_RTF_LEVELNUMBERS;
            for( BYTE i = 0; i <= nLvl && aNumLvlPos[ i ]; ++i )
            {
                Strm() << "\\'"; OutHex( aNumLvlPos[ i ] );
            }
            Strm() << ";}";

            if( rFmt.GetCharFmt() )
                Out_SfxItemSet( aRTFAttrFnTab, *this,
                                rFmt.GetCharFmt()->GetAttrSet(), TRUE );

            if( bWriteBulletFont )
            {
                Strm() << OOO_STRING_SVTOOLS_RTF_F;
                const Font* pFont = rFmt.GetBulletFont();
                if( !pFont )
                    // --> OD 2006-06-27 #b6440955#
                    pFont = &numfunc::GetDefBulletFont();
                    // <--
                OutULong( GetId( *pFont ));
            }

            Strm() << OOO_STRING_SVTOOLS_RTF_FI;
            OutLong( rFmt.GetFirstLineIndent() ) << OOO_STRING_SVTOOLS_RTF_LI;
            OutLong( rFmt.GetIndentAt() );

            Strm() << '}';

            if( nLvl > 8 )		// RTF-kennt nur 9 Ebenen
                Strm() << '}';
        }

        if( !pRule->IsAutoRule() )
        {
            Strm() << '{' << OOO_STRING_SVTOOLS_RTF_LISTNAME << ' ';
            RTFOutFuncs::Out_String( Strm(), pRule->GetName(), eDefaultEncoding,
                                    bWriteHelpFmt ) << ";}" ;
        }
        Strm() << OOO_STRING_SVTOOLS_RTF_LISTID;
        OutULong( nId ) << '}' << sNewLine;

        sTmp = '{';
        ((((((( sTmp += OOO_STRING_SVTOOLS_RTF_LISTOVERRIDE ) +=
                    OOO_STRING_SVTOOLS_RTF_LISTID ) += ByteString::CreateFromInt32( nId )) +=
                    OOO_STRING_SVTOOLS_RTF_LISTOVERRIDECOUNT ) += '0' ) +=
                    OOO_STRING_SVTOOLS_RTF_LS ) += ByteString::CreateFromInt32( n )) += '}';
        sOverrideList += sTmp;

        ++nId, ++nTmplId;
    }

    if( sOverrideList.Len() )
        Strm() << "}{" << OOO_STRING_SVTOOLS_RTF_LISTOVERRIDETABLE
               << sOverrideList.GetBuffer() << '}' << sNewLine;
}

USHORT SwRTFWriter::GetId( const SwNumRuleItem& rItem ) const
{
    USHORT n, nId = USHRT_MAX;
    if( !pNumRuleTbl )
    {
        SwRTFWriter* pThis = (SwRTFWriter*)this;
        pThis->BuildNumRuleTbl();
    }
    const String& rNm = rItem.GetValue();
    for( n = pNumRuleTbl->Count(); n; )
        if( (*pNumRuleTbl)[ --n ]->GetName() == rNm )
        {
            nId = n;
            break;
        }

    return nId;
}

USHORT SwRTFWriter::GetNumRuleId( const SwNumRule& rRule )
{
    if( !pNumRuleTbl )
        BuildNumRuleTbl();
    SwNumRulePtr pR = (SwNumRulePtr)&rRule;
    return pNumRuleTbl->GetPos( pR );
}

void SwRTFWriter::BuildNumRuleTbl()
{
    const SwNumRuleTbl& rListTbl = pDoc->GetNumRuleTbl();
    if( !pNumRuleTbl )
        pNumRuleTbl = new SwNumRuleTbl( (BYTE)rListTbl.Count() );
    for( USHORT n = rListTbl.Count()+1; n; )
    {
        SwNumRule* pRule;
        --n;
        if( n == rListTbl.Count() )
            pRule = (SwNumRule*)pDoc->GetOutlineNumRule();
        else
        {
            pRule = rListTbl[ n ];
            if( !pDoc->IsUsed( *pRule ))
                continue;
        }

        if( lcl_IsExportNumRule( *pRule ))
            pNumRuleTbl->Insert( pRule, pNumRuleTbl->Count() );
    }
}

BOOL SwRTFWriter::OutListNum( const SwTxtNode& rNd )
{
    BOOL bRet = FALSE;
    const SwNumRule* pRule = rNd.GetNumRule();

    // --> OD 2008-03-18 #refactorlists#
//    if( pRule && MAXLEVEL > rNd.GetActualListLevel() )
    if( pRule && rNd.IsInList() )
    // <--
    {
        // --> OD 2008-03-18 #refactorlists#
        ASSERT( rNd.GetActualListLevel() >= 0 && rNd.GetActualListLevel() < MAXLEVEL,
                "<SwRTFWriter::OutListNum(..)> - text node does not have valid list level. Serious defect -> please inform OD" );
        // <--

        bOutFmtAttr = FALSE;
        bOutListNumTxt = TRUE;
        bRet = TRUE;

        const bool bExportNumRule = USHRT_MAX != GetNumRuleId( *pRule );
        const BYTE nLvl = static_cast< BYTE >(rNd.GetActualListLevel());
        const SwNumFmt* pFmt = pRule->GetNumFmt( nLvl );
        if( !pFmt )
            pFmt = &pRule->Get( nLvl );

        const SfxItemSet& rNdSet = rNd.GetSwAttrSet();

        SfxItemSet aSet( *rNdSet.GetPool(), rNdSet.GetRanges() );
        aSet.SetParent( &rNdSet );
        SvxLRSpaceItem aLR( (SvxLRSpaceItem&)rNdSet.Get( RES_LR_SPACE ) );
        aLR.SetTxtLeft( aLR.GetTxtLeft() + pFmt->GetIndentAt() );

        aLR.SetTxtFirstLineOfst( pFmt->GetFirstLineIndent() );
        if ( bExportNumRule )
            Strm() << '{' << OOO_STRING_SVTOOLS_RTF_LISTTEXT << OOO_STRING_SVTOOLS_RTF_PARD << OOO_STRING_SVTOOLS_RTF_PLAIN << ' ';

        aSet.Put( aLR );
        Out_SfxItemSet( aRTFAttrFnTab, *this, aSet, TRUE );

        if( pFmt->GetCharFmt() )
            Out_SfxItemSet( aRTFAttrFnTab, *this,
                        pFmt->GetCharFmt()->GetAttrSet(), TRUE );

        {
            String sTxt;
            if( SVX_NUM_CHAR_SPECIAL == pFmt->GetNumberingType() || SVX_NUM_BITMAP == pFmt->GetNumberingType() )
                sTxt = pFmt->GetBulletChar();
            else
                sTxt = rNd.GetNumString();

            if( bOutFmtAttr )
            {
                Strm() << ' ';
                bOutFmtAttr = FALSE;
            }

            if (sTxt.Len())
            {
                RTFOutFuncs::Out_String(Strm(), sTxt, eDefaultEncoding,
                    bWriteHelpFmt);
            }

            if( bExportNumRule )
            {
                if( OUTLINE_RULE != pRule->GetRuleType() )
                {
                    Strm() << OOO_STRING_SVTOOLS_RTF_TAB << '}' << OOO_STRING_SVTOOLS_RTF_ILVL;
                    if( nLvl > 8 )			// RTF-kennt nur 9 Ebenen
                    {
                        OutULong( 8 );
                        OutComment( *this, OOO_STRING_SVTOOLS_RTF_SOUTLVL );
                        OutULong( nLvl ) << '}';
                    }
                    else
                        OutULong( nLvl );
                    Strm()  << ' ';
                }
                else
                    Strm() << OOO_STRING_SVTOOLS_RTF_TAB << '}';
            }
            else if( sTxt.Len() )
                Strm() << OOO_STRING_SVTOOLS_RTF_TAB;
        }
        bOutListNumTxt = FALSE;
    }
    return bRet;
}
