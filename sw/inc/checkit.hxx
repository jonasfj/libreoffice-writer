/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: checkit.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:36:11 $
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

#ifndef _CHECKIT_HXX
#define _CHECKIT_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_I18N_XINPUTSEQUENCECHECKER_HPP_
#include <com/sun/star/i18n/XInputSequenceChecker.hpp>
#endif

/*************************************************************************
 * Input strings with length > MAX_SEQUENCE_CHECK_LEN are not checked.
 *************************************************************************/

#define MAX_SEQUENCE_CHECK_LEN 5

/*************************************************************************
 *                      class SwCheckIt
 *
 * Wrapper for the XInputSequenceChecker
 *************************************************************************/

class SwCheckIt
{
public:
    com::sun::star::uno::Reference < com::sun::star::i18n::XInputSequenceChecker > xCheck;

    SwCheckIt();
};

extern SwCheckIt* pCheckIt;

#endif

