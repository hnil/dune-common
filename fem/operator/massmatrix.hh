// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef __DUNE_MASSMATRIX_HH__
#define __DUNE_MASSMATRIX_HH__

namespace Dune {

  /** \brief The mass matrix
   */
  template <class DiscFunctionType>
  class MassMatrixFEOp :
    public FiniteElementOperator<DiscFunctionType,
        SparseRowMatrix<double>,
        MassMatrixFEOp<DiscFunctionType> > {
    typedef typename DiscFunctionType::FunctionSpaceType FunctionSpaceType;
    typedef typename FiniteElementOperator<DiscFunctionType,
        SparseRowMatrix<double>,
        MassMatrixFEOp<DiscFunctionType> >::OpMode OpMode;

    typedef typename FunctionSpaceType::RangeField RangeFieldType;
    typedef typename FunctionSpaceType::GridType GridType;
    typedef typename FunctionSpaceType::Range RangeType;

  public:
    FastQuad < typename FunctionSpaceType::RangeField, typename
        FunctionSpaceType::Domain , 1 > quad;

    MassMatrixFEOp( const typename DiscFunctionType::FunctionSpace &f, OpMode opMode ) :   //= ON_THE_FLY ) :
                                                                                           FiniteElementOperator<DiscFunctionType,SparseRowMatrix<double>,MassMatrixFEOp<DiscFunctionType> >( f, opMode ) ,
                                                                                           quad ( *(f.getGrid().template lbegin<0> (0))) {}

    SparseRowMatrix<double>* newEmptyMatrix( ) const {
      return new SparseRowMatrix<double>( this->functionSpace_.size ( this->functionSpace_.getGrid().maxlevel() ) ,
                                          this->functionSpace_.size ( this->functionSpace_.getGrid().maxlevel() ) ,
                                          10, 0.0 );
    }

    template <class EntityType>
    double getLocalMatrixEntry( EntityType &entity, const int i, const int j ) const
    {
      typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
      double val = 0.;

      Vec<GridType::dimension> tmp(1.0);

      const BaseFunctionSetType & baseSet
        = this->functionSpace_.getBaseFunctionSet( entity );

      RangeType v1 (0.0);
      RangeType v2 (0.0);

      const double vol =
        entity.geometry().integration_element( tmp );
      for ( int pt=0; pt < quad.nop(); pt++ )
      {
        baseSet.eval(i,quad,pt,v1);
        baseSet.eval(j,quad,pt,v2);
        val += ( v1 * v2 ) * quad.weight( pt );
      }
      val *= vol;
      return val;
    }

    template < class EntityType, class MatrixType>
    void getLocalMatrix( EntityType &entity, const int matSize, MatrixType& mat) const
    {
      int i,j;
      typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
      const BaseFunctionSetType & baseSet
        = this->functionSpace_.getBaseFunctionSet( entity );

      static Vec<GridType::dimension> tmp(1.0);
      const double vol = entity.geometry().integration_element(tmp);

      static RangeType v[4];

      for(i=0; i<matSize; i++)
        for (j=0; j<=i; j++ )
          mat(i,j)=0.0;

      for ( int pt=0; pt < quad.nop(); pt++ )
      {
        for(i=0; i<matSize; i++)
          baseSet.eval(i,quad,pt,v[i]);

        for(i=0; i<matSize; i++)
          for (j=0; j<=i; j++ )
            mat(i,j) += ( v[i] * v[j] ) * quad.weight( pt );
      }

      for(i=0; i<matSize; i++)
        for (j=0; j<=i; j++ )
          mat(i,j) *= vol;

      for(i=0; i<matSize; i++)
        for (j=matSize; j>i; j--)
          mat(i,j) = mat(j,i);


      return;
    }
  protected:
    DiscFunctionType *_tmp;
  };

} // namespace Dune

#endif
