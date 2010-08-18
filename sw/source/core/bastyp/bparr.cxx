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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#include <string.h>
#include <limits.h>
#include "bparr.hxx"

// die Blockverwaltung waechst/schrumpft immer um 20 Bloecke, das sind dann
// immer ~ 20 * MAXENTRY == 20000 Eintraege
const USHORT nBlockGrowSize = 20;

#ifndef DBG_UTIL

#define CHECKIDX( p, n, i, c )

#else

#define CHECKIDX( p, n, i, c ) CheckIdx( p, n, i, c );

void CheckIdx( BlockInfo** ppInf, USHORT nBlock, ULONG nSize, USHORT nCur )
{
    DBG_ASSERT( !nSize || nCur < nBlock, "BigPtrArray: CurIndex steht falsch" );

    ULONG nIdx = 0;
    for( USHORT nCnt = 0; nCnt < nBlock; ++nCnt, ++ppInf )
    {
        nIdx += (*ppInf)->nElem;
        // Array mit Luecken darf es nicht geben
        DBG_ASSERT( !nCnt || (*(ppInf-1))->nEnd + 1 == (*ppInf)->nStart,
                    "BigPtrArray: Luecke in der Verwaltung!" );
    }

    DBG_ASSERT( nIdx == nSize, "BigPtrArray: Anzahl ungueltig" );
}

#endif


BigPtrArray::BigPtrArray()
{
    nBlock = nCur = 0;
    nSize = 0;
    nMaxBlock = nBlockGrowSize;		// == 20 * 1000 Eintraege
    ppInf = new BlockInfo* [ nMaxBlock ];
}



BigPtrArray::~BigPtrArray()
{
    if( nBlock )
    {
        BlockInfo** pp = ppInf;
        for( USHORT n = 0; n < nBlock; ++n, ++pp )
        {
            delete[] (*pp)->pData;
            delete    *pp;
        }
    }
    delete[] ppInf;
}

// Auch der Move ist schlicht. Optimieren ist hier wg. der
// Stueckelung des Feldes zwecklos!

void BigPtrArray::Move( ULONG from, ULONG to )
{
    USHORT cur = Index2Block( from );
    BlockInfo* p = ppInf[ cur ];
    ElementPtr pElem = p->pData[ from - p->nStart ];
    Insert( pElem, to );			// erst einfuegen, dann loeschen !!!!
    Remove( ( to < from ) ? ( from + 1 ) : from );
}

// das Ende ist EXCLUSIV


void BigPtrArray::ForEach( ULONG nStart, ULONG nEnd,
                            FnForEach fn, void* pArgs )
{
    if( nEnd > nSize )
        nEnd = nSize;

    if( nStart < nEnd )
    {
        USHORT cur = Index2Block( nStart );
        BlockInfo** pp = ppInf + cur;
        BlockInfo* p = *pp;
        USHORT nElem = USHORT( nStart - p->nStart );
        ElementPtr* pElem = p->pData + nElem;
        nElem = p->nElem - nElem;
        for(;;)
        {
            if( !(*fn)( *pElem++, pArgs ) || ++nStart >= nEnd )
                break;

            // naechstes Element
            if( !--nElem )
            {
                // neuer Block
                p = *++pp;
                pElem = p->pData;
                nElem = p->nElem;
            }
        }
    }
}


ElementPtr BigPtrArray::operator[]( ULONG idx ) const
{
    // weil die Funktion eben doch nicht const ist:
    DBG_ASSERT( idx < nSize, "operator[]: Index aussserhalb" );
    BigPtrArray* pThis = (BigPtrArray*) this;
    USHORT cur = Index2Block( idx );
    BlockInfo* p = ppInf[ cur ];
    pThis->nCur = cur;
    return p->pData[ idx - p->nStart ];
}

///////////////////////////////////////////////////////////////////////////

// private Methoden

// Suchen des Blocks einer bestimmten Position
// Algorithmus:
// 1. Test, ob der letzte Block der gesuchte Block ist
// 2. Sonderfall: Index = 0?
// 3. Test der Nachbarbloecke

// 4. Binaere Suche



