#include "posfilereader.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

PosFileReader::PosFileReader()
{
}

PosFileReader::~PosFileReader()
{
}

bool PosFileReader::readFile(const QString &filename)
{
    epochs_.clear();
    errorMessage_.clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage_ = QString("Cannot open file: %1").arg(filename);
        return false;
    }

    QTextStream in(&file);
    int lineNum = 0;
    bool firstDataLinePrinted = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNum++;

        // Skip empty lines and comment lines
        if (line.trimmed().isEmpty() || line.startsWith('%')) {
            continue;
        }

        // Print first data line for debugging
        if (!firstDataLinePrinted) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            qDebug("First data line has %d fields", parts.size());
            if (parts.size() >= 28) {
                qDebug("Fields 22-27: bgx=%s bgy=%s bgz=%s bax=%s bay=%s baz=%s",
                       parts[22].toUtf8().constData(), parts[23].toUtf8().constData(),
                       parts[24].toUtf8().constData(), parts[25].toUtf8().constData(),
                       parts[26].toUtf8().constData(), parts[27].toUtf8().constData());
            }
            firstDataLinePrinted = true;
        }

        PosEpoch epoch;
        if (parseLine(line, epoch)) {
            epochs_.append(epoch);
        } else {
            // Continue reading even if one line fails
            qWarning("Failed to parse line %d", lineNum);
        }
    }

    file.close();

    if (epochs_.isEmpty()) {
        errorMessage_ = "No valid data found in file";
        return false;
    }

    qDebug("Successfully loaded %d epochs from %s", epochs_.size(), filename.toUtf8().constData());
    return true;
}

bool PosFileReader::parseLine(const QString &line, PosEpoch &epoch)
{
    // Split by whitespace
    QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Expected format:
    // YYYY/MM/DD HH:MM:SS.SSS x y z Q ns sdx sdy sdz sdxy sdyz sdzx age ratio roll pitch yaw ve vn vu vstat sdve sdvn sdvu sdroll sdpitch sdyaw bgx bgy bgz bax bay baz ...
    // Fields: 0-1=datetime, 2-4=xyz, 5-6=Q/ns, 7-12=sd, 13-14=age/ratio, 15-17=rpy, 18-20=vel, 21=vstat, 22-24=sdvel, 25-27=sdrpy, 28-30=bgxyz, 31-33=baxyz
    // Minimum 34 fields to include zero bias data (fields 28-33)
    if (parts.size() < 34) {
        qDebug("Line has only %d fields, need at least 34", parts.size());
        return false;
    }

    try {
        // Parse date and time
        QString dateStr = parts[0];  // YYYY/MM/DD
        QString timeStr = parts[1];  // HH:MM:SS.SSS

        QStringList dateParts = dateStr.split('/');
        QStringList timeParts = timeStr.split(':');

        if (dateParts.size() != 3 || timeParts.size() != 3) {
            return false;
        }

        int year = dateParts[0].toInt();
        int month = dateParts[1].toInt();
        int day = dateParts[2].toInt();
        int hour = timeParts[0].toInt();
        int minute = timeParts[1].toInt();
        double second = timeParts[2].toDouble();
        int millisecond = (int)((second - (int)second) * 1000);

        epoch.time = QDateTime(QDate(year, month, day),
                              QTime(hour, minute, (int)second, millisecond));

        // Parse ECEF coordinates
        epoch.x = parts[2].toDouble();
        epoch.y = parts[3].toDouble();
        epoch.z = parts[4].toDouble();

        // Parse quality and satellite count
        epoch.Q = parts[5].toInt();
        epoch.ns = parts[6].toInt();

        // Skip standard deviations (parts 7-12) and age, ratio (parts 13-14)

        // Parse attitude (roll, pitch, yaw)
        epoch.roll = parts[15].toDouble();
        epoch.pitch = parts[16].toDouble();
        epoch.yaw = parts[17].toDouble();

        // Parse velocity (ve, vn, vu)
        epoch.ve = parts[18].toDouble();
        epoch.vn = parts[19].toDouble();
        epoch.vu = parts[20].toDouble();

        // Parse velocity status
        epoch.vstat = parts[21].toInt();

        // Skip velocity std (parts 22-24) and attitude std (parts 25-27)

        // Parse MEMS zero bias data (gyro bias and accel bias)
        // Fields 28-33: bgx, bgy, bgz (deg/h), bax, bay, baz (ug)
        epoch.bgx = parts[28].toDouble();
        epoch.bgy = parts[29].toDouble();
        epoch.bgz = parts[30].toDouble();
        epoch.bax = parts[31].toDouble();
        epoch.bay = parts[32].toDouble();
        epoch.baz = parts[33].toDouble();

        return true;
    } catch (...) {
        return false;
    }
}

void PosFileReader::getTimeSeries(QVector<double> &time,
                                  QVector<double> &bgx, QVector<double> &bgy, QVector<double> &bgz,
                                  QVector<double> &bax, QVector<double> &bay, QVector<double> &baz)
{
    time.clear();
    bgx.clear();
    bgy.clear();
    bgz.clear();
    bax.clear();
    bay.clear();
    baz.clear();

    if (epochs_.isEmpty()) {
        return;
    }

    // Use epoch time (seconds since 1970-01-01 00:00:00 UTC) for real time axis
    // This allows the X-axis to show actual date/time information
    QDateTime unixEpoch(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::UTC);

    for (int i = 0; i < epochs_.size(); i++) {
        // Time in seconds since Unix epoch (1970-01-01)
        // This gives us absolute time that can be formatted as real date/time
        double t = unixEpoch.msecsTo(epochs_[i].time) / 1000.0;

        time.append(t);
        bgx.append(epochs_[i].bgx);
        bgy.append(epochs_[i].bgy);
        bgz.append(epochs_[i].bgz);
        bax.append(epochs_[i].bax);
        bay.append(epochs_[i].bay);
        baz.append(epochs_[i].baz);
    }
}
