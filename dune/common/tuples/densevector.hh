#ifndef DUNE_COMMON_TUPLES_DENSEVECTOR_HH
#define DUNE_COMMON_TUPLES_DENSEVECTOR_HH

#include <cassert>
#include <iosfwd>

#include <dune/common/bartonnackmanifcheck.hh>
#include <dune/common/forloop.hh>
#include <dune/common/genericiterator.hh>
#include <dune/common/nullptr.hh>

#include <dune/common/tuples/tuples.hh>
#include <dune/common/tuples/typetraits.hh>

namespace Dune
{

  // Internal forward declaration
  // ----------------------------

  template< class Imp >
  struct DenseVectorTuple;



  // RawTuple< DenseVectorTuple >
  // ----------------------------

  template< class Imp >
  struct RawTuple< DenseVectorTuple< Imp > >
  {
    typedef typename RawTuple< Imp >::Type Type;

    static Type &raw ( DenseVectorTuple< Imp > &denseVectorTuple )
    {
      return raw_tuple( static_cast< Imp & >( denseVectorTuple ) );
    }
  };



  // DenseVectorTupleTraits
  // ----------------------

  /** \ingroup Tuples_DenseVector
   *
   *  \brief Traits class for a dense vector tuple
   *         implementation.
   *
   *  Class outline:
   *  @code
   *  template< Implementation >
   *  struct DenseVectorTupleTraits< Implementation >
   *  {
   *    // implementation type
   *    typedef Implementation derived_type;
   *    // raw tuple type
   *    typedef ImplementationDefined tuple;
   *
   *    // field type
   *    typedef ImplementationDefined field_type;
   *    // size type
   *    typedef ImplementationDefined size_type;
   *    // field type tuple of equal length
   *    typedef ImplementationDefined field_type_tuple;
   *  };
   *  @endcode
   *
   * \note For each implementation of a dense vector tuple a template
   *       specialization of this class is needed.
   */
  template< class Imp >
  struct DenseVectorTupleTraits;




  // DenseVectorTuple
  // ----------------

  /** \ingroup Tuples_DenseVector
   *
   *  \brief Facade class for dense vector tuples.
   *
   *  This facade class adds support for a large number
   *  of dense vector functionality (cf. Dune::DenseVector).
   *
   *  \tparam  Imp  Implementation type
   *
   *  \note For implementors: The implementation is
   *        required to specialize Dune::RawTuple.
   */
  template< class Imp >
  class DenseVectorTuple
  {
    typedef DenseVectorTuple< Imp > This;

  public:
    //! \brief traits class
    typedef DenseVectorTupleTraits< Imp > Traits;

    //! \brief wrapped Dune::tuple type
    typedef typename Traits::tuple tuple;
    //! \brief size of tuple
    static const int tuple_size = tuple_size< tuple >::value;

    //! \brief field type
    typedef typename Traits::field_type field_type;
    //! \brief field type
    typedef field_type value_type;
    //! \brief field type tuple of equal length
    typedef typename Traits::field_type_tuple field_type_tuple;

    //! \brief size type
    typedef typename Traits::size_type size_type;

    //! \brief derived type
    typedef typename Traits::derived_type derived_type;

    //! \brief iterator type
    typedef GenericIterator< This, field_type > iterator;
    //! \brief const iterator type
    typedef GenericIterator< const This, const field_type > const_iterator;


    ////////////////
    // assignment //
    ////////////////

  private:
    template< int i >
    struct AssignFunctor
    {
      static void apply ( tuple &t, const field_type &alpha )
      {
        get< i >( t ) = typename tuple_element< i, tuple >::type( alpha );
      }
    };

