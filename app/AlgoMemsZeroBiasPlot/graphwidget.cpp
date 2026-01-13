#include "graphwidget.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDateTime>
#include <cmath>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
    , showFile1_(true)
    , showFile2_(true)
    , timeRangeStart_(0.0)
    , timeRangeEnd_(1.0)
    , fullTimeRangeStart_(0.0)
    , fullTimeRangeEnd_(1.0)
    , isPanning_(false)
{
    setMinimumSize(800, 600);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(false);
    calculateGraphAreas();
}

void GraphWidget::setData(const QVector<double> &time1,
                          const QVector<double> &bgx1, const QVector<double> &bgy1, const QVector<double> &bgz1,
                          const QVector<double> &bax1, const QVector<double> &bay1, const QVector<double> &baz1,
                          const QVector<double> &time2,
                          const QVector<double> &bgx2, const QVector<double> &bgy2, const QVector<double> &bgz2,
                          const QVector<double> &bax2, const QVector<double> &bay2, const QVector<double> &baz2,
                          bool showFile1, bool showFile2)
{
    time1_ = time1; bgx1_ = bgx1; bgy1_ = bgy1; bgz1_ = bgz1;
    bax1_ = bax1; bay1_ = bay1; baz1_ = baz1;
    time2_ = time2; bgx2_ = bgx2; bgy2_ = bgy2; bgz2_ = bgz2;
    bax2_ = bax2; bay2_ = bay2; baz2_ = baz2;
    showFile1_ = showFile1;
    showFile2_ = showFile2;

    // Calculate full time range from data
    fullTimeRangeStart_ = 1e30;
    fullTimeRangeEnd_ = -1e30;

    for (int i = 0; i < time1_.size(); i++) {
        if (time1_[i] < fullTimeRangeStart_) fullTimeRangeStart_ = time1_[i];
        if (time1_[i] > fullTimeRangeEnd_) fullTimeRangeEnd_ = time1_[i];
    }
    for (int i = 0; i < time2_.size(); i++) {
        if (time2_[i] < fullTimeRangeStart_) fullTimeRangeStart_ = time2_[i];
        if (time2_[i] > fullTimeRangeEnd_) fullTimeRangeEnd_ = time2_[i];
    }

    // Handle case where no data is available
    if (fullTimeRangeStart_ >= fullTimeRangeEnd_) {
        fullTimeRangeStart_ = 0.0;
        fullTimeRangeEnd_ = 1.0;
    }

    // Reset view to show all data
    resetView();
    update();
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    drawGraphs(painter);
}

void GraphWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    calculateGraphAreas();
}

void GraphWidget::calculateGraphAreas()
{
    int w = width() - 20;
    int h = height() - 20;

    // Divide into 2 columns x 3 rows
    int colWidth = (w - 30) / 2;
    int rowHeight = (h - 40) / 3;

    for (int i = 0; i < 6; i++) {
        int row = i / 2;
        int col = i % 2;

        graphAreas_[i].x = 10 + col * (colWidth + 20);
        graphAreas_[i].y = 10 + row * (rowHeight + 20);
        graphAreas_[i].width = colWidth;
        graphAreas_[i].height = rowHeight;
    }
}

void GraphWidget::drawGraphs(QPainter &painter)
{
    // Graph titles for MEMS zero bias data
    const char *titles[] = {
        "bgx (deg/h)", "bgy (deg/h)", "bgz (deg/h)",
        "bax (ug)", "bay (ug)", "baz (ug)"
    };

    QVector<double> *data1[] = {&bgx1_, &bgy1_, &bgz1_, &bax1_, &bay1_, &baz1_};
    QVector<double> *data2[] = {&bgx2_, &bgy2_, &bgz2_, &bax2_, &bay2_, &baz2_};

    for (int i = 0; i < 6; i++) {
        QVector<double> emptyVec;
        drawGraph(painter, graphAreas_[i], titles[i], titles[i],
                 showFile1_ ? time1_ : emptyVec,
                 showFile1_ ? *data1[i] : emptyVec,
                 showFile2_ ? time2_ : emptyVec,
                 showFile2_ ? *data2[i] : emptyVec);
    }
}

