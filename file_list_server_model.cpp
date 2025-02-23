#include "file_list_server_model.h"

FileListServerModel::FileListServerModel(QList<File>& fileList, QObject* parent )
    : QAbstractTableModel(parent), itemData(fileList)
{

}

int FileListServerModel::rowCount(const QModelIndex& parent) const
{
    return itemData.count();
}

int FileListServerModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant FileListServerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= itemData.size())
        return QVariant();

    // image or icon
    if (role == Qt::DecorationRole && index.column() == 0) {
        return QVariant(itemData[index.row()].m_icon);
    }
    // Data to display
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 1)
            return QVariant(itemData[index.row()].m_fileName);
        else if (index.column() == 2 && itemData[index.row()].m_fileSize != 0)
            return QVariant(locale.formattedDataSize(itemData[index.row()].m_fileSize));
        else if (index.column() == 3)
            return QVariant(itemData[index.row()].m_lastModified);
        else if (index.column() == 4)
            return QVariant(itemData[index.row()].m_filePath);
        else
            return QVariant();
    }
    else
        return QVariant();
}

QVariant FileListServerModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    // Only handle DisplayRole
    if (role != Qt::DisplayRole)
        return QVariant();

    // QFont font;
    // font.setBold(true);

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return QString("");
            case 1: return QString("Name");
            case 2: return QString("Size");
            case 3: return QString("Date Modified");
            case 4: return QString("Path On Server");
            default: return QString("");
        }
    }
    return QVariant();
}
