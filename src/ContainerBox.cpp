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
 * @file        ContainerBox.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <ContainerBox.hpp>
#include <Parser.hpp>
#include <BinaryDataStream.hpp>

namespace ISOBMFF
{
    class ContainerBox::IMPL
    {
        public:

            IMPL();
            IMPL( const IMPL & o );
            ~IMPL();

            std::vector< std::shared_ptr< Box > > _boxes;
    };

    ContainerBox::ContainerBox( const std::string & name ):
        Box( name ),
        impl( std::make_unique< IMPL >() )
    {}

    ContainerBox::ContainerBox( const ContainerBox & o ):
        Box( o ),
        impl( std::make_unique< IMPL >( *( o.impl ) ) )
    {}

    ContainerBox::ContainerBox( ContainerBox && o ) noexcept:
        Box( std::move( o ) ),
        impl( std::move( o.impl ) )
    {
        o.impl = nullptr;
    }

    ContainerBox::~ContainerBox()
    {}

    ContainerBox & ContainerBox::operator =( ContainerBox o )
    {
        Box::operator=( o );
        swap( *( this ), o );

        return *( this );
    }

    void swap( ContainerBox & o1, ContainerBox & o2 )
    {
        using std::swap;

        swap( static_cast< Box & >( o1 ), static_cast< Box & >( o2 ) );
        swap( o1.impl, o2.impl );
    }

    Error ContainerBox::ReadData( Parser & parser, BinaryStream & stream )
    {
        uint64_t               length;
        std::string            name;
        std::shared_ptr< Box > box;
        BinaryDataStream     * content;

        this->impl->_boxes.clear();

        while( stream.HasBytesAvailable() )
        {
            ( void )parser;

            Error err;
            uint32_t temp32;
            err = stream.ReadBigEndianUInt32( temp32 );
            if( err ) return err;
            length = temp32;

            if( length == 0 && !stream.HasBytesAvailable() )
            {
                break;
            }

            err = stream.ReadFourCC( name );
            if( err ) return err;

            content  = nullptr;

            if( length == 1 )
            {
                err = stream.ReadBigEndianUInt64( length );
                if( err ) return err;

                if
                (
                       length > ( std::numeric_limits< size_t >::max )()
                    || ( name == "mdat" && !parser.HasOption( Parser::Options::DoNotSkipMDATData ) )
                )
                {
                    err = stream.Seek( length - 16, BinaryStream::SeekDirection::Current );
                    if( err ) return err;
                }
                else
                {
                    std::vector< uint8_t > data;
                    err = stream.Read( data, static_cast< size_t >( length ) - 16 );
                    if( err ) return err;

                    content = new BinaryDataStream( data );
                }
            }
            else
            {
                if( name == "mdat" && !parser.HasOption( Parser::Options::DoNotSkipMDATData ) )
                {
                    err = stream.Seek( length - 8, BinaryStream::SeekDirection::Current );
                    if( err ) return err;
                }
                else
                {
                    std::vector< uint8_t > data;
                    err = stream.Read( data, static_cast< uint32_t >( length ) - 8 );
                    if( err ) return err;

                    content = new BinaryDataStream( data );
                }
            }

            box = parser.CreateBox( name );

            if( box != nullptr )
            {
                if( content )
                {
                    Error box_err = box->ReadData( parser, *content );
                    if( box_err )
                    {
                        delete( content );
                        std::cerr << "Error while reading box " << name << ": " << box_err.GetMessage() << std::endl;
                        return box_err;
                    }
                }

                this->AddBox( box );
            }
            if( content )
            {
                delete( content );
            }
        }

        return Error();
    }

    void ContainerBox::AddBox( std::shared_ptr< Box > box )
    {
        if( box != nullptr )
        {
            this->impl->_boxes.push_back( box );
        }
    }

    std::vector< std::shared_ptr< Box > > ContainerBox::GetBoxes() const
    {
        return this->impl->_boxes;
    }

    void ContainerBox::WriteDescription( std::ostream & os, std::size_t indentLevel ) const
    {
        Box::WriteDescription( os, indentLevel );
        Container::WriteBoxes( os, indentLevel );
    }

    ContainerBox::IMPL::IMPL()
    {}

    ContainerBox::IMPL::IMPL( const IMPL & o ):
        _boxes( o._boxes )
    {}

    ContainerBox::IMPL::~IMPL()
    {}
}
