//
//  Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_ENCODING_ERRORS_HPP_INCLUDED
#define NOWIDE_ENCODING_ERRORS_HPP_INCLUDED

#include <nowide/config.hpp>
#include <stdexcept>

namespace nowide {
    namespace conv {
        ///
        /// \addtogroup codepage
        ///
        /// @{

        ///
        /// \brief The excepton that is thrown in case of conversion error
        ///
        class conversion_error : public std::runtime_error {
        public:
            conversion_error() : std::runtime_error("Conversion failed") {}
        };

        ///
        /// enum that defines conversion policy
        ///
        enum method_type{
            skip            = 0,    ///< Skip illegal/unconvertable characters
            stop            = 1,    ///< Stop conversion and throw conversion_error
            default_method  = skip  ///< Default method - skip
        };


        /// @}

    } // conv

} // nowide

#endif

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

