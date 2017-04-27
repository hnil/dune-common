// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_COMMON_TEST_ARITHMETICTESTSUITE_HH
#define DUNE_COMMON_TEST_ARITHMETICTESTSUITE_HH

#include <string>
#include <type_traits>
#include <utility>

#include <dune/common/classname.hh>
#include <dune/common/test/testsuite.hh>

namespace Dune {

  //! Test suite for arithmetic types
  /**
   * You usually want to call the member function `checkArithmetic()`.  The
   * individual component tests are however available for special needs.
   */
  class ArithmeticTestSuite :
    public TestSuite
  {
  public:
    //! tag denoting any arithmetic type
    struct Arithmetic            {};
    //! tag denoting integral types
    struct Integral : Arithmetic {};
    //! tag denoting boolean types
    struct Boolean  : Integral   {};
    //! tag denoting signed integral types
    struct Signed   : Integral   {};
    //! tag denoting unsigned integral types
    struct Unsigned : Integral   {};
    //! tag denoting floating point types
    struct Floating : Arithmetic {};

  private:
    static const char *name(Arithmetic) { return "Arithmetic"; }
    static const char *name(Integral  ) { return "Integral";   }
    static const char *name(Boolean   ) { return "Boolean";    }
    static const char *name(Signed    ) { return "Signed";     }
    static const char *name(Unsigned  ) { return "Unsigned";   }
    static const char *name(Floating  ) { return "Floating";   }

    template<class C, class Then, class Else = void>
    using Cond = typename std::conditional<C::value, Then, Else>::type;

  public:
    //! determine arithmetic tag for the given type
    /**
     * `T` can be either one of the fundamental arithmetic types, in which
     * case a default-constructed tag object for that type is returned.  Or it
     * can be a class derived from `Arithmetic`, in which case a
     * default-constructed object of that class is is returned.
     */
    template<class T>
    constexpr static auto tag(T = T{})
    {
      return
        Cond<std::is_convertible<T, Arithmetic>, T,
        Cond<std::is_floating_point<T>,          Floating,
        Cond<std::is_integral<T>,
             Cond<std::is_same<bool, T>,         Boolean,
             Cond<std::is_signed<T>,             Signed,
             Cond<std::is_unsigned<T>,           Unsigned
                  > > >
             > > >{};
    }

#define DUNE_TEST_FUNCTION(T, tag)                                      \
    static const auto function =                                        \
    std::string(__func__) + "<" + className<T>() + ">(" + name(tag) + ")"

#define DUNE_TEST_CHECK(expr)                                           \
    (check((expr), function)                                            \
     << __FILE__ << ":" << __LINE__ << ": Check \"" << #expr << "\"")

    //
    // check basic operations: construct, copy, compare
    //

    //! check the default constructors
    template<class T>
    void checkDefaultConstruct(Arithmetic tag)
    {
      // the conversion to void stops the compiler from complaining about
      // unused variables
      T t0;      (void)t0;
      (void)T();
      T t1{};    (void)t1;
      T t2 = {}; (void)t2;
    }

    //! check explicit conversion from and to int
    template<class T>
    void checkExplicitIntConvert(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);
      // this test may be applied to boolean-type types. 0 and 1 are the only
      // values that survive that.
      T t0(0); DUNE_TEST_CHECK(int(t0) == 0);
      T t1(1); DUNE_TEST_CHECK(int(t1) == 1);
    }

    //! check the move constructor
    template<class T>
    void checkMoveConstruct(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);
      for(int i : { 0, 1 })
      {
        T t0(i);

        T t1(std::move(t0));
        T t2 = std::move(t1);
        T t3{ std::move(t2) };
        T t4 = { std::move(t3) };

        DUNE_TEST_CHECK(bool(t4 == T(i)));
      }
    }

