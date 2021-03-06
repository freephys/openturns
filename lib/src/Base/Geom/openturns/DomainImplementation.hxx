//                                               -*- C++ -*-
/**
 *  @brief DomainImplementation is defined as a domain of \mathbb{R}^d
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
#ifndef OPENTURNS_DOMAINIMPLEMENTATION_HXX
#define OPENTURNS_DOMAINIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DomainImplementation
 *
 * A class that holds a collection of domain
 */
class OT_API DomainImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<UnsignedInteger>   BoolCollection;
  typedef Pointer<DomainImplementation> Implementation;

  /** Default constructor */
  explicit DomainImplementation(const UnsignedInteger dimension = 1);

  /** Virtual constructor method */
  virtual DomainImplementation * clone() const;

  /** @deprecated */
  virtual Bool isEmpty() const;

  /** @deprecated */
  virtual Bool isNumericallyEmpty() const;

  /** Check if the closed domain contains a given point */
  virtual Bool contains(const Point & point) const;

  /** Check if the closed domain contains given points */
  virtual BoolCollection contains(const Sample & sample) const;

  /** @deprecated */
  virtual Bool numericallyContains(const Point & point) const;

  /** @deprecated */
  virtual Scalar getNumericalVolume() const;

  /** @deprecated */
  virtual Scalar getVolume() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Dimension accessors */
  UnsignedInteger getDimension() const;

  /** @deprecated */
  virtual Point getLowerBound() const;

  /** @deprecated */
  virtual Point getUpperBound() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  // @deprecated
  virtual Scalar computeVolume() const;

  /** The dimension of the DomainImplementation */
  UnsignedInteger dimension_;

  // @deprecated
  mutable Scalar volumeOld_;

  // @deprecated
  mutable Bool isAlreadyComputedVolumeOld_;

}; /* class DomainImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINIMPLEMENTATION_HXX */
