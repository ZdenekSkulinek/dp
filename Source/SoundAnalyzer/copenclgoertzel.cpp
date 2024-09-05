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


#include "copenclgoertzel.h"
#include "capplication.h"
#include "copenclgoertzeldata.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <thread>


#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif



#define TEST_ERROR(_msg,_cmd)\
    if (error != CL_SUCCESS) {	\
        std::stringstream ss;\
        ss<<std::hex<<error;\
        CApplication::displayError("CL error",std::string(_msg)+std::string(" err=0x")+ss.str() );	\
        _cmd;\
    \
    }

COpenClGoertzel::COpenClGoertzel():
    m_data(nullptr),
    m_inputQueue(nullptr),
    m_outputQueue(nullptr),
    m_bRunning(false),
    m_thread(),
    m_iChannels(1),
    m_iBytesPerSample(1),
    m_iSampleFrequency(44100),
    m_iLength(512),
    m_iPrefferedVectorWidth(1),
    m_iFrequencies(0),
    m_arrFrequencyIndexes(nullptr),
    m_bUseCpu(false),
    m_stats(20000)
{

}



COpenClGoertzel::~COpenClGoertzel()
{
    stop();
    if (m_data)
    {
        delete m_data;
    }
    if (m_arrFrequencyIndexes)
    {
        delete m_arrFrequencyIndexes;
    }
}


int COpenClGoertzel::setFrequencyIndexes(std::string strIndexes)
{
    if (m_arrFrequencyIndexes)
    {
        delete m_arrFrequencyIndexes;
    }
    int cnt=0;
    std::string::const_iterator it = strIndexes.begin();
    while(it != strIndexes.end())
    {
        if(*it == ';')
        {
            cnt++;
        }
        it++;
    }
    m_arrFrequencyIndexes = new int[cnt+1];
    if ( !m_arrFrequencyIndexes )
    {
        CApplication::displayError("Goertzel","No memory (2)");
        return 0;
    }
    std::stringstream ss(strIndexes);
    int windex=0;
    while(!ss.eof())
    {
        int i;
        ss>>i;
        if(!ss.fail())
        {
            m_arrFrequencyIndexes[windex] = i;
            windex++;
        }
        char c;
        ss>>c;
    }
    m_iFrequencies = windex;
    return m_iFrequencies;
}

bool COpenClGoertzel::isRunning()
{
    return m_bRunning;
}

void COpenClGoertzel::stop()
{

    if ( !isRunning() ) {

        return;
    }
    m_bRunning = false;
    m_thread.join();
    CApplication::displayInfo(std::string("Goertzel info"),std::string("Goertzel stopped."));
}


void COpenClGoertzel::start()
{

    assert( !m_bRunning );
    assert( !m_data );



    if ( m_iLength<=0 || m_iLength>16384 ) {

        CApplication::displayError("Goertzel","Segment length is invalid");
        return;
    }

    getInfo();
    m_data = computeData();
    if (!m_data)
    {
        CApplication::displayError("Goertzel","Data cannot be computed");
        return;
    }

    m_bRunning = true;
    if ( m_thread.joinable() ) m_thread.join();
    m_thread = std::thread( &COpenClGoertzel::doGoertzel, this,
                            m_data,
                            nullptr,
                            nullptr,
                            nullptr
                            );
    m_data = nullptr;

}


void COpenClGoertzel::mutliplyMatrixes22(double* dest, double* a, double*b)
{
    dest[0] = a[0] * b[0] + a[1] * b[2];
    dest[1] = a[0] * b[1] + a[1] * b[3];
    dest[2] = a[2] * b[0] + a[3] * b[2];
    dest[3] = a[2] * b[1] + a[3] * b[3];
}


void COpenClGoertzel::powerOfA(double* dest, double* aPowers, unsigned int power)
{
    double mat[4]={1.0,0.0,0.0,1.0};
    int ndx=power;
    int matrixIndex = 0;
    while (ndx)
    {
        if( ndx&1 )
        {
            double tmpMat[4];
            mutliplyMatrixes22(tmpMat,&aPowers[matrixIndex*4],mat);
            for(int mi=0;mi<4;mi++)
            {
                mat[mi]=tmpMat[mi];
            }
        }
        ndx=ndx>>1;
        matrixIndex++;
    }
    for(int i=0;i<4;i++)
    {
        dest[i]=mat[i];
    }
}

