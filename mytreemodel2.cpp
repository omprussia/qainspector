// Copyright (c) 2020 Open Mobile Platform LLС.
#include "mytreemodel2.h"

#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>

#include <QGuiApplication>
#include <QClipboard>

#include <QJsonValue>

#include <QXmlStreamWriter>
#include <QXmlQuery>


MyTreeModel2::MyTreeModel2(QObject *parent)
    : QAbstractItemModel(parent)
{
    headers.append("classname");
    headers.append("abs_x");
    headers.append("abs_y");
    headers.append("mainTextProperty");
    headers.append("objectName");
    headers.append("enabled");
    headers.append("visible");
    headers.append("width");
    headers.append("height");

    QJsonObject rootObject;
    rootObject.insert("classname", QJsonValue::fromVariant("Classname"));
    rootObject.insert("abs_x", QJsonValue::fromVariant("abx"));
    rootObject.insert("abs_y", QJsonValue::fromVariant("aby"));
    rootObject.insert("mainTextProperty", QJsonValue::fromVariant("Text"));
    rootObject.insert("objectName", QJsonValue::fromVariant("ObjectName"));
    rootObject.insert("enabled", QJsonValue::fromVariant("ena"));
    rootObject.insert("visible", QJsonValue::fromVariant("vis"));
    rootObject.insert("width", QJsonValue::fromVariant("wid"));
    rootObject.insert("height", QJsonValue::fromVariant("hei"));

    m_rootItem = new TreeItem2(rootObject);
}

void MyTreeModel2::fillModel(const QJsonObject &object)
{
    beginResetModel();

    m_rootItem->genocide();

    QJsonObject data = object;
    const QJsonArray childsData = data.take(QStringLiteral("children")).toArray();

    TreeItem2 *firstItem = new TreeItem2(data, m_rootItem);
    m_rootItem->appendChild(firstItem);

    QList<TreeItem2 *> childs = processChilds(childsData, firstItem);
    for (TreeItem2 *child : childs) {
        firstItem->appendChild(child);
    }

    endResetModel();
}

void MyTreeModel2::loadDump(const QString &dump)
{
    qDebug() << Q_FUNC_INFO << dump.length();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(dump.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError) {
        fillModel(doc.object());
    } else {
        qWarning() << Q_FUNC_INFO << error.errorString();
    }
}

QList<TreeItem2 *> MyTreeModel2::processChilds(const QJsonArray &data, TreeItem2 *parentItem)
{
    QList<TreeItem2 *> childs;
    for (const QJsonValue &value : data) {
        QJsonObject childData = value.toObject();
        const QJsonArray childsData = childData.take(QStringLiteral("children")).toArray();
        TreeItem2 *child = new TreeItem2(childData, parentItem);
        QList<TreeItem2 *> itemChilds = processChilds(childsData, child);
        for (TreeItem2 *itemChild : itemChilds) {
            child->appendChild(itemChild);
        }
        childs.append(child);
    }
    return childs;
}

QVariant MyTreeModel2::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
            return QVariant();

    if (role > headers.count())
        return QVariant();

    TreeItem2 *item = static_cast<TreeItem2*>(index.internalPointer());

    return item->data(headers[index.column()]).toString();
}

Qt::ItemFlags MyTreeModel2::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant MyTreeModel2::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_rootItem->data(headers[section]);
    }

    return QVariant();
}

QModelIndex MyTreeModel2::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    TreeItem2 *parentItem;

    if (parent.isValid()) {
        parentItem = static_cast<TreeItem2*>(parent.internalPointer());
    } else {
        parentItem = m_rootItem;
    }

    TreeItem2 *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex MyTreeModel2::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    TreeItem2 *childItem = static_cast<TreeItem2*>(index.internalPointer());
    TreeItem2 *parentItem = childItem->parent();

    if (parentItem == m_rootItem) {
        createIndex(0, 0, parentItem);
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int MyTreeModel2::rowCount(const QModelIndex &parent) const
{
    TreeItem2 *parentItem = nullptr;
    if (!parent.isValid()) {
        if (!m_rootItem) {
            return 0;
        }
        parentItem = m_rootItem;
    }
    else {
        if (parent.column() > 0) {
            return 0;
        }
        parentItem = static_cast<TreeItem2*>(parent.internalPointer());
    }
    if (!parentItem) {
        return 0;
    }

    return parentItem->childCount();
}

int MyTreeModel2::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return static_cast<TreeItem2*>(parent.internalPointer())->columnCount();
    }
    return headers.count();
}

QRect MyTreeModel2::getRect(const QModelIndex &index)
{
    QRect rect;
    if (!index.isValid()) {
        return rect;
    }

    TreeItem2 *item = static_cast<TreeItem2*>(index.internalPointer());
    rect.setX(item->data("abs_x").toInt());
    rect.setY(item->data("abs_y").toInt());
    rect.setWidth(item->data("width").toInt());
    rect.setHeight(item->data("height").toInt());

    return rect;
}

QJsonObject MyTreeModel2::getData(const QModelIndex &index)
{
    if (!index.isValid()) {
        return QJsonObject();
    }

    TreeItem2 *item = static_cast<TreeItem2*>(index.internalPointer());

    return item->data();
}

void MyTreeModel2::copyToClipboard(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    TreeItem2 *item = static_cast<TreeItem2*>(index.internalPointer());

    qGuiApp->clipboard()->setText(item->data("id").toString());
}

