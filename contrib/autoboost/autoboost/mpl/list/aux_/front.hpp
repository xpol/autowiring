
#ifndef AUTOBOOST_MPL_LIST_AUX_FRONT_HPP_INCLUDED
#define AUTOBOOST_MPL_LIST_AUX_FRONT_HPP_INCLUDED

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

#include <autoboost/mpl/front_fwd.hpp>
#include <autoboost/mpl/list/aux_/tag.hpp>

namespace autoboost { namespace mpl {

template<>
struct front_impl< aux::list_tag >
{
    template< typename List > struct apply
    {
        typedef typename List::item type;
    };
};

}}

#endif // AUTOBOOST_MPL_LIST_AUX_FRONT_HPP_INCLUDED