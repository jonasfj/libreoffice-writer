/*************************************************************************
 *
 *  $RCSfile: IDocumentStatistics.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 15:13:53 $
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

 #ifndef IDOCUMENTSTATISTICS_HXX_INCLUDED
 #define IDOCUMENTSTATISTICS_HXX_INCLUDED

 class SfxDocumentInfo;
 struct SwDocStat;

 /** Document statistics information
 */
 class IDocumentStatistics
 {
 public:

    /** Dokument info
        @return the current document information
    */
    virtual SfxDocumentInfo* GetInfo() = 0;

    /** Return a pointer to the document info.
        May also return NULL!
    */
    virtual const SfxDocumentInfo* GetpInfo() const = 0;

    /** setze ueber die DocShell in den entsp. Storage-Stream. Hier wird
        jetzt die DocInfo verwaltet. Fuer die Felder ist am Doc eine Kopie
        der Info, um einen schnellen Zugriff zu ermoeglichen.
        (impl. in docsh2.cxx)
    */
    virtual void SetInfo(const SfxDocumentInfo& rInfo) = 0;

    /** die DocInfo hat siche geaendert (Notify ueber die DocShell)
        stosse die entsp. Felder zum Updaten an.
    */
    virtual void DocInfoChgd(const SfxDocumentInfo& rInfo) = 0;

    /**	Dokument - Statistics
    */
    virtual const SwDocStat	&GetDocStat() const = 0;

    /**
    */
    virtual void SetDocStat(const SwDocStat& rStat) = 0;

    /**
    */
    virtual void UpdateDocStat(SwDocStat& rStat) = 0;

protected:
    virtual ~IDocumentStatistics() {};
 };

 #endif // IDOCUMENTSTATISTICS_HXX_INCLUDED
