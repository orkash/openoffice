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



package org.openoffice.xmerge.converter.xml.sxc.pexcel.records;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.openoffice.xmerge.util.Debug;
import org.openoffice.xmerge.util.EndianConverter;
import org.openoffice.xmerge.converter.xml.sxc.pexcel.PocketExcelConstants;

/**
 * ColInfo describes the formatting for a column 
 *
 */
public class ColInfo implements BIFFRecord {

    private byte[] colFirst	= new byte[2];	// first column this formatting applies to
    private byte[] colLast	= new byte[2];	// last column this formatting applies to
    private byte[] colDX	= new byte[2];	// column width
    private byte[] ixfe		= new byte[2];	// index for formatting
    private byte   grbit;					// options flags 
	private float  scale = (float) 2.5;		// 1.798;
    
	/**
 	 * Constructs a pocket Excel Document from the
 	 * <code>InputStream</code> and assigns it the document name passed in
 	 *
 	 * @param	colFirst	the first column this formatting applies to	
 	 * @param	colLast		last column this formatting applies to	
 	 * @param	coldx		column width 	
 	 * @param	grbit		options flags	
 	 */
    public ColInfo(int colFirst, int colLast, int colDX, int ixfe) {
		this.colFirst	= EndianConverter.writeShort((short)colFirst);
		this.colLast	= EndianConverter.writeShort((short)colLast);
		colDX *= scale;
		this.colDX		= EndianConverter.writeShort((short)colDX);
		this.ixfe		= EndianConverter.writeShort((short)ixfe);
		this.grbit		= 0x00;
    }

	/**
	 * Construct a ColInfo from the InputStream
	 *
	 * @param is the <code>Inputstream</code> to read from
	 */
    public ColInfo(InputStream is) throws IOException {
		read(is);
    }

	/**
	 * Reads ColInfo record from the InputStream
	 *
	 * @param input the InputStream to read from 
	 * @return the number of bytes read
	 */
    public int read(InputStream input) throws IOException {

        int numOfBytesRead	= input.read(colFirst);
        numOfBytesRead 		+= input.read(colLast);
        numOfBytesRead		+= input.read(colDX);
		short scaledDX = (short) (EndianConverter.readShort(colDX) / scale);
		colDX = EndianConverter.writeShort(scaledDX);
        numOfBytesRead		+= input.read(ixfe);
        grbit				= (byte) input.read();
        numOfBytesRead		++;
        
        Debug.log(Debug.TRACE,"\tcolFirst : "+ EndianConverter.readShort(colFirst) + 
                            " colLast : " + EndianConverter.readShort(colLast) +
                            " colDX : " + EndianConverter.readShort(colDX) +
                            " ixfe : " + EndianConverter.readShort(ixfe) +
                            " grbit : " + grbit);
        
        return numOfBytesRead;
    }
	
	/**
	 * Get the hex code for this particular <code>BIFFRecord</code> 
	 *
	 * @return the hex code for <code>ColInfo</code> 
	 */
    public short getBiffType() {
        return PocketExcelConstants.COLINFO;
	}
	/**
	 * Get the width of this column 
	 *
	 * @return the width of this column 
	 */
    public short getColWidth() {
        return EndianConverter.readShort(colDX);
	}	
	
	/**
	 * Get the hex code for this particular <code>BIFFRecord</code> 
	 *
	 * @return the hex code for <code>ColInfo</code> 
	 */
    public short getFirst() {
        return EndianConverter.readShort(colFirst);
	}
	
	/**
	 * Get the hex code for this particular <code>BIFFRecord</code> 
	 *
	 * @return the hex code for <code>ColInfo</code> 
	 */
    public short getLast() {
        return EndianConverter.readShort(colLast);
	}

	/**
	 * Writes a ColInfo to the specified <code>Outputstream</code>
	 *
	 * @param os the <code>OutputStream</code> to write to  
	 */
    public void write(OutputStream output) throws IOException {
	
		output.write(getBiffType());
		output.write(colFirst);
		output.write(colLast);
		output.write(colDX);
		output.write(ixfe);
		output.write(grbit);

		Debug.log(Debug.TRACE,"Writing ColInfo record");

    }
    
}
