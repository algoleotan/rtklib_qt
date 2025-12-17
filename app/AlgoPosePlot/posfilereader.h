#ifndef POSFILEREADER_H
#define POSFILEREADER_H

#include <QString>
#include <QVector>
#include <QDateTime>

// Structure to hold one epoch of position data
struct PosEpoch {
    QDateTime time;          // GPST time
    double x, y, z;          // ECEF coordinates
    int Q;                   // Solution quality
    int ns;                  // Number of satellites
    double roll;             // Roll angle (deg)
    double pitch;            // Pitch angle (deg)
    double yaw;              // Yaw angle (deg)
    double ve;               // East velocity (m/s)
    double vn;               // North velocity (m/s)
    double vu;               // Up velocity (m/s)
    int vstat;               // Velocity status
};

class PosFileReader
{
public:
    PosFileReader();
    ~PosFileReader();

    // Read .pos file and return all epochs
    bool readFile(const QString &filename);

    // Get all epochs
    const QVector<PosEpoch>& getEpochs() const { return epochs_; }

    // Get time series for plotting
    void getTimeSeries(QVector<double> &time, QVector<double> &roll,
                      QVector<double> &pitch, QVector<double> &yaw,
                      QVector<double> &ve, QVector<double> &vn, QVector<double> &vu);

    // Get error message
    QString getErrorMessage() const { return errorMessage_; }

private:
    QVector<PosEpoch> epochs_;
    QString errorMessage_;

    // Parse one line of data
    bool parseLine(const QString &line, PosEpoch &epoch);
};

#endif // POSFILEREADER_H
