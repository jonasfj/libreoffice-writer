/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: needed_cast.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2005-11-16 13:54:03 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
#ifndef WW_NEEDED_CAST_HXX
#define WW_NEEDED_CAST_HXX

#ifndef WW_STATICASSERT_HXX
#   include "staticassert.hxx"
#endif

namespace ww
{
    template<typename Ret, typename Param> Ret checking_cast(Param in, Ret)
    {
        return static_cast<Ret>(in);
    }

    template<typename Ret> Ret checking_cast(Ret in, Ret)
    {
        StaticAssert(false, UnnecessaryCast);
        return in;
    }

    /*
     needed_cast is the same as static_cast except that there will be a compile
     time assert when NDEBUG is not defined and the in and out types are the
     same.  i.e. needed_cast catches unneccessary casts
    */
    template<typename Ret, typename Param> Ret needed_cast(Param in)
    {
        /*
         Massage a single argument and a ret value into two arguments to allow 
         a determination if the dest type is the same as the sourct type
        */
        return checking_cast(in, Ret());
    }
}
#endif
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
