#include "mytreemodel.h"

#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>

#include <QGuiApplication>
#include <QClipboard>
#include <QXmlStreamWriter>
#include <QXmlQuery>

MyTreeModel::MyTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void MyTreeModel::fillModel(const QJsonObject &object)
{
    beginResetModel();

    QJsonObject data = object;
    const QJsonArray childsData = data.take(QStringLiteral("children")).toArray();

    QVariantMap rootMap;

    if (m_roleNames.isEmpty()) {
        m_roleNames = data.keys();
        int role = Qt::UserRole;

        for (const QString &roleName : m_roleNames) {
            rootMap[roleName] = roleName;
            m_roles[++role] = roleName.toLatin1();
        }
    }

    const QJsonObject rootData = QJsonDocument::fromVariant(rootMap).object();


    if (!m_rootItem) {
        m_rootItem = new TreeItem(rootData);
    } else {
        m_rootItem->genocide();
    }

    TreeItem *firstItem = new TreeItem(data, m_rootItem);
    m_rootItem->appendChild(firstItem);

    QList<TreeItem *> childs = processChilds(childsData, m_rootItem);
    for (TreeItem *child : childs) {
        firstItem->appendChild(child);
    }

    endResetModel();
}

void MyTreeModel::loadDump(const QString &dump)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(dump.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError) {
        fillModel(doc.object());
    } else {
        qWarning() << Q_FUNC_INFO << error.errorString();
    }
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

QString MyTreeModel::searchXPath(const QString &xpath, const QString &currentId)
{
    QString out;
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    recursiveDumpXml(&writer, m_rootItem->child(0));
    writer.writeEndDocument();

    qDebug().noquote() << out;

    QXmlQuery query;
    query.setFocus(out);
    query.setQuery(xpath);

    if (!query.isValid()) {
        return QString();
    }
    QString tempString;
    query.evaluateTo(&tempString);

    if (tempString.trimmed().isEmpty()) {
        return QString();
    }

    const QString resultData = QLatin1String("<results>") + tempString + QLatin1String("</results>");
    QXmlStreamReader reader(resultData);
    reader.readNextStartElement();
    QStringList items;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            items.append(reader.attributes().value(QStringLiteral("id")).toString());
        }
    }
    if (items.isEmpty()) {
        return QString();
    }
    if (currentId.isEmpty()) {
        return items.first();
    }
    const int currentIndex = items.indexOf(currentId);
    if (currentIndex == -1) {
        return items.first();
    }
    if (currentIndex == items.size() - 1) {
        return items.first();
    }

    return items.at(currentIndex + 1);
}

void MyTreeModel::recursiveDumpXml(QXmlStreamWriter *writer, TreeItem *parent)
{
    const QJsonObject object = parent->data();
    writer->writeStartElement(object.value(QStringLiteral("classname")).toString());
    for (auto i = object.constBegin(), objEnd = object.constEnd(); i != objEnd; ++i) {
        const QJsonValue& val = *i;
        const QString &name = i.key();

        writer->writeAttribute(name, val.toVariant().toString());
    }
    if (object.contains(QStringLiteral("mainTextProperty"))) {
        writer->writeCharacters(object.value(QStringLiteral("mainTextProperty")).toString());
    }
    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem *child = parent->child(i);
        recursiveDumpXml(writer, child);
    }
    writer->writeEndElement();
}

QMap<int, QVariant> MyTreeModel::itemData(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() < rowCount()) {
        return {};
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    QMap<int, QVariant> data;
    int roleIndex = Qt::UserRole;
    for (const QString &roleName : m_roleNames) {
        data.insert(++roleIndex, item->data(roleName));
    }
    return data;
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

    if (!m_rootItem) {
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
    rect.setX(item->data(QStringLiteral("abs_x")).toInt());
    rect.setY(item->data(QStringLiteral("abs_y")).toInt());
    rect.setWidth(item->data(QStringLiteral("width")).toInt());
    rect.setHeight(item->data(QStringLiteral("height")).toInt());

    return rect;
}

QVariantList MyTreeModel::getDataList(const QModelIndex &index)
{
    if (!index.isValid()) {
        return QVariantList();
    }

    QVariantList dataList;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    const QJsonObject jsonData = item->data();
    for (const QString &key : jsonData.keys()) {
        dataList.append(QVariantMap({{QStringLiteral("name"), key},
                                     {QStringLiteral("value"), jsonData.value(key).toVariant()}}));
    }

    return dataList;
}

void MyTreeModel::copyToClipboard(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    qGuiApp->clipboard()->setText(item->data(QStringLiteral("id")).toString());
}

void MyTreeModel::copyText(const QString &text)
{
    qGuiApp->clipboard()->setText(text);
}

QVariantList MyTreeModel::getChildrenIndexes(TreeItem *node)
{
    QVariantList indexes;
    TreeItem *parent = node ? node : m_rootItem;

    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem *child = parent->child(i);
        indexes.push_back(createIndex(i, 0, reinterpret_cast<quintptr>(child)));

        indexes.append(getChildrenIndexes(child));
    }

    return indexes;
}