    //! check the copy constructor
    template<class T>
    void checkCopyConstruct(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);
      for(int i : { 0, 1 })
      {
        T t0(i);

        T t1(t0);
        T t2 = t1;
        T t3{ t2 };
        T t4 = { t3 };

        DUNE_TEST_CHECK(bool(t0 == T(i)));
        DUNE_TEST_CHECK(bool(t1 == T(i)));
        DUNE_TEST_CHECK(bool(t2 == T(i)));
        DUNE_TEST_CHECK(bool(t3 == T(i)));
        DUNE_TEST_CHECK(bool(t4 == T(i)));
      }
    }

    //! check the move assignment operator
    template<class T>
    void checkMoveAssign(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);
      for(int i : { 0, 1 })
      {
        T t0(i);
        T t2, t4;

        t2 = std::move(t0);
        t4 = { std::move(t2) };

        DUNE_TEST_CHECK(bool(t4 == T(i)));
      }
    }

    //! check the copy assignment operator
    template<class T>
    void checkCopyAssign(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);
      for(int i : { 0, 1 })
      {
        T t0(i);
        T t2, t4;

        t2 = t0;
        t4 = { t2 };

        DUNE_TEST_CHECK(bool(t0 == T(i)));
        DUNE_TEST_CHECK(bool(t2 == T(i)));
        DUNE_TEST_CHECK(bool(t4 == T(i)));
      }
    }

    //! check `==` and `!=`
    /**
     * \note We do not require the result to be _implicitly_ convertible to
     *       bool, but it must be contextually convertible to bool.
     */
    template<class T>
    void checkEqual(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);
      T t0(0);
      T t1(1);

      DUNE_TEST_CHECK(bool(t0 == T(0)));
      DUNE_TEST_CHECK(bool(t1 == T(1)));

      DUNE_TEST_CHECK(!bool(t0 == T(1)));
      DUNE_TEST_CHECK(!bool(t1 == T(0)));
      DUNE_TEST_CHECK(!bool(t0 == t1));

      DUNE_TEST_CHECK(!bool(t0 != T(0)));
      DUNE_TEST_CHECK(!bool(t1 != T(1)));

      DUNE_TEST_CHECK(bool(t0 != T(1)));
      DUNE_TEST_CHECK(bool(t1 != T(0)));
      DUNE_TEST_CHECK(bool(t0 != t1));
    }

    //
    // checks for unary operators
    //

    //! check postfix `++`
    /**
     * Applies to boolean (deprecated), integral, and floating point.
     */
    template<class T>
    void checkPostfixInc(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      T t0(0);
      DUNE_TEST_CHECK(bool(T(t0++) == T(0)));
      DUNE_TEST_CHECK(bool(t0 == T(1)));
    }
    template<class T>
    void checkPostfixInc(Boolean) {}

    //! check postfix `--`
    /**
     * Applies to integral (no boolean), and floating point.
     */
    template<class T>
    void checkPostfixDec(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      T t1(1);
      DUNE_TEST_CHECK(bool(T(t1--) == T(1)));
      DUNE_TEST_CHECK(bool(t1 == T(0)));
    }
    template<class T>
    void checkPostfixDec(Boolean) {}

    //! check prefix `+`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkPrefixPlus(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(+T(0)) == T(0)));
      DUNE_TEST_CHECK(bool(T(+T(1)) == T(1)));
    }

    //! check prefix `-`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkPrefixMinus(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(-T(0)) == T( 0)));
      DUNE_TEST_CHECK(bool(T(-T(1)) == T(-1)));
    }

    //! check prefix `!`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkPrefixNot(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(!T(0)));
      DUNE_TEST_CHECK(!bool(!T(1)));
    }

    //! check prefix `~`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkPrefixBitNot(Boolean tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(~T(0))));
    }
    template<class T>
    void checkPrefixBitNot(Integral tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(~T(0))));
      DUNE_TEST_CHECK(bool(T(~T(1))));

      DUNE_TEST_CHECK(bool(T(~T(~T(0))) == T(0)));
      DUNE_TEST_CHECK(bool(T(~T(~T(1))) == T(1)));
    }
    template<class T>
    void checkPrefixBitNot(Unsigned tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      checkPrefixBitNot<T>(Integral{});

      DUNE_TEST_CHECK(bool(T(~T(0)) == T(-1)));
      DUNE_TEST_CHECK(bool(T(~T(1)) == T(-2)));
    }
    template<class T>
    void checkPrefixBitNot(Floating) {}

    //! check postfix `++`
    /**
     * Applies to boolean (deprecated), integral, and floating point.
     */
    template<class T>
    void checkPrefixInc(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      T t0(0);
      DUNE_TEST_CHECK(bool(T(++t0) == T(1)));
      DUNE_TEST_CHECK(bool(t0 == T(1)));
      ++t0 = T(0);
      DUNE_TEST_CHECK(bool(t0 == T(0)));
    }
    template<class T>
    void checkPrefixInc(Boolean) {}

    //! check postfix `--`
    /**
     * Applies to integral (no boolean), and floating point.
     */
    template<class T>
    void checkPrefixDec(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      T t1(1);
      DUNE_TEST_CHECK(bool(T(--t1) == T(0)));
      DUNE_TEST_CHECK(bool(t1 == T(0)));
      t1 = T(1);
      --t1 = T(1);
      DUNE_TEST_CHECK(bool(t1 == T(1)));
    }
    template<class T>
    void checkPrefixDec(Boolean) {}

    //
    // checks for infox operators
    //

    //! check infix `*`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixMul(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)*T(0)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)*T(0)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(0)*T(1)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)*T(1)) == T(1)));
    }

    //! check infix `/`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixDiv(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)/T(1)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)/T(1)) == T(1)));
    }

    //! check infix `%`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkInfixRem(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)%T(1)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)%T(1)) == T(0)));
    }
    template<class T>
    void checkInfixRem(Floating) {}

    //! check infix `+`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixPlus(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)+T(0)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)+T(0)) == T(1)));
      DUNE_TEST_CHECK(bool(T(T(0)+T(1)) == T(1)));
      DUNE_TEST_CHECK(bool(T(T(1)+T(1)) == T(2)));
    }

    //! check infix `-`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixMinus(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)-T(0)) == T( 0)));
      DUNE_TEST_CHECK(bool(T(T(1)-T(0)) == T( 1)));
      DUNE_TEST_CHECK(bool(T(T(0)-T(1)) == T(-1)));
      DUNE_TEST_CHECK(bool(T(T(1)-T(1)) == T( 0)));
    }

    //! check infix `<<`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkInfixLShift(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)<<T(0)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)<<T(0)) == T(1)));
      DUNE_TEST_CHECK(bool(T(T(0)<<T(1)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)<<T(1)) == T(2)));
    }
    template<class T>
    void checkInfixLShift(Floating) {}

    //! check infix `>>`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkInfixRShift(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(T(0)>>T(0)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)>>T(0)) == T(1)));
      DUNE_TEST_CHECK(bool(T(T(0)>>T(1)) == T(0)));
      DUNE_TEST_CHECK(bool(T(T(1)>>T(1)) == T(0)));
    }
    template<class T>
    void checkInfixRShift(Floating) {}

    //! check infix `<`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixLess(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      DUNE_TEST_CHECK(bool(T(0)<T(0)) == false);
      DUNE_TEST_CHECK(bool(T(1)<T(0)) == false);
      DUNE_TEST_CHECK(bool(T(0)<T(1)) == true );
      DUNE_TEST_CHECK(bool(T(1)<T(1)) == false);
    }
    template<class T>
    void checkInfixLess(Signed tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      checkInfixLess<T>(Integral{});

      DUNE_TEST_CHECK(bool(T(-1)<T( 0)) == true);
    }
    template<class T>
    void checkInfixLess(Unsigned tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      checkInfixLess<T>(Integral{});

      DUNE_TEST_CHECK(bool(T(-1)<T( 0)) == false);
    }

    //! check infix `>`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixGreater(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      int values[] = { -1, 0, 1 };
      for(int i : values)
        for(int j : values)
          DUNE_TEST_CHECK(bool(T(i) > T(j)) == bool(T(j) < T(i)));
    }

    //! check infix `<=`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixLessEqual(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      int values[] = { -1, 0, 1 };
      for(int i : values)
        for(int j : values)
          DUNE_TEST_CHECK(bool(T(i) <= T(j)) != bool(T(j) < T(i)));
    }

    //! check infix `>=`
    /**
     * Applies to boolean, integral, and floating point.
     */
    template<class T>
    void checkInfixGreaterEqual(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      int values[] = { -1, 0, 1 };
      for(int i : values)
        for(int j : values)
          DUNE_TEST_CHECK(bool(T(i) >= T(j)) != bool(T(i) < T(j)));
    }

    //! check infix `&`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkInfixBitAnd(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
          DUNE_TEST_CHECK(bool(T(T(i) & T(j)) == T(i&j)));
    }
    template<class T>
    void checkInfixBitAnd(Boolean tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 2; ++i)
        for(int j = 0; j < 2; ++j)
          DUNE_TEST_CHECK(bool(T(T(i) & T(j)) == T(i&j)));
    }
    template<class T>
    void checkInfixBitAnd(Floating) {}

    //! check infix `^`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkInfixBitXor(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
          DUNE_TEST_CHECK(bool(T(T(i) ^ T(j)) == T(i^j)));
    }
    template<class T>
    void checkInfixBitXor(Boolean tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 2; ++i)
        for(int j = 0; j < 2; ++j)
          // compare the bit-flipped versions so we don't depend on the number
          // of bits in T.
          DUNE_TEST_CHECK(bool(T(~T(T(i) ^ T(j))) == T(~(i^j))));
    }
    template<class T>
    void checkInfixBitXor(Floating) {}

    //! check infix `|`
    /**
     * Applies to boolean and integral.
     */
    template<class T>
    void checkInfixBitOr(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
          DUNE_TEST_CHECK(bool(T(T(i) | T(j)) == T(i|j)));
    }
    template<class T>
    void checkInfixBitOr(Boolean tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 2; ++i)
        for(int j = 0; j < 2; ++j)
          DUNE_TEST_CHECK(bool(T(T(i) | T(j)) == T(i|j)));
    }
    template<class T>
    void checkInfixBitOr(Floating) {}

    //! check infix `&&`
    /**
     * Applies to boolean, integral and floating point.
     */
    template<class T>
    void checkInfixAnd(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
          DUNE_TEST_CHECK(bool(T(i) && T(j)) == (i && j));
    }

    //! check infix `||`
    /**
     * Applies to boolean, integral and floating point.
     */
    template<class T>
    void checkInfixOr(Arithmetic tag)
    {
      DUNE_TEST_FUNCTION(T, tag);

      for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
          DUNE_TEST_CHECK(bool(T(i) || T(j)) == (i || j));
    }

    //
    // checks for compound assignment operators
    //

