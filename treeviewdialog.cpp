// Copyright (c) 2019-2020 Open Mobile Platform LLC.
#include "treeviewdialog.h"
#include <QLineEdit>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDir>
#include <QSizePolicy>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QDebug>
#include <QEvent>
#include <QSettings>
#include <QMouseEvent>
#include <QHeaderView>
#include <QAction>
#include <QShortcut>
#include <QEventLoop>
#include <QSplitter>

#include "mytreemodel.h"
#include "iteminfodialog.h"
#include "socketconnector.h"

TreeViewDialog::TreeViewDialog()
{
    socket = new SocketConnector(this);
    settings = new QSettings(QStringLiteral("qainspector.ini"), QSettings::IniFormat, this);

    treeView = new QTreeView(this);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    model = new MyTreeModel2;
    treeView->setModel(model);
    treeView->setColumnWidth(0, 300);
    treeView->setColumnWidth(1, 50);
    treeView->setColumnWidth(2, 50);
    treeView->setColumnWidth(3, 300);
    treeView->setColumnWidth(4, 200);
    treeView->setColumnWidth(5, 50);
    treeView->setColumnWidth(6, 50);
    treeView->setColumnWidth(7, 50);
    treeView->setColumnWidth(8, 50);

    connect(treeView, &QTreeView::customContextMenuRequested, this, &TreeViewDialog::onContextMenuRequested);
    connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged, [this](const QModelIndex &current, const QModelIndex &) {
        paintedWidget->setItemRect(model->getRect(current));
    });

    restoreGeometry(settings->value("window/geometry").toByteArray());
}

QLayout *TreeViewDialog::createTopLayout()
{
    auto connectionLayout = new QHBoxLayout;
    auto ipLineEdit = new QLineEdit(this);
    connect(ipLineEdit, &QLineEdit::textChanged, [&](const QString &text) {
        settings->setValue(QStringLiteral("connection/host"), text);
        socket->setProperty("hostname", text);
    });
    ipLineEdit->setPlaceholderText("127.0.0.1");
    ipLineEdit->setText(settings->value(QStringLiteral("connection/host"),
                                        ipLineEdit->placeholderText()).toString());
    ipLineEdit->setFixedWidth(80);
    connectionLayout->addWidget(ipLineEdit);

    auto portLineEdit = new QLineEdit(this);
    connect(portLineEdit, &QLineEdit::textChanged, [&](const QString &text) {
        settings->setValue(QStringLiteral("connection/port"), text);
        socket->setProperty("port", text);
    });
    portLineEdit->setPlaceholderText("8888");
    portLineEdit->setText(settings->value(QStringLiteral("connection/port"),
                                          portLineEdit->placeholderText()).toString());
    portLineEdit->setFixedWidth(50);
    connectionLayout->addWidget(portLineEdit);

    auto appNameLineEdit = new QLineEdit(this);
    connect(appNameLineEdit, &QLineEdit::textChanged, [&](const QString &text) {
        settings->setValue(QStringLiteral("application/name"), text);
        socket->setProperty("applicationName", text);
    });
    appNameLineEdit->setPlaceholderText("application-name");
    appNameLineEdit->setText(settings->value(QStringLiteral("application/name"),
                                             appNameLineEdit->placeholderText()).toString());
    appNameLineEdit->setFixedWidth(108);
    connectionLayout->addWidget(appNameLineEdit);

    auto connectButton = new QPushButton(tr("Connect"), this);
    connect(connectButton, &QPushButton::released, [=]() {
        socket->setConnected(!socket->isConnected());
    });
    connectButton->setFixedWidth(80);
    connectionLayout->addWidget(connectButton);

    auto dumpTreeButton = new MyPushButton(tr("Dump tree"), this);
    connect(dumpTreeButton, &MyPushButton::released, [=]() {
        const QString data = socket->getDumpTree();
        if (!data.isEmpty()) {
            model->loadDump(data);
        }
        if (socket->getGrabWindow()) {
            paintedWidget->setImage("dump.png", true);
        }
    });
    connect(dumpTreeButton, &MyPushButton::shiftClicked, [=]() {
        QEventLoop loop;
        QTimer::singleShot(3000, &loop, &QEventLoop::quit);
        loop.exec();

        const QString data = socket->getDumpTree();
        if (!data.isEmpty()) {
            model->loadDump(data);
        }
        if (socket->getGrabWindow()) {
            paintedWidget->setImage("dump.png", true);
        }
    });
    dumpTreeButton->setVisible(false);
    dumpTreeButton->setFixedWidth(80);
    connectionLayout->addWidget(dumpTreeButton);

    auto dumpPageButton = new QPushButton(tr("Dump page"), this);
    connect(dumpPageButton, &QPushButton::released, [=]() {
        const QString data = socket->getDumpPage();
        if (!data.isEmpty()) {
            model->loadDump(data);
        }
        if (socket->getGrabWindow()) {
            paintedWidget->setImage("dump.png", true);
        }
    });
    dumpPageButton->setVisible(false);
    dumpPageButton->setFixedWidth(80);
    connectionLayout->addWidget(dumpPageButton);

    auto dumpCoverButton = new QPushButton(tr("Dump cover"), this);
    connect(dumpCoverButton, &QPushButton::released, [=]() {
        const QString data = socket->getDumpCover();
        if (!data.isEmpty()) {
            model->loadDump(data);
        }
        if (socket->getGrabCover()) {
            paintedWidget->setImage("dump.png", true);
        }
    });
    dumpCoverButton->setVisible(false);
    dumpCoverButton->setFixedWidth(80);
    connectionLayout->addWidget(dumpCoverButton);

    auto expandAllButton = new QPushButton(tr("Expand all"), this);
    connect(expandAllButton, &QPushButton::clicked, [=]() {
        treeView->expandAll();
    });
    expandAllButton->setVisible(false);
    expandAllButton->setFixedWidth(80);
    connectionLayout->addWidget(expandAllButton);

    connect(socket, &SocketConnector::connectedChanged, [=](bool isSocketConnected) {
        connectButton->setText(isSocketConnected ? tr("Disconnect") : tr("Connect"));
        dumpTreeButton->setVisible(isSocketConnected);
        dumpPageButton->setVisible(isSocketConnected);
        dumpCoverButton->setVisible(isSocketConnected);
        expandAllButton->setVisible(isSocketConnected);
    });

    connectionLayout->setAlignment(Qt::AlignLeft);

    return connectionLayout;
}

