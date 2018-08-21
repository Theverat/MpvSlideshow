#include "exifparser.h"

#include <QFile>
#include <QDataStream>
#include <QtEndian>
#include <QDebug>

#include <vector>


ExifParser::ExifParser(const QString &filepath) {
    QByteArray buffer;
    if (!readHeaderBytes(filepath, buffer)) {
        qDebug() << "EXIF read fail: Could not read file header";
        return;
    }
    
    if (!areBytesEqual(buffer, MARKER_JPEG_START, 0)) {
        qDebug() << "EXIF read fail: Not a jpeg image";
        return;
    }
    
    const int exifStartPos = buffer.indexOf(MARKER_EXIF_START);
    if (exifStartPos == -1) {
        qDebug() << "EXIF read fail: No exif start marker";
        return;
    }
    
    const int exifLengthPos = exifStartPos + MARKER_EXIF_START.size();
    // const unsigned short exifLength = readUnsignedShort(buffer, 
    //                                                     exifLengthPos);
    
    const int exifCodePos = exifLengthPos + UNSIGNED_SHORT_SIZE;
    if (!areBytesEqual(buffer, EXIF_CODE, exifCodePos)) {
        qDebug() << "EXIF read fail: Header does not contain EXIF data";
        return;
    }
    
    // now we are in the TIFF header (8 bytes)
    const int tiffHeaderPos = exifCodePos + EXIF_CODE.size();
    bool isBigEndian;
    if (areBytesEqual(buffer, BIG_ENDIAN_CODE, tiffHeaderPos)) {
        isBigEndian = true;
    } else if (areBytesEqual(buffer, LITTLE_ENDIAN_CODE, tiffHeaderPos)) {
        isBigEndian = false;
    } else {
        qDebug() << "EXIF read fail: Incorrect endian information in TIFF header";
        return;
    }
    
    // last 6 bytes of TIFF header contain always the same data, skip them
    
    // now we are in the first image file directory (IDF)
    // get amount of EXIF tags (2 bytes)
    const int tagAmountPos = tiffHeaderPos + TIFF_HEADER_SIZE;
    unsigned short tagAmount = readUnsignedShort(buffer, tagAmountPos, 
                                                 isBigEndian);
    
    // read tags
    for (int i = 0; i < tagAmount; ++i) {
        const int tagPosition = tagAmountPos + UNSIGNED_SHORT_SIZE + (TAG_SIZE * i);
        if (readOrientationTag(buffer, tagPosition, isBigEndian)) {
            // Since we are only interested in the orientation, 
            // we can skip all other tags once we found it
            break;
        }
    }
    
    valid = true;
}

bool ExifParser::isValid() const {
    return valid;
}

unsigned short ExifParser::getOrientation() const {
    return orientation;
}

//------------------------------------------------------------------
// private

bool ExifParser::readHeaderBytes(const QString &filepath, QByteArray &buffer) {
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QDataStream stream(&file);
    buffer.resize(JPEG_HEADER_MAX_SIZE);
    const int r = stream.readRawData(buffer.data(), JPEG_HEADER_MAX_SIZE);
    
    if (r == -1 || buffer.size() == 0) {
        return false;
    }
    
    return true;
}

bool ExifParser::areBytesEqual(const QByteArray &source, 
                               const QByteArray &comp, 
                               int startIndex) {
    if (startIndex + comp.size() > source.size())
        return false;
    
    for (int i = 0; i < comp.size(); ++i)
        if (source.at(startIndex + i) != comp.at(i))
            return false;
    return true;
}

unsigned short ExifParser::readUnsignedShort(const QByteArray &source, 
                                             int startIndex,
                                             bool isBigEndian) {
    Q_ASSERT(startIndex + UNSIGNED_SHORT_SIZE <= source.size());
    
    unsigned short raw[UNSIGNED_SHORT_SIZE];
    for(int i = 0; i < UNSIGNED_SHORT_SIZE; ++i)
        raw[i] = static_cast<unsigned short>(source.at(startIndex + i));
    
    unsigned short combined = (raw[1] << 8) | raw[0];
    
    if (isBigEndian)
        return combined;
    else
        return qFromLittleEndian(combined);
}


unsigned long ExifParser::readUnsignedLong(const QByteArray &source, 
                                           int startIndex,
                                           bool isBigEndian) {
    Q_ASSERT(startIndex + UNSIGNED_LONG_SIZE <= source.size());
    
    unsigned long raw[UNSIGNED_LONG_SIZE];
    for(int i = 0; i < UNSIGNED_LONG_SIZE; ++i)
        raw[i] = static_cast<unsigned long>(source.at(startIndex + i));
    
    unsigned long combined = (raw[3] << (8 * 3))
                             | (raw[2] << (8 * 2)) 
                             | (raw[1] << 8) 
                             | raw[0];
    
    if (isBigEndian)
        return combined;
    else
        return qFromLittleEndian(combined);
}

bool ExifParser::readOrientationTag(const QByteArray &source, int startIndex, 
                                    bool isBigEndian) {
    const unsigned short tagType = readUnsignedShort(source, startIndex, isBigEndian);
    // 4 bytes: length of data (as 32 bit number)
    // const unsigned long dataLength = readUnsignedLong(source, startIndex + 4, 
    //                                                   isBigEndian);
    
    // 4 bytes: data in the tag or offset to the data if more than 4 bytes
    if (tagType == TAG_TYPE_ORIENTATION) {
        this->orientation = readUnsignedShort(source, startIndex + 8, 
                                              isBigEndian);
        return true;
    }
    return false;
}
