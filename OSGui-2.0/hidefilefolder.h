#ifndef HIDEFILEFOLDER
#define HIDEFILEFOLDER

#include <QString>
#include <QTreeWidgetItem>
#include "FileSystem.h"

struct hideFileFolderItem {
    QString path ;					/*  文件或文件夹路径 */
    int type;
    QTreeWidgetItem * itself;
    bool hideChild;
} ;

#endif // HIDEFILEFOLDER