size_t COpenClGoertzel::divideGlobals(size_t* global, size_t* local, int length ,int items)
{
      size_t loc[length];
      for(int j=0;j<length;j++){

        loc[j] = local[j];
      }
      size_t max=0;
      size_t maxi = 1;
      int n = global[0];
      if( n>items ) n=items;
      for(int i=1;i<=n;i++)
      {
          if(global[0]%i==0){

              if (length==1)
              {
                  maxi = i;
                  max = i;
              }
              else{

                  size_t nmax = divideGlobals(&global[1],&loc[1], length-1, items / i);
                  if( max < nmax*i ) {

                      maxi = i;
                      max = nmax*i;
                      for(int j=0;j<length;j++){

                          local[j] = loc[j];
                      }

                  }
              }
          }
      }
      local[0] = maxi;
      return max;
}

COpenClGoertzelData* COpenClGoertzel::computeData()
{

    if (m_iChannels<=0)
    {

        CApplication::displayError("Goertzel","invalid channels parameter");
        return nullptr;
    }
    if (m_iLength<=0 || m_iLength>16384)
    {

        CApplication::displayError("Goertzel","invalid segment length parameter");
        return nullptr;
    }
    if (m_iBytesPerSample!=1 && m_iBytesPerSample!=2 && m_iBytesPerSample!=4)
    {

        CApplication::displayError("Goertzel","invalid bytes per sample parameter");
        return nullptr;
    }
    COpenClGoertzelData* data = new COpenClGoertzelData();
    if (!data)
    {
        return nullptr;
    }
    //length
    data->Length = m_iLength;
    //overlap
    data->Overlap = m_iOverlap;
    //Channels
    data->Channels = m_iChannels;
    //BytesPerSample
    data->BytesPerSample = m_iBytesPerSample;
    //K
    data->K = 4;
    if ( data->K > m_iPrefferedVectorWidth )
    {
        CApplication::displayError("Goertzel","Vector with on device < 4");
        delete data;
        return nullptr;
    }
    //L,M
    double sqrtnum = sqrt( double(m_iLength) / double(data->K) );
    unsigned int loops = floor(sqrtnum);
    if ( loops <= 0)
    {
        loops= 1;
    }
    double mm = double(m_iLength) / double(data->K) / double(loops);
    if( mm > floor(mm))
    {
        data->M = floor(mm)+1;
    }
    else
    {
        data->M = floor(mm);
    }
    assert( data->M * data->K * loops >= m_iLength);
    data->L = loops;
    data->SegLength = data->L * data->K * data->M;
    //N
    data->N = m_iFrequencies;
    //D
    data->D1 = new cl_float[data->K * loops * data->N];
    if (!data->D1)
    {
        CApplication::displayError("Goertzel","No memory (3).");
        delete data;
        return nullptr;
    }
    data->D2 = new cl_float[data->K * loops * data->N];
    if (!data->D2)
    {
        CApplication::displayError("Goertzel","No memory (4).");
        delete data;
        return nullptr;
    }
    data->Ak1 = new cl_float2[data->N];
    if (!data->Ak1)
    {
        CApplication::displayError("Goertzel","No memory (5).");
        delete data;
        return nullptr;
    }
    data->Ak2 = new cl_float2[data->N];
    if (!data->Ak2)
    {
        CApplication::displayError("Goertzel","No memory (6).");
        delete data;
        return nullptr;
    }
    data->CSin = new cl_float[data->N];
    if (!data->CSin)
    {
        CApplication::displayError("Goertzel","No memory (9).");
        delete data;
        return nullptr;
    }



    //D
    int fiIndexD=0;
    for(unsigned int fi=0;fi<data->N;fi++)
    {
        double aPowers[MAX_BITS_SEGMENT_SIZE][4];

        //CSin
        double Csin =  sin( double(m_arrFrequencyIndexes[fi]) * 2.0 * M_PI / double(m_iLength) );
        data->CSin[fi] = (float)Csin;
        // A powers
        double C =  2 * cos( double(m_arrFrequencyIndexes[fi]) * 2.0 * M_PI / double(m_iLength) );
        aPowers[0][0] = 0.0;
        aPowers[0][1] = 1.0;
        aPowers[0][2] = -1.0;
        aPowers[0][3] = C;
        for(int i=1;i<MAX_BITS_SEGMENT_SIZE;i++)
        {
            mutliplyMatrixes22(aPowers[i],aPowers[i-1],aPowers[i-1]);
        }


        //D
        for(int li=(loops * data->K)-1;li>=0;li--)
        {
            double mat[4];
            powerOfA(mat,(double*)aPowers,li);
            data->D1[fiIndexD] = mat[1];
            data->D2[fiIndexD] = mat[3];
            fiIndexD++;
        }

        //A

        double matLoops[4];
        powerOfA(matLoops,(double*)aPowers,loops* data->K);
        data->Ak1[fi] = cl_float2 { (cl_float)matLoops[0],(cl_float)matLoops[1] };
        data->Ak2[fi] = cl_float2 { (cl_float)matLoops[2],(cl_float)matLoops[3] };

    }

    // Use CPU
    data->UseCpu = m_bUseCpu;

    return data;
}

