//  Boost string_algo library trim.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef AUTOBOOST_STRING_TRIM_DETAIL_HPP
#define AUTOBOOST_STRING_TRIM_DETAIL_HPP

#include <autoboost/algorithm/string/config.hpp>
#include <autoboost/detail/iterator.hpp>

namespace autoboost {
    namespace algorithm {
        namespace detail {

//  trim iterator helper -----------------------------------------------//

            template< typename ForwardIteratorT, typename PredicateT >
            inline ForwardIteratorT trim_end_iter_select(
                ForwardIteratorT InBegin,
                ForwardIteratorT InEnd,
                PredicateT IsSpace,
                std::forward_iterator_tag )
            {
                ForwardIteratorT TrimIt=InBegin;

                for( ForwardIteratorT It=InBegin; It!=InEnd; ++It )
                {
                    if ( !IsSpace(*It) )
                    {
                        TrimIt=It;
                        ++TrimIt;
                    }
                }

                return TrimIt;
            }

            template< typename ForwardIteratorT, typename PredicateT >
            inline ForwardIteratorT trim_end_iter_select(
                ForwardIteratorT InBegin,
                ForwardIteratorT InEnd,
                PredicateT IsSpace,
                std::bidirectional_iterator_tag )
            {
                for( ForwardIteratorT It=InEnd; It!=InBegin;  )
                {
                    if ( !IsSpace(*(--It)) )
                        return ++It;
                }

                return InBegin;
            }
   // Search for first non matching character from the beginning of the sequence
            template< typename ForwardIteratorT, typename PredicateT >
            inline ForwardIteratorT trim_begin(
                ForwardIteratorT InBegin,
                ForwardIteratorT InEnd,
                PredicateT IsSpace )
            {
                ForwardIteratorT It=InBegin;
                for(; It!=InEnd; ++It )
                {
                    if (!IsSpace(*It))
                        return It;
                }

                return It;
            }

            // Search for first non matching character from the end of the sequence
            template< typename ForwardIteratorT, typename PredicateT >
            inline ForwardIteratorT trim_end(
                ForwardIteratorT InBegin,
                ForwardIteratorT InEnd,
                PredicateT IsSpace )
            {
                typedef AUTOBOOST_STRING_TYPENAME autoboost::detail::
                    iterator_traits<ForwardIteratorT>::iterator_category category;

                return ::autoboost::algorithm::detail::trim_end_iter_select( InBegin, InEnd, IsSpace, category() );
            }


        } // namespace detail
    } // namespace algorithm
} // namespace autoboost


#endif  // AUTOBOOST_STRING_TRIM_DETAIL_HPP
