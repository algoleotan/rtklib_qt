#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "posfilereader.h"
#include "graphwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLoadFile1_clicked();
    void on_btnLoadFile2_clicked();
    void on_btnClearFile1_clicked();
    void on_btnClearFile2_clicked();
    void on_chkShowFile1_toggled(bool checked);
    void on_chkShowFile2_toggled(bool checked);
    void on_btnRefresh_clicked();

private:
    Ui::MainWindow *ui;

    // Graph widget
    GraphWidget *graphWidget_;

    // File readers
    PosFileReader reader1_;
    PosFileReader reader2_;

    // Data for plotting
    QVector<double> time1_, roll1_, pitch1_, yaw1_, ve1_, vn1_, vu1_;
    QVector<double> time2_, roll2_, pitch2_, yaw2_, ve2_, vn2_, vu2_;

    // File names (full paths)
    QString fileName1_;
    QString fileName2_;

    // Show/hide flags
    bool showFile1_;
    bool showFile2_;

    // Settings
    QSettings settings_;

    // Helper functions
    void loadFile(int fileIndex);
    void updateDisplay();
    void saveSettings();
    void loadSettings();
    void reloadFile(int fileIndex);
};

#endif // MAINWINDOW_H
