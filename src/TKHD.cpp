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
 * @file        TKHD.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <TKHD.hpp>
#include <cstring>

namespace ISOBMFF
{
    class TKHD::IMPL
    {
        public:

            IMPL();
            IMPL( const IMPL & o );
            ~IMPL();

            uint64_t _creationTime;
            uint64_t _modificationTime;
            uint32_t _trackID;
            uint32_t _reserved1;
            uint64_t _duration;
            uint32_t _reserved2[ 2 ];
            uint16_t _layer;
            uint16_t _alternateGroup;
            uint16_t _volume;
            uint16_t _reserved3;
            Matrix   _matrix;
            float    _width;
            float    _height;
    };

    TKHD::TKHD():
        FullBox( "tkhd" ),
        impl( std::make_unique< IMPL >() )
    {}

    TKHD::TKHD( const TKHD & o ):
        FullBox( o ),
        impl( std::make_unique< IMPL >( *( o.impl ) ) )
    {}

    TKHD::TKHD( TKHD && o ) noexcept:
        FullBox( std::move( o ) ),
        impl( std::move( o.impl ) )
    {
        o.impl = nullptr;
    }

    TKHD::~TKHD()
    {}

    TKHD & TKHD::operator =( TKHD o )
    {
        FullBox::operator=( o );
        swap( *( this ), o );

        return *( this );
    }

    void swap( TKHD & o1, TKHD & o2 )
    {
        using std::swap;

        swap( static_cast< FullBox & >( o1 ), static_cast< FullBox & >( o2 ) );
        swap( o1.impl, o2.impl );
    }

    Error TKHD::ReadData( Parser & parser, BinaryStream & stream )
    {
        Error err;

        err = FullBox::ReadData( parser, stream );
        if( err ) return err;

        if( this->GetVersion() == 1 )
        {
            uint64_t temp;
            err = stream.ReadBigEndianUInt64( temp );
            if( err ) return err;
            this->SetCreationTime( temp );

            err = stream.ReadBigEndianUInt64( temp );
            if( err ) return err;
            this->SetModificationTime( temp );

            uint32_t temp32;
            err = stream.ReadBigEndianUInt32( temp32 );
            if( err ) return err;
            this->SetTrackID( temp32 );

            err = stream.ReadBigEndianUInt32( this->impl->_reserved1 );
            if( err ) return err;

            err = stream.ReadBigEndianUInt64( temp );
            if( err ) return err;
            this->SetDuration( temp );
        }
        else
        {
            uint32_t temp32;
            err = stream.ReadBigEndianUInt32( temp32 );
            if( err ) return err;
            this->SetCreationTime( temp32 );

            err = stream.ReadBigEndianUInt32( temp32 );
            if( err ) return err;
            this->SetModificationTime( temp32 );

            err = stream.ReadBigEndianUInt32( temp32 );
            if( err ) return err;
            this->SetTrackID( temp32 );

            err = stream.ReadBigEndianUInt32( this->impl->_reserved1 );
            if( err ) return err;

            err = stream.ReadBigEndianUInt32( temp32 );
            if( err ) return err;
            this->SetDuration( temp32 );
        }

        err = stream.ReadBigEndianUInt32( this->impl->_reserved2[ 0 ] );
        if( err ) return err;
        err = stream.ReadBigEndianUInt32( this->impl->_reserved2[ 1 ] );
        if( err ) return err;

        uint16_t temp16;
        err = stream.ReadBigEndianUInt16( temp16 );
        if( err ) return err;
        this->SetLayer( temp16 );

        err = stream.ReadBigEndianUInt16( temp16 );
        if( err ) return err;
        this->SetAlternateGroup( temp16 );

        err = stream.ReadBigEndianUInt16( temp16 );
        if( err ) return err;
        this->SetVolume( temp16 );

        err = stream.ReadBigEndianUInt16( this->impl->_reserved3 );
        if( err ) return err;

        Matrix tempMatrix;
        err = stream.ReadMatrix( tempMatrix );
        if( err ) return err;
        this->SetMatrix( tempMatrix );

        float tempFloat;
        err = stream.ReadBigEndianFixedPoint( tempFloat, 16, 16 );
        if( err ) return err;
        this->SetWidth( tempFloat );

        err = stream.ReadBigEndianFixedPoint( tempFloat, 16, 16 );
        if( err ) return err;
        this->SetHeight( tempFloat );
        return Error();
    }