void GraphWidget::drawGraph(QPainter &painter, const GraphArea &area,
                            const QString &title, const QString &yLabel,
                            const QVector<double> &time1, const QVector<double> &data1,
                            const QVector<double> &time2, const QVector<double> &data2)
{
    Q_UNUSED(yLabel);

    // Draw border
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::white);
    painter.drawRect(area.x, area.y, area.width, area.height);

    // Draw title
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(area.x, area.y - 5, area.width, 20,
                    Qt::AlignCenter, title);
    painter.setFont(QFont());

    // Graph drawing area (leave margins for axes)
    int marginLeft = 60;
    int marginRight = 10;
    int marginTop = 30;
    int marginBottom = 30;

    int plotX = area.x + marginLeft;
    int plotY = area.y + marginTop;
    int plotW = area.width - marginLeft - marginRight;
    int plotH = area.height - marginTop - marginBottom;

    if (plotW <= 0 || plotH <= 0) return;

    // Draw axes
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(plotX, plotY + plotH, plotX + plotW, plotY + plotH);  // X axis
    painter.drawLine(plotX, plotY, plotX, plotY + plotH);  // Y axis

    // Use current time range for display
    double minTime = timeRangeStart_;
    double maxTime = timeRangeEnd_;

    double minData = 1e30, maxData = -1e30;

    auto updateRange = [&](const QVector<double> &t, const QVector<double> &d) {
        for (int i = 0; i < t.size() && i < d.size(); i++) {
            // Only include data within time range
            if (t[i] >= minTime && t[i] <= maxTime) {
                if (d[i] < minData) minData = d[i];
                if (d[i] > maxData) maxData = d[i];
            }
        }
    };

    if (!time1.isEmpty()) updateRange(time1, data1);
    if (!time2.isEmpty()) updateRange(time2, data2);

    if (minTime >= maxTime || minData >= maxData) {
        painter.drawText(area.x, area.y, area.width, area.height,
                        Qt::AlignCenter, "No Data");
        return;
    }

    // Add 10% margin to data range
    double dataRange = maxData - minData;
    if (dataRange < 1e-6) dataRange = 1.0;  // Avoid division by zero
    minData -= dataRange * 0.1;
    maxData += dataRange * 0.1;

    // Enable clipping to restrict drawing to plot area
    painter.setClipRect(plotX, plotY, plotW, plotH);

    // Draw grid lines
    painter.setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
    for (int i = 1; i < 5; i++) {
        int y = plotY + i * plotH / 5;
        painter.drawLine(plotX, y, plotX + plotW, y);
    }

    // Draw data
    auto drawData = [&](const QVector<double> &t, const QVector<double> &d, const QColor &color) {
        if (t.isEmpty() || d.isEmpty()) return;

        painter.setPen(QPen(color, 2));
        for (int i = 1; i < t.size() && i < d.size(); i++) {
            int x1 = plotX + (int)((t[i-1] - minTime) / (maxTime - minTime) * plotW);
            int y1 = plotY + plotH - (int)((d[i-1] - minData) / (maxData - minData) * plotH);
            int x2 = plotX + (int)((t[i] - minTime) / (maxTime - minTime) * plotW);
            int y2 = plotY + plotH - (int)((d[i] - minData) / (maxData - minData) * plotH);

            painter.drawLine(x1, y1, x2, y2);
        }
    };

    if (!time1.isEmpty()) drawData(time1, data1, Qt::blue);
    if (!time2.isEmpty()) drawData(time2, data2, Qt::red);

    // Draw legend (inside clipped region)
    if (!time1.isEmpty() || !time2.isEmpty()) {
        int legendX = plotX + plotW - 100;
        int legendY = plotY + 10;
        QFont smallFont = painter.font();
        smallFont.setPointSize(8);

        if (!time1.isEmpty()) {
            painter.setPen(QPen(Qt::blue, 2));
            painter.drawLine(legendX, legendY, legendX + 20, legendY);
            painter.setPen(QPen(Qt::black, 1));
            painter.setFont(smallFont);
            painter.drawText(legendX + 25, legendY - 5, 70, 15,
                           Qt::AlignLeft, "File 1");
            legendY += 20;
        }

        if (!time2.isEmpty()) {
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(legendX, legendY, legendX + 20, legendY);
            painter.setPen(QPen(Qt::black, 1));
            painter.setFont(smallFont);
            painter.drawText(legendX + 25, legendY - 5, 70, 15,
                           Qt::AlignLeft, "File 2");
        }
    }

    // Disable clipping to draw axis labels outside the plot area
    painter.setClipping(false);

    // Draw axis labels
    painter.setPen(QPen(Qt::black, 1));
    QFont smallFont = painter.font();
    smallFont.setPointSize(8);
    painter.setFont(smallFont);

    // Y axis labels
    for (int i = 0; i <= 5; i++) {
        double val = minData + (maxData - minData) * i / 5.0;
        int y = plotY + plotH - i * plotH / 5;
        // Draw Y axis label to the left of the plot area
        painter.drawText(area.x, y - 8, marginLeft - 5, 16,
                        Qt::AlignRight | Qt::AlignVCenter,
                        QString::number(val, 'f', 2));
    }

    // X axis labels - convert Unix timestamp to date/time string
    for (int i = 0; i <= 5; i++) {
        double val = minTime + (maxTime - minTime) * i / 5.0;
        int x = plotX + i * plotW / 5;

        // Convert Unix timestamp (seconds) to QDateTime
        QDateTime unixEpoch(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::UTC);
        QDateTime dt = unixEpoch.addSecs((qint64)val);

        // Format as MM-DD HH:MM
        QString timeStr = dt.toString("MM-dd HH:mm");

        painter.drawText(x - 40, plotY + plotH + 5, 80, 16,
                        Qt::AlignCenter, timeStr);
    }

    // X axis label "Date/Time (UTC)"
    painter.drawText(plotX, plotY + plotH + 25, plotW, 20,
                    Qt::AlignCenter, "Date/Time (UTC)");
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    // Get mouse position
    QPoint pos = event->position().toPoint();

    // Check if mouse is over any graph
    int graphIndex = getGraphIndexAt(pos);
    if (graphIndex < 0) {
        event->ignore();
        return;
    }

    // Calculate zoom factor (inverted for natural feeling)
    double delta = event->angleDelta().y();
    if (delta > 0) {
        // Scroll up = zoom in
        zoomIn(0.8);
    } else {
        // Scroll down = zoom out
        zoomOut(1.25);
    }

    event->accept();
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Accept panning anywhere within the widget
        isPanning_ = true;
        lastPanPos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning_) {
        // Calculate pan distance in pixels
        int dx = event->pos().x() - lastPanPos_.x();

        // Only process if there's meaningful movement
        if (dx != 0) {
            // Convert to time units
            // Use the plot width from first graph area
            int marginLeft = 60;
            int marginRight = 10;
            int plotW = graphAreas_[0].width - marginLeft - marginRight;

            if (plotW > 0 && timeRangeEnd_ > timeRangeStart_) {
                double currentRange = timeRangeEnd_ - timeRangeStart_;
                double timeShift = -dx * currentRange / plotW;  // Negative for natural pan direction

                // Apply pan
                double newStart = timeRangeStart_ + timeShift;
                double newEnd = timeRangeEnd_ + timeShift;

                // Clamp to data bounds
                if (newStart < fullTimeRangeStart_) {
                    double shift = fullTimeRangeStart_ - newStart;
                    newStart = fullTimeRangeStart_;
                    newEnd += shift;
                }
                if (newEnd > fullTimeRangeEnd_) {
                    double shift = newEnd - fullTimeRangeEnd_;
                    newEnd = fullTimeRangeEnd_;
                    newStart -= shift;
                }

                // Ensure we don't go below minimum bounds
                if (newStart < fullTimeRangeStart_) {
                    newStart = fullTimeRangeStart_;
                }
                if (newEnd > fullTimeRangeEnd_) {
                    newEnd = fullTimeRangeEnd_;
                }

                timeRangeStart_ = newStart;
                timeRangeEnd_ = newEnd;

                update();
            }
        }

        lastPanPos_ = event->pos();
    } else {
        // Change cursor when over a graph
        if (getGraphIndexAt(event->pos()) >= 0) {
            setCursor(Qt::OpenHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning_ = false;
        setCursor(Qt::ArrowCursor);
    }
}

void GraphWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Double-click to reset view
    if (getGraphIndexAt(event->pos()) >= 0) {
        resetView();
    }
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        zoomIn();
        break;
    case Qt::Key_Minus:
    case Qt::Key_Underscore:
        zoomOut();
        break;
    case Qt::Key_Home:
    case Qt::Key_R:
        resetView();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

int GraphWidget::getGraphIndexAt(const QPoint &pos)
{
    // Check which graph contains the mouse position
    for (int i = 0; i < 6; i++) {
        const GraphArea &area = graphAreas_[i];
        if (pos.x() >= area.x && pos.x() <= area.x + area.width &&
            pos.y() >= area.y && pos.y() <= area.y + area.height) {
            return i;
        }
    }
    return -1;
}

void GraphWidget::resetView()
{
    timeRangeStart_ = fullTimeRangeStart_;
    timeRangeEnd_ = fullTimeRangeEnd_;
    update();
}

void GraphWidget::zoomIn(double factor)
{
    if (timeRangeEnd_ <= timeRangeStart_) return;

    double currentRange = timeRangeEnd_ - timeRangeStart_;
    double newRange = currentRange * factor;

    // Don't zoom in too much
    double minRange = (fullTimeRangeEnd_ - fullTimeRangeStart_) * 0.01;  // 1% of full range
    if (newRange < minRange) newRange = minRange;

    // Center the zoom
    double center = (timeRangeStart_ + timeRangeEnd_) / 2.0;
    timeRangeStart_ = center - newRange / 2.0;
    timeRangeEnd_ = center + newRange / 2.0;

    // Clamp to data bounds
    if (timeRangeStart_ < fullTimeRangeStart_) {
        double shift = fullTimeRangeStart_ - timeRangeStart_;
        timeRangeStart_ = fullTimeRangeStart_;
        timeRangeEnd_ += shift;
    }
    if (timeRangeEnd_ > fullTimeRangeEnd_) {
        double shift = timeRangeEnd_ - fullTimeRangeEnd_;
        timeRangeEnd_ = fullTimeRangeEnd_;
        timeRangeStart_ -= shift;
    }

    // Final check
    if (timeRangeStart_ < fullTimeRangeStart_) timeRangeStart_ = fullTimeRangeStart_;
    if (timeRangeEnd_ > fullTimeRangeEnd_) timeRangeEnd_ = fullTimeRangeEnd_;

    update();
}

void GraphWidget::zoomOut(double factor)
{
    if (timeRangeEnd_ <= timeRangeStart_) return;

    double currentRange = timeRangeEnd_ - timeRangeStart_;
    double newRange = currentRange * factor;

    // Don't zoom out beyond full data range
    double fullRange = fullTimeRangeEnd_ - fullTimeRangeStart_;
    if (newRange > fullRange) {
        // Just reset to full view
        resetView();
        return;
    }

    // Center the zoom
    double center = (timeRangeStart_ + timeRangeEnd_) / 2.0;
    timeRangeStart_ = center - newRange / 2.0;
    timeRangeEnd_ = center + newRange / 2.0;

    // Clamp to data bounds
    if (timeRangeStart_ < fullTimeRangeStart_) {
        double shift = fullTimeRangeStart_ - timeRangeStart_;
        timeRangeStart_ = fullTimeRangeStart_;
        timeRangeEnd_ += shift;
    }
    if (timeRangeEnd_ > fullTimeRangeEnd_) {
        double shift = timeRangeEnd_ - fullTimeRangeEnd_;
        timeRangeEnd_ = fullTimeRangeEnd_;
        timeRangeStart_ -= shift;
    }

    // Final check
    if (timeRangeStart_ < fullTimeRangeStart_) timeRangeStart_ = fullTimeRangeStart_;
    if (timeRangeEnd_ > fullTimeRangeEnd_) timeRangeEnd_ = fullTimeRangeEnd_;

    update();
}
