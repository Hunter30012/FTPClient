#include "file_handler.h"

QString FileHandler::getPreviousFolderPath(QString fullpath)
{
    if (fullpath.endsWith(":/") || fullpath.endsWith(":")) {
        // is root path
        return "";
    }
    else if(fullpath.endsWith("/.")) {
        // is current path  -> cut /.
        fullpath = fullpath.left(fullpath.lastIndexOf("/"));
    }
    return fullpath.left(fullpath.lastIndexOf("/"));
}