QLayout *TreeViewDialog::createDeviceLayout()
{
    auto deviceLayout = new QVBoxLayout;

    paintedWidget = new PaintedWidget(this);
    paintedWidget->setImage("dump.png");
    paintedWidget->installEventFilter(this);

    deviceLayout->addWidget(paintedWidget);

    auto refreshButton = new QPushButton(tr("Refresh"), this);
    connect(refreshButton, &QPushButton::released, [=]() {
        paintedWidget->setImage("dump.png", true);
    });
    refreshButton->setFixedWidth(100);
    refreshButton->setVisible(false);
    deviceLayout->addWidget(refreshButton);

    deviceLayout->setSpacing(1);
    deviceLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    deviceLayout->setAlignment(Qt::AlignTop);

    connect(socket, &SocketConnector::connectedChanged, [=](bool isSocketConnected) {
        refreshButton->setVisible(isSocketConnected);
    });

    return deviceLayout;
}

QLayout *TreeViewDialog::createSearchLayout()
{
    auto searchLayout = new QHBoxLayout;
    searchLineEdit = new QLineEdit;
    searchLineEdit->setPlaceholderText("Search pattern");
    partialCheckBox = new QCheckBox(tr("Partial"));

    auto groupBox = new QGroupBox;
    auto groupBoxLayout = new QHBoxLayout;
    auto classNameButton = new QRadioButton("ClassName");
    classNameButton->setChecked(true);
    connect(classNameButton, &QRadioButton::clicked, [&](){
        searchType = MyTreeModel2::SearchType::ClassName;
    });
    auto textButton = new QRadioButton("Text");
    connect(textButton, &QRadioButton::clicked, [&](){
        searchType = MyTreeModel2::SearchType::Text;
    });
    auto objectNameButton = new QRadioButton("ObjectName");
    connect(objectNameButton, &QRadioButton::clicked, [&](){
        searchType = MyTreeModel2::SearchType::ObjectName;
    });
    auto xPathButton = new QRadioButton("XPath");
    connect(xPathButton, &QRadioButton::clicked, [&](){
        searchType = MyTreeModel2::SearchType::XPath;
    });
    groupBoxLayout->addWidget(classNameButton);
    groupBoxLayout->addWidget(textButton);
    groupBoxLayout->addWidget(objectNameButton);
    groupBoxLayout->addWidget(xPathButton);
    groupBox->setLayout(groupBoxLayout);

    auto searchButton = new QPushButton(tr("Search"));
    connect(searchButton, &QPushButton::clicked, [&](bool) {
        auto result = model->searchIndex(
            searchType,
            searchLineEdit->text(),
            partialCheckBox->isChecked(),
            model->index(treeView->currentIndex().row(), 0, treeView->currentIndex().parent()));
        selectSearchResult(result);
    });

    QShortcut *searchShortcut = new QShortcut(QKeySequence("F3"), this);
    connect(searchShortcut, &QShortcut::activated, [&]() {
        auto result = model->searchIndex(
            searchType,
            searchLineEdit->text(),
            partialCheckBox->isChecked(),
            model->index(treeView->currentIndex().row(), 0, treeView->currentIndex().parent()));
        selectSearchResult(result);
    });

    QShortcut *openSearchShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(openSearchShortcut, &QShortcut::activated, [&]() {
        searchLineEdit->setFocus(Qt::ShortcutFocusReason);
    });

    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(partialCheckBox);
    searchLayout->addWidget(groupBox);
    searchLayout->addWidget(searchButton);

    connect(model, &QAbstractItemModel::modelReset, [=]() {
        searchButton->setEnabled(model->rowCount() > 0);
    });
    searchButton->setEnabled(false);

    return searchLayout;
}