#define DUNE_TEST_PEEL(...) __VA_ARGS__
#define DUNE_TEST_ASSIGN(OP, name, Tag, lrange, rrange)                 \
    template<class T>                                                   \
    void checkAssign##name(Tag tag)                                     \
    {                                                                   \
      DUNE_TEST_FUNCTION(T, tag);                                       \
                                                                        \
      for(int i : { DUNE_TEST_PEEL lrange })                            \
        for(int j : { DUNE_TEST_PEEL rrange })                          \
        {                                                               \
          T t(i);                                                       \
          DUNE_TEST_CHECK(bool((t OP##= T(j)) == T(T(i) OP T(j))));     \
          DUNE_TEST_CHECK(bool(t == T(T(i) OP T(j))));                  \
        }                                                               \
    }

#define DUNE_TEST_ASSIGN_DISABLE(name, Tag)     \
    template<class T>                           \
    void checkAssign##name(Tag) {}

    DUNE_TEST_ASSIGN(*,      Mul,    Arithmetic, (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN(/,      Div,    Arithmetic, (0, 1, 2, 3), (   1, 2, 3))
    DUNE_TEST_ASSIGN(%,      Rem,    Arithmetic, (0, 1, 2, 3), (   1, 2, 3))
    DUNE_TEST_ASSIGN_DISABLE(Rem,    Floating)

    DUNE_TEST_ASSIGN(+,      Plus,   Arithmetic, (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN(-,      Minus,  Arithmetic, (0, 1, 2, 3), (0, 1, 2, 3))

    DUNE_TEST_ASSIGN(<<,     LShift, Integral,   (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN(>>,     RShift, Integral,   (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN(<<,     LShift, Boolean,    (0, 1      ), (0, 1      ))
    DUNE_TEST_ASSIGN(>>,     RShift, Boolean,    (0, 1      ), (0, 1      ))
    DUNE_TEST_ASSIGN_DISABLE(LShift, Floating)
    DUNE_TEST_ASSIGN_DISABLE(RShift, Floating)

    DUNE_TEST_ASSIGN(&,      BitAnd, Integral,   (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN(^,      BitXor, Integral,   (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN(|,      BitOr,  Integral,   (0, 1, 2, 3), (0, 1, 2, 3))
    DUNE_TEST_ASSIGN_DISABLE(BitAnd, Floating)
    DUNE_TEST_ASSIGN_DISABLE(BitXor, Floating)
    DUNE_TEST_ASSIGN_DISABLE(BitOr,  Floating)

#undef DUNE_TEST_ASSIGN_DISABLE
#undef DUNE_TEST_ASSIGN
#undef DUNE_TEST_PEEL
#undef DUNE_TEST_FUNCTION
#undef DUNE_TEST_CHECK

    //
    // checks collections
    //

    //! run the full arithmetic type testsuite
    /**
     * `T` is the type to check.  `Tag` determines the arithmetic category; it
     * is one of the classes derived from `Arithmetic`.  Alternatively, `Tag`
     * may be one of the fundamental arithmetic types, in which case it will
     * be converted to the appropriate category tag automatically.
     *
     * To check an extended unsigned integer type, you might use one of the
     * following calls:
     * \code
     * test.checkArithmetic<MyExtUnsigned, unsigned>();
     * test.checkArithmetic<MyExtUnsigned>(0u);
     * test.checkArithmetic<MyExtUnsigned, ArithemticTestSuite::Unsigned>();
     * test.checkArithmetic<MyExtUnsigned>(ArithemticTestSuite::Unsigned{});
     * \endcode
     */
    template<class T, class Tag>
    void checkArithmetic(Tag = Tag{})
    {
      auto tag = this->tag<Tag>();

      checkDefaultConstruct<T>(tag);
      checkExplicitIntConvert<T>(tag);
      checkMoveConstruct<T>(tag);
      checkCopyConstruct<T>(tag);
      checkMoveAssign<T>(tag);
      checkCopyAssign<T>(tag);
      checkEqual<T>(tag);

      checkPostfixInc<T>(tag);
      checkPostfixDec<T>(tag);

      checkPrefixPlus<T>(tag);
      checkPrefixMinus<T>(tag);
      checkPrefixNot<T>(tag);
      checkPrefixBitNot<T>(tag);

      checkPrefixInc<T>(tag);
      checkPrefixDec<T>(tag);

      checkInfixMul<T>(tag);
      checkInfixDiv<T>(tag);
      checkInfixRem<T>(tag);

      checkInfixPlus<T>(tag);
      checkInfixMinus<T>(tag);

      checkInfixLShift<T>(tag);
      checkInfixRShift<T>(tag);

      checkInfixLess<T>(tag);
      checkInfixGreater<T>(tag);
      checkInfixLessEqual<T>(tag);
      checkInfixGreaterEqual<T>(tag);

      checkInfixBitAnd<T>(tag);
      checkInfixBitXor<T>(tag);
      checkInfixBitOr<T>(tag);

      checkInfixAnd<T>(tag);
      checkInfixOr<T>(tag);

      checkAssignMul<T>(tag);
      checkAssignDiv<T>(tag);
      checkAssignRem<T>(tag);

      checkAssignPlus<T>(tag);
      checkAssignMinus<T>(tag);

      checkAssignLShift<T>(tag);
      checkAssignRShift<T>(tag);

      checkAssignBitAnd<T>(tag);
      checkAssignBitXor<T>(tag);
      checkAssignBitOr<T>(tag);
    }
  };

} // namespace Dune

#endif // DUNE_COMMON_TEST_ARITHMETICTESTSUITE_HH
