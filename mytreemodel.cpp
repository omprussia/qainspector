#include "mytreemodel.h"

#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>

MyTreeModel::MyTreeModel(const QJsonObject &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    fillModel(data);
}

void MyTreeModel::fillModel(const QJsonObject &object)
{

    if (m_rootItem) {
        delete m_rootItem;
        m_rootItem = nullptr;
    }

    QJsonObject data = object;
    const QJsonArray childsData = data.take(QStringLiteral("children")).toArray();

    QVariantMap rootMap;

    m_roleNames = data.keys();
    m_roles.clear();
    int role = Qt::UserRole;

    for (const QString &roleName : m_roleNames) {
        rootMap[roleName] = roleName;
        m_roles[++role] = roleName.toLatin1();
    }

    beginResetModel();

    const QJsonObject rootData = QJsonDocument::fromVariant(rootMap).object();

    m_rootItem = new TreeItem(rootData);

    TreeItem *firstItem = new TreeItem(data, m_rootItem);
    m_rootItem->appendChild(firstItem);

    QList<TreeItem *> childs = processChilds(childsData, m_rootItem);
    for (TreeItem *child : childs) {
        firstItem->appendChild(child);
    }

    endResetModel();
}

QList<TreeItem *> MyTreeModel::processChilds(const QJsonArray &data, TreeItem *parentItem)
{
    QList<TreeItem *> childs;
    for (const QJsonValue &value : data) {
        QJsonObject childData = value.toObject();
        const QJsonArray childsData = childData.take(QStringLiteral("children")).toArray();
        TreeItem *child = new TreeItem(childData, parentItem);
        QList<TreeItem *> itemChilds = processChilds(childsData, child);
        for (TreeItem *itemChild : itemChilds) {
            child->appendChild(itemChild);
        }
        childs.append(child);
    }
    return childs;
}

QVariant MyTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if ((role - Qt::UserRole) > m_roleNames.count()) {
        return QVariant();
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(m_roleNames[role - Qt::UserRole - 1]);
}

Qt::ItemFlags MyTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant MyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_rootItem->data(m_roleNames[section]);
    }

    return QVariant();
}

QModelIndex MyTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    TreeItem *parentItem;

    if (parent.isValid()) {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    } else {
        parentItem = m_rootItem;
    }

    TreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex MyTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int MyTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    TreeItem *parentItem;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int MyTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    }

    return m_rootItem->columnCount();
}

QHash<int, QByteArray> MyTreeModel::roleNames() const
{
    return m_roles;
}

QRect MyTreeModel::getRect(const QModelIndex &index)
{
    QRect rect;
    if (!index.isValid()) {
        return rect;
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    rect.setX(item->data("abs_x").toInt());
    rect.setY(item->data("abs_y").toInt());
    rect.setWidth(item->data("width").toInt());
    rect.setHeight(item->data("height").toInt());

    return rect;
}


TreeItem::TreeItem(const QJsonObject &data, TreeItem *parent)
    : m_data(data)
    , m_parent(parent)
{

}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childs);
}

void TreeItem::appendChild(TreeItem *child)
{
    m_childs.append(child);
}

TreeItem *TreeItem::child(int index)
{
    if (index < 0 || index >= m_childs.count()) {
        return nullptr;
    }

    return m_childs.value(index);
}

int TreeItem::childCount() const
{
    return m_childs.count();
}

QVariant TreeItem::data(const QString &roleName) const
{
    return m_data.value(roleName).toVariant();
}

int TreeItem::columnCount() const
{
    return m_data.count();
}

int TreeItem::row()
{
    if (!m_parent) {
        return 0;
    }

    return m_parent->row(this);
}

int TreeItem::row(TreeItem *child)
{
    return m_childs.indexOf(child);
}

TreeItem *TreeItem::parent()
{
    return m_parent;
}
