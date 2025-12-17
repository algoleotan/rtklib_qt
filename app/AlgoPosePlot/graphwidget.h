#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    // Set data for display
    void setData(const QVector<double> &time1, const QVector<double> &roll1,
                 const QVector<double> &pitch1, const QVector<double> &yaw1,
                 const QVector<double> &ve1, const QVector<double> &vn1, const QVector<double> &vu1,
                 const QVector<double> &time2, const QVector<double> &roll2,
                 const QVector<double> &pitch2, const QVector<double> &yaw2,
                 const QVector<double> &ve2, const QVector<double> &vn2, const QVector<double> &vu2,
                 bool showFile1, bool showFile2);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Data
    QVector<double> time1_, roll1_, pitch1_, yaw1_, ve1_, vn1_, vu1_;
    QVector<double> time2_, roll2_, pitch2_, yaw2_, ve2_, vn2_, vu2_;
    bool showFile1_, showFile2_;

    // Graph areas
    struct GraphArea {
        int x, y, width, height;
    };
    GraphArea graphAreas_[6];

    // Zoom and pan state
    double timeRangeStart_;      // Start time for display
    double timeRangeEnd_;        // End time for display
    double fullTimeRangeStart_;  // Full data range start
    double fullTimeRangeEnd_;    // Full data range end

    // Pan state
    bool isPanning_;
    QPoint lastPanPos_;

    // Helper functions
    void calculateGraphAreas();
    void drawGraphs(QPainter &painter);
    void drawGraph(QPainter &painter, const GraphArea &area,
                  const QString &title, const QString &yLabel,
                  const QVector<double> &time1, const QVector<double> &data1,
                  const QVector<double> &time2, const QVector<double> &data2);
    int getGraphIndexAt(const QPoint &pos);
    void resetView();
    void zoomIn(double factor = 0.8);
    void zoomOut(double factor = 1.25);
};

#endif // GRAPHWIDGET_H
