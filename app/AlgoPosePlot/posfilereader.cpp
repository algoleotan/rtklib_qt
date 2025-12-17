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
    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNum++;

        // Skip empty lines and comment lines
        if (line.trimmed().isEmpty() || line.startsWith('%')) {
            continue;
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

    return true;
}

bool PosFileReader::parseLine(const QString &line, PosEpoch &epoch)
{
    // Split by whitespace
    QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Expected format:
    // YYYY/MM/DD HH:MM:SS.SSS x y z Q ns sdx sdy sdz sdxy sdyz sdzx age ratio roll pitch yaw ve vn vu vstat ...
    if (parts.size() < 21) {
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
        if (parts.size() > 21) {
            epoch.vstat = parts[21].toInt();
        } else {
            epoch.vstat = 0;
        }

        return true;
    } catch (...) {
        return false;
    }
}

void PosFileReader::getTimeSeries(QVector<double> &time, QVector<double> &roll,
                                  QVector<double> &pitch, QVector<double> &yaw,
                                  QVector<double> &ve, QVector<double> &vn, QVector<double> &vu)
{
    time.clear();
    roll.clear();
    pitch.clear();
    yaw.clear();
    ve.clear();
    vn.clear();
    vu.clear();

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
        roll.append(epochs_[i].roll);
        pitch.append(epochs_[i].pitch);
        yaw.append(epochs_[i].yaw);
        ve.append(epochs_[i].ve);
        vn.append(epochs_[i].vn);
        vu.append(epochs_[i].vu);
    }
}
