#ifndef ACTIVATIONXSIMD_H_INCLUDED
#define ACTIVATIONXSIMD_H_INCLUDED

#include "../common.h"

namespace RTNeural
{

/** Dynamic implementation of a tanh activation layer. */
template <typename T>
class TanhActivation : public Activation<T>
{
public:
    /** Constructs a tanh activation layer for a given size. */
    explicit TanhActivation(int size)
        : Activation<T>(size, {}, "tanh")
    {
    }

    TanhActivation(std::initializer_list<int> sizes)
        : TanhActivation(*sizes.begin())
    {
    }

    /** Performs forward propagation for tanh activation. */
    inline void forward(const T* input, T* out) noexcept override
    {
        tanh(input, out, Layer<T>::in_size);
    }
};

/** Static implementation of a tanh activation layer. */
template <typename T, int size>
class TanhActivationT
{
    using v_type = xsimd::simd_type<T>;
    static constexpr auto v_size = (int)v_type::size;
    static constexpr auto v_io_size = ceil_div(size, v_size);

public:
    static constexpr auto in_size = size;
    static constexpr auto out_size = size;

    TanhActivationT()
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = v_type((T)0);
    }

    /** Returns the name of this layer. */
    std::string getName() const noexcept { return "tanh"; }

    /** Returns true since this layer is an activation layer. */
    constexpr bool isActivation() const noexcept { return true; }

    void reset() { }

    /** Performs forward propagation for tanh activation. */
    inline void forward(const v_type (&ins)[v_io_size]) noexcept
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = xsimd::tanh(ins[i]);
    }

    v_type outs[v_io_size];
};

/** Dynamic implementation of an approximate tanh activation layer. */
template <typename T>
class FastTanh : public Activation<T>
{
public:
    /** Constructs a tanh activation layer for a given size. */
    explicit FastTanh(int size)
        : Activation<T>(size, {}, "tanh")
    {
    }

    FastTanh(std::initializer_list<int> sizes)
        : FastTanh(*sizes.begin())
    {
    }

    /** Performs forward propagation for tanh activation. */
    inline void forward(const T* input, T* out) noexcept override
    {
        fast_tanh(input, out, Layer<T>::in_size);
    }
};

/** Static implementation of an approximate tanh activation layer. */
template <typename T, int size>
class FastTanhT
{
    using v_type = xsimd::simd_type<T>;
    static constexpr auto v_size = (int)v_type::size;
    static constexpr auto v_io_size = ceil_div(size, v_size);

public:
    static constexpr auto in_size = size;
    static constexpr auto out_size = size;

    FastTanhT()
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = v_type((T)0);
    }

    /** Returns the name of this layer. */
    std::string getName() const noexcept { return "tanh"; }

    /** Returns true since this layer is an activation layer. */
    constexpr bool isActivation() const noexcept { return true; }

    void reset() { }

    /** Performs forward propagation for tanh activation. */
    inline void forward(const v_type (&ins)[v_io_size]) noexcept
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = fast_tanh<T>(ins[i]);
    }

    v_type outs[v_io_size];
};

/** Dynamic implementation of a ReLU activation layer. */
template <typename T>
class ReLuActivation : public Activation<T>
{
public:
    /** Constructs a ReLU activation layer for a given size. */
    explicit ReLuActivation(int size)
        : Activation<T>(size, {}, "relu")
    {
        zeros.resize(size, (T)0);
    }

    ReLuActivation(std::initializer_list<int> sizes)
        : ReLuActivation(*sizes.begin())
    {
    }

    /** Performs forward propagation for ReLU activation. */
    inline void forward(const T* input, T* out) noexcept override
    {
        xsimd::transform(
            input, &input[Layer<T>::in_size], zeros.begin(), out,
            [](auto const& a, auto const& b) { return xsimd::max(a, b); });
    }

    std::vector<T, xsimd::aligned_allocator<T>> zeros;
};

/** Static implementation of a ReLU activation layer. */
template <typename T, int size>
class ReLuActivationT
{
    using v_type = xsimd::simd_type<T>;
    static constexpr auto v_size = (int)v_type::size;
    static constexpr auto v_io_size = ceil_div(size, v_size);

public:
    static constexpr auto in_size = size;
    static constexpr auto out_size = size;

    ReLuActivationT()
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = v_type((T)0);
    }

    /** Returns the name of this layer. */
    std::string getName() const noexcept { return "relu"; }

    /** Returns true since this layer is an activation layer. */
    constexpr bool isActivation() const noexcept { return true; }

    void reset() { }

    /** Performs forward propagation for ReLU activation. */
    inline void forward(const v_type (&ins)[v_io_size]) noexcept
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = xsimd::max(ins[i], v_type((T)0));
    }

    v_type outs[v_io_size];
};

/** Dynamic implementation of a sigmoid activation layer. */
template <typename T>
class SigmoidActivation : public Activation<T>
{
public:
    /** Constructs a sigmoid activation layer for a given size. */
    explicit SigmoidActivation(int size)
        : Activation<T>(size, {}, "sigmoid")
    {
    }

    SigmoidActivation(std::initializer_list<int> sizes)
        : SigmoidActivation(*sizes.begin())
    {
    }

    /** Performs forward propagation for sigmoid activation. */
    inline void forward(const T* input, T* out) noexcept override
    {
        sigmoid(input, out, Layer<T>::in_size);
    }
};

