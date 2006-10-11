/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: IDocumentRedlineAccess.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-11 08:44:04 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

 #ifndef IDOCUMENTREDLINE_HXX_INCLUDED
 #define IDOCUMENTREDLINE_HXX_INCLUDED

 #ifndef _SAL_TYPES_H_
 #include <sal/types.h>
 #endif
 #ifndef _SOLAR_H
 #include <tools/solar.h>
 #endif

 #include <limits.h> // USHRT_MAX

 #ifndef _COM_SUN_STAR_SEQUENCE_HXX_
 #include <com/sun/star/uno/Sequence.hxx>
 #endif

 class SwRedline;
 class SwRedlineTbl;
 class SwPaM;
 struct SwPosition;
 class SwStartNode;
 class SwNode;
 class String;

 /** IDocumentRedlineAccess
 */
 class IDocumentRedlineAccess
 {
 public:
   
   enum RedlineMode_t
   { 
       REDLINE_NONE = 0, // no RedlineMode
       REDLINE_ON = 0x01,// RedlineMode on
       REDLINE_IGNORE = 0x02,// ignore Redlines
       REDLINE_SHOW_INSERT = 0x10,// show all inserts
       REDLINE_SHOW_DELETE = 0x20,// show all delets
       REDLINE_SHOW_MASK = REDLINE_SHOW_INSERT | REDLINE_SHOW_DELETE,

       // fuer die interne Verwaltung:
       // die originalen Redlines inclusive des Contents entfernen
       // (ClipBoard/Textbausteine)
       REDLINE_DELETE_REDLINES = 0x100,
       // beim Loeschen innerhalb ein RedlineObjectes, waehrend des Appends,
       // das DeleteRedline ignorieren
       REDLINE_IGNOREDELETE_REDLINES = 0x200,
       // don't combine any readlines. This flags is may only used in the Undo.
      REDLINE_DONTCOMBINE_REDLINES = 0x400
   };

   enum RedlineType_t
   {
     // die RedlineTypen gehen von 0 bis 127
     REDLINE_INSERT = 0x0,// Inhalt wurde eingefuegt
     REDLINE_DELETE = 0x1,// Inhalt wurde geloescht
     REDLINE_FORMAT = 0x2,// Attributierung wurde angewendet
     REDLINE_TABLE = 0x3,// TabellenStruktur wurde veraendert
     REDLINE_FMTCOLL = 0x4,// FormatVorlage wurde veraendert (Autoformat!)
     
     // ab 128 koennen Flags hineinverodert werden
     REDLINE_NO_FLAG_MASK = 0x7F,
     REDLINE_FLAG_MASK = 0xFF80,
     REDLINE_FORM_AUTOFMT = 0x80// kann als Flag im RedlineType stehen
   };

    // Static helper functions
public:
    static int IsShowChanges(const USHORT eM)
    { return (REDLINE_SHOW_INSERT | REDLINE_SHOW_DELETE) == (eM & REDLINE_SHOW_MASK); }

    static int IsHideChanges(const USHORT eM)
    { return REDLINE_SHOW_INSERT == (eM & REDLINE_SHOW_MASK); }

    static int IsShowOriginal(const USHORT eM)
    { return REDLINE_SHOW_DELETE == (eM & REDLINE_SHOW_MASK); }

    static int IsRedlineOn(const USHORT eM)
    { return REDLINE_ON == (eM & (REDLINE_ON | REDLINE_IGNORE )); }

 public:

    /*************************************************
        Query
    *************************************************/

    /** Query the currently set redline mode

        @returns
        the currently set redline mode
    */
     virtual RedlineMode_t GetRedlineMode() const = 0;

    /** Set a new redline mode.

        @param eMode
        [in] the new redline mode.
    */
    virtual void SetRedlineMode_intern(/*[in]*/RedlineMode_t eMode) = 0;

    /** Set a new redline mode.

        @param eMode
        [in] the new redline mode.
    */
    virtual void SetRedlineMode(/*[in]*/RedlineMode_t eMode) = 0;

    /** Query if redlining is on.

        @returns
        </TRUE> if redlining is on </FALSE> otherwise
    */
    virtual bool IsRedlineOn() const = 0;

    /**
    */
    virtual bool IsIgnoreRedline() const = 0;

    /**
    */
    virtual const SwRedlineTbl& GetRedlineTbl() const = 0;


   /*
    */
   virtual bool IsInRedlines(const SwNode& rNode) const = 0;

    /***************************************************
        Manipulation
     ***************************************************/

    /** Append a new redline

        @param pPtr

        @param bCallDelete

        @returns
    */
    virtual bool AppendRedline(/*[in]*/SwRedline* pPtr, /*[in]*/bool bCallDelete) = 0;

    /**
    */
    virtual bool SplitRedline(/*[in]*/const SwPaM& rPam) = 0;

    /**
    */
    virtual bool DeleteRedline(
        /*[in]*/const SwPaM& rPam,
        /*[in]*/bool bSaveInUndo,
        /*[in]*/sal_uInt16 nDelType) = 0;

    /**
    */
    virtual bool DeleteRedline(
        /*[in]*/const SwStartNode& rSection,
        /*[in]*/bool bSaveInUndo,
        /*[in]*/sal_uInt16 nDelType) = 0;

    /**
    */
    virtual sal_uInt16 GetRedlinePos(
        /*[in]*/const SwNode& rNode,
        /*[in]*/sal_uInt16 nType) const = 0;

    virtual void CompressRedlines() = 0;

    /**
    */
    virtual const SwRedline* GetRedline(
        /*[in]*/const SwPosition& rPos,
        /*[in]*/sal_uInt16* pFndPos) const = 0;

    /**
    */
    virtual bool IsRedlineMove() const = 0;

    /**
    */
    virtual void SetRedlineMove(/*[in]*/bool bFlag) = 0;

    /**
    */
    virtual bool AcceptRedline(/*[in]*/sal_uInt16 nPos, /*[in]*/bool bCallDelete) = 0;

    /**
    */
    virtual bool AcceptRedline(/*[in]*/const SwPaM& rPam, /*[in]*/bool bCallDelete) = 0;

    /**
    */
    virtual bool RejectRedline(/*[in]*/sal_uInt16 nPos, /*[in]*/bool bCallDelete) = 0;

    /**
    */
    virtual bool RejectRedline(/*[in]*/const SwPaM& rPam, /*[in]*/bool bCallDelete) = 0;

    /**
    */
    virtual const SwRedline* SelNextRedline(/*[in]*/SwPaM& rPam) const = 0;

    /**
    */
    virtual const SwRedline* SelPrevRedline(/*[in]*/SwPaM& rPam) const = 0;

    // alle Redline invalidieren, die Darstellung hat sich geaendert
    virtual void UpdateRedlineAttr() = 0;

    // legt gegebenenfalls einen neuen Author an
    virtual sal_uInt16 GetRedlineAuthor() = 0;

    // fuer die Reader usw. - neuen Author in die Tabelle eintragen
    virtual sal_uInt16 InsertRedlineAuthor(const String& rAuthor) = 0;

    // Kommentar am Redline an der Position setzen
    virtual bool SetRedlineComment(
        /*[in]*/const SwPaM& rPam,
        /*[in]*/const String& rComment) = 0;

    /**
    */
    virtual const ::com::sun::star::uno::Sequence <sal_Int8>& GetRedlinePassword() const = 0;

    /**
    */
    virtual void SetRedlinePassword(
        /*[in]*/const ::com::sun::star::uno::Sequence <sal_Int8>& rNewPassword) = 0;

 protected:
     virtual ~IDocumentRedlineAccess() {};
 };

 #endif