USHORT BigPtrArray::Index2Block( ULONG pos ) const
{
    // zuletzt verwendeter Block?
    BlockInfo* p = ppInf[ nCur ];
    if( p->nStart <= pos && p->nEnd >= pos )
        return nCur;
    // Index = 0?
    if( !pos )
        return 0;
    // Folgeblock?
    if( nCur < ( nBlock - 1 ) )
    {
        p = ppInf[ nCur+1 ];
        if( p->nStart <= pos && p->nEnd >= pos )
            return nCur+1;
    }
    // vorangehender Block?
    else if( pos < p->nStart && nCur > 0 )
    {
        p = ppInf[ nCur-1 ];
        if( p->nStart <= pos && p->nEnd >= pos )
            return nCur-1;
    }
    // Binaere Suche:
    // Diese fuehrt immer zum Erfolg
    USHORT lower = 0, upper = nBlock - 1;
    USHORT cur = 0;
    for(;;)
    {
        USHORT n = lower + ( upper - lower ) / 2;
        cur = ( n == cur ) ? n+1 : n;
        p = ppInf[ cur ];
        if( p->nStart <= pos && p->nEnd >= pos )
            return cur;
        if( p->nStart > pos )
            upper = cur;
        else
            lower = cur;
    }
}


// Update aller Indexbereiche ab einer bestimmten Position

// pos bezeichnet den letzten korrekten Block

void BigPtrArray::UpdIndex( USHORT pos )
{
    BlockInfo** pp = ppInf + pos;
    ULONG idx = (*pp)->nEnd + 1;
    BlockInfo* p;
    while( ++pos < nBlock )
    {
        p = *++pp;
        p->nStart = idx;
        idx       += p->nElem;
        p->nEnd   = idx - 1;
    }
}

// Einrichten eines neuen Blocks an einer bestimmten Position

// Vorhandene Blocks werden nach hinten verschoben



BlockInfo* BigPtrArray::InsBlock( USHORT pos )
{
    if( nBlock == nMaxBlock )
    {
        // dann sollte wir mal das Array erweitern
        BlockInfo** ppNew = new BlockInfo* [ nMaxBlock + nBlockGrowSize ];
        memcpy( ppNew, ppInf, nMaxBlock * sizeof( BlockInfo* ));
        delete[] ppInf;
        nMaxBlock += nBlockGrowSize;
        ppInf = ppNew;
    }
    if( pos != nBlock )
        memmove( ppInf + pos+1, ppInf + pos ,
                 ( nBlock - pos ) * sizeof (BlockInfo*) );
    ++nBlock;
    BlockInfo* p = new BlockInfo;
    ppInf[ pos ] = p;

    if( pos )
        p->nStart = p->nEnd = ppInf[ pos-1 ]->nEnd + 1;
    else
        p->nStart = p->nEnd = 0;
    p->nEnd--;	// keine Elemente
    p->nElem = 0;
    p->pData = new ElementPtr [ MAXENTRY ];
    p->pBigArr = this;
    return p;
}

void BigPtrArray::BlockDel( USHORT nDel )
{
    nBlock = nBlock - nDel;
    if( nMaxBlock - nBlock > nBlockGrowSize )
    {
        // dann koennen wir wieder schrumpfen
        nDel = (( nBlock / nBlockGrowSize ) + 1 ) * nBlockGrowSize;
        BlockInfo** ppNew = new BlockInfo* [ nDel ];
        memcpy( ppNew, ppInf, nBlock * sizeof( BlockInfo* ));
        delete[] ppInf;
        ppInf = ppNew;
        nMaxBlock = nDel;
    }
}


