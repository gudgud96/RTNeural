#include "lstm.h"

namespace RTNeural
{

#if !RTNEURAL_USE_EIGEN && !RTNEURAL_USE_XSIMD && !RTNEURAL_USE_ACCELERATE

template <typename T>
LSTMLayer<T>::LSTMLayer(int in_size, int out_size)
    : Layer<T>(in_size, out_size)
    , fWeights(in_size, out_size)
    , iWeights(in_size, out_size)
    , oWeights(in_size, out_size)
    , cWeights(in_size, out_size)
{
    ht1 = new T[out_size];
    ct1 = new T[out_size];

    fVec = new T[out_size];
    iVec = new T[out_size];
    oVec = new T[out_size];
    ctVec = new T[out_size];
    cVec = new T[out_size];
}

template <typename T>
LSTMLayer<T>::LSTMLayer(std::initializer_list<int> sizes)
    : LSTMLayer<T>(*sizes.begin(), *(sizes.begin() + 1))
{
}

template <typename T>
LSTMLayer<T>::LSTMLayer(const LSTMLayer& other)
    : LSTMLayer<T>(other.in_size, other.out_size)
{
}

template <typename T>
LSTMLayer<T>& LSTMLayer<T>::operator=(const LSTMLayer<T>& other)
{
    if(&other != this)
        *this = LSTMLayer<T>(other);
    return *this;
}

template <typename T>
LSTMLayer<T>::~LSTMLayer()
{
    delete[] ht1;
    delete[] ct1;

    delete[] fVec;
    delete[] iVec;
    delete[] oVec;
    delete[] ctVec;
    delete[] cVec;
}

template <typename T>
void LSTMLayer<T>::reset()
{
    std::fill(ht1, ht1 + Layer<T>::out_size, (T)0);
    std::fill(ct1, ct1 + Layer<T>::out_size, (T)0);
}

template <typename T>
LSTMLayer<T>::WeightSet::WeightSet(int in_size, int out_size)
    : out_size(out_size)
{
    W = new T*[out_size];
    U = new T*[out_size];
    b = new T[out_size];

    for(int i = 0; i < out_size; ++i)
    {
        W[i] = new T[in_size];
        U[i] = new T[out_size];
    }
}

template <typename T>
LSTMLayer<T>::WeightSet::~WeightSet()
{
    delete[] b;

    for(int i = 0; i < out_size; ++i)
    {
        delete[] W[i];
        delete[] U[i];
    }

    delete[] W;
    delete[] U;
}

template <typename T>
void LSTMLayer<T>::setWVals(const std::vector<std::vector<T>>& wVals)
{
    for(int i = 0; i < Layer<T>::in_size; ++i)
    {
        for(int k = 0; k < Layer<T>::out_size; ++k)
        {
            iWeights.W[k][i] = wVals[i][k];
            fWeights.W[k][i] = wVals[i][k + Layer<T>::out_size];
            cWeights.W[k][i] = wVals[i][k + Layer<T>::out_size * 2];
            oWeights.W[k][i] = wVals[i][k + Layer<T>::out_size * 3];
        }
    }
}

template <typename T>
void LSTMLayer<T>::setUVals(const std::vector<std::vector<T>>& uVals)
{
    for(int i = 0; i < Layer<T>::out_size; ++i)
    {
        for(int k = 0; k < Layer<T>::out_size; ++k)
        {
            iWeights.U[k][i] = uVals[i][k];
            fWeights.U[k][i] = uVals[i][k + Layer<T>::out_size];
            cWeights.U[k][i] = uVals[i][k + Layer<T>::out_size * 2];
            oWeights.U[k][i] = uVals[i][k + Layer<T>::out_size * 3];
        }
    }
}

template <typename T>
void LSTMLayer<T>::setBVals(const std::vector<T>& bVals)
{
    for(int k = 0; k < Layer<T>::out_size; ++k)
    {
        iWeights.b[k] = bVals[k];
        fWeights.b[k] = bVals[k + Layer<T>::out_size];
        cWeights.b[k] = bVals[k + Layer<T>::out_size * 2];
        oWeights.b[k] = bVals[k + Layer<T>::out_size * 3];
    }
}

//====================================================
template <typename T, int in_sizet, int out_sizet>
LSTMLayerT<T, in_sizet, out_sizet>::LSTMLayerT()
{
    for(int i = 0; i < out_size; ++i)
    {
        // single-input kernel weights
        Wf_1[i] = (T)0;
        Wi_1[i] = (T)0;
        Wo_1[i] = (T)0;
        Wc_1[i] = (T)0;

        // biases
        bf[i] = (T)0;
        bi[i] = (T)0;
        bo[i] = (T)0;
        bc[i] = (T)0;

        // intermediate vars
        ft[i] = (T)0;
        it[i] = (T)0;
        ot[i] = (T)0;
        ht[i] = (T)0;
    }

    for(int i = 0; i < out_size; ++i)
    {
        // recurrent weights
        for(int k = 0; k < out_size; ++k)
        {
            Uf[i][k] = (T)0;
            Ui[i][k] = (T)0;
            Uo[i][k] = (T)0;
            Uc[i][k] = (T)0;
        }

        // kernel weights
        for(int k = 0; k < in_size; ++k)
        {
            Wf[i][k] = (T)0;
            Wi[i][k] = (T)0;
            Wo[i][k] = (T)0;
            Wc[i][k] = (T)0;
        }
    }

    reset();
}

template <typename T, int in_sizet, int out_sizet>
void LSTMLayerT<T, in_sizet, out_sizet>::reset()
{
    // reset output state
    for(int i = 0; i < out_size; ++i)
    {
        ct[i] = (T)0;
        outs[i] = (T)0;
    }
}

template <typename T, int in_sizet, int out_sizet>
void LSTMLayerT<T, in_sizet, out_sizet>::setWVals(const std::vector<std::vector<T>>& wVals)
{
    for(int i = 0; i < in_size; ++i)
    {
        for(int j = 0; j < out_size; ++j)
        {
            Wi[j][i] = wVals[i][j];
            Wf[j][i] = wVals[i][j + out_size];
            Wc[j][i] = wVals[i][j + 2 * out_size];
            Wo[j][i] = wVals[i][j + 3 * out_size];
        }
    }

    for(int j = 0; j < out_size; ++j)
    {
        Wi_1[j] = wVals[0][j];
        Wf_1[j] = wVals[0][j + out_size];
        Wc_1[j] = wVals[0][j + 2 * out_size];
        Wo_1[j] = wVals[0][j + 3 * out_size];
    }
}

template <typename T, int in_sizet, int out_sizet>
void LSTMLayerT<T, in_sizet, out_sizet>::setUVals(const std::vector<std::vector<T>>& uVals)
{
    for(int i = 0; i < out_size; ++i)
    {
        for(int j = 0; j < out_size; ++j)
        {
            Ui[j][i] = uVals[i][j];
            Uf[j][i] = uVals[i][j + out_size];
            Uc[j][i] = uVals[i][j + 2 * out_size];
            Uo[j][i] = uVals[i][j + 3 * out_size];
        }
    }
}

template <typename T, int in_sizet, int out_sizet>
void LSTMLayerT<T, in_sizet, out_sizet>::setBVals(const std::vector<T>& bVals)
{
    for(int k = 0; k < out_size; ++k)
    {
        bi[k] = bVals[k];
        bf[k] = bVals[k + out_size];
        bc[k] = bVals[k + 2 * out_size];
        bo[k] = bVals[k + 3 * out_size];
    }
}

#endif // !RTNEURAL_USE_EIGEN && !RTNEURAL_USE_XSIMD

} // namespace RTNeural
