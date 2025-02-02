#include "conv1d.h"

namespace RTNeural
{

#if !RTNEURAL_USE_EIGEN && !RTNEURAL_USE_XSIMD && !RTNEURAL_USE_ACCELERATE

template <typename T>
Conv1D<T>::Conv1D(int in_size, int out_size, int kernel_size, int dilation)
    : Layer<T>(in_size, out_size)
    , dilation_rate(dilation)
    , kernel_size(kernel_size)
    , state_size(kernel_size * dilation)
{
    kernelWeights = new T**[out_size];
    for(int i = 0; i < out_size; ++i)
    {
        kernelWeights[i] = new T*[in_size];
        for(int k = 0; k < in_size; ++k)
        {
            kernelWeights[i][k] = new T[state_size];
            std::fill(kernelWeights[i][k], &kernelWeights[i][k][state_size], (T)0);
        }
    }

    bias = new T[out_size];

    state = new T*[in_size];
    for(int k = 0; k < in_size; ++k)
        state[k] = new T[2 * state_size];
}

template <typename T>
Conv1D<T>::Conv1D(std::initializer_list<int> sizes)
    : Conv1D<T>(*sizes.begin(), *(sizes.begin() + 1), *(sizes.begin() + 2), *(sizes.begin() + 3))
{
}

template <typename T>
Conv1D<T>::Conv1D(const Conv1D<T>& other)
    : Conv1D<T>(other.in_size, other.out_size, other.kernel_size, other.dilation_rate)
{
}

template <typename T>
Conv1D<T>& Conv1D<T>::operator=(const Conv1D<T>& other)
{
    if(&other != this)
        *this = Conv1D<T>(other);

    return *this;
}

template <typename T>
Conv1D<T>::~Conv1D()
{
    for(int i = 0; i < Layer<T>::out_size; ++i)
    {
        for(int k = 0; k < Layer<T>::in_size; ++k)
            delete[] kernelWeights[i][k];

        delete[] kernelWeights[i];
    }

    delete[] kernelWeights;
    delete[] bias;

    for(int k = 0; k < Layer<T>::in_size; ++k)
        delete[] state[k];
    delete[] state;
}

template <typename T>
void Conv1D<T>::reset()
{
    state_ptr = 0;
    for(int k = 0; k < Layer<T>::in_size; ++k)
        std::fill(state[k], &state[k][2 * state_size], (T)0);
}

template <typename T>
void Conv1D<T>::setWeights(const std::vector<std::vector<std::vector<T>>>& weights)
{
    for(int i = 0; i < Layer<T>::out_size; ++i)
        for(int k = 0; k < Layer<T>::in_size; ++k)
            for(int j = 0; j < kernel_size; ++j)
                kernelWeights[i][k][j * dilation_rate] = weights[i][k][j];
}

template <typename T>
void Conv1D<T>::setBias(const std::vector<T>& biasVals)
{
    for(int i = 0; i < Layer<T>::out_size; ++i)
        bias[i] = biasVals[i];
}

//====================================================
template <typename T, int in_sizet, int out_sizet, int kernel_size, int dilation_rate>
Conv1DT<T, in_sizet, out_sizet, kernel_size, dilation_rate>::Conv1DT()
{
    for(int i = 0; i < out_size; ++i)
        for(int j = 0; j < in_size; ++j)
            for(int k = 0; k < state_size; ++k)
                weights[i][j][k] = (T)0.0;

    for(int i = 0; i < out_size; ++i)
        bias[i] = (T)0.0;

    for(int i = 0; i < out_size; ++i)
        outs[i] = (T)0.0;

    reset();
}

template <typename T, int in_sizet, int out_sizet, int kernel_size, int dilation_rate>
void Conv1DT<T, in_sizet, out_sizet, kernel_size, dilation_rate>::reset()
{
    state_ptr = 0;
    for(int k = 0; k < in_size; ++k)
        for(int i = 0; i < 2 * state_size; ++i)
            state[k][i] = (T)0.0;
}

template <typename T, int in_sizet, int out_sizet, int kernel_size, int dilation_rate>
void Conv1DT<T, in_sizet, out_sizet, kernel_size, dilation_rate>::setWeights(const std::vector<std::vector<std::vector<T>>>& ws)
{
    for(int i = 0; i < out_size; ++i)
    {
        for(int k = 0; k < in_size; ++k)
        {
            for(int j = 0; j < kernel_size; ++j)
                weights[i][k][j * dilation_rate] = ws[i][k][j];
        }
    }
}

template <typename T, int in_sizet, int out_sizet, int kernel_size, int dilation_rate>
void Conv1DT<T, in_sizet, out_sizet, kernel_size, dilation_rate>::setBias(const std::vector<T>& biasVals)
{
    for(int i = 0; i < out_size; ++i)
        bias[i] = biasVals[i];
}

#endif

} // namespace RTNeural
