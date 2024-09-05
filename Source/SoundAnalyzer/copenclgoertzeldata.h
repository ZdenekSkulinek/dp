/* SoundAnalyzer
 *
 * This program demonstrates usage of OpenCL for signal processing.
 * Simply computes spectrum of signal with Goertzel algorithm.
 *
 * Copyright (C) 2017 Zdeněk Skulínek
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef COPENCLGOERTZELDATA_H
#define COPENCLGOERTZELDATA_H

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif


class COpenClGoertzelData
{
public:
    /*
     * Length - total length of segmen in samples
     */
    cl_uint     Length;
    /*
     * Overlap - lengthshared with two segments
     */
    cl_uint     Overlap;
    /*
     * SegLength - total length of segmen in samples aligned
     */
    cl_uint     SegLength;
    /*
     * Channels is number of channels within stream (x vector)
     */
    cl_uint     Channels;
    /*
     * BytesPerSample valis is 1 (byte), 2 (short), 4 (float)
     */
    cl_uint     BytesPerSample;
    /*
     * K is size of vector, K shlould be 1,2,3,4,8,16. In a best case K=16, that means, length of samples should be divisible by 16
     */
    cl_uint     K;
    /*
     * N nuber of frequencies that is computed
     */
    cl_uint     N;
    /*
     * M number of items in one frequency within one channel
     */
    cl_uint     M;
    /*
     * L number of loops within one work item. Loops is separated for items so this is array. length of array is M*channels
     */
    cl_uint     L;
    /*
     * LLastLoop number of loops within one work item. Loops is separated for items so this is array. length of array is M*channels
     */
    //cl_uint     LLastLoop;
    /*
     * Ak1[N] is upper vector of matrix A powered by K. has size 2. Ak1* is array over all frequencies. Size of array is N*M*channels.
     */
    cl_float2*  Ak1;
    /*
     * Ak1lastLoop[N] is upper vector of matrix A powered by K. has size 2. Ak1* is array over all frequencies. Size of array is N*M*channels.
     */
    //cl_float2*  Ak1LastLoop;
    /*
     * Ak2[N] is botom vector of matrix A powered by K. has size 2. Ak1* is array over all frequencies. Size of array is N*M*channels.
     */
    cl_float2*  Ak2;
    /*
     * Ak2LastLoop[N] is botom vector of matrix A powered by K. has size 2. Ak1* is array over all frequencies. Size of array is N*M*channels.
     */
    //cl_float2*  Ak2LastLoop;
    /*
     * D1[N][L*K] is upper vector od D matrix.vector has K items, is defined for all frequencies. size of array id K*L*N.
     */
    cl_float*   D1;
    /*
     * D2[N][L*K] is bottom vector od D matrix.vector has K items, is defined for all frequencies. size of array id K*L*N.
     */
    cl_float*   D2;
    /*
     * CSin[N] sinus - sin(k * 2 * pi / Length)
     */
    cl_float*   CSin;
    /*
     * UseCPU if set, used is software mode of open CL
     */
    bool UseCpu;
public:
    COpenClGoertzelData():
        Length(0),
        Overlap(0),
        SegLength(0),
        Channels(0),
        BytesPerSample(0),
        K(0),
        N(0),
        M(0),
        L(0),
        //LLastLoop(0),
        Ak1(nullptr),
        //Ak1LastLoop(nullptr),
        Ak2(nullptr),
        //Ak2LastLoop(nullptr),
        D1(nullptr),
        D2(nullptr)
    {

    }

    ~COpenClGoertzelData()
    {
        if(Ak1) delete Ak1;
        if(Ak2) delete Ak2;
        //if(Ak1LastLoop) delete Ak1LastLoop;
        //if(Ak2LastLoop) delete Ak2LastLoop;
        if(D1) delete D1;
        if(D2) delete D2;
    }
};

#endif // COPENCLGOERTZELDATA_H
