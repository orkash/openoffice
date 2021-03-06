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

#include <math.h>

#define PREC_float 1
#define PREC_double 2
#define PREC_long_double 3

template<class T>
bool is_equal(T x, T y, sal_Int16 _nPrec)
{
    // due to the fact that this check looks only if both values are equal
    // we only need to look on one value
    
    // 14 digits will announce the checkPrecisionSize
    
    sal_Int32 nPRECISION;
    switch(_nPrec)
    {
    case PREC_float:
        nPRECISION = 6;
        break;
    case PREC_double:
        nPRECISION = 14;
        break;
    case PREC_long_double:
        nPRECISION = 20;
        break;
    default:
        nPRECISION = 2;
    }
    
    if (x < 0)
    {
        x = -x;
    }
    if (y < 0)
    {
        y = -y;
    }
    
    // LLA: due to a bug in printf with '%f' and long double within linux environment
    //      we have to use %lf instead.

    if (_nPrec != PREC_long_double)
    {
        t_print(T_VERBOSE, "double equal: %.20f\n", x);
        t_print(T_VERBOSE, "              %.20f\n", y);
    }
    //here nPrecOfN is the number after dot
    sal_Int32 nBeforeDot = sal_Int32( log10(x) );
    if ( nBeforeDot < 0)
    {
     	nBeforeDot = 0;
    }
    //t_print(T_VERBOSE, "nPRECISION is  %d\n", nPRECISION);
    sal_Int32 nPrecOfN = -nPRECISION + nBeforeDot;
    
    if (_nPrec != PREC_long_double)
        t_print(T_VERBOSE, "nPrecOfN is  %d\n", nPrecOfN);

    long double nPrec = pow(0.1, -nPrecOfN);
    
    if (_nPrec != PREC_long_double)
        t_print(T_VERBOSE, "        prec: %.20f\n", nPrec);
    
    long double nDelta = fabs( x - y ) ;

    if (_nPrec != PREC_long_double)
    {
        t_print(T_VERBOSE, "       delta: %.20f\n", nDelta); 
        t_print(T_VERBOSE, "       nPrec: %.20f\n", nPrec); 
        t_print(T_VERBOSE, "delta must be less or equal to prec!\n\n");
    }
    
    if (nDelta > nPrec)
    {
        // t_print(T_VERBOSE, "values are not equal! ndelta:%.20f\n", nDelta);
        return false;
    }
    // else
    // {
    // t_print(T_VERBOSE, "values are equal.     ndelta:%.20f\n", nDelta);
    return true;
    // }
}

// LLA: bool is_float_equal(float x, float y)
// LLA: {
// LLA:     // due to the fact that this check looks only if both values are equal
// LLA:     // we only need to look on one value
// LLA:     
// LLA:     // 6 digits will announce the checkPrecisionSize
// LLA:     
// LLA:     const sal_Int32 nPRECISION = 6;
// LLA:     if (x < 0)
// LLA:     {
// LLA:         x = -x;
// LLA:     }
// LLA:     if (y < 0)
// LLA:     {
// LLA:         y = -y;
// LLA:     }
// LLA:     
// LLA:     t_print(T_VERBOSE, "double equal: %.20f\n#               %.20f\n", x, y);
// LLA:     sal_Int32 nPrecOfN = -nPRECISION + sal_Int32( log10(x) );
// LLA:     
// LLA:     t_print(T_VERBOSE, "prec: %d\n", nPrecOfN);
// LLA:     double nPrec = pow(10, nPrecOfN) * 1;
// LLA:     
// LLA:     t_print(T_VERBOSE, "        prec: %.20f\n", nPrec);
// LLA:     
// LLA:     double nDelta = fabs( x - y );
// LLA:     t_print(T_VERBOSE, "       delta: %.20f\n\n", nDelta);
// LLA:     
// LLA:     if (nDelta > nPrec)
// LLA:     {
// LLA:         // t_print(T_VERBOSE, "values are not equal! ndelta:%.20f\n", nDelta);
// LLA:         return false;
// LLA:     }
// LLA:     // else
// LLA:     // {
// LLA:     // t_print(T_VERBOSE, "values are equal.     ndelta:%.20f\n", nDelta);
// LLA:     return true;
// LLA:     // }
// LLA: }

bool is_float_equal(float x, float y)
{
    return is_equal<float>(x, y, PREC_float);
}
bool is_double_equal(double x, double y)
{
    return is_equal<double>(x, y, PREC_double);
}
