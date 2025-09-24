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
 * @file        BinaryStream.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 * @author      Boris Conforty - www.digidna.net
 */

#include <fstream>
#include <cmath>
#include <BinaryStream.hpp>

namespace ISOBMFF
{
    bool BinaryStream::HasBytesAvailable()
    {
        return this->AvailableBytes() > 0;
    }

    size_t BinaryStream::AvailableBytes()
    {
        size_t cur( this->Tell() );
        size_t pos;

        this->Seek( 0, SeekDirection::End );

        pos = this->Tell();

        Error err;
        std::streamoff offset;
        err = numeric_cast< std::streamoff >( offset, cur );
        if( !err )
        {
            this->Seek( offset, SeekDirection::Begin );
        }

        return pos - cur;
    }

    Error BinaryStream::Seek( std::streamoff offset )
    {
        return this->Seek( offset, SeekDirection::Current );
    }

    Error BinaryStream::Get( uint8_t * buf, uint64_t pos, size_t length )
    {
        size_t cur = this->Tell();

        Error err = this->Seek( pos, SeekDirection::Current );
        if( err ) return err;

        err = this->Read( buf, length );
        if( err ) return err;

        return this->Seek( cur, SeekDirection::Begin );
    }

    Error BinaryStream::Read( std::vector< uint8_t > & data, size_t size )
    {
        if (size > AvailableBytes())
        {
            data = std::vector< uint8_t >();
            return Error( ErrorCode::InsufficientData, "Insufficient data available for read" );
        }

        data = std::vector< uint8_t >( size, 0 );

        if( size > 0 )
        {
            Error err = this->Read( &( data[ 0 ] ), size );
            if( err )
            {
                data = std::vector< uint8_t >();
                return err;
            }
        }

        return Error();
    }

    Error BinaryStream::ReadAllData( std::vector< uint8_t > & data )
    {
        return this->Read( data, this->AvailableBytes() );
    }

    Error BinaryStream::ReadUInt8( uint8_t & value )
    {
        Error err = this->Read( reinterpret_cast< uint8_t * >( &value ), 1 );
        if( err ) value = 0;
        return err;
    }

    Error BinaryStream::ReadInt8( int8_t & value )
    {
        Error err = this->Read( reinterpret_cast< uint8_t * >( &value ), 1 );
        if( err ) value = 0;
        return err;
    }

    Error BinaryStream::ReadUInt16( uint16_t & value )
    {
        Error err = this->Read( reinterpret_cast< uint8_t * >( &value ), 2 );
        if( err ) value = 0;
        return err;
    }

    Error BinaryStream::ReadBigEndianUInt16( uint16_t & value )
    {
        uint8_t  c[ 2 ] = { 0, 0 };
        uint16_t n1;
        uint16_t n2;

        Error err = this->Read( reinterpret_cast< uint8_t * >( c ), 2 );
        if( err )
        {
            value = 0;
            return err;
        }

        numeric_cast< uint16_t >( n1, c[ 0 ] );
        numeric_cast< uint16_t >( n2, c[ 1 ] );

        uint16_t tmp;
        numeric_cast< uint16_t >( tmp, n1 << 8 );
        value = tmp | n2;

        return Error();
    }

    Error BinaryStream::ReadLittleEndianUInt16( uint16_t & value )
    {
        uint8_t  c[ 2 ] = { 0, 0 };
        uint16_t n1;
        uint16_t n2;

        Error err = this->Read( reinterpret_cast< uint8_t * >( c ), 2 );
        if( err )
        {
            value = 0;
            return err;
        }

        numeric_cast< uint16_t >( n1, c[ 1 ] );
        numeric_cast< uint16_t >( n2, c[ 0 ] );

        uint16_t tmp;
        numeric_cast< uint16_t >( tmp, n1 << 8 );
        value = tmp | n2;

        return Error();
    }

    Error BinaryStream::ReadUInt32( uint32_t & value )
    {
        Error err = this->Read( reinterpret_cast< uint8_t * >( &value ), 4 );
        if( err ) value = 0;
        return err;
    }

    Error BinaryStream::ReadBigEndianUInt32( uint32_t & value )
    {
        uint8_t  c[ 4 ] = { 0, 0, 0, 0 };
        uint32_t n1;
        uint32_t n2;
        uint32_t n3;
        uint32_t n4;

        Error err = this->Read( reinterpret_cast< uint8_t * >( c ), 4 );
        if( err )
        {
            value = 0;
            return err;
        }

        numeric_cast< uint32_t >( n1, c[ 0 ] );
        numeric_cast< uint32_t >( n2, c[ 1 ] );
        numeric_cast< uint32_t >( n3, c[ 2 ] );
        numeric_cast< uint32_t >( n4, c[ 3 ] );

        uint32_t tmp1, tmp2, tmp3;
        numeric_cast< uint32_t >( tmp1, n1 << 24 );
        numeric_cast< uint32_t >( tmp2, n2 << 16 );
        numeric_cast< uint32_t >( tmp3, n3 << 8 );
        value  = tmp1 | tmp2 | tmp3 | n4;

        return Error();
    }