  public:
    //! \brief assignment from scalar
    derived_type &operator= ( const field_type &alpha )
    {
      ForLoop< AssignFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), alpha );
      return asImp();
    }


    ////////////////////////////////////////
    // random access and iterator methods //
    ////////////////////////////////////////

  private:
    template< int i >
    struct ApplyFunctor
    {
      template< class Function >
      static void apply ( const tuple &t, Function function, int &j )
      {
        const typename tuple_element< i, tuple >::type &x = get< i >( t );
        const size_type size = x.size();
        for( size_type k = 0; k < size; ++k )
          function( x[ k ], j++ );
      }

      template< class Function >
      static void apply ( tuple &t, Function function, int &j )
      {
        typename tuple_element< i, tuple >::type &x = get< i >( t );
        const size_type size = x.size();
        for( size_type k = 0; k < size; ++k )
          function( x[ k ], j++ );
      }
    };

    struct FindFunctor
    {
      FindFunctor ( int i, const field_type *&x ) : i_( i ), x_( x ) {}

      void operator() ( const field_type &x, size_type j )
      {
        if( j == i_ )
          x_ = &x;
      }

    private:
      size_type i_;
      const field_type *&x_;
    };

  public:
    /** \brief iterate over all scalar entries in dense vector tuple
     *
     *  \tparam  Function  some function
     *
     *  \param[in]  function  function object
     *
     *  \note The argument must be copyable and is expected to fulfill the
     *        following interface:
\code
  struct Function
  {
    void operator() ( field_type &x, size_type i );
  };
\endcode
     *
     *  \returns function object (see std::for_each)
     */
    template< class Function >
    Function for_each ( Function function )
    {
      int i = 0;
      ForLoop< ApplyFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), function, i );
      return function;
    }

    /** \brief iterate over all scalar entries in dense vector tuple
     *
     *  \tparam  Function  some function
     *
     *  \param[in]  function  function object
     *
     *  \note The argument must be copyable and is expected to fulfill the
     *        following interface:
\code
  struct Function
  {
    void operator() ( const field_type &x, size_type i );
  };
\endcode
     *
     *  \returns function object (see std::for_each)
     */
    template< class Function >
    Function for_each ( Function function ) const
    {
      int i = 0;
      ForLoop< ApplyFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), function, i );
      return function;
    }

#ifndef DOXYGEN
  protected:
    // random access to scalar entries is too expensive
    // currently, we need it to implement methods begin(), end()

    template< class, class, class, class, template< class, class, class, class > class > friend class GenericIterator;

    field_type &operator[] ( size_type i )
    {
      return const_cast< field_type & >( static_cast< const This & >( *this ).operator[]( i ) );
    }

    const field_type &operator[] ( size_type i ) const
    {
      const field_type *x = nullptr;
      FindFunctor functor( i, x );
      for_each( functor );
      assert( x );
      return *x;
    }