/** Static implementation of a sigmoid activation layer. */
template <typename T, int size>
class SigmoidActivationT
{
    using v_type = xsimd::simd_type<T>;
    static constexpr auto v_size = (int)v_type::size;
    static constexpr auto v_io_size = ceil_div(size, v_size);

public:
    static constexpr auto in_size = size;
    static constexpr auto out_size = size;

    SigmoidActivationT()
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = v_type((T)0);
    }

    /** Returns the name of this layer. */
    std::string getName() const noexcept { return "sigmoid"; }

    /** Returns true since this layer is an activation layer. */
    constexpr bool isActivation() const noexcept { return true; }

    void reset() { }

    /** Performs forward propagation for sigmoid activation. */
    inline void forward(const v_type (&ins)[v_io_size]) noexcept
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = (T)1.0 / ((T)1.0 + xsimd::exp(-ins[i]));
    }

    v_type outs[v_io_size];
};

/** Dynamic implementation of a softmax activation layer. */
template <typename T>
class SoftmaxActivation : public Activation<T>
{
public:
    /** Constructs a softmax activation layer for a given size. */
    explicit SoftmaxActivation(int size)
        : Activation<T>(size, {}, "softmax")
    {
    }

    SoftmaxActivation(std::initializer_list<int> sizes)
        : SoftmaxActivation(*sizes.begin())
    {
    }

    /** Performs forward propagation for softmax activation. */
    inline void forward(const T* input, T* out) noexcept override
    {
        softmax(input, out, Layer<T>::in_size);
    }
};

/** Static implementation of a softmax activation layer. */
template <typename T, int size>
class SoftmaxActivationT
{
    using v_type = xsimd::simd_type<T>;
    static constexpr auto v_size = (int)v_type::size;
    static constexpr auto v_io_size = ceil_div(size, v_size);

public:
    static constexpr auto in_size = size;
    static constexpr auto out_size = size;

    SoftmaxActivationT()
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = v_type((T)0);
    }

    /** Returns the name of this layer. */
    std::string getName() const noexcept { return "softmax"; }

    /** Returns true since this layer is an activation layer. */
    constexpr bool isActivation() const noexcept { return true; }

    void reset() { }

    /** Performs forward propagation for softmax activation. */
    inline void forward(const v_type (&ins)[v_io_size]) noexcept
    {
        v_type exp_sum {};
        for(int i = 0; i < v_io_size; ++i)
        {
            outs[i] = xsimd::exp(ins[i]);
            exp_sum += outs[i];
        }

        const auto exp_sum_recip = v_type((T)1 / xsimd::reduce_add(exp_sum));
        for(int i = 0; i < v_io_size; ++i)
            outs[i] *= exp_sum_recip;
    }

    v_type outs[v_io_size];
};

/** Dynamic implementation of a elu activation layer. */
template <typename T>
class ELuActivation final : public Activation<T>
{
public:
    /** Constructs a elu activation layer for a given size. */
    explicit ELuActivation(int size)
        : Activation<T>(
            size, {}, "elu")
    {
    }

    ELuActivation(std::initializer_list<int> sizes)
        : ELuActivation(*sizes.begin())
    {
    }

    /** Performs forward propagation for softmax activation. */
    inline void forward(const T* input, T* out) noexcept override
    {
        elu(input, out, Layer<T>::in_size, alpha);
    }

    /** Sets a custom value for the layer's "alpha" parameter. */
    void set_alpha(T newAlpha) { alpha = newAlpha; }

private:
    T alpha = (T)1;
};

/** Static implementation of a elu activation layer. */
template <typename T, int size, int AlphaNumerator = 1, int AlphaDenominator = 1>
class ELuActivationT
{
    using v_type = xsimd::simd_type<T>;
    static constexpr auto v_size = (int)v_type::size;
    static constexpr auto v_io_size = ceil_div(size, v_size);

public:
    static constexpr auto in_size = size;
    static constexpr auto out_size = size;

    ELuActivationT() = default;

    /** Returns the name of this layer. */
    std::string getName() const noexcept { return "elu"; }

    /** Returns true since this layer is an activation layer. */
    constexpr bool isActivation() const noexcept { return true; }

    void reset() { }

    /** Performs forward propagation for elu activation. */
    template <int A_N = AlphaNumerator, int A_D = AlphaDenominator>
    inline typename std::enable_if<A_N == 1 && A_D == 1, void>::type
    forward(const v_type (&ins)[v_io_size]) noexcept
    {
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = xsimd::select(ins[i] > (T)0, ins[i], xsimd::exp(ins[i]) - (T)1);
    }

    /** Performs forward propagation for elu activation (with custom alpha parameter). */
    template <int A_N = AlphaNumerator, int A_D = AlphaDenominator>
    inline typename std::enable_if<A_N != 1 || A_D != 1, void>::type
    forward(const v_type (&ins)[v_io_size]) noexcept
    {
        static constexpr T alpha = (T)AlphaNumerator / (T)AlphaDenominator;
        for(int i = 0; i < v_io_size; ++i)
            outs[i] = xsimd::select(ins[i] > (T)0, ins[i], alpha * (xsimd::exp(ins[i]) - (T)1));
    }

    v_type outs[v_io_size];
};

} // namespace RTNeural

#endif // ACTIVATIONXSIMD_H_INCLUDED
