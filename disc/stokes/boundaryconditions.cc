// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifdef HAVE_CONFIG_H
# include "config.h"     // autoconf defines, needed by the dune headers
#endif
#include "boundaryconditions.hh"

template<class Grid>
double
DirichletBoundary<Grid>::dirichletValue(int variable,  const Point& global, Point& local) const
{
  return exact.velocity(variable,global);
}
