//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/InverseBoxCoxGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseBoxCoxGradient)

static const Factory<InverseBoxCoxGradient> Factory_InverseBoxCoxGradient;

/* Default constructor */
InverseBoxCoxGradient::InverseBoxCoxGradient()
  : GradientImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseBoxCoxGradient::InverseBoxCoxGradient(const InverseBoxCoxEvaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Clone constructor */
InverseBoxCoxGradient * InverseBoxCoxGradient::clone() const
{
  return new InverseBoxCoxGradient(*this);
}

/* Comparison operator */
Bool InverseBoxCoxGradient::operator ==(const InverseBoxCoxGradient & other) const
{
  if (this == &other) return true;
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String InverseBoxCoxGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseBoxCoxGradient::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter __str__ */
String InverseBoxCoxGradient::__str__(const String & offset) const
{
  OSS oss (false);
  oss << "InverseBoxCoxGradient(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Accessor for the evaluation */
InverseBoxCoxEvaluation InverseBoxCoxGradient::getEvaluation() const
{
  return evaluation_;
}

/* Gradient evaluation method */
Matrix InverseBoxCoxGradient::gradient(const Point & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  Matrix result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the gradient is used in a stochastic context, in the InverseBoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const Scalar x = inP[index] + getShift()[index];
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox gradient function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const Scalar lambda_i = getLambda()[index];
    if (std::abs(lambda_i * x * x) < 1e-8) result(0, index) = exp(x) * (1.0 - lambda_i * x * (1.0 + 0.5 * x));
    else result(0, index) = pow(x, 1.0 / lambda_i - 1.0);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseBoxCoxGradient::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseBoxCoxGradient::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the lambda */
Point InverseBoxCoxGradient::getLambda() const
{
  return evaluation_.getLambda();
}

/* Accessor for the shift */
Point InverseBoxCoxGradient::getShift() const
{
  return evaluation_.getShift();
}

/* Method save() stores the object through the StorageManager */
void InverseBoxCoxGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseBoxCoxGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
