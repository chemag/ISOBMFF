/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 DigiDNA - www.digidna.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @header      Error.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_ERROR_HPP
#define ISOBMFF_ERROR_HPP

#include <string>
#include <Macros.hpp>

namespace ISOBMFF
{
    enum class ErrorCode
    {
        Success = 0,
        InvalidFileStream,
        InvalidReadSize,
        InvalidSeekOffset,
        InsufficientData,
        BadNumericCast,
        CannotReadFile,
        NotISOMediaFile,
        InvalidBoxData,
        GenericError
    };

    class ISOBMFF_EXPORT Error
    {
        public:
            Error();
            Error( ErrorCode code );
            Error( ErrorCode code, const std::string & message );

            bool IsError() const;
            ErrorCode GetCode() const;
            std::string GetMessage() const;

            explicit operator bool() const;

        private:
            ErrorCode _code;
            std::string _message;
    };

    inline Error::Error() : _code( ErrorCode::Success ), _message( "" ) {}

    inline Error::Error( ErrorCode code ) : _code( code ), _message( "" ) {}

    inline Error::Error( ErrorCode code, const std::string & message ) : _code( code ), _message( message ) {}

    inline bool Error::IsError() const
    {
        return _code != ErrorCode::Success;
    }

    inline ErrorCode Error::GetCode() const
    {
        return _code;
    }

    inline std::string Error::GetMessage() const
    {
        return _message;
    }

    inline Error::operator bool() const
    {
        return IsError();
    }
}

#endif /* ISOBMFF_ERROR_HPP */