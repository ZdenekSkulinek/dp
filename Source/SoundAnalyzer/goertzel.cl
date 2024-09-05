#define M_MY_PI 314159265358979323846.0 / 100000000000000000000.0

__kernel void convertShortToStreams(
        __global float* streams,
        __global short* input,
        unsigned int channels,
        unsigned int length,
        unsigned int overlap,
        unsigned int seglength
        )
{
    int i = get_global_id(0);
    int index;
    int freeSpace = seglength - length;
    for(index=0;index<channels;index++)
    {
        __global float* stream = &streams[index * length];
        if ( i < overlap)
        {
            stream[i+freeSpace] = stream[i+freeSpace+length-overlap];
        }
        else
        {
            int ndx = i - overlap;
            stream[i+freeSpace] = ((float)input[ndx*channels]) / 0x7FFF;
        }
    }
}

__kernel void convertFloatToStreams(
        __global float* streams,
        __global float* input,
        unsigned int length,
        unsigned int seglength
        )
{
    int i = get_global_id(0);
    int freeSpace = seglength - length;
    streams[i+freeSpace] = input[i];
}


__kernel void main(
    __constant float*   streams,
    __constant float*   D1,
    __constant float*   D2,
    unsigned int        Channels,
    unsigned int        K,
    unsigned int        L,
    unsigned int        M,
    unsigned int        Length,
    __global float2*    outStreams
)
{
    unsigned int indexFrequency = get_global_id(0);
    unsigned int indexChannel = get_global_id(1);
    unsigned int indexItem = get_global_id(2);
    unsigned int i;
    float2 acc={0.0,0.0};
    __constant float4* data = (__constant float4*)&streams[indexChannel * Length + indexItem * L * K];
    __constant float4* mxD1 = (__constant float4*)&D1[indexFrequency * L * K];
    __constant float4* mxD2 = (__constant float4*)&D2[indexFrequency * L * K];
    for(i=0;i<L;i++)
    {
        acc.x = acc.x + dot(data[i] , mxD1[i]);
        acc.y = acc.y + dot(data[i] , mxD2[i]);
    } 
    outStreams[indexFrequency * Channels * M + indexChannel * M + indexItem] = acc;
}

__kernel void fromStreams(
    __constant float2*  streams,
    __constant float2*  Ak1,
    __constant float2*  Ak2,
    //__constant float2*  Ak1LastLoop,
    //__constant float2*  Ak2LastLoop,
    __constant float*   D2,
    __constant float*   CSin,
    unsigned int        Channels,
    unsigned int        K,
    unsigned int        L,
    unsigned int        M,
    __global   float*   output,
    __global   float*   outputPhase
)
{
    unsigned int indexFrequency = get_global_id(0);
    unsigned int indexChannel = get_global_id(1);
    unsigned int i;
    __constant float2* data = (__constant float2*)&streams[indexFrequency * Channels * M + indexChannel * M];
    float2  accum = *data;
    data++;
    float2 tAk1 = Ak1[indexFrequency];
    float2 tAk2 = Ak2[indexFrequency];
    for(i=1;i<M;i++)
    {
        float2 v;
        v.x = dot(tAk1,accum);
        v.y = dot(tAk2,accum);
        v = v + *data;
        accum = v;
        data++;
    }
    float2 v;

    float c2cosk2piT = D2[(indexFrequency+1) * L * K -2];
    float c2sink2piT = CSin[indexFrequency];
    float2 A1={0,1};
    float2 A2={-1,c2cosk2piT};
    v.x = dot(A1,accum);
    v.y = dot(A2,accum);

    float y2 = v.x * v.x + v.y * v.y - v.x * v.y * c2cosk2piT;
    float y = sqrt(y2);


    output[indexFrequency * Channels + indexChannel] = y;

    float cosvx = v.y-v.x*c2cosk2piT/2.0f;
    float sinvx = v.x * c2sink2piT;
    float phase = atan( sinvx / cosvx );

    if (isnan(phase))
    {
        phase = 0.0;
    }

    if (cosvx < 0 )
    {
        if (sinvx < 0 )
        {
            phase = phase - M_MY_PI;
        }
        else
        {
            phase = phase + M_MY_PI;
        }
    }
    output[indexFrequency * Channels + indexChannel] = y;
    outputPhase[indexFrequency * Channels + indexChannel] = phase;
}
