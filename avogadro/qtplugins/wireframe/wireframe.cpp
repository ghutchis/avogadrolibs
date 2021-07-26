/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "wireframe.h"

#include <avogadro/core/elements.h>
#include <avogadro/core/molecule.h>
#include <avogadro/rendering/geometrynode.h>
#include <avogadro/rendering/groupnode.h>
#include <avogadro/rendering/linestripgeometry.h>

#include <QtCore/QSettings>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace Avogadro {
namespace QtPlugins {

using Core::Array;
using Core::Elements;
using Core::Molecule;
using Rendering::GeometryNode;
using Rendering::GroupNode;
using Rendering::LineStripGeometry;

Wireframe::Wireframe(QObject* p)
  : ScenePlugin(p), m_enabled(false), m_group(nullptr), m_setupWidget(nullptr)
{ 
  QSettings settings;
  m_multiBonds = settings.value("wireframe/multiBonds", true).toBool();
  m_showHydrogens = settings.value("wireframe/showHydrogens", true).toBool();
  m_lineWidth = settings.value("wireframe/lineWidth", 1.0).toDouble();
}

Wireframe::~Wireframe()
{
  if (m_setupWidget)
    m_setupWidget->deleteLater();
}

void Wireframe::process(const Molecule& molecule, Rendering::GroupNode& node)
{
  // Add a sphere node to contain all of the spheres.
  m_group = &node;
  GeometryNode* geometry = new GeometryNode;
  node.addChild(geometry);

  LineStripGeometry* lines = new LineStripGeometry;
  lines->identifier().molecule = &molecule;
  lines->identifier().type = Rendering::BondType;
  geometry->addDrawable(lines);
  for (Index i = 0; i < molecule.bondCount(); ++i) {
    Core::Bond bond = molecule.bond(i);
    if (!m_showHydrogens && (bond.atom1().atomicNumber() == 1 ||
                             bond.atom2().atomicNumber() == 1)) {
      continue;
    }
    Vector3f pos1 = bond.atom1().position3d().cast<float>();
    Vector3f pos2 = bond.atom2().position3d().cast<float>();
    Vector3ub color1(Elements::color(bond.atom1().atomicNumber()));
    Vector3ub color2(Elements::color(bond.atom2().atomicNumber()));
    Array<Vector3f> points;
    Array<Vector3ub> colors;
    points.push_back(pos1);
    points.push_back(pos2);
    colors.push_back(color1);
    colors.push_back(color2);
    lines->addLineStrip(points, colors, m_lineWidth);
  }
}

bool Wireframe::isEnabled() const
{
  return m_enabled;
}

void Wireframe::setEnabled(bool enable)
{
  m_enabled = enable;
}

QWidget* Wireframe::setupWidget()
{
  if (!m_setupWidget) {
    m_setupWidget = new QWidget(qobject_cast<QWidget*>(parent()));
    QVBoxLayout* v = new QVBoxLayout;

    // line width
    QDoubleSpinBox* spin = new QDoubleSpinBox;
    spin->setRange(0.5, 5.0);
    spin->setSingleStep(0.25);
    spin->setDecimals(2);
    spin->setValue(m_lineWidth);
    connect(spin, SIGNAL(valueChanged(double)), SLOT(setWidth(double)));
    QFormLayout* form = new QFormLayout;
    form->addRow(tr("Line width:"), spin);
    v->addLayout(form);

    // options
    QCheckBox* check = new QCheckBox(tr("Show multiple bonds?"));
    check->setChecked(m_multiBonds);
    connect(check, SIGNAL(clicked(bool)), SLOT(multiBonds(bool)));
    v->addWidget(check);

    check = new QCheckBox(tr("Show hydrogens?"));
    check->setChecked(m_showHydrogens);
    connect(check, SIGNAL(toggled(bool)), SLOT(showHydrogens(bool)));
    v->addWidget(check);

    v->addStretch(1);
    m_setupWidget->setLayout(v);
  }
  return m_setupWidget;
}

void Wireframe::multiBonds(bool show)
{
  if (show != m_multiBonds) {
    m_multiBonds = show;
    emit drawablesChanged();
  }
  QSettings settings;
  settings.setValue("wireframe/multiBonds", show);
}

void Wireframe::showHydrogens(bool show)
{
  if (show != m_showHydrogens) {
    m_showHydrogens = show;
    emit drawablesChanged();
  }
  QSettings settings;
  settings.setValue("wireframe/showHydrogens", show);
}

void Wireframe::setWidth(double width)
{
  m_lineWidth = float(width);
  emit drawablesChanged();

  QSettings settings;
  settings.setValue("wireframe/lineWidth", m_lineWidth);
}

} // namespace QtPlugins
} // namespace Avogadro
