#ifndef TREEVIEWDIALOG_H
#define TREEVIEWDIALOG_H

#include <QObject>
#include <QDialog>
#include "PaintedWidget.hpp"
#include "mytreemodel2.h"

class QTreeView;
class QAbstractItemModel;
class QLineEdit;
class QCheckBox;
class QSettings;
class QLabel;

class SocketConnector;

class TreeViewDialog : public QDialog
{
    Q_OBJECT
public:
    TreeViewDialog();

public slots:
    void init();

signals:
    void search(const QString &searchStr, const QString &key, bool partialSearch, const QModelIndex &currentIndex, TreeItem2 *node = nullptr);

private slots:
    void selectSearchResult(const QModelIndex &index);

    void onContextMenuRequested(const QPoint &pos);

    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void reloadImage();

    QLayout *createTopLayout();
    QLayout *createDeviceLayout();
    QLayout *createTreeLayout();
    QLayout *createSearchLayout();

    MyTreeModel2 *model;

    QTreeView *treeView;
    QLineEdit *searchLineEdit;
    QCheckBox *partialCheckBox;
    PaintedWidget *paintedWidget;

    QSettings *settings;

    SocketConnector *socket;
    MyTreeModel2::SearchType searchType{MyTreeModel2::SearchType::ClassName};
};

#endif // TREEVIEWDIALOG_H