    Error BinaryStream::ReadBigEndianInt32( int32_t & value )
    {
        uint32_t uint_value;
        Error err = ReadBigEndianUInt32( uint_value );
        if( err )
        {
            value = 0;
            return err;
        }
        int32_t sign_bit = (uint_value & 0x80000000) ? -1 : 0;
        int32_t int_value = (int32_t)(uint_value & 0x7FFFFFFF);
        if (sign_bit == -1) {
            int_value = -int_value;
        }
        value = int_value;
        return Error();
    }

    Error BinaryStream::ReadLittleEndianUInt32( uint32_t & value )
    {
        uint8_t  c[ 4 ] = { 0, 0, 0, 0 };
        uint32_t n1;
        uint32_t n2;
        uint32_t n3;
        uint32_t n4;

        Error err = this->Read( reinterpret_cast< uint8_t * >( c ), 4 );
        if( err )
        {
            value = 0;
            return err;
        }

        numeric_cast< uint32_t >( n1, c[ 3 ] );
        numeric_cast< uint32_t >( n2, c[ 2 ] );
        numeric_cast< uint32_t >( n3, c[ 1 ] );
        numeric_cast< uint32_t >( n4, c[ 0 ] );

        uint32_t tmp1, tmp2, tmp3;
        numeric_cast< uint32_t >( tmp1, n1 << 24 );
        numeric_cast< uint32_t >( tmp2, n2 << 16 );
        numeric_cast< uint32_t >( tmp3, n3 << 8 );
        value  = tmp1 | tmp2 | tmp3 | n4;

        return Error();
    }

    Error BinaryStream::ReadUInt64( uint64_t & value )
    {
        Error err = this->Read( reinterpret_cast< uint8_t * >( &value ), 8 );
        if( err ) value = 0;
        return err;
    }

