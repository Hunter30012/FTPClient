#ifndef FILE_LIST_SERVER_MODEL_H
#define FILE_LIST_SERVER_MODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QFileIconProvider>
#include "file.h"

class FileListServerModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FileListServerModel(QList<File>& fileList, QObject* parent = nullptr);

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QList<File> itemData;
    QFileIconProvider provider;

    QLocale locale; // format size (kB, MB, GB)
};

#endif // FILE_LIST_SERVER_MODEL_H
