//                                               -*- C++ -*-
/**
 *  @brief DomainDifference represents the difference of two Domain
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_DOMAINDIFFERENCE_HXX
#define OPENTURNS_DOMAINDIFFERENCE_HXX

#include "openturns/DomainImplementation.hxx"
#include "openturns/Domain.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DomainDifference
 *
 * A class that holds the difference of two Domain.
 * A point x is contained in this domain if it is contained
 * in left domain but not in right domain.
 */
class OT_API DomainDifference
  : public DomainImplementation
{
  CLASSNAME

public:
  typedef DomainImplementation::BoolCollection  BoolCollection;

  /** Default constructor */
  DomainDifference();

  /** Default constructor */
  DomainDifference(const Domain & left, const Domain & right);

  /** Virtual constructor method */
  virtual DomainDifference * clone() const;

  /** Check if the given point is inside this domain */
  Bool contains(const Point & point) const;

  /** Check if the given points are inside this domain */
  BoolCollection contains(const Sample & sample) const;

  /** Comparison operator */
  Bool operator == (const DomainDifference & other) const;
  Bool operator != (const DomainDifference & other) const;
  using DomainImplementation::operator ==;
  using DomainImplementation::operator !=;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  // The first domain
  Domain left_;

  // The second domain
  Domain right_;

}; /* class DomainDifference */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINDIFFERENCE_HXX */
