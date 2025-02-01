#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include "appearance_settings_widget.hpp"
#include "geometry_settings_widget.hpp"

namespace {

int setIndexByValue(QComboBox* box, const QVariant& value)
{
  auto idx = box->findData(value);
  if (idx >= 0)
    box->setCurrentIndex(idx);
  return idx;
}

} // namespace

namespace plugin::text::impl {

SettingsDialog::SettingsDialog(PluginConfig* cfg, QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , _atab(new AppearanceSettingsWidget)
  , _gtab(new GeometrySettingsWidget)
  , _pcfg(cfg)
{
  ui->setupUi(this);

  ui->tabWidget->addTab(_atab, _atab->windowTitle());
  ui->tabWidget->addTab(_gtab, _gtab->windowTitle());

  connect(_atab, &AppearanceSettingsWidget::pluginOptionChanged,
          this, &SettingsDialog::pluginOptionChanged);
  connect(_atab, &AppearanceSettingsWidget::sharedOptionChanged,
          this, &SettingsDialog::sharedOptionChanged);

  connect(_gtab, &GeometrySettingsWidget::pluginOptionChanged,
          this, &SettingsDialog::pluginOptionChanged);
  connect(_gtab, &GeometrySettingsWidget::sharedOptionChanged,
          this, &SettingsDialog::sharedOptionChanged);
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::insertPagesBeforeCommon(const QVector<QWidget*>& pages)
{
  int i = 0;
  for (auto w : pages)
    ui->tabWidget->insertTab(i++, w, w->windowTitle());
}

void SettingsDialog::insertPagesAfterCommon(const QVector<QWidget*>& pages)
{
  for (auto w : pages)
    ui->tabWidget->addTab(w, w->windowTitle());
  ui->tabWidget->setCurrentIndex(0);
}

void SettingsDialog::addInstance(size_t idx)
{
  QSignalBlocker _(this);
  ui->instance_box->addItem(tr("instance %1").arg(idx + 1), QVariant::fromValue(idx));
}

void SettingsDialog::setInstance(size_t idx)
{
  setIndexByValue(ui->instance_box, QVariant::fromValue(idx));
  on_instance_box_activated(ui->instance_box->currentIndex());
}

void SettingsDialog::on_instance_box_activated(int index)
{
  auto idx = ui->instance_box->itemData(index).value<size_t>();
  _atab->initControls(_pcfg->instance(idx));
  _gtab->initControls(_pcfg->instance(idx));
  emit instanceSwitched(idx);
}

} // namespace plugin::text::impl