void BigPtrArray::Insert( const ElementPtr& rElem, ULONG pos )
{
    CHECKIDX( ppInf, nBlock, nSize, nCur );

    BlockInfo* p;
    USHORT cur;
    if( !nSize )
        // Sonderfall: erstes Element einfuegen
        p = InsBlock( cur = 0 );
    else if( pos == nSize )
    {
        // Sonderfall: Einfuegen am Ende
        cur = nBlock - 1;
        p = ppInf[ cur ];
        if( p->nElem == MAXENTRY )
            // Der letzte Block ist voll, neuen anlegen
            p = InsBlock( ++cur );
    }
    else
    {
        // Standardfall:
        cur = Index2Block( pos );
        p = ppInf[ cur ];
    }
    if( p->nElem == MAXENTRY )
    {
        // passt der letzte Eintrag in den naechsten Block?
        BlockInfo* q;
        if( cur < ( nBlock - 1 ) && ppInf[ cur+1 ]->nElem < MAXENTRY )
        {
            q = ppInf[ cur+1 ];
            if( q->nElem )
            {
                int nCount = q->nElem;
                ElementPtr *pFrom = q->pData + nCount,
                                    *pTo = pFrom+1;
                while( nCount-- )
                    ++( *--pTo = *--pFrom )->nOffset;
            }
            q->nStart--;
            q->nEnd--;
        }
        else
        {
            // Wenn er auch nicht in den Folgeblock passt, muss ein
            // neuer Block eingefuegt werden
            // erst mal bei Bedarf komprimieren

            // wenn mehr als 50% "Luft" im Array ist, dann sollte man mal das
            // Compress aufrufen
            if( /*nBlock == nMaxBlock &&*/
                nBlock > ( nSize / ( MAXENTRY / 2 ) ) &&
                cur >= Compress() )
            {
                // es wurde vor der akt. Pos etwas verschoben und alle
                // Pointer koennen ungueltig sein. Also das Insert
                // nochmals aufsetzen
                Insert( rElem, pos );
                return ;
            }

            q = InsBlock( cur+1 );
        }

        // Eintrag passt nicht mehr. Dann muss Platz gemacht werden
        ElementPtr pLast = p->pData[ MAXENTRY-1 ];
        pLast->nOffset = 0;
        pLast->pBlock = q;

        q->pData[ 0 ] = pLast;
        q->nElem++;
        q->nEnd++;

        p->nEnd--;
        p->nElem--;
    }
    // Nun haben wir einen freien Block am Wickel: eintragen
    pos -= p->nStart;
    DBG_ASSERT( pos < MAXENTRY, "falsche Pos" );
    if( pos != p->nElem )
    {
        int nCount = p->nElem - USHORT(pos);
        ElementPtr *pFrom = p->pData + p->nElem,
                            *pTo = pFrom + 1;
        while( nCount-- )
            ++( *--pTo = *--pFrom )->nOffset;
    }
    // Element eintragen und Indexe updaten
    ((ElementPtr&)rElem)->nOffset = USHORT(pos);
    ((ElementPtr&)rElem)->pBlock = p;
    p->pData[ pos ] = rElem;
    p->nEnd++;
    p->nElem++;
    nSize++;
    if( cur != ( nBlock - 1 ) ) UpdIndex( cur );
    nCur = cur;

    CHECKIDX( ppInf, nBlock, nSize, nCur );
}

void BigPtrArray::Remove( ULONG pos, ULONG n )
{
    CHECKIDX( ppInf, nBlock, nSize, nCur );

    USHORT nBlkdel = 0;					// entfernte Bloecke
    USHORT cur = Index2Block( pos );	// aktuelle Blocknr
    USHORT nBlk1 = cur;                 // 1. behandelter Block
    USHORT nBlk1del = USHRT_MAX;		// 1. entfernter Block
    BlockInfo* p = ppInf[ cur ];
    pos -= p->nStart;
    ULONG nElem = n;
    while( nElem )
    {
        USHORT nel = p->nElem - USHORT(pos);
        if( ULONG(nel) > nElem )
            nel = USHORT(nElem);
        // Eventuell Elemente verschieben
        if( ( pos + nel ) < ULONG(p->nElem) )
        {
            ElementPtr *pTo = p->pData + pos,
                                *pFrom = pTo + nel;
            int nCount = p->nElem - nel - USHORT(pos);
            while( nCount-- )
            {
                *pTo = *pFrom++;
                (*pTo)->nOffset = (*pTo)->nOffset - nel;
                ++pTo;
            }
        }
        p->nEnd -= nel;
        p->nElem = p->nElem - nel;
        if( !p->nElem )
        {
            // eventuell Block ganz entfernen
            delete[] p->pData;
            nBlkdel++;
            if( USHRT_MAX == nBlk1del )
                nBlk1del = cur;
        }
        nElem -= nel;
        if( !nElem )
            break;
        p = ppInf[ ++cur ];
        pos = 0;
    }
    // Am Ende die Tabelle updaten, falls Bloecke geloescht waren
    if( nBlkdel )
    {
        // loeschen sollte man immer !!
        for( USHORT i = nBlk1del; i < ( nBlk1del + nBlkdel ); i++ )
            delete ppInf[ i ];

        if( ( nBlk1del + nBlkdel ) < nBlock )
        {
            memmove( ppInf + nBlk1del, ppInf + nBlk1del + nBlkdel,
                     ( nBlock - nBlkdel - nBlk1del ) * sizeof( BlockInfo* ) );

            // JP 19.07.95: nicht den ersten behandelten, sondern den davor!!
            //				UpdateIdx updatet nur alle Nachfolgende!!
            if( !nBlk1 )
            {
                p = ppInf[ 0 ];
                p->nStart = 0;
                p->nEnd = p->nElem-1;
            }
            else
                --nBlk1;
        }
        BlockDel( nBlkdel );			// es wurden Bloecke geloescht
    }

    nSize -= n;
    if( nBlk1 != ( nBlock - 1 ) && nSize )
        UpdIndex( nBlk1 );
    nCur = nBlk1;

    // wenn mehr als 50% "Luft" im Array ist, dann sollte man mal das
    // Compress aufrufen
    if( nBlock > ( nSize / ( MAXENTRY / 2 ) ) )
        Compress();

    CHECKIDX( ppInf, nBlock, nSize, nCur );
}


