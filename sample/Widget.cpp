#include "Widget.h"
#include "TableModel.h"
#include "TreeModel.h"
#include "TreeModelCustomItem.h"
#include "TreeModelStandardItem.h"

#include <QTableView>
#include <QTreeView>
#include <QStringList>

struct ContentItemData
{
    ContentItemData(const QString &name, size_t page):
        name(name),
        page(page) {}

    QString name;
    size_t page;
};

class ContentItem: public TreeModelCustomItem<ContentItemData>
{
public:
    ContentItem(const ContentItemData &data):
        TreeModelCustomItem(data)
    {
        addGetter(0, Qt::DisplayRole, &ContentItemData::name);
        addGetter(1, Qt::DisplayRole, &ContentItemData::page);
    }
};

Widget::Widget(QWidget *parent):
    QTabWidget(parent)
{
    //table
    auto *tableView = new QTableView(this);
    addTab(tableView, "Table");

    auto *tableModel = new TableModel<QStringList>(this);
    tableView->setModel(tableModel);
    tableModel->setHeaderData(0, Qt::Horizontal, "Text", Qt::DisplayRole);
    tableModel->setHeaderData(1, Qt::Horizontal, "Text size", Qt::DisplayRole);
    tableModel->addGetter(0, Qt::DisplayRole);
    tableModel->addGetter(0, Qt::ToolTipRole, [](const QString &data){return "Size = " + data;});
    tableModel->addGetter(1, Qt::DisplayRole, &QString::size);
    tableModel->setValues(QStringList() << "One" << "Two" << "Three");

    //tree
    auto *treeView = new QTreeView(this);
    addTab(treeView, "Tree");

    auto *treeModel = new TreeModel(this);
    treeView->setModel(treeModel);
    auto *t1 = new ContentItem(ContentItemData("Preface", 10));
    auto *t2 = new ContentItem(ContentItemData("Introduction", 13));
    t2->appendChild(new ContentItem(ContentItemData("Demystifying GIS", 13)));
    t2->appendChild(new ContentItem(ContentItemData("Finding Free Data Sources and Applications", 14)));
    t2->appendChild(new ContentItem(ContentItemData("Becoming a GIS Programmer", 16)));
    auto *t3 = new ContentItem(ContentItemData("Vectors", 19));
    t3->appendChild(new ContentItem(ContentItemData("Raw Materials", 19)));
    t3->appendChild(new ContentItem(ContentItemData("Raster Data", 20)));
    t3->appendChild(new ContentItem(ContentItemData("Vector Data", 24)));
    t3->appendChild(new ContentItem(ContentItemData("Types of Vector Data ", 24)));

    auto *rootItem = new TreeModelStandardItem(2);
    rootItem->setData(0, "Name", Qt::DisplayRole);
    rootItem->setData(1, "Page", Qt::DisplayRole);
    treeModel->setRootItem(rootItem);
    treeModel->getRootItem()->appendChild(t1);
    treeModel->getRootItem()->appendChild(t2);
    treeModel->getRootItem()->appendChild(t3);
}