void COpenClGoertzel::getInfo()
{
    m_iPrefferedVectorWidth =4;
    return;
    int error;                            // error code returned from api calls


    cl_device_id device_id;             // compute device id

    int gpu = 1;

    cl_platform_id platform_ids[16];
    cl_uint        nPlatforms;

    error =  clGetPlatformIDs(16, platform_ids, &nPlatforms);
    if (error)
    {
        CApplication::displayWarning(std::string("Goertzel warning"),std::string("Can't create platform ID."));
        platform_ids[0]=0;
    }
    else{
        cl_char ver_str[256];
        size_t str_len;
        error = clGetPlatformInfo( platform_ids[0], CL_PLATFORM_VERSION , 255, ver_str, &str_len);
        TEST_ERROR("Failed to version str!", goto end);


        printf(" version str=%s\n",ver_str);
    }

    error = clGetDeviceIDs(platform_ids[0], gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    TEST_ERROR("Failed to create a device group!", goto end);


    /*
     * Query the device to find outstr it's prefered integer vector width.
     * Although we are only printing the value here, it can be used to select between
     * different versions of a kernel.
     */
    cl_uint vectorWidth;
    clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &vectorWidth, NULL);
    m_iPrefferedVectorWidth = vectorWidth;

end:
    return ;
}