    Error BinaryStream::ReadBigEndianUInt64( uint64_t & value )
    {
        uint8_t  c[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        uint64_t n1;
        uint64_t n2;
        uint64_t n3;
        uint64_t n4;
        uint64_t n5;
        uint64_t n6;
        uint64_t n7;
        uint64_t n8;

        Error err = this->Read( reinterpret_cast< uint8_t * >( c ), 8 );
        if( err )
        {
            value = 0;
            return err;
        }

        numeric_cast< uint64_t >( n1, c[ 0 ] );
        numeric_cast< uint64_t >( n2, c[ 1 ] );
        numeric_cast< uint64_t >( n3, c[ 2 ] );
        numeric_cast< uint64_t >( n4, c[ 3 ] );
        numeric_cast< uint64_t >( n5, c[ 4 ] );
        numeric_cast< uint64_t >( n6, c[ 5 ] );
        numeric_cast< uint64_t >( n7, c[ 6 ] );
        numeric_cast< uint64_t >( n8, c[ 7 ] );

        uint64_t tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        numeric_cast< uint64_t >( tmp1, n1 << 56 );
        numeric_cast< uint64_t >( tmp2, n2 << 48 );
        numeric_cast< uint64_t >( tmp3, n3 << 40 );
        numeric_cast< uint64_t >( tmp4, n4 << 32 );
        numeric_cast< uint64_t >( tmp5, n5 << 24 );
        numeric_cast< uint64_t >( tmp6, n6 << 16 );
        numeric_cast< uint64_t >( tmp7, n7 << 8 );
        value  = tmp1 | tmp2 | tmp3 | tmp4 | tmp5 | tmp6 | tmp7 | n8;

        return Error();
    }

    Error BinaryStream::ReadLittleEndianUInt64( uint64_t & value )
    {
        uint8_t  c[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        uint64_t n1;
        uint64_t n2;
        uint64_t n3;
        uint64_t n4;
        uint64_t n5;
        uint64_t n6;
        uint64_t n7;
        uint64_t n8;

        Error err = this->Read( reinterpret_cast< uint8_t * >( c ), 8 );
        if( err )
        {
            value = 0;
            return err;
        }

        numeric_cast< uint64_t >( n1, c[ 7 ] );
        numeric_cast< uint64_t >( n2, c[ 6 ] );
        numeric_cast< uint64_t >( n3, c[ 5 ] );
        numeric_cast< uint64_t >( n4, c[ 4 ] );
        numeric_cast< uint64_t >( n5, c[ 3 ] );
        numeric_cast< uint64_t >( n6, c[ 2 ] );
        numeric_cast< uint64_t >( n7, c[ 1 ] );
        numeric_cast< uint64_t >( n8, c[ 0 ] );

        uint64_t tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        numeric_cast< uint64_t >( tmp1, n1 << 56 );
        numeric_cast< uint64_t >( tmp2, n2 << 48 );
        numeric_cast< uint64_t >( tmp3, n3 << 40 );
        numeric_cast< uint64_t >( tmp4, n4 << 32 );
        numeric_cast< uint64_t >( tmp5, n5 << 24 );
        numeric_cast< uint64_t >( tmp6, n6 << 16 );
        numeric_cast< uint64_t >( tmp7, n7 << 8 );
        value  = tmp1 | tmp2 | tmp3 | tmp4 | tmp5 | tmp6 | tmp7 | n8;

        return Error();
    }

    Error BinaryStream::ReadBigEndianFixedPoint( float & value, unsigned int integerLength, unsigned int fractionalLength )
    {
        uint32_t     n;
        unsigned int integer;
        unsigned int fractionalMask;
        float        fractional;
        Error        err;

        if( integerLength + fractionalLength == 16 )
        {
            uint16_t n16;
            err = this->ReadBigEndianUInt16( n16 );
            if( err )
            {
                value = 0.0f;
                return err;
            }
            n = n16;
        }
        else
        {
            err = this->ReadBigEndianUInt32( n );
            if( err )
            {
                value = 0.0f;
                return err;
            }
        }

        integer        = n >> fractionalLength;
        fractionalMask = static_cast< unsigned int >( pow( 2, fractionalLength ) - 1 );
        fractional     = static_cast< float >( n & fractionalMask ) / static_cast< float >( 1 << fractionalLength );

        value = static_cast< float >( integer ) + fractional;
        return Error();
    }

    Error BinaryStream::ReadLittleEndianFixedPoint( float & value, unsigned int integerLength, unsigned int fractionalLength )
    {
        uint32_t     n;
        unsigned int integer;
        unsigned int fractionalMask;
        float        fractional;
        Error        err;

        if( integerLength + fractionalLength == 16 )
        {
            uint16_t n16;
            err = this->ReadLittleEndianUInt16( n16 );
            if( err )
            {
                value = 0.0f;
                return err;
            }
            n = n16;
        }
        else
        {
            err = this->ReadLittleEndianUInt32( n );
            if( err )
            {
                value = 0.0f;
                return err;
            }
        }

        integer        = n >> fractionalLength;
        fractionalMask = static_cast< unsigned int >( pow( 2, fractionalLength ) - 1 );
        fractional     = static_cast< float >( n & fractionalMask ) / static_cast< float >( 1 << fractionalLength );

        value = static_cast< float >( integer ) + fractional;
        return Error();
    }

    Error BinaryStream::ReadFourCC( std::string & value )
    {
        uint8_t s[ 4 ];

        Error err = this->Read( s, 4 );
        if( err )
        {
            value = "";
            return err;
        }

        value = std::string( reinterpret_cast< char * >( s ), 4 );
        return Error();
    }

    Error BinaryStream::ReadPascalString( std::string & value )
    {
        uint8_t length;
        Error   err;

        err = this->ReadUInt8( length );
        if( err )
        {
            value = "";
            return err;
        }

        if( length == 0 )
        {
            value = "";
            return Error();
        }

        value = std::string( length, ' ' );

        err = this->Read( reinterpret_cast< uint8_t * >( &( value[ 0 ] ) ), length );
        if( err )
        {
            value = "";
            return err;
        }

        return Error();
    }

    Error BinaryStream::ReadString( std::string & value, size_t length )
    {
        std::vector< char > cp( length + 1, 0 );

        Error err = this->Read( reinterpret_cast< uint8_t * >( &( cp[ 0 ] ) ), length );
        if( err )
        {
            value = "";
            return err;
        }

        value = &( cp[ 0 ] );
        return Error();
    }

    Error BinaryStream::ReadNULLTerminatedString( std::string & value )
    {
        char c;
        value = "";

        while( 1 )
        {
            c = 0;

            Error err = this->Read( reinterpret_cast< uint8_t * >( &c ), 1 );
            if( err )
            {
                return err;
            }

            if( c == 0 )
            {
                break;
            }

            value.append( 1, c );
        }

        return Error();
    }

    Error BinaryStream::ReadMatrix( Matrix & value )
    {
        uint32_t a, b, c, d, tx, ty, u, v, w;
        Error err;

        if( ( err = this->ReadBigEndianUInt32( a ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( b ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( u ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( c ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( d ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( v ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( tx ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( ty ) ) ) return err;
        if( ( err = this->ReadBigEndianUInt32( w ) ) ) return err;

        value = Matrix( a, b, u, c, d, v, tx, ty, w );
        return Error();
    }
}