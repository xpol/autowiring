//
// version.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_VERSION_HPP
#define AUTOBOOST_ASIO_VERSION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

// AUTOBOOST_ASIO_VERSION % 100 is the sub-minor version
// AUTOBOOST_ASIO_VERSION / 100 % 1000 is the minor version
// AUTOBOOST_ASIO_VERSION / 100000 is the major version
#define AUTOBOOST_ASIO_VERSION 101005 // 1.10.5

#endif // AUTOBOOST_ASIO_VERSION_HPP