#ifndef MYTREEMODEL_H
#define MYTREEMODEL_H

#include <QAbstractItemModel>
#include <QJSonObject>
#include <QRect>

class TreeItem
{
public:
    TreeItem(const QJsonObject &data, TreeItem *parent = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);
    TreeItem *child(int index);
    int childCount() const;

    QVariant data(const QString &roleName) const;
    int columnCount() const;

    int row();
    int row(TreeItem *child);

    TreeItem *parent();

private:

    QVector<TreeItem*> m_childs;
    QJsonObject m_data;
    TreeItem *m_parent = nullptr;

};

class MyTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit MyTreeModel(const QJsonObject &data, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QRect getRect(const QModelIndex &index);
    Q_INVOKABLE void copyToClipboard(const QModelIndex &index);

public slots:
    void fillModel(const QJsonObject &object);

private:
    QList<TreeItem*> processChilds(const QJsonArray &data, TreeItem *parentItem);

    QStringList m_roleNames;
    QHash<int, QByteArray> m_roles;

    TreeItem *m_rootItem = nullptr;
};

#endif // MYTREEMODEL_H
