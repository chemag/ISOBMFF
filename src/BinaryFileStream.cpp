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
 * @file        BinaryFileStream.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <fstream>
#include <cmath>
#include <vector>
#include <BinaryFileStream.hpp>
#include <Casts.hpp>

#ifdef _WIN32
#include <WIN32.hpp>
#endif

namespace ISOBMFF
{
    class BinaryFileStream::IMPL
    {
        public:

            IMPL( const std::string & path );
            ~IMPL();

            std::ifstream _stream;
            std::string   _path;
            size_t        _size;
            size_t        _pos;
    };

    BinaryFileStream::BinaryFileStream( const std::string & path ):
        impl( std::make_unique< IMPL >( path ) )
    {}

    BinaryFileStream::~BinaryFileStream()
    {}

    Error BinaryFileStream::Read( uint8_t * buf, size_t size )
    {
        if( this->impl->_stream.is_open() == false )
        {
            return Error( ErrorCode::InvalidFileStream, "Invalid file stream" );
        }

        if( size > this->impl->_size - this->impl->_pos )
        {
            return Error( ErrorCode::InvalidReadSize, "Invalid read - Not enough data available" );
        }

        this->impl->_pos += size;

        Error err;
        std::streamsize stream_size;
        err = numeric_cast< std::streamsize >( stream_size, size );
        if( err ) return err;

        this->impl->_stream.read( reinterpret_cast< char * >( buf ), stream_size );

        return Error();
    }

    Error BinaryFileStream::Seek( std::streamoff offset, SeekDirection dir )
    {
        size_t pos;

        if( dir == SeekDirection::Begin )
        {
            if( offset < 0 )
            {
                return Error( ErrorCode::InvalidSeekOffset, "Invalid seek offset" );
            }

            Error err;
            err = numeric_cast< size_t >( pos, offset );
            if( err ) return err;
        }
        else if( dir == SeekDirection::End )
        {
            if( offset > 0 )
            {
                return Error( ErrorCode::InvalidSeekOffset, "Invalid seek offset" );
            }

            Error err;
            size_t abs_offset;
            err = numeric_cast< size_t >( abs_offset, std::abs( offset ) );
            if( err ) return err;

            pos = this->impl->_size - abs_offset;
        }
        else if( offset < 0 )
        {
            Error err;
            size_t abs_offset;
            err = numeric_cast< size_t >( abs_offset, std::abs( offset ) );
            if( err ) return err;

            pos = this->impl->_pos - abs_offset;
        }
        else
        {
            Error err;
            size_t offset_val;
            err = numeric_cast< size_t >( offset_val, offset );
            if( err ) return err;

            pos = this->impl->_pos + offset_val;
        }

        if( pos > this->impl->_size )
        {
            return Error( ErrorCode::InvalidSeekOffset, "Invalid seek offset" );
        }

        this->impl->_pos = pos;

        Error err;
        std::streamsize stream_pos;
        err = numeric_cast< std::streamsize >( stream_pos, pos );
        if( err ) return err;

        this->impl->_stream.seekg( stream_pos, std::ios_base::beg );

        return Error();
    }

    size_t BinaryFileStream::Tell() const
    {
        return this->impl->_pos;
    }

    BinaryFileStream::IMPL::IMPL( const std::string & path ):
        _path( path ),
        _size( 0 ),
        _pos(  0 )
    {
        #ifdef _WIN32
        this->_stream.open( ISOBMFF::StringToWideString( path ), std::ios::binary );
        #else
        this->_stream.open( path, std::ios::binary );
        #endif

        if( this->_stream.good() )
        {
            std::streamsize pos;

            this->_stream.seekg( 0, std::ios_base::end );

            pos         = this->_stream.tellg();

            Error err;
            err = numeric_cast< size_t >( this->_size, pos );

            this->_stream.seekg( 0, std::ios_base::beg );
        }
    }

    BinaryFileStream::IMPL::~IMPL()
    {
        if( this->_stream.is_open() )
        {
            this->_stream.close();
        }
    }
}