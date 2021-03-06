/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include "oox/xls/biffdetector.hxx"

#include <algorithm>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <comphelper/mediadescriptor.hxx>
#include <rtl/strbuf.hxx>
#include "oox/helper/binaryinputstream.hxx"
#include "oox/ole/olestorage.hxx"

namespace oox {
namespace xls {

// ============================================================================

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

using ::comphelper::MediaDescriptor;
using ::rtl::OStringBuffer;
using ::rtl::OUString;

// ============================================================================

Sequence< OUString > BiffDetector_getSupportedServiceNames()
{
    Sequence< OUString > aServiceNames( 1 );
    aServiceNames[ 0 ] = CREATE_OUSTRING( "com.sun.star.frame.ExtendedTypeDetection" );
    return aServiceNames;
}

OUString BiffDetector_getImplementationName()
{
    return CREATE_OUSTRING( "com.sun.star.comp.oox.xls.BiffDetector" );
}

Reference< XInterface > SAL_CALL BiffDetector_createInstance( const Reference< XComponentContext >& rxContext ) throw( Exception )
{
    return static_cast< ::cppu::OWeakObject* >( new BiffDetector( rxContext ) );
}

// ============================================================================

BiffDetector::BiffDetector( const Reference< XComponentContext >& rxContext ) throw( RuntimeException ) :
    mxContext( rxContext, UNO_SET_THROW )
{
}

BiffDetector::~BiffDetector()
{
}

/*static*/ BiffType BiffDetector::detectStreamBiffVersion( BinaryInputStream& rInStream )
{
    BiffType eBiff = BIFF_UNKNOWN;
    if( !rInStream.isEof() && rInStream.isSeekable() && (rInStream.size() > 4) )
    {
        sal_Int64 nOldPos = rInStream.tell();
        rInStream.seekToStart();
        sal_uInt16 nBofId, nBofSize;
        rInStream >> nBofId >> nBofSize;

        if( (4 <= nBofSize) && (nBofSize <= 16) && (rInStream.tell() + nBofSize <= rInStream.size()) )
        {
            switch( nBofId )
            {
                case BIFF2_ID_BOF:
                    eBiff = BIFF2;
                break;
                case BIFF3_ID_BOF:
                    eBiff = BIFF3;
                break;
                case BIFF4_ID_BOF:
                    eBiff = BIFF4;
                break;
                case BIFF5_ID_BOF:
                {
                    if( 6 <= nBofSize )
                    {
                        sal_uInt16 nVersion;
                        rInStream >> nVersion;
                        // #i23425# #i44031# #i62752# there are some *really* broken documents out there...
                        switch( nVersion & 0xFF00 )
                        {
                            case 0:                 eBiff = BIFF5;  break;  // #i44031# #i62752#
                            case BIFF_BOF_BIFF2:    eBiff = BIFF2;  break;
                            case BIFF_BOF_BIFF3:    eBiff = BIFF3;  break;
                            case BIFF_BOF_BIFF4:    eBiff = BIFF4;  break;
                            case BIFF_BOF_BIFF5:    eBiff = BIFF5;  break;
                            case BIFF_BOF_BIFF8:    eBiff = BIFF8;  break;
                            default:    OSL_ENSURE( false,
                                OStringBuffer( "lclDetectStreamBiffVersion - unknown BIFF version: 0x" ).
                                append( static_cast< sal_Int32 >( nVersion ), 16 ).getStr() );
                        }
                    }
                }
                break;
                // else do nothing, no BIFF stream
            }
        }
        rInStream.seek( nOldPos );
    }
    return eBiff;
}

/*static*/ BiffType BiffDetector::detectStorageBiffVersion( OUString& orWorkbookStreamName, const StorageRef& rxStorage )
{
    static const OUString saBookName = CREATE_OUSTRING( "Book" );
    static const OUString saWorkbookName = CREATE_OUSTRING( "Workbook" );

    BiffType eBiff = BIFF_UNKNOWN;
    if( rxStorage.get() )
    {
        if( rxStorage->isStorage() )
        {
            // try to open the "Book" stream
            BinaryXInputStream aBookStrm5( rxStorage->openInputStream( saBookName ), true );
            BiffType eBookStrm5Biff = detectStreamBiffVersion( aBookStrm5 );

            // try to open the "Workbook" stream
            BinaryXInputStream aBookStrm8( rxStorage->openInputStream( saWorkbookName ), true );
            BiffType eBookStrm8Biff = detectStreamBiffVersion( aBookStrm8 );

            // decide which stream to use
            if( (eBookStrm8Biff != BIFF_UNKNOWN) && ((eBookStrm5Biff == BIFF_UNKNOWN) || (eBookStrm8Biff > eBookStrm5Biff)) )
            {
                /*  Only "Workbook" stream exists; or both streams exist, and
                    "Workbook" has higher BIFF version than "Book" stream. */
                eBiff = eBookStrm8Biff;
                orWorkbookStreamName = saWorkbookName;
            }
            else if( eBookStrm5Biff != BIFF_UNKNOWN )
            {
                /*  Only "Book" stream exists; or both streams exist, and
                    "Book" has higher BIFF version than "Workbook" stream. */
                eBiff = eBookStrm5Biff;
                orWorkbookStreamName = saBookName;
            }
        }
        else
        {
            // no storage, try plain input stream from medium (even for BIFF5+)
            BinaryXInputStream aStrm( rxStorage->openInputStream( OUString() ), false );
            eBiff = detectStreamBiffVersion( aStrm );
            orWorkbookStreamName = OUString();
        }
    }

    return eBiff;
}

// com.sun.star.lang.XServiceInfo interface -----------------------------------

OUString SAL_CALL BiffDetector::getImplementationName() throw( RuntimeException )
{
    return BiffDetector_getImplementationName();
}

sal_Bool SAL_CALL BiffDetector::supportsService( const OUString& rService ) throw( RuntimeException )
{
    const Sequence< OUString > aServices = BiffDetector_getSupportedServiceNames();
    const OUString* pArray = aServices.getConstArray();
    const OUString* pArrayEnd = pArray + aServices.getLength();
    return ::std::find( pArray, pArrayEnd, rService ) != pArrayEnd;
}

Sequence< OUString > SAL_CALL BiffDetector::getSupportedServiceNames() throw( RuntimeException )
{
    return BiffDetector_getSupportedServiceNames();
}

// com.sun.star.document.XExtendedFilterDetect interface ----------------------

OUString SAL_CALL BiffDetector::detect( Sequence< PropertyValue >& rDescriptor ) throw( RuntimeException )
{
    OUString aTypeName;

    MediaDescriptor aDescriptor( rDescriptor );
    aDescriptor.addInputStream();

    Reference< XInputStream > xInStrm( aDescriptor[ MediaDescriptor::PROP_INPUTSTREAM() ], UNO_QUERY_THROW );
    StorageRef xStorage( new ::oox::ole::OleStorage( mxContext, xInStrm, true ) );

    OUString aWorkbookName;
    switch( detectStorageBiffVersion( aWorkbookName, xStorage ) )
    {
        case BIFF2:
        case BIFF3:
        case BIFF4: aTypeName = CREATE_OUSTRING( "calc_MS_Excel_40" );  break;
        case BIFF5: aTypeName = CREATE_OUSTRING( "calc_MS_Excel_95" );  break;
        case BIFF8: aTypeName = CREATE_OUSTRING( "calc_MS_Excel_97" );  break;
        default:;
    }

    return aTypeName;
}

// ============================================================================

} // namespace xls
} // namespace oox