#endif // #ifndef DOXYGEN

  public:
    //! \brief return begin iterator
    iterator begin () { return iterator( *this, 0 ); }
    //! \brief return end iterator
    iterator end (){ return iterator( *this, size() ); }

    //! \brief return begin const iterator
    const_iterator begin () const { return const_iterator( *this, 0 ); }
    //! \brief return end const iterator
    const_iterator end () const { return const_iterator( *this, size() ); }


    //////////
    // size //
    //////////

  private:
    template< int i >
    struct SizeFunctor
    {
      static void apply ( const tuple &t, size_type &size )
      {
        size += get< i >( t ).size();
      }
    };

  public:
    //! \brief return number of entries (sum over all tuple elements)
    size_type size () const
    {
      size_type size = size_type( 0 );
      ForLoop< SizeFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), size );
      return size;
    }


    /////////////////////////////
    // vector space arithmetic //
    /////////////////////////////

  private:
    template< int i >
    struct AxpyFunctor
    {
      template< class other >
      static void apply ( tuple &x, const field_type alpha, const other &y )
      {
        get< i >( x ).axpy( alpha, get< i >( y ) );
      }
    };

    template< int i >
    struct ScaleFunctor
    {
      static void apply ( tuple &t, const field_type &alpha )
      {
        get< i >( t ) *= alpha;
      }
    };

  public:
    //! \brief \f$x += y\f$
    template< class Other >
    derived_type &operator+= ( const DenseVectorTuple< Other > &other )
    {
      axpy( field_type( 1 ), other );
      return asImp();
    }

    //! \brief \f$x -= y\f$
    template< class Other >
    derived_type &operator-= ( const DenseVectorTuple< Other > &other )
    {
      axpy( field_type( -1 ), other );
      return asImp();
    }

    //! \brief \f$x + y\f$
    template< class Other >
    derived_type operator+ ( const DenseVectorTuple< Other > &other ) const
    {
      derived_type ret = asImp();
      return ( ret += other );
    }

    //! \brief \f$x - y\f$
    template< class Other >
    derived_type operator- ( const DenseVectorTuple< Other > &other ) const
    {
      derived_type ret = asImp();
      return ( ret -= other );
    }

    //! \brief \f$x += \alpha\f$
    derived_type &operator+= ( const field_type &alpha )
    {
      derived_type other( alpha );
      return ( (*this) += other );
    }

    //! \brief \f$x -= \alpha\f$
    derived_type &operator-= ( const field_type &alpha )
    {
      derived_type other( alpha );
      return ( (*this) -= other );
    }

    //! \brief \f$x *= \alpha\f$
    derived_type &operator*= ( const field_type &alpha )
    {
      ForLoop< ScaleFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), alpha );
      return asImp();
    }

    //! \brief \f$x /= \alpha\f$
    derived_type &operator/= ( const field_type &alpha )
    {
      return ( (*this) *= field_type( 1 )/alpha );
    }

    //! \brief \f$x += \alpha y\f$
    template< class Other >
    derived_type &axpy ( const field_type &alpha, const DenseVectorTuple< Other > &other )
    {
      ForLoop< AxpyFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), alpha, raw_tuple( other ) );
      return asImp();
    }


    ///////////
    // norms //
    ///////////

  private:
    struct OneNorm
    {
      template< class T >
      static typename T::field_type apply ( const T &x ) { return x.one_norm(); }
    };

    struct OneNormReal
    {
      template< class T >
      static typename T::field_type apply ( const T &x ) { return x.one_norm_real(); }
    };

    struct TwoNorm
    {
      template< class T >
      static typename T::field_type apply ( const T &x ) { return x.two_norm(); }
    };

    struct TwoNorm2
    {
      template< class T >
      static typename T::field_type apply ( const T &x ) { return x.two_norm2(); }
    };

    struct InfinityNorm
    {
      template< class T >
      static typename T::field_type apply ( const T &x ) { return x.infinity_norm(); }
    };

    struct InfinityNormReal
    {
      template< class T >
      static typename T::field_type apply ( const T &x ) { return x.infinity_norm_real(); }
    };

    template< int i >
    struct NormFunctor
    {
      template< class Tuple, class FieldTypeTuple, class Evaluate >
      static void apply ( const Tuple &tuple, FieldTypeTuple &values, const Evaluate & )
      {
        get< i >( values ) = Evaluate::apply( get< i >( tuple ) );
      }
    };

    template< class Norm >
    field_type_tuple norm ( const Norm &type ) const
    {
      field_type_tuple ret;
      ForLoop< NormFunctor, 0, tuple_size-1 >::apply( raw_tuple( *this ), ret, type );
      return ret;
    }

  public:
    //! \brief return one norm
    field_type_tuple one_norm () const { return norm( OneNorm() ); }
    //! \brief return one norm, resul is real
    field_type_tuple one_norm_real () const { return norm( OneNormReal() ); }
    //! \brief return two norm
    field_type_tuple two_norm () const { return norm( TwoNorm() ); }
    //! \brief return two norm square
    field_type_tuple two_norm2 () const { return norm( TwoNorm2() ); }
    //! \brief return infinity norm
    field_type_tuple infinity_norm () const { return norm( InfinityNorm() ); }
    //! \brief return infinity norm, result is real
    field_type_tuple infinity_norm_real () const { return norm( InfinityNormReal() ); }

  private:
    Imp &asImp() { return static_cast< Imp & >( *this ); }
    const Imp &asImp() const { return static_cast< const Imp & >( *this ); }
  };



  // std::ostream &operator<< for DenseVectorTuple
  // ---------------------------------------------

  namespace
  {
    template< int i >
    struct PrintFunctor
    {
      template< class Tuple >
      static void apply ( std::ostream &out, const Tuple &tuple )
      {
        out << get< i >( tuple );
        if( i < tuple_size< Tuple >::value-1 )
          out << " ";
      }
    };

  } // namespace

  template< class Imp >
  std::ostream &operator<< ( std::ostream& out, const DenseVectorTuple< Imp > &tuple )
  {
    ForLoop< PrintFunctor, 0, DenseVectorTuple< Imp >::tuple_size-1 >
      ::apply( out, raw_tuple( tuple ) );
    return out;
  }

} // namespace Dune

#endif // #ifndef DUNE_COMMON_TUPLES_DENSEVECTOR_HH