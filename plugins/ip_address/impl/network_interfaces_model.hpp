// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include <QNetworkInterface>

class NetworkInterfacesModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit NetworkInterfacesModel(QObject* parent = nullptr);

  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

  using SelectedIPs = QMap<QString, int>;
  SelectedIPs selectedIPs() const { return _selected_ips; }
  void setSelectedIPs(const SelectedIPs& selected_ips);

signals:
  void selectedIPsChanged(const SelectedIPs& ips);

private:
  QList<QNetworkInterface> _ifaces;
  SelectedIPs _selected_ips;
};


class InactiveInterfacesFilterModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit InactiveInterfacesFilterModel(QObject* parent = nullptr);

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
};


class LinkLocalAddressesFilterModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit LinkLocalAddressesFilterModel(QObject* parent = nullptr);

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
};
