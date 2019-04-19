#include "filebuffer.h"

FileBuffer::FileBuffer(QObject *parent) : QObject(parent)
{
    
}

// public slots

void FileBuffer::load(const QStringList &filepaths) {
    this->filepaths = filepaths;
    // todo load first file
}

void FileBuffer::next() {
    
}
