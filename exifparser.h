#ifndef EXIFPARSER_H
#define EXIFPARSER_H

#include <QString>
#include <QByteArray>


class ExifParser
{
public:
    ExifParser(const QString &filepath);
    bool isValid() const;
    unsigned short getOrientation() const;
    
private:
    bool valid = false;
    unsigned short orientation = 1;
    
    const QByteArray MARKER_JPEG_START = QByteArray::fromHex("FFD8");
    const QByteArray MARKER_EXIF_START = QByteArray::fromHex("FFE1");
    const QByteArray EXIF_CODE = QByteArray::fromHex("457869660000");
    const QByteArray LITTLE_ENDIAN_CODE = QByteArray::fromHex("4949");
    const QByteArray BIG_ENDIAN_CODE = QByteArray::fromHex("4D4D");
    // All sizes in bytes
    const int JPEG_HEADER_MAX_SIZE = 65536;
    const int TIFF_HEADER_SIZE = 8;
    const int UNSIGNED_SHORT_SIZE = 2;
    const int UNSIGNED_LONG_SIZE = 4;
    const int TAG_SIZE = 12;
    // Tag types
    const unsigned short TAG_TYPE_ORIENTATION = 0x112;
    
    bool readHeaderBytes(const QString &filepath, QByteArray &buffer);
    bool areBytesEqual(const QByteArray &source, const QByteArray &comp, 
                              int startIndex);
    unsigned short readUnsignedShort(const QByteArray &source, int startIndex, 
                                            bool isBigEndian=true);
    unsigned long readUnsignedLong(const QByteArray &source, int startIndex, 
                                          bool isBigEndian=true);
    bool readOrientationTag(const QByteArray &source, int startIndex, bool isBigEndian);
};

#endif // EXIFPARSER_H
