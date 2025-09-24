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
 * @header      BinaryStream.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_BINARY_STREAM_HPP
#define ISOBMFF_BINARY_STREAM_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <type_traits>
#include <Casts.hpp>
#include <Macros.hpp>
#include <Matrix.hpp>
#include <Error.hpp>

namespace ISOBMFF
{
    class ISOBMFF_EXPORT BinaryStream
    {
        public:

            enum class SeekDirection
            {
                Current,
                Begin,
                End
            };

            virtual ~BinaryStream() = default;

            virtual Error  Read( uint8_t * buf, size_t size )               = 0;
            virtual size_t Tell()                                     const = 0;
            virtual Error  Seek( std::streamoff offset, SeekDirection dir ) = 0;

            bool   HasBytesAvailable();
            size_t AvailableBytes();

            Error Seek( std::streamoff offset );

            template< typename T, typename std::enable_if< std::is_integral< T >::value && std::is_unsigned< T >::value >::type * = nullptr >
            Error Seek( T offset )
            {
                Error err;
                std::streamoff off;
                err = numeric_cast< std::streamoff >( off, offset );
                if( err ) return err;
                return this->Seek( off );
            }

            template< typename T, typename std::enable_if< std::is_integral< T >::value && std::is_unsigned< T >::value >::type * = nullptr >
            Error Seek( T offset, SeekDirection dir )
            {
                Error err;
                std::streamoff off;
                err = numeric_cast< std::streamoff >( off, offset );
                if( err ) return err;
                return this->Seek( off, dir );
            }

            Error Get( uint8_t * buf, uint64_t pos, size_t length );

            Error Read( std::vector< uint8_t > & data, size_t size );
            Error ReadAllData( std::vector< uint8_t > & data );

            Error ReadUInt8( uint8_t & value );
            Error ReadInt8( int8_t & value );

            Error ReadUInt16( uint16_t & value );
            Error ReadBigEndianUInt16( uint16_t & value );
            Error ReadLittleEndianUInt16( uint16_t & value );

            Error ReadUInt32( uint32_t & value );
            Error ReadBigEndianUInt32( uint32_t & value );
            Error ReadLittleEndianUInt32( uint32_t & value );
            Error ReadBigEndianInt32( int32_t & value );

            Error ReadUInt64( uint64_t & value );
            Error ReadBigEndianUInt64( uint64_t & value );
            Error ReadLittleEndianUInt64( uint64_t & value );

            Error ReadBigEndianFixedPoint( float & value, unsigned int integerLength, unsigned int fractionalLength );
            Error ReadLittleEndianFixedPoint( float & value, unsigned int integerLength, unsigned int fractionalLength );

            Error ReadFourCC( std::string & value );
            Error ReadPascalString( std::string & value );
            Error ReadString( std::string & value, size_t length );
            Error ReadNULLTerminatedString( std::string & value );

            Error ReadMatrix( Matrix & value );
    };
}

#endif /* ISOBMFF_BINARY_STREAM_HPP */
