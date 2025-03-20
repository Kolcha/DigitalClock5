// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "network_interfaces_model.hpp"

namespace {

constexpr const quintptr type_offset = 16;
constexpr const quintptr type_mask = 0x03 << type_offset;

constexpr const int iface_type_id = 1;
constexpr const int addr_type_id = 2;

constexpr quintptr encode_item_type(int type_id) noexcept
{
  return (type_id << type_offset) & type_mask;
}

constexpr int decode_item_type(quintptr id) noexcept
{
  return (id& type_mask) >> type_offset;
}

constexpr quintptr encode_iface_id(int idx) noexcept
{
  return encode_item_type(iface_type_id) | idx;
}

constexpr int parse_iface_id(quintptr id) noexcept
{
  return id & 0x0FFF;
}

constexpr quintptr encode_addr_id(int iface, int idx) noexcept
{
  return encode_item_type(addr_type_id) | (iface << 4) | idx;
}

constexpr std::pair<int, int> parse_addr_id(quintptr id) noexcept
{
  return {(id & 0xFFF0) >> 4, id & 0x0F};
}

constexpr bool is_iface_item(quintptr id) noexcept
{
  return decode_item_type(id) == iface_type_id;
}

constexpr bool is_addr_item(quintptr id) noexcept
{
  return decode_item_type(id) == addr_type_id;
}

} // namespace

NetworkInterfacesModel::NetworkInterfacesModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  _ifaces = QNetworkInterface::allInterfaces();
}

QModelIndex NetworkInterfacesModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!parent.isValid())                  // iface item
    return createIndex(row, column, encode_iface_id(row));

  if (is_iface_item(parent.internalId())) // addr item
    return createIndex(row, column, encode_addr_id(parse_iface_id(parent.internalId()), row));

  return QModelIndex();
}

QModelIndex NetworkInterfacesModel::parent(const QModelIndex& index) const
{
  if (is_iface_item(index.internalId()))  // iface item
    return QModelIndex();

  if (is_addr_item(index.internalId())) { // addr item
    auto [iface, _] = parse_addr_id(index.internalId());
    return createIndex(iface, 0, encode_iface_id(iface));
  }

  return QModelIndex();
}

int NetworkInterfacesModel::rowCount(const QModelIndex& parent) const
{
  if (is_addr_item(parent.internalId()))
    return 0;

  if (!parent.isValid())
    return _ifaces.size();

  Q_ASSERT(is_iface_item(parent.internalId()));
  Q_ASSERT(parent.row() < _ifaces.size());
  return _ifaces[parent.row()].addressEntries().size();
}

int NetworkInterfacesModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1;
}

QVariant NetworkInterfacesModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (is_iface_item(index.internalId())) {
    Q_ASSERT(index.row() == parse_iface_id(index.internalId()));
    Q_ASSERT(index.row() < _ifaces.size());
    if (role == Qt::DisplayRole)
      return _ifaces[index.row()].humanReadableName();
  }

  if (is_addr_item(index.internalId())) {
    auto [iface, iaddr] = parse_addr_id(index.internalId());
    Q_ASSERT(index.row() == iaddr);
    Q_ASSERT(iface < _ifaces.size());
    if (role == Qt::DisplayRole)
      return _ifaces[iface].addressEntries().at(index.row()).ip().toString();
    if (role == Qt::CheckStateRole)
      return (_selected_ips.value(_ifaces[iface].name()) & (1 << index.row())) != 0 ? Qt::Checked : Qt::Unchecked;
  }

  return QVariant();
}

Qt::ItemFlags NetworkInterfacesModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return Qt::ItemFlags();

  if (is_iface_item(index.internalId())) {
    auto iface_flags = _ifaces[index.row()].flags();
    if (iface_flags.testFlag(QNetworkInterface::IsLoopBack))
      return Qt::ItemNeverHasChildren;
    if (!iface_flags.testFlag(QNetworkInterface::IsRunning))
      return Qt::ItemNeverHasChildren;
  }

  if (is_addr_item(index.internalId()))
    return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;

  return Qt::ItemIsEnabled;
}

bool NetworkInterfacesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  if (!is_addr_item(index.internalId()))
    return false;

  if (!value.canConvert<bool>())
    return false;

  if (role != Qt::CheckStateRole)
    return false;

  auto [iface, iaddr] = parse_addr_id(index.internalId());
  Q_ASSERT(iface < _ifaces.size());
  Q_ASSERT(index.row() == iaddr);
  const auto m = 1 << iaddr;
  auto& selected_idxs = _selected_ips[_ifaces[iface].name()];
  if (value.toBool())
    selected_idxs |= m;
  else
    selected_idxs &= ~m;

  emit dataChanged(index, index, {Qt::CheckStateRole});
  emit selectedIPsChanged(_selected_ips);
  return true;
}

void NetworkInterfacesModel::setSelectedIPs(const SelectedIPs& selected_ips)
{
  if (_selected_ips == selected_ips) return;
  _selected_ips = selected_ips;
  emit dataChanged(index(0, 0), index(_ifaces.size(), 0), {Qt::CheckStateRole});
  emit selectedIPsChanged(_selected_ips);
}


InactiveInterfacesFilterModel::InactiveInterfacesFilterModel(QObject* parent)
  : QSortFilterProxyModel(parent)
{
}

bool InactiveInterfacesFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  return sourceModel()->flags(sourceModel()->index(source_row, 0, source_parent)) != Qt::ItemNeverHasChildren;
}


LinkLocalAddressesFilterModel::LinkLocalAddressesFilterModel(QObject* parent)
  : QSortFilterProxyModel(parent)
{
}

bool LinkLocalAddressesFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  const auto* m = sourceModel();
  const auto sidx = m->index(source_row, 0, source_parent);
  return !is_addr_item(sidx.internalId()) || !QHostAddress(m->data(sidx).toString()).isLinkLocal();
}
