
#ifndef AUTOBOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/pop_front_fwd.hpp>
#include <autoboost/mpl/aux_/traits_lambda_spec.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>

namespace autoboost { namespace mpl {

// no default implementation; the definition is needed to make MSVC happy

template< typename Tag >
struct pop_front_impl
{
    template< typename Sequence > struct apply
    // conservatively placed, but maybe should go outside surrounding
    // braces.
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300)
    {
        typedef int type;
    }
#endif
    ;
};

AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(1, pop_front_impl)

}}

#endif // AUTOBOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED
