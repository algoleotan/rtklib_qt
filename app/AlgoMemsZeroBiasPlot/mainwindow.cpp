#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , showFile1_(true)
    , showFile2_(true)
    , settings_("AlgoMemsZeroBiasPlot", "AlgoMemsZeroBiasPlot")
{
    ui->setupUi(this);

    // Create graph widget and replace the label
    graphWidget_ = new GraphWidget(this);

    // Replace lblDisplay with graphWidget
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (layout) {
        // Find and remove the label
        layout->removeWidget(ui->lblDisplay);
        ui->lblDisplay->hide();

        // Add graph widget
        layout->addWidget(graphWidget_);
    }

    // Load saved settings
    loadSettings();

    // Signals are auto-connected by Qt's naming convention (on_objectName_signalName)
    // No need to manually connect them
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::on_btnLoadFile1_clicked()
{
    loadFile(1);
}

void MainWindow::on_btnLoadFile2_clicked()
{
    loadFile(2);
}

void MainWindow::on_btnClearFile1_clicked()
{
    fileName1_.clear();
    ui->txtFile1->clear();
    time1_.clear();
    bgx1_.clear();
    bgy1_.clear();
    bgz1_.clear();
    bax1_.clear();
    bay1_.clear();
    baz1_.clear();
    updateDisplay();
}

void MainWindow::on_btnClearFile2_clicked()
{
    fileName2_.clear();
    ui->txtFile2->clear();
    time2_.clear();
    bgx2_.clear();
    bgy2_.clear();
    bgz2_.clear();
    bax2_.clear();
    bay2_.clear();
    baz2_.clear();
    updateDisplay();
}

void MainWindow::on_chkShowFile1_toggled(bool checked)
{
    showFile1_ = checked;
    updateDisplay();
}

void MainWindow::on_chkShowFile2_toggled(bool checked)
{
    showFile2_ = checked;
    updateDisplay();
}

void MainWindow::loadFile(int fileIndex)
{
    // Get last directory from settings
    QString lastDir = settings_.value("lastDirectory", "").toString();

    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString("Select POS File %1").arg(fileIndex),
        lastDir,
        "POS Files (*.pos);;All Files (*)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    // Save directory for next time
    QFileInfo fileInfo(fileName);
    settings_.setValue("lastDirectory", fileInfo.absolutePath());

    PosFileReader *reader = (fileIndex == 1) ? &reader1_ : &reader2_;

    if (!reader->readFile(fileName)) {
        QMessageBox::critical(this, "Error", reader->getErrorMessage());
        return;
    }

    // Extract time series (MEMS zero bias data)
    QVector<double> *time, *bgx, *bgy, *bgz, *bax, *bay, *baz;
    if (fileIndex == 1) {
        time = &time1_; bgx = &bgx1_; bgy = &bgy1_; bgz = &bgz1_;
        bax = &bax1_; bay = &bay1_; baz = &baz1_;
        fileName1_ = fileName;
        ui->txtFile1->setText(fileName);
    } else {
        time = &time2_; bgx = &bgx2_; bgy = &bgy2_; bgz = &bgz2_;
        bax = &bax2_; bay = &bay2_; baz = &baz2_;
        fileName2_ = fileName;
        ui->txtFile2->setText(fileName);
    }

    reader->getTimeSeries(*time, *bgx, *bgy, *bgz, *bax, *bay, *baz);

    ui->statusbar->showMessage(
        QString("Loaded %1: %2 epochs").arg(fileIndex).arg(time->size()),
        3000
    );

    saveSettings();
    updateDisplay();
}

void MainWindow::reloadFile(int fileIndex)
{
    QString fileName = (fileIndex == 1) ? fileName1_ : fileName2_;

    if (fileName.isEmpty()) {
        return;
    }

    PosFileReader *reader = (fileIndex == 1) ? &reader1_ : &reader2_;

    if (!reader->readFile(fileName)) {
        QMessageBox::critical(this, "Error",
            QString("Failed to reload file %1:\n%2").arg(fileIndex).arg(reader->getErrorMessage()));
        return;
    }

    // Extract time series (MEMS zero bias data)
    QVector<double> *time, *bgx, *bgy, *bgz, *bax, *bay, *baz;
    if (fileIndex == 1) {
        time = &time1_; bgx = &bgx1_; bgy = &bgy1_; bgz = &bgz1_;
        bax = &bax1_; bay = &bay1_; baz = &baz1_;
    } else {
        time = &time2_; bgx = &bgx2_; bgy = &bgy2_; bgz = &bgz2_;
        bax = &bax2_; bay = &bay2_; baz = &baz2_;
    }

    reader->getTimeSeries(*time, *bgx, *bgy, *bgz, *bax, *bay, *baz);

    ui->statusbar->showMessage(
        QString("Reloaded %1: %2 epochs").arg(fileIndex).arg(time->size()),
        3000
    );
}

void MainWindow::on_btnRefresh_clicked()
{
    // Reload both files if they exist
    if (!fileName1_.isEmpty()) {
        reloadFile(1);
    }
    if (!fileName2_.isEmpty()) {
        reloadFile(2);
    }

    updateDisplay();

    ui->statusbar->showMessage("Files refreshed", 2000);
}

void MainWindow::updateDisplay()
{
    // Update graph widget with current data (MEMS zero bias data)
    graphWidget_->setData(time1_, bgx1_, bgy1_, bgz1_, bax1_, bay1_, baz1_,
                          time2_, bgx2_, bgy2_, bgz2_, bax2_, bay2_, baz2_,
                          showFile1_, showFile2_);
}

void MainWindow::saveSettings()
{
    settings_.setValue("file1", fileName1_);
    settings_.setValue("file2", fileName2_);
    settings_.setValue("showFile1", showFile1_);
    settings_.setValue("showFile2", showFile2_);
}

void MainWindow::loadSettings()
{
    fileName1_ = settings_.value("file1", "").toString();
    fileName2_ = settings_.value("file2", "").toString();
    showFile1_ = settings_.value("showFile1", true).toBool();
    showFile2_ = settings_.value("showFile2", true).toBool();

    // Update UI
    ui->chkShowFile1->setChecked(showFile1_);
    ui->chkShowFile2->setChecked(showFile2_);

    // Load files if they exist
    if (!fileName1_.isEmpty() && QFile::exists(fileName1_)) {
        ui->txtFile1->setText(fileName1_);
        reloadFile(1);
    }
    if (!fileName2_.isEmpty() && QFile::exists(fileName2_)) {
        ui->txtFile2->setText(fileName2_);
        reloadFile(2);
    }

    updateDisplay();
}