QModelIndex MyTreeModel::searchIndex(const QString &key, const QVariant &value, bool partialSearch, const QModelIndex &currentIndex, TreeItem *node)
{
    if (key == QLatin1String("xpath")) {
        QString currentId;
        if (currentIndex.internalPointer()) {
            TreeItem *item = static_cast<TreeItem*>(currentIndex.internalPointer());
            currentId = item->data(QStringLiteral("id")).toString();
        }

        const QString itemId = searchXPath(value.toString(), currentId);
        if (itemId.isEmpty()) {
            return QModelIndex();
        }
        return searchIndex(QStringLiteral("id"), itemId, false, currentIndex, node);
    }

    static bool currentFound = false;
    if (!node) {
        currentFound = false;
    }
    if (!currentIndex.isValid()) {
        currentFound = true;
    }
    TreeItem *parent = node ? node : m_rootItem;

    QModelIndex index;
    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem *child = parent->child(i);
        const QModelIndex newIndex = createIndex(i, 0, reinterpret_cast<quintptr>(child));
        const bool match = child->data(key) == value
                || (partialSearch && value.type() == QVariant::String && child->data(key).toString().contains(value.toString()));
        if (!currentFound && newIndex == currentIndex) {
            currentFound = true;
        }
        if (newIndex != currentIndex && match) {
            if (currentFound) {
                index = newIndex;
                break;
            }
        }

        QModelIndex childIndex = searchIndex(key, value, partialSearch, currentIndex, child);
        if (childIndex.internalPointer()) {
            index = childIndex;
            break;
        }
    }

    if (!node && currentIndex.isValid() && !index.isValid()) {
        return searchIndex(key, value, partialSearch, currentIndex, m_rootItem->child(0));
    }

    return index;
}

QModelIndex MyTreeModel::searchByCoordinates(int posx, int posy, TreeItem *node)
{
//    qDebug() << Q_FUNC_INFO << posx << posy;
    TreeItem *parent = node ? node : m_rootItem;
    QModelIndex childIndex;

    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem *child = parent->child(i);
        const QVariant activeVariant = child->data(QStringLiteral("active"));
        const bool active = activeVariant.isValid() ? activeVariant.toBool() : true;
        if (!active) {
            continue;
        }

        const bool visible = child->data(QStringLiteral("visible")).toBool();
        if (!visible) {
            continue;
        }

        const bool enabled = child->data(QStringLiteral("enabled")).toBool();
        if (!enabled) {
            continue;
        }

        const float opacity = child->data(QStringLiteral("opacity")).toFloat();
        if (opacity == 0.0f) {
            continue;
        }

        const QString classname = child->data(QStringLiteral("classname")).toString();
        const int itemx = child->data(QStringLiteral("abs_x")).toInt();
        const int itemy = child->data(QStringLiteral("abs_y")).toInt();
        const int itemw = child->data(QStringLiteral("width")).toInt();
        const int itemh = child->data(QStringLiteral("height")).toInt();

        if (classname != QLatin1String("QQuickLoader")
                && classname != QLatin1String("DeclarativeTouchBlocker")
                && classname != QLatin1String("QQuickItem")
                && classname != QLatin1String("RotatingItem")
                && classname != QLatin1String("QQuickShaderEffect")
                && classname != QLatin1String("QQuickShaderEffectSource")
                && !classname.endsWith(QLatin1String("Gradient"))
                && posx >= itemx && posx <= (itemx + itemw)
                && posy >= itemy && posy <= (itemy + itemh)) {
            childIndex = createIndex(i, 0, reinterpret_cast<quintptr>(child));
        }

        QModelIndex someIndex = searchByCoordinates(posx, posy, child);
        if (someIndex.isValid()) {
            childIndex = someIndex;
        }
    }

    return childIndex;
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

QVector<TreeItem*> TreeItem::childs()
{
    return m_childs;
}

int TreeItem::childCount() const
{
    return m_childs.count();
}

void TreeItem::genocide()
{
    qDeleteAll(m_childs);
    m_childs.clear();
}

QVariant TreeItem::data(const QString &roleName) const
{
    if (!m_data.contains(roleName)) {
        return QVariant();
    }
    return m_data.value(roleName).toVariant();
}

QJsonObject TreeItem::data() const
{
    return m_data;
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