void BigPtrArray::Replace( ULONG idx, const ElementPtr& rElem)
{
    // weil die Funktion eben doch nicht const ist:
    DBG_ASSERT( idx < nSize, "Set: Index aussserhalb" );
    BigPtrArray* pThis = (BigPtrArray*) this;
    USHORT cur = Index2Block( idx );
    BlockInfo* p = ppInf[ cur ];
    pThis->nCur = cur;
    ((ElementPtr&)rElem)->nOffset = USHORT(idx - p->nStart);
    ((ElementPtr&)rElem)->pBlock = p;
    p->pData[ idx - p->nStart ] = rElem;
}


// Array komprimieren

USHORT BigPtrArray::Compress( short nMax )
{
    CHECKIDX( ppInf, nBlock, nSize, nCur );

    // Es wird von vorne nach hinten ueber das InfoBlock Array iteriert.
    // Wenn zwischen durch Block gel�scht werden, dann mussen alle
    // nachfolgenden verschoben werden. Dazu werden die Pointer pp und qq
    // benutzt; wobei pp das "alte" Array, qq das "neue" Array ist.
    BlockInfo** pp = ppInf, **qq = pp;
    BlockInfo* p;
    BlockInfo* pLast(0);				// letzter nicht voller Block
    USHORT nLast = 0;					// fehlende Elemente
    USHORT nBlkdel = 0;					// Anzahl der geloeschte Bloecke
    USHORT nFirstChgPos = USHRT_MAX;	// ab welcher Pos gab es die 1. Aenderung?

    // von Fuell-Prozenten auf uebrige Eintrage umrechnen
    nMax = MAXENTRY - (long) MAXENTRY * nMax / 100;

    for( USHORT cur = 0; cur < nBlock; ++cur )
    {
        p = *pp++;
        USHORT n = p->nElem;
        // Testen, ob der noch nicht volle Block so gelassen wird
        // dies ist der Fall, wenn der aktuelle Block gesplittet
        // werden muesste, der noch nicht volle Block aber bereits
        // ueber dem uebergebenen Break-Wert voll ist. In diesem
        // Fall wird von einer weiteren Fuellung (die ja wegen dem
        // zweifachen memmove() zeitaufwendig ist) abgesehen.
        if( nLast && ( n > nLast ) && ( nLast < nMax ) )
            nLast = 0;
        if( nLast )
        {
            if( USHRT_MAX == nFirstChgPos )
                nFirstChgPos = cur;

            // ein nicht voller Block vorhanden: auffuellen
            if( n > nLast )
                n = nLast;

            // Elemente uebertragen, vom akt. in den letzten
            ElementPtr* pElem = pLast->pData + pLast->nElem;
            ElementPtr* pFrom = p->pData;
            for( USHORT nCount = n, nOff = pLast->nElem;
                            nCount; --nCount, ++pElem )
                *pElem = *pFrom++,
                    (*pElem)->pBlock = pLast,
                    (*pElem)->nOffset = nOff++;

            // korrigieren
            pLast->nElem = pLast->nElem + n;
            nLast = nLast - n;
            p->nElem = p->nElem - n;

            // Ist der aktuelle Block dadurch leer geworden?
            if( !p->nElem )
            {
                // dann kann der entfernt werden
                delete[] p->pData;
                delete   p, p = 0;
                ++nBlkdel;
            }
            else
            {
                pElem = p->pData, pFrom = pElem + n;
                int nCount = p->nElem;
                while( nCount-- )
                {
                    *pElem = *pFrom++;
                    (*pElem)->nOffset = (*pElem)->nOffset - n;
                    ++pElem;
                }
            }
        }

        if( p )		// die Blockinfo wurde nicht geloescht
        {
            *qq++ = p;		// dann setze sie an die richtige neue Position

            // eventuell den letzten halbvollen Block festhalten
            if( !nLast && p->nElem < MAXENTRY )
            {
                pLast = p;
                nLast = MAXENTRY - p->nElem;
            }
        }
    }

    // Bloecke geloescht wurden, ggfs. das BlockInfo Array verkuerzen
    if( nBlkdel )
        BlockDel( nBlkdel );

    // Und neu durchindizieren
    p = ppInf[ 0 ];
    p->nEnd = p->nElem - 1;
    UpdIndex( 0 );

    if( nCur >= nFirstChgPos )
        nCur = 0;

    CHECKIDX( ppInf, nBlock, nSize, nCur );

    return nFirstChgPos;
}


