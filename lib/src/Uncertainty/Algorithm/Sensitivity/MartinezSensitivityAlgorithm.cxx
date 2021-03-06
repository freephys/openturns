//                                               -*- C++ -*-
/**
 *  @brief Implementation for Martinez sensitivity algorithm
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

#include "openturns/MartinezSensitivityAlgorithm.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MartinezSensitivityAlgorithm)

static const Factory<MartinezSensitivityAlgorithm> Factory_MartinezSensitivityAlgorithm;

/* Default constructor */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
  , useAsymptoticInterval_(ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval"))
{
  // Nothing to do
}

/** Constructor with parameters */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
  : SobolIndicesAlgorithmImplementation(inputDesign, outputDesign, size)
  , useAsymptoticInterval_(ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval"))
{
  // Nothing to do
}

/** Constructor with distribution / model parameters */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(distribution, size, model, computeSecondOrder)
  , useAsymptoticInterval_(ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval"))
{
  // Nothing to do
}

/** Constructor with experiment / model parameters */
MartinezSensitivityAlgorithm::MartinezSensitivityAlgorithm(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder)
  : SobolIndicesAlgorithmImplementation(experiment, model, computeSecondOrder)
  , useAsymptoticInterval_(ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval"))
{
  // Nothing to do
}

/* Virtual constructor */
MartinezSensitivityAlgorithm * MartinezSensitivityAlgorithm::clone() const
{
  return new MartinezSensitivityAlgorithm(*this);
}

Sample MartinezSensitivityAlgorithm::computeIndices(const Sample & sample,
    Sample & VTi) const
{
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger size = size_;
  Sample varianceI(outputDimension, inputDimension);
  VTi = Sample(outputDimension, inputDimension);

  // Use reference samples
  // Reference sample yA
  Sample yA(sample, 0, size);
  const Point muA(yA.computeMean());
  const Point sigmaA(yA.computeStandardDeviationPerComponent());
  // center sample yA
  yA -= muA;
  // Reference sample yB
  Sample yB(sample, size, 2 * size);
  const Point muB(yB.computeMean());
  const Point sigmaB(yB.computeStandardDeviationPerComponent());
  // center-reduce sample yB
  yB -= muB;
  yB /= sigmaB;

  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    Sample yE(sample, (2 + p) * size, (3 + p) * size);
    const Point muE(yE.computeMean());
    const Point sigmaE(yE.computeStandardDeviationPerComponent());
    // center-reduce sample yB
    yE -= muE;
    yE /= sigmaE;
    // Compute yE * yB
    const Point yEDotyB(computeSumDotSamples(yE, yB));
    // Compute yE * yA
    const Point yEDotyA(computeSumDotSamples(yE, yA));
    for (UnsignedInteger q = 0; q < outputDimension; ++q)
    {
      // Compute rho(yB, yE) with rho : Pearson correlation
      // Si = rho(yB, yE) thus Vi = Si * V
      varianceI[q][p] =  yEDotyB[q] / (size - 1.0)  * sigmaA[q] * sigmaA[q];
      // STi = 1 - rho(yA, yE),  Vti = V -VTi thus VTi = rho(yA, yE) * var(yA)
      VTi[q][p] = referenceVariance_[q] - yEDotyA[q] / (size - 1.0)  * sigmaA[q];
    }
  }
  return varianceI;
}


/* String converter */
String MartinezSensitivityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesAlgorithmImplementation::__repr__();
  return oss;
}

// Compute the fisher transform
void MartinezSensitivityAlgorithm::computeAsymptoticInterval() const
{
  // Do nothing if already computed
  if (0 != firstOrderIndiceInterval_.getDimension()) return;

  // Compute Fisher transform
  // Build interval using sample variance
  // Mean reference is the Sensitivity values
  const Point aggregatedFirstOrder(getAggregatedFirstOrderIndices());
  const Point aggregatedTotalOrder(getAggregatedTotalOrderIndices());
  const Scalar t = DistFunc::qNormal(1.0 - 0.5 * confidenceLevel_);
  const UnsignedInteger size = size_;
  if (size <= 3)
    throw InvalidArgumentException(HERE) << "Could not compute asymptotic confidence interval for sensitivity indices with size=" << size
                                         << ", sample's size should be at least 4";
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  // First order interval
  Point firstOrderLowerBound(inputDimension, 0.0);
  Point firstOrderUpperBound(inputDimension, 0.0);
  // Total order interval
  Point totalOrderLowerBound(inputDimension, 0.0);
  Point totalOrderUpperBound(inputDimension, 0.0);
  // Numerical scalar that will be used
  Scalar z, rho;
  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // Correlation indices evaluation
    // first order
    // We get Si which is a correlation coefficient
    // Even if multidimensional, linear combination is still gaussian
    rho = aggregatedFirstOrder[p];
    // Fisher transform
    z = 0.5 * std::log((1.0 + rho) / (1.0 - rho));
    // zmin/zmax
    Scalar zmin = std::tanh(z - t / std::sqrt(size - 3.0));
    Scalar zmax = std::tanh(z + t / std::sqrt(size - 3.0));
    // TODO if interval is outside [0,1], how to procede?
    firstOrderLowerBound[p] = zmin;
    firstOrderUpperBound[p] = zmax;
    // total order
    // We compute STi which is a correlation coefficient
    // Even if multidimensional, linear combination is still gaussian
    rho = 1.0 - aggregatedTotalOrder[p];
    // Fisher transform
    z = 0.5 * std::log((1.0 + rho) / (1.0 - rho));
    // zmin/zmax
    zmin = std::tanh(z - t / std::sqrt(size - 3.0));
    zmax = std::tanh(z + t / std::sqrt(size - 3.0));
    totalOrderLowerBound[p] = 1.0 - zmax;
    totalOrderUpperBound[p] = 1.0 - zmin;
  }
  // Compute confidence interval
  firstOrderIndiceInterval_ = Interval(firstOrderLowerBound, firstOrderUpperBound);
  totalOrderIndiceInterval_ = Interval(totalOrderLowerBound, totalOrderUpperBound);
}


/** Interval for the first order indices accessor */
Interval MartinezSensitivityAlgorithm::getFirstOrderIndicesInterval() const
{
  if (useAsymptoticInterval_ != ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval"))
  {
    useAsymptoticInterval_ = ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval");
    firstOrderIndiceInterval_ = Interval();
    totalOrderIndiceInterval_ = Interval();
  }
  if (useAsymptoticInterval_)
  {
    computeAsymptoticInterval();
  }
  else
  {
    // Interval evaluation using Bootstrap
    computeIndicesInterval();
  }
  return firstOrderIndiceInterval_;
}

/** Interval for the total order indices accessor */
Interval MartinezSensitivityAlgorithm::getTotalOrderIndicesInterval() const
{
  if (useAsymptoticInterval_ != ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval"))
  {
    useAsymptoticInterval_ = ResourceMap::GetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval");
    firstOrderIndiceInterval_ = Interval();
    totalOrderIndiceInterval_ = Interval();
  }
  if (useAsymptoticInterval_)
  {
    computeAsymptoticInterval();
  }
  else
  {
    // Interval evaluation using Bootstrap
    computeIndicesInterval();
  }
  return totalOrderIndiceInterval_;
}

/* Method save() stores the object through the StorageManager */
void MartinezSensitivityAlgorithm::save(Advocate & adv) const
{
  SobolIndicesAlgorithmImplementation::save(adv);
  adv.saveAttribute("useAsymptoticInterval_", useAsymptoticInterval_);
}

/* Method load() reloads the object from the StorageManager */
void MartinezSensitivityAlgorithm::load(Advocate & adv)
{
  SobolIndicesAlgorithmImplementation::load(adv);
  adv.loadAttribute("useAsymptoticInterval_", useAsymptoticInterval_);
}

END_NAMESPACE_OPENTURNS
