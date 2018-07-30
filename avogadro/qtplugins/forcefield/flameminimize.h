/******************************************************************************
  This source file is part of the Avogadro project.

  This source code is released under the New BSD License, (the "License").
******************************************************************************/

#ifndef AVOGADRO_FLAMEMINIMIZE_H
#define AVOGADRO_FLAMEMINIMIZE_H

#include <avogadro/core/vector.h>
#include <avogadro/qtgui/rwmolecule.h>

#include "energycalculator.h"

namespace Avogadro {
namespace QtGui {
class Molecule;
}

class FlameMinimize : public QObject
{
  Q_OBJECT
public:
  FlameMinimize(QObject* parent_ = 0);
  ~FlameMinimize() {}

  bool minimize(EnergyCalculator &cal, Eigen::VectorXd &positions);

  // @todo probably want a "take N steps" and a way to set convergence
  //     (e.g., if the forces/gradients are very small)
  //     ( might also check if energy changes are v. small)

public slots:
  /**
   * Called when the current molecule changes.
   */
  void setMolecule(QtGui::Molecule* mol);

protected:

  void verletIntegrate(Eigen::VectorXd &positions, double deltaT);

  QtGui::Molecule* m_molecule;
  EnergyCalculator* m_calc;
  Eigen::VectorXd m_invMasses;
  Eigen::VectorXd m_forces;
  Eigen::VectorXd m_velocities;
  Eigen::VectorXd m_accel;
};

} // namespace Avogadro

#endif // AVOGADRO_FLAMEMINIMIZE_H
