// Copyright (c) 2019-2020 Open Mobile Platform LLC.
#ifndef TREEVIEWDIALOG_H
#define TREEVIEWDIALOG_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include "PaintedWidget.hpp"
#include "mytreemodel2.h"

class QTreeView;
class QAbstractItemModel;
class QLineEdit;
class QCheckBox;
class QSettings;
class QLabel;

class SocketConnector;

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent = nullptr);
    explicit MyPushButton(const QString &text, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void shiftClicked();
};

class TreeViewDialog : public QWidget
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

    void onContextMenuRequested(const QPoint &);

    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void reloadImage();

    QLayout *createTopLayout();
    QLayout *createDeviceLayout();
    QLayout *createSearchLayout();

    MyTreeModel2 *model;

    QTreeView *treeView;
    QLineEdit *searchLineEdit;
    QCheckBox *partialCheckBox;
    PaintedWidget *paintedWidget;

    QSettings *settings;

    SocketConnector *socket;
    MyTreeModel2::SearchType searchType{MyTreeModel2::SearchType::ClassName};

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // TREEVIEWDIALOG_H
