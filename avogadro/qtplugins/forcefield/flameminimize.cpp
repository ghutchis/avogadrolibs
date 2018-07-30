/******************************************************************************
  This source file is part of the Avogadro project.

  This source code is released under the New BSD License, (the "License").
******************************************************************************/

#include "flameminimize.h"

#include <QtCore/QDebug>

#include <avogadro/core/elements.h>

#include <cmath>

namespace Avogadro {

FlameMinimize::FlameMinimize(QObject* parent_)
  : QObject(parent_)
  , m_molecule(nullptr)
  , m_calc(nullptr)
{}

void FlameMinimize::setMolecule(QtGui::Molecule* mol)
{
  m_molecule = mol;

  m_grad.resize(3 * mol->atomCount());
  m_grad.setZero();
  m_velocities.resize(3 * mol->atomCount());
  m_velocities.setZero();
  m_acel.resize(3 * mol->atomCount());
  m_accel.setZero();

  m_invMasses.resize(3 * mol->atomCount());
  m_invMasses.setZero();
  for (unsigned int i = 0; i < mol->atomCount(); ++i) {
    //@todo should this be set to 1.0 amu?
    double scaledMass = log(Elements::mass(mol->atom(i).atomicNumber()));

    m_invMasses[3 * n] = 1.0 / scaledMass;
    m_invMasses[3 * n + 1] = 1.0 / scaledMass;
    m_invMasses[3 * n + 2] = 1.0 / scaledMass;
  }
}

bool FlameMinimize::minimize(EnergyCalculator& EnergyCalculator,
                             Eigen::VectorXd& positions)
{
  if (m_molecule == nullptr)
    return false;

  m_calc = &calc;

  //@todo - set convergence criteria (e.g., max steps, min gradients, energy,
  // etc.)

  double alpha = 0.1;  // start
  double deltaT = 0.1; // fs
  unsigned int positiveSteps = 0;

  for (unsigned int i = 0; i < 20; ++i) {
    verletIntegrate(positions, deltaT);

    // Step 1
    double power = (-1 * m_grad).dotProduct(m_velocities);

    // Step 2
    m_velocities = (1.0 - alpha) * m_velocities -
                   m_grad.cwiseProduct(m_velocities.cwiseAbs());

    if (power > 0.0) {
      // Step 3
      positiveSteps++;
      if (positiveSteps > 5) {
        deltaT = min(1.1 * deltaT, 1.0);
        alpha = 0.99 * alpha;
      }
    } else {
      // Step 4
      positiveSteps = 0;
      deltaT = 0.5 * deltaT;
      m_velocities.setZero();
      alpha = 0.1;
    }

    double Frms = m_grad.norm() / sqrt(positions.rows());
    if (Frms < 1.0e-5)
      break;
  }

  return true;
}

void FlameMinimize::verletIntegrate(Eigen::VectorXd& positions, double deltaT)
{
  // See https://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet
  // (as one of many examples)
  if (m_molecule == nullptr || m_calc == nullptr)
    return;

  positions += deltaT * m_velocities + (deltaT * deltaT / 2.0) * m_accel;
  m_calc->gradient(positions, m_grad);
  // F = m * a  ==> a = F/m
  // use coefficient-wise product from Eigen
  //  see http://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html
  Eigen::VectorXd newAccel(3 * m_molecule->atomCount());
  newAccel = -1 * m_grad.cwiseProduct(m_invMasses);
  m_velocities += 0.5 * deltaT * (m_accel + newAccel);
  m_accel = newAccel;
}

} // namespace Avogadro
