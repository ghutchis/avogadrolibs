/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_QTPLUGINS_CONFIGUREPYTHONDIALOG_H
#define AVOGADRO_QTPLUGINS_CONFIGUREPYTHONDIALOG_H

#include <QtWidgets/QDialog>

namespace Avogadro {
namespace QtPlugins {

namespace Ui {
class ConfigurePythonDialog;
}

/**
 * @brief Dialog to prompt a format and descriptor string.
 */
class ConfigurePythonDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConfigurePythonDialog(QWidget* parent = nullptr);
  ~ConfigurePythonDialog() override;

  void setOptions(const QStringList& options);
  void setCurrentOption(const QString& option);
  QString currentOption() const;

signals:
  void accepted();
  void rejected();

protected slots:
  void optionChanged(int index);
  void accept() override;
  void reject() override;

private:
  Ui::ConfigurePythonDialog* m_ui;
};

} // namespace QtPlugins
} // namespace Avogadro

#endif // AVOGADRO_QTPLUGINS_CONFIGUREPYTHONDIALOG_H