    std::vector< std::pair< std::string, std::string > > TKHD::GetDisplayableProperties() const
    {
        auto props( FullBox::GetDisplayableProperties() );

        props.push_back( { "Creation time",     std::to_string( this->GetCreationTime() ) } );
        props.push_back( { "Modification time", std::to_string( this->GetModificationTime() ) } );
        props.push_back( { "Track ID",          std::to_string( this->GetTrackID() ) } );
        props.push_back( { "Duration",          std::to_string( this->GetDuration() ) } );
        props.push_back( { "Layer",             std::to_string( this->GetLayer() ) } );
        props.push_back( { "Alternate group",   std::to_string( this->GetAlternateGroup() ) } );
        props.push_back( { "Volume",            std::to_string( this->GetVolume() ) } );
        props.push_back( { "Matrix",            this->GetMatrix().ToString() } );
        props.push_back( { "Width",             std::to_string( this->GetWidth() ) } );
        props.push_back( { "Height",            std::to_string( this->GetHeight() ) } );

        return props;
    }

    uint64_t TKHD::GetCreationTime() const
    {
        return this->impl->_creationTime;
    }

    uint64_t TKHD::GetModificationTime() const
    {
        return this->impl->_modificationTime;
    }

    uint32_t TKHD::GetTrackID() const
    {
        return this->impl->_trackID;
    }

    uint64_t TKHD::GetDuration() const
    {
        return this->impl->_duration;
    }

    uint16_t TKHD::GetLayer() const
    {
        return this->impl->_layer;
    }

    uint16_t TKHD::GetAlternateGroup() const
    {
        return this->impl->_alternateGroup;
    }

    uint16_t TKHD::GetVolume() const
    {
        return this->impl->_volume;
    }

    Matrix TKHD::GetMatrix() const
    {
        return this->impl->_matrix;
    }

    float TKHD::GetWidth() const
    {
        return this->impl->_width;
    }

    float TKHD::GetHeight() const
    {
        return this->impl->_height;
    }

    void TKHD::SetCreationTime( uint64_t value )
    {
        this->impl->_creationTime = value;
    }

    void TKHD::SetModificationTime( uint64_t value )
    {
        this->impl->_modificationTime = value;
    }

    void TKHD::SetTrackID( uint32_t value )
    {
        this->impl->_trackID = value;
    }

    void TKHD::SetDuration( uint64_t value )
    {
        this->impl->_duration = value;
    }

    void TKHD::SetLayer( uint16_t value )
    {
        this->impl->_layer = value;
    }

    void TKHD::SetAlternateGroup( uint16_t value )
    {
        this->impl->_alternateGroup = value;
    }

    void TKHD::SetVolume( uint16_t value )
    {
        this->impl->_volume = value;
    }

    void TKHD::SetMatrix( Matrix value )
    {
        this->impl->_matrix = value;
    }

    void TKHD::SetWidth( float value )
    {
        this->impl->_width = value;
    }

    void TKHD::SetHeight( float value )
    {
        this->impl->_height = value;
    }

    TKHD::IMPL::IMPL():
        _creationTime( 0 ),
        _modificationTime( 0 ),
        _trackID( 0 ),
        _reserved1( 0 ),
        _duration( 0 ),
        _layer( 0 ),
        _alternateGroup( 0 ),
        _volume( 0 ),
        _reserved3( 0 ),
        _width( 0 ),
        _height( 0 )
    {
        memset( this->_reserved2, 0, sizeof( this->_reserved2 ) );
    }

    TKHD::IMPL::IMPL( const IMPL & o ):
        _creationTime( o._creationTime ),
        _modificationTime( o._modificationTime ),
        _trackID( o._trackID ),
        _reserved1( o._reserved1 ),
        _duration( o._duration ),
        _layer( o._layer ),
        _alternateGroup( o._alternateGroup ),
        _volume( o._volume ),
        _reserved3( o._reserved3 ),
        _matrix( o._matrix ),
        _width( o._width ),
        _height( o._height )
    {
        memcpy( this->_reserved2, o._reserved2, sizeof( this->_reserved2 ) );
    }

    TKHD::IMPL::~IMPL()
    {}
}