void TreeViewDialog::closeEvent(QCloseEvent *event)
{
    settings->setValue("window/geometry", saveGeometry());
    QWidget::closeEvent(event);
}

void TreeViewDialog::init()
{
    auto formLayout = new QVBoxLayout;
    formLayout->addLayout(createTopLayout());

    auto leftFrame = new QFrame();
    auto leftWidget = new QWidget(leftFrame);
    auto device = createDeviceLayout();
    leftWidget->setLayout(device);
    leftFrame->setMinimumWidth(100);
    leftFrame->setMaximumWidth(paintedWidget->width());

    treeView->setMinimumWidth(100);

    auto splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(leftFrame);
    splitter->addWidget(treeView);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(1, 2);

    splitter->restoreState(settings->value("splitterSizes").toByteArray());


    connect(splitter, &QSplitter::splitterMoved, [splitter, this](int, int) {
        settings->setValue("splitterSizes", splitter->saveState());
    });

    formLayout->addWidget(splitter);

    formLayout->addLayout(createSearchLayout());

    setLayout(formLayout);

    QFile file("dump.json");
    file.open(QIODevice::ReadOnly);
    model->loadDump(file.readAll());
}

void expandChildren(const QModelIndex &index, QTreeView *view)
{
    if (!index.isValid()) {
        return;
    }

    auto current = index;
    while (current.isValid()) {
        view->setExpanded(current, true);
        current = current.parent();
    }
}

void TreeViewDialog::selectSearchResult(const QModelIndex &index)
{
    qDebug() << Q_FUNC_INFO << index;
    if (index.isValid()) {
        treeView->selectionModel()->clear();
        expandChildren(index, treeView);
        treeView->setCurrentIndex(index);
        treeView->scrollTo(index);
        treeView->selectionModel()->select(
            QItemSelection(index, index),
            QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void TreeViewDialog::onContextMenuRequested(const QPoint &)
{
    ItemInfoDialog info;
    info.setWindowTitle(tr("properties list"));
    info.setData(model->getData(treeView->currentIndex()));
    info.exec();
}

bool TreeViewDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        paintedWidget->setClickPoint(me->localPos());

        QModelIndex index = model->searchByCoordinates(paintedWidget->scaledClickPoint());
        if (index.isValid()) {
            selectSearchResult(index);
        }
    }

    return QObject::eventFilter(obj, event);
}

void TreeViewDialog::reloadImage()
{

}

MyPushButton::MyPushButton(QWidget *parent)
    : QPushButton(parent)
{

}

MyPushButton::MyPushButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{

}

void MyPushButton::mousePressEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO << event;

    if (event->modifiers() == Qt::ShiftModifier) {
        emit shiftClicked();
    } else {
        QPushButton::mousePressEvent(event);
    }
}
