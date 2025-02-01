#pragma once

#include <QDialog>

#include "plugin/text/plugin_config.hpp"

namespace plugin::text::impl {

class AppearanceSettingsWidget;
class GeometrySettingsWidget;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(PluginConfig* cfg, QWidget *parent = nullptr);
  ~SettingsDialog();

  void insertPagesBeforeCommon(const QVector<QWidget*>& pages);
  void insertPagesAfterCommon(const QVector<QWidget*>& pages);

  void addInstance(size_t idx);
  void setInstance(size_t idx);

signals:
  void instanceSwitched(size_t idx);

  void pluginOptionChanged(wpo::InstanceOptions opt, const QVariant& val);
  void sharedOptionChanged(opt::InstanceOptions opt, const QVariant& val);

private slots:
  void on_instance_box_activated(int index);

private:
  Ui::SettingsDialog *ui;

  AppearanceSettingsWidget* _atab;
  GeometrySettingsWidget* _gtab;

  PluginConfig* _pcfg;
};

} // namespace plugin::text::impl