void COpenClGoertzel::doGoertzel(
        COpenClGoertzelData* data,
        float*               oneShotData,
        float*               oneShotResult,
        float*               oneShotResultPhase
        )
{
    int lclsize = data->Overlap;
    data->Overlap = 0;

    CApplication::displayInfo(std::string("Goertzel info"),std::string("Goertzel started."));

    int error;                            // error code returned from api calls

    size_t globalTo;                      // global domain size for our calculation

    size_t localTo;                       // local domain size for our calculation

    size_t global[3];                     // global domain size for our calculation

    size_t local[3];                      // local domain size for our calculation

    size_t globalFrom[2];                 // global domain size for our calculation

    size_t localFrom[2];                  // local domain size for our calculation

    cl_device_id device_id;             // compute device id

    cl_context context;                 // compute context

    cl_command_queue commands;          // compute command queue

    cl_program program;                 // compute program

    cl_kernel kernelToStreams;          // compute kernel

    cl_kernel kernelFromStreams;        // compute kernel

    cl_kernel kernelMain;               // compute kernel


    cl_mem bufAk1;
    cl_mem bufAk2;
    //cl_mem bufAk1LastLoop;
    //cl_mem bufAk2LastLoop;
    cl_mem bufD1;
    cl_mem bufD2;
    cl_mem bufCSin;
    cl_mem bufStreams;
    cl_mem bufOutStreams;
    cl_mem bufInput;
    cl_mem bufOutput;
    cl_mem bufOutputPhase;

    size_t wgsize = 0;

    std::string ssstr;
    std::stringstream ss;
    ss.str(ssstr);
    bool refreshStatsText;

    std::string goertzelSrc = CApplication::loadFromResources(":/goertzel.cl");
    const char* chProgramStrings[]={
        goertzelSrc.c_str()
    };


    cl_float* streams = new cl_float[data->Channels * data->SegLength ];
    if (!streams)
    {
        CApplication::displayError("Goertzel","No memory (9).");
        goto end;
    }
    for(unsigned int i=0;i<data->Channels * data->SegLength;i++)
    {
        streams[i]=0.0f;
    }


    // Connect to a compute device

    //

    cl_platform_id platform_ids[16];
    cl_uint        nPlatforms;

    error =  clGetPlatformIDs(16, platform_ids, &nPlatforms);
    TEST_ERROR("Failed to create a device platforms!", goto end);


    error = clGetDeviceIDs(platform_ids[0], data->UseCpu ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    TEST_ERROR("Failed to create a device group!", goto end);


    // Create a compute context
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &error);
    if ( !context )
    {

        TEST_ERROR("Failed to create a compute context!", goto end);
    }


    // Create a command commands
    //
    commands = clCreateCommandQueue(context, device_id, 0, &error);
    if (!commands)
    {
        TEST_ERROR("Failed to create a command commands!", goto releaseContext);
    }



    // Create the compute program from the source buffer
    //

    program = clCreateProgramWithSource(context, 1, chProgramStrings, NULL , &error);
    if (!program)
    {
        TEST_ERROR("Failed to create compute program!", goto releaseCommandQueue);
    }



    // Build the program executable
    //
    error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (error != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048] = "Error: Failed to build program executable!\n";
        int bufferlen = strlen(buffer);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer)-bufferlen, &buffer[bufferlen], &len);
        TEST_ERROR(buffer, goto releaseProgram);
    }



    // Create the compute kernel instr the program we wish to run
    //
    kernelToStreams = clCreateKernel(program, "convertShortToStreams", &error);
    kernelMain = clCreateKernel(program, "main", &error);
    kernelFromStreams = clCreateKernel(program, "fromStreams", &error);
    if ( !kernelToStreams || !kernelMain || !kernelFromStreams)
    {
        TEST_ERROR("Failed to create compute kernel!", goto releaseKernels);
    }



    // Create the input and output arrays instr device memory for our calculation
    //
    bufAk1 = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float2) * data->N, NULL, NULL);
    bufAk2 = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float2) * data->N, NULL, NULL);
    //bufAk1LastLoop = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float2) * data->N, NULL, NULL);
    //bufAk2LastLoop = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float2) * data->N, NULL, NULL);
    bufD1 = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float) * data->N * data->L * data->K, NULL, NULL);
    bufD2 = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float) * data->N * data->L * data->K, NULL, NULL);
    bufCSin = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float) * data->N , NULL, NULL);
    bufStreams = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(cl_float) * data->Channels * data->SegLength, NULL, NULL);
    bufOutStreams = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(cl_float2) * data->N * data->Channels * data->M, NULL, NULL);
    bufInput = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_ushort) * data->Length * data->Channels, NULL, NULL);
    bufOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * data->Channels * data->N, NULL, NULL);
    bufOutputPhase = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * data->Channels * data->N, NULL, NULL);
    if (!bufAk1 || !bufAk2 /*|| !bufAk1LastLoop || !bufAk2LastLoop*/ || !bufD1 || !bufD2 || !bufCSin || !bufStreams || !bufInput || !bufOutput || !bufOutputPhase)
    {
        error = -1;
        TEST_ERROR("Failed to allocate device memory!", goto releaseInputOutput);
    }

    // Write our data set into the input array instr device memory
    //
    error = clEnqueueWriteBuffer(commands, bufAk1, CL_TRUE, 0, sizeof(cl_float2) * data->N, data->Ak1, 0, NULL, NULL);
    TEST_ERROR("Failed to write to source array! (1)", goto releaseInputOutput);
    error = clEnqueueWriteBuffer(commands, bufAk2, CL_TRUE, 0, sizeof(cl_float2) * data->N, data->Ak2, 0, NULL, NULL);
    TEST_ERROR("Failed to write to source array! (2)", goto releaseInputOutput);
    error = clEnqueueWriteBuffer(commands, bufD1, CL_TRUE, 0, sizeof(cl_float) * data->N * data->L * data->K, data->D1, 0, NULL, NULL);
    TEST_ERROR("Failed to write to source array! (5)", goto releaseInputOutput);
    error = clEnqueueWriteBuffer(commands, bufD2, CL_TRUE, 0, sizeof(cl_float) * data->N * data->L * data->K, data->D2, 0, NULL, NULL);
    TEST_ERROR("Failed to write to source array! (6)", goto releaseInputOutput);
    error = clEnqueueWriteBuffer(commands, bufCSin, CL_TRUE, 0, sizeof(cl_float) * data->N , data->CSin, 0, NULL, NULL);
    TEST_ERROR("Failed to write to source array! (7)", goto releaseInputOutput);
    error = clEnqueueWriteBuffer(commands, bufStreams, CL_TRUE, 0, sizeof(cl_float) * data->Channels * data->SegLength, streams, 0, NULL, NULL);
    TEST_ERROR("Failed to write to source array! (8)", goto releaseInputOutput);



    // Set the arguments to our compute kernel
    //
    error  = clSetKernelArg(kernelToStreams, 0, sizeof(cl_mem), &bufStreams);
    TEST_ERROR("Failed to set argument input for kernel! (1)", goto releaseInputOutput);
    error = clSetKernelArg(kernelToStreams, 1, sizeof(cl_mem), &bufInput);
    TEST_ERROR("Failed to set argument output for kernel! (2)", goto releaseInputOutput);
    error = clSetKernelArg(kernelToStreams, 2, sizeof(unsigned int), &data->Channels);
    TEST_ERROR("Failed to set argument count for kernel! (3)", goto releaseInputOutput);
    error = clSetKernelArg(kernelToStreams, 3, sizeof(unsigned int), &data->Length);
    TEST_ERROR("Failed to set argument count for kernel! (4)", goto releaseInputOutput);
    error = clSetKernelArg(kernelToStreams, 4, sizeof(unsigned int), &data->Overlap);
    TEST_ERROR("Failed to set argument count for kernel! (5)", goto releaseInputOutput);
    error = clSetKernelArg(kernelToStreams, 5, sizeof(unsigned int), &data->SegLength);
    TEST_ERROR("Failed to set argument count for kernel! (6)", goto releaseInputOutput);

    error  = clSetKernelArg(kernelMain, 0, sizeof(cl_mem), &bufStreams);
    TEST_ERROR("Failed to set argument input for kernel! (10)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 1, sizeof(cl_mem), &bufD1);
    TEST_ERROR("Failed to set argument output for kernel! (11)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 2, sizeof(cl_mem), &bufD2);
    TEST_ERROR("Failed to set argument output for kernel! (12)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 3, sizeof(unsigned int), &data->Channels);
    TEST_ERROR("Failed to set argument count for kernel! (13)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 4, sizeof(unsigned int), &data->K);
    TEST_ERROR("Failed to set argument count for kernel! (14)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 5, sizeof(unsigned int), &data->L);
    TEST_ERROR("Failed to set argument count for kernel! (15)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 6, sizeof(unsigned int), &data->M);
    TEST_ERROR("Failed to set argument count for kernel! (17)", goto releaseInputOutput);
    error = clSetKernelArg(kernelMain, 7, sizeof(unsigned int), &data->Length);
    TEST_ERROR("Failed to set argument count for kernel! (18)", goto releaseInputOutput);
    error  = clSetKernelArg(kernelMain, 8, sizeof(cl_mem), &bufOutStreams);
    TEST_ERROR("Failed to set argument input for kernel! (19)", goto releaseInputOutput);


    error  = clSetKernelArg(kernelFromStreams, 0, sizeof(cl_mem), &bufOutStreams);
    TEST_ERROR("Failed to set argument input for kernel! (20)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 1, sizeof(cl_mem), &bufAk1);
    TEST_ERROR("Failed to set argument output for kernel! (21)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 2, sizeof(cl_mem), &bufAk2);
    TEST_ERROR("Failed to set argument output for kernel! (22)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 3, sizeof(cl_mem), &bufD2);
    TEST_ERROR("Failed to set argument output for kernel! (25)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 4, sizeof(cl_mem), &bufCSin);
    TEST_ERROR("Failed to set argument output for kernel! (26)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 5, sizeof(unsigned int), &data->Channels);
    TEST_ERROR("Failed to set argument count for kernel! (27)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 6, sizeof(unsigned int), &data->K);
    TEST_ERROR("Failed to set argument count for kernel! (28)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 7, sizeof(unsigned int), &data->L);
    TEST_ERROR("Failed to set argument count for kernel! (29)", goto releaseInputOutput);
    error = clSetKernelArg(kernelFromStreams, 8, sizeof(unsigned int), &data->M);
    TEST_ERROR("Failed to set argument count for kernel! (30)", goto releaseInputOutput);
    error  = clSetKernelArg(kernelFromStreams, 9, sizeof(cl_mem), &bufOutput);
    TEST_ERROR("Failed to set argument input for kernel! (31)", goto releaseInputOutput);
    error  = clSetKernelArg(kernelFromStreams, 10, sizeof(cl_mem), &bufOutputPhase);
    TEST_ERROR("Failed to set argument input for kernel! (32)", goto releaseInputOutput);


    // Get the maximum work group size for executing the kernel on the device
    //

    error = clGetKernelWorkGroupInfo(kernelToStreams, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(localTo), &wgsize, NULL);
    TEST_ERROR("Failed to retrieve kernel work group info!", goto releaseInputOutput);


    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device
    //
    globalTo = data->Length;
    divideGlobals(&globalTo,&localTo,1,wgsize);



    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device
    //
    global[0] = data->N;
    global[1] = data->Channels;
    global[2] = data->M;
    //divideGlobals(global,local,3,wgsize);
    local[0] = lclsize;
    local[1] = 1;
    local[2] = 1;

    ss.seekp(0LL);
    ss<<"Global Cores "<<global[0]<<"/"<<global[1]<<"/"<<global[2]<<"\nLocal cores "<<local[0]<<"/"<<local[1]<<"/"<<local[2]<<" max "<<wgsize<<std::flush;
    CApplication::displayInfo("",ss.str(),DISPLAYINFO_CORES);
    ss.str(std::string());
    ss<<"Computing time\n---"<<std::flush;
    CApplication::displayInfo("",ss.str(),DISPLAYINFO_TIME);

    ss.str(std::string());
    ss<<"Computing freq.\n---"<<std::flush;
    CApplication::displayInfo("",ss.str(),DISPLAYINFO_COMPUTE_FREQUENCY);



    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device
    //
    globalFrom[0] = data->N;
    globalFrom[1] = data->Channels;
    divideGlobals(globalFrom,localFrom,2,wgsize);


    if (oneShotData)
    {
        float* input = new float[data->SegLength];
        if (!input)
        {
             CApplication::displayError("Goertzel","No memory (12).");
        }
        else
        {
            memset(input,0,sizeof(float)*(data->SegLength-data->Length));
            memcpy(&input[data->SegLength-data->Length],oneShotData,sizeof(float)*(data->Length));
            error = clEnqueueWriteBuffer(commands, bufStreams, CL_TRUE, 0, sizeof(cl_float) * data->SegLength, input, 0, NULL, NULL);
            TEST_ERROR("Failed to write to source array! (40)", goto releaseInputOutput);

            error = clEnqueueNDRangeKernel(commands, kernelMain, 3, NULL, global, local , 0, NULL, NULL);
            TEST_ERROR("Failed to execute kernel! (22)", goto releaseInputOutput);

            clEnqueueBarrier(commands);

            error = clEnqueueNDRangeKernel(commands, kernelFromStreams, 2, NULL, globalFrom, localFrom , 0, NULL, NULL);
            TEST_ERROR("Failed to execute kernel! (23)", goto releaseInputOutput);

            clFinish(commands);

            error = clEnqueueReadBuffer( commands, bufOutput, CL_TRUE, 0, sizeof(cl_float) * data->Channels * data->N, oneShotResult, 0, NULL, NULL );
            error = clEnqueueReadBuffer( commands, bufOutputPhase, CL_TRUE, 0, sizeof(cl_float) * data->Channels * data->N, oneShotResultPhase, 0, NULL, NULL );
            delete input;
        }
    }
    else
    {
        m_stats.reset();
        while( m_bRunning ) {

            CInterThreadQueue* inputQueue = m_inputQueue;
            if ( !inputQueue )
            {
                std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
                continue;
            }

            unsigned char *mem = inputQueue->pop();
            if ( !mem ) {

                CInterThreadQueue* outQueue = m_outputQueue;
                if (outQueue)
                {
                    outQueue->setLast();
                }
                outQueue = m_outputQueuePhase;
                if (outQueue)
                {
                    outQueue->setLast();
                }
                break;
            }

            std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now();

            error = clEnqueueWriteBuffer(commands, bufInput, CL_TRUE, 0, sizeof(cl_ushort) * data->Length * data->Channels, mem , 0, NULL, NULL);
            TEST_ERROR("Failed to write to source array! (7)", goto releaseInputOutput);
            inputQueue->free(mem);


            error = clEnqueueNDRangeKernel(commands, kernelToStreams, 1, NULL, &globalTo, &localTo, 0, NULL, NULL);
            TEST_ERROR("Failed to execute kernel!", goto releaseInputOutput);

            clEnqueueBarrier(commands);


            error = clEnqueueNDRangeKernel(commands, kernelMain, 3, NULL, global, local , 0, NULL, NULL);
            TEST_ERROR("Failed to execute kernel! (2)", goto releaseInputOutput);

            clEnqueueBarrier(commands);

            error = clEnqueueNDRangeKernel(commands, kernelFromStreams, 2, NULL, globalFrom, localFrom , 0, NULL, NULL);
            TEST_ERROR("Failed to execute kernel! (3)", goto releaseInputOutput);

            clEnqueueBarrier(commands);

            CInterThreadQueue* outQueue = m_outputQueue;
            if (outQueue)
            {
                unsigned char *mem = outQueue->alloc();
                if ( mem ) {

                    error = clEnqueueReadBuffer( commands, bufOutput, CL_TRUE, 0, sizeof(cl_float) * data->Channels * data->N, mem, 0, NULL, NULL );
                    TEST_ERROR("Failed to retrieve kernel data (1)", goto releaseInputOutput);
                }
                outQueue->push(mem);
            }
            CInterThreadQueue* outQueuePhase = m_outputQueuePhase;
            if (outQueuePhase)
            {
                unsigned char *mem = outQueuePhase->alloc();
                if ( mem ) {

                    error = clEnqueueReadBuffer( commands, bufOutputPhase, CL_TRUE, 0, sizeof(cl_float) * data->Channels * data->N, mem, 0, NULL, NULL );
                    TEST_ERROR("Failed to retrieve kernel data (2)", goto releaseInputOutput);
                }
                outQueuePhase->push(mem);
            }
            clFinish(commands);

            std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();

            refreshStatsText = m_stats.pushTime(endTime, std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count());

            if ( refreshStatsText ) {

                    ss.str(std::string());
                    ss<<"Computing time\n"<<std::fixed << std::setprecision( 3 )<< double(m_stats.duration()) / double(m_stats.length()) / 1000.0 << " ms"<<std::flush;
                    CApplication::displayInfo("",ss.str(),DISPLAYINFO_TIME);

                    ss.str(std::string());
                    ss<<"Computing freq.\n"<<std::fixed << std::setprecision( 3 )<< double(m_stats.length()) / double(m_stats.interval()) * 1000000.0 << " Hz"<<std::flush;
                    CApplication::displayInfo("",ss.str(),DISPLAYINFO_COMPUTE_FREQUENCY);
            }

        }
    }

releaseInputOutput:
    if (bufAk1) clReleaseMemObject(bufAk1);
    if (bufAk2) clReleaseMemObject(bufAk2);
    if (bufD1) clReleaseMemObject(bufD1);
    if (bufD2) clReleaseMemObject(bufD2);
    if (bufCSin) clReleaseMemObject(bufCSin);
    if (bufStreams) clReleaseMemObject(bufStreams);
    if (bufOutStreams) clReleaseMemObject(bufOutStreams);
    if (bufInput) clReleaseMemObject(bufInput);
    if (bufOutput) clReleaseMemObject(bufOutput);
    if (bufOutputPhase) clReleaseMemObject(bufOutputPhase);
releaseKernels:
    if(kernelToStreams) clReleaseKernel(kernelToStreams);
    if(kernelFromStreams) clReleaseKernel(kernelFromStreams);
    if(kernelMain) clReleaseKernel(kernelMain);

releaseProgram:
    clReleaseProgram(program);

releaseCommandQueue:
    clReleaseCommandQueue(commands);

releaseContext:
    clReleaseContext(context);

end:
    return ;
}


void COpenClGoertzel::computeOneFile(std::string filename, CInterThreadQueue *amplitudeQueue, CInterThreadQueue *phaseQueue)
{
    m_iChannels = 1;
    m_iBytesPerSample = 4;
    m_iOverlap = 0;
    getInfo();
    if (m_arrFrequencyIndexes)
    {
        delete m_arrFrequencyIndexes;
        m_arrFrequencyIndexes = nullptr;
        m_iFrequencies = 0;
    }

    std::string line;
    std::ifstream instr(filename);
    if(!instr.is_open())
    {
        CApplication::displayError("Goertzel","Can't open file for reading.");
        return;
    }
    std::string outfilename;
    std::string::iterator it = filename.end();
    while(it!=filename.begin())
    {
        if(*it=='/')
        {
            outfilename=filename;
            outfilename+="_ap";
            break;
        }
        if(*it=='.')
        {
            outfilename=std::string(filename.begin(),it--);
            outfilename+="_ap";
            outfilename+=std::string(it,filename.end());
            break;
        }
        it--;
        if (it==filename.begin())
        {
            outfilename=filename;
            outfilename+="_ap";
            break;
        }
    }
    std::ofstream outstr(outfilename);
    if(!outstr.is_open())
    {
        CApplication::displayError("Goertzel","Can't open file for writing.");
        instr.close();
        return;
    }
    getline(instr,line);
    outstr<<"created by Sound Analyzer"<<std::endl;
    while(!instr.eof())
    {
        getline(instr,line);
        if (line=="")
        {
            continue;
        }
        if (instr.eof())
        {
            break;
        }
        if(line.compare(0,8,"# name: ")!=0)
        {
            CApplication::displayError("Goertzel","parse file (1).");
            break;
        }
        std::string name=line.substr(8,line.length()-1);
        getline(instr,line);
        if(line.compare(0,14,"# type: matrix")!=0)
        {
            CApplication::displayError("Goertzel","parse file (2).");
            break;
        }
        getline(instr,line);
        if(line.compare(0,9,"# rows: 1")!=0)
        {
            CApplication::displayError("Goertzel","parse file (3).");
            break;
        }
        getline(instr,line);
        if(line.compare(0,11,"# columns: ")!=0)
        {
            CApplication::displayError("Goertzel","parse file (4).");
            break;
        }
        int sz = std::stoi(line.substr(11,line.length()-1));
        float* data = new float[sz];
        if(!data)
        {
            CApplication::displayError("Goertzel","parse file (5).");
            break;
        }
        float* dataam = new float[sz];
        if(!data)
        {
            CApplication::displayError("Goertzel","parse file (6).");
            break;
        }
        float* dataph = new float[sz];
        if(!data)
        {
            CApplication::displayError("Goertzel","parse file (7).");
            break;
        }
        int* freequencies = new int[sz];
        if(!freequencies)
        {
            CApplication::displayError("Goertzel","parse file (8).");
            break;
        }
        for(int i=0;i<sz;i++)
        {
            instr >> data[i];
        }

        m_iLength = sz;
        for(int i=0;i<sz;i++)
        {
            freequencies[i] = i;
        }
        m_arrFrequencyIndexes = freequencies;
        m_iFrequencies = sz;
        COpenClGoertzelData* gd = computeData();
        doGoertzel(gd,data,dataam,dataph);

        outstr<<"# name: "<<name<<"_am"<<std::endl;
        outstr<<"# type: matrix"<<std::endl;
        outstr<<"# rows: 1"<<std::endl;
        outstr<<"# columns: "<<sz<<std::endl;
        for(int i=0;i<sz;i++)
        {
            outstr<<dataam[i]<<" ";
        }
        outstr<<std::endl<<std::endl;

        outstr<<"# name: "<<name<<"_ph"<<std::endl;
        outstr<<"# type: matrix"<<std::endl;
        outstr<<"# rows: 1"<<std::endl;
        outstr<<"# columns: "<<sz<<std::endl;
        for(int i=0;i<sz;i++)
        {
            outstr<<dataph[i]<<" ";
        }
        outstr<<std::endl<<std::endl;

        if ( amplitudeQueue ) {

            amplitudeQueue->setLast();
            amplitudeQueue->reset(1,sizeof(float)*sz);
            unsigned char* data = amplitudeQueue->alloc();
            memcpy(data,dataam,sizeof(float)*sz);
            amplitudeQueue->push(data);
        }
        if ( phaseQueue ) {

            phaseQueue->setLast();
            phaseQueue->reset(1,sizeof(float)*sz);
            unsigned char* data = phaseQueue->alloc();
            memcpy(data,dataph,sizeof(float)*sz);
            phaseQueue->push(data);
        }

        delete data;
        delete dataam;
        delete dataph;
    }
    instr.close();
    outstr.close();
}

