/*************************************************************************
 *
 *  $RCSfile: dbtablepreviewdialog.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-20 13:12:05 $
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
#ifndef _DBTABLEPREVIEWDIALOG_HXX
#define _DBTABLEPREVIEWDIALOG_HXX

#ifndef _BASEDLGS_HXX 
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _STDCTRL_HXX 
#include <svtools/stdctrl.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

namespace com{ namespace sun{ namespace star{ 
    namespace beans{  struct PropertyValue; }
    namespace frame{ class XFrame;     }
    }}}


/*-- 08.04.2004 14:04:29---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwDBTablePreviewDialog : public SfxModalDialog
{
    FixedInfo       m_aDescriptionFI;
    Window*         m_pBeamerWIN;
    OKButton        m_aOK;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >         m_xFrame;
public: 
    SwDBTablePreviewDialog(Window* pParent,
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rValues  );
    ~SwDBTablePreviewDialog();
    
};
#endif

