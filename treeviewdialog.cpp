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
    connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged, [this](const QModelIndex &current, const QModelIndex &previous) {
        paintedWidget->setItemRect(model->getRect(current));
    });
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

    auto portLineEdit = new QLineEdit(this);
    connect(portLineEdit, &QLineEdit::textChanged, [&](const QString &text) {
        settings->setValue(QStringLiteral("connection/port"), text);
        socket->setProperty("port", text);
    });
    portLineEdit->setPlaceholderText("8888");
    portLineEdit->setText(settings->value(QStringLiteral("connection/port"),
                                          portLineEdit->placeholderText()).toString());
    portLineEdit->setFixedWidth(50);

    auto appNameLineEdit = new QLineEdit(this);
    connect(appNameLineEdit, &QLineEdit::textChanged, [&](const QString &text) {
        settings->setValue(QStringLiteral("application/name"), text);
        socket->setProperty("applicationName", text);
    });
    appNameLineEdit->setPlaceholderText("application-name");
    appNameLineEdit->setText(settings->value(QStringLiteral("application/name"),
                                             appNameLineEdit->placeholderText()).toString());
    appNameLineEdit->setFixedWidth(108);

    auto connectButton = new QPushButton(tr("Connect"), this);
    connect(connectButton, &QPushButton::released, [=]() {
        socket->setConnected(!socket->isConnected());
    });

    auto dumpTreeButton = new QPushButton(tr("Dump tree"), this);
    connect(dumpTreeButton, &QPushButton::released, [=]() {
        const QString data = socket->getDumpTree();
        if (!data.isEmpty()) {
            model->loadDump(data);
        }
        if (socket->getGrabWindow()) {
            paintedWidget->setImage("dump.png", true);
        }
    });
    dumpTreeButton->setVisible(false);

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

    auto expandAllButton = new QPushButton(tr("Expand all"), this);
    connect(expandAllButton, &QPushButton::clicked, [=]() {
        treeView->expandAll();
    });
    expandAllButton->setVisible(false);

    connectButton->setFixedWidth(80);
    dumpTreeButton->setFixedWidth(80);
    dumpPageButton->setFixedWidth(80);
    dumpCoverButton->setFixedWidth(80);
    expandAllButton->setFixedWidth(80);

    connectionLayout->addWidget(ipLineEdit);
    connectionLayout->addWidget(portLineEdit);
    connectionLayout->addWidget(appNameLineEdit);
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(dumpTreeButton);
    connectionLayout->addWidget(dumpPageButton);
    connectionLayout->addWidget(dumpCoverButton);
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

QLayout *TreeViewDialog::createTreeLayout()
{
    auto treeLayout = new QVBoxLayout;
    treeLayout->addWidget(treeView);

    return treeLayout;
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

void TreeViewDialog::init()
{
    auto formLayout = new QVBoxLayout;
    formLayout->addLayout(createTopLayout());

    auto centerLayout = new QHBoxLayout;
    centerLayout->addLayout(createDeviceLayout());
    centerLayout->addLayout(createTreeLayout());
    formLayout->addLayout(centerLayout);

    formLayout->addLayout(createSearchLayout());

    setLayout(formLayout);

//    QFile file("dump.json");
//    file.open(QIODevice::ReadOnly);
//    model->loadDump(file.readAll());
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

void TreeViewDialog::onContextMenuRequested(const QPoint &pos)
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

        QModelIndex index = model->searchByCoordinates(me->localPos());
        if (index.isValid()) {
            selectSearchResult(index);
        }
    }

    return QObject::eventFilter(obj, event);
}

void TreeViewDialog::reloadImage()
{

}
