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

    // Set data for display (MEMS zero bias data)
    void setData(const QVector<double> &time1,
                 const QVector<double> &bgx1, const QVector<double> &bgy1, const QVector<double> &bgz1,
                 const QVector<double> &bax1, const QVector<double> &bay1, const QVector<double> &baz1,
                 const QVector<double> &time2,
                 const QVector<double> &bgx2, const QVector<double> &bgy2, const QVector<double> &bgz2,
                 const QVector<double> &bax2, const QVector<double> &bay2, const QVector<double> &baz2,
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
    // Data for MEMS zero bias
    QVector<double> time1_, bgx1_, bgy1_, bgz1_, bax1_, bay1_, baz1_;
    QVector<double> time2_, bgx2_, bgy2_, bgz2_, bax2_, bay2_, baz2_;
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