QString MyTreeModel2::searchXPath(const QString &xpath, const QString &currentId)
{
    QString out;
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    recursiveDumpXml(&writer, m_rootItem->child(0));
    writer.writeEndDocument();

    qDebug().noquote() << out;

    QXmlQuery query(QXmlQuery::XPath20);
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

void MyTreeModel2::recursiveDumpXml(QXmlStreamWriter *writer, TreeItem2 *parent)
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
        TreeItem2 *child = parent->child(i);
        recursiveDumpXml(writer, child);
    }
    writer->writeEndElement();
}

QVariantList MyTreeModel2::getChildrenIndexes(TreeItem2 *node)
{
    QVariantList indexes;
    TreeItem2 *parent = node ? node : m_rootItem;

    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem2 *child = parent->child(i);
        indexes.push_back(createIndex(i, 0, reinterpret_cast<quintptr>(child)));

        indexes.append(getChildrenIndexes(child));
    }

    return indexes;
}

QModelIndex MyTreeModel2::searchIndex(const QString &key, const QVariant &value, bool partialSearch, const QModelIndex &currentIndex, TreeItem2 *node)
{
    static bool currentFound = false;
    if (!node) {
        currentFound = false;
    }
    if (!currentIndex.isValid()) {
        currentFound = true;
    }
    TreeItem2 *parent = node ? node : m_rootItem;

    QModelIndex index;
    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem2 *child = parent->child(i);
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
        index = searchIndex(key, value, partialSearch, currentIndex, m_rootItem->child(0));
    }

    return index;
}

QModelIndex MyTreeModel2::searchIndex(SearchType key, const QVariant &value, bool partialSearch, const QModelIndex &currentIndex, TreeItem2 *node)
{
    if (key == SearchType::XPath) {
        QString currentId;
        if (currentIndex.internalPointer()) {
            TreeItem2 *item = static_cast<TreeItem2*>(currentIndex.internalPointer());
            currentId = item->data(QStringLiteral("id")).toString();
        }

        const QString itemId = searchXPath(value.toString(), currentId);
        if (itemId.isEmpty()) {
            return QModelIndex();
        }
        return searchIndex(QStringLiteral("id"), itemId, false, currentIndex, node);
    } else {
        const QStringList keys{
            QStringLiteral("classname"),
            QStringLiteral("mainTextProperty"),
            QStringLiteral("objectName")};
        const QString sKey = keys[static_cast<std::underlying_type<SearchType>::type>(key)];
        qDebug() << Q_FUNC_INFO << sKey << currentIndex << node;
        return searchIndex(sKey, value, partialSearch, currentIndex, node);
    }
}

QModelIndex MyTreeModel2::searchByCoordinates(qreal posx, qreal posy, TreeItem2 *node)
{
    TreeItem2 *parent = node ? node : m_rootItem;
    QModelIndex childIndex;

    for (int i = 0; i != parent->childCount(); ++i) {
        TreeItem2 *child = parent->child(i);
        const QVariant activeVariant = child->data(QStringLiteral("active"));
        const bool active = activeVariant.isValid() ? activeVariant.toBool() : true;
        if (!active) {
            continue;
        }

        const QVariant visibleVariant = child->data(QStringLiteral("visible"));
        const bool visible = visibleVariant.isValid() ? visibleVariant.toBool() : true;
        if (!visible) {
            continue;
        }

        const QVariant enabledVariant = child->data(QStringLiteral("enabled"));
        const bool enabled = enabledVariant.isValid() ? enabledVariant.toBool() : true;
        if (!enabled) {
            continue;
        }

        const QVariant opacityVariant = child->data(QStringLiteral("opacity"));
        const float opacity = opacityVariant.isValid() ? opacityVariant.toFloat() : 1.0f;
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

QModelIndex MyTreeModel2::searchByCoordinates(const QPointF &pos, TreeItem2 *node)
{
    return searchByCoordinates(pos.x() * 2, pos.y() * 2, node);
}

TreeItem2::TreeItem2(const QJsonObject &data, TreeItem2 *parent)
    : m_data(data)
    , m_parent(parent)
{

}

TreeItem2::~TreeItem2()
{
    qDeleteAll(m_childs);
}

void TreeItem2::appendChild(TreeItem2 *child)
{
    m_childs.append(child);
}

TreeItem2 *TreeItem2::child(int index)
{
    if (index < 0 || index >= m_childs.count()) {
        return nullptr;
    }

    return m_childs.value(index);
}

QVector<TreeItem2*> TreeItem2::childs()
{
    return m_childs;
}

int TreeItem2::childCount() const
{
    return m_childs.count();
}

void TreeItem2::genocide()
{
    qDeleteAll(m_childs);
    m_childs.clear();
}

QVariant TreeItem2::data(const QString &roleName) const
{
    return m_data.value(roleName).toVariant();
}

QJsonObject TreeItem2::data() const
{
    return m_data;
}

int TreeItem2::columnCount() const
{
    return m_data.count();
}

int TreeItem2::row()
{
    if (!m_parent) {
        return 0;
    }

    return m_parent->row(this);
}

int TreeItem2::row(TreeItem2 *child)
{
    return m_childs.indexOf(child);
}

TreeItem2 *TreeItem2::parent()
{
    return m_parent;
}
