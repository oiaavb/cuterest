/*
* Copyright (c) 2015 CuteRest
* CuteRest, a json REST viewer [https://github.com/dridk/cuterest]
* This file is part of CuteRest Project.
* CuteRest is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "requestserializer.h"
#include <QFileDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mManager = new Manager(this);

    mResponseWidget    = new ResponseTabWidget;
    mFavoriteDock      = new FavoriteDockWidget;
    mHistoryDock       = new HistoryDockWidget;
    mConsoleDockWidget = new ConsoleDockWidget;
    mControlBar        = new ControlBar;
    mStatusBar         = new StatusBar;
    mBottomView        = new QTabWidget;


    addToolBar(Qt::TopToolBarArea,mControlBar);
    addDockWidget(Qt::BottomDockWidgetArea, mHistoryDock);
    addDockWidget(Qt::RightDockWidgetArea,mFavoriteDock);
    addDockWidget(Qt::BottomDockWidgetArea,mConsoleDockWidget);

    setStatusBar(mStatusBar);
    setCentralWidget(mResponseWidget);
    tabifyDockWidget(mHistoryDock,mConsoleDockWidget);

    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::South);

    mHistoryDock->hide();
    mFavoriteDock->hide();
    mConsoleDockWidget->hide();

    //Init completer
    mControlBar->setUrlModel(mResponseWidget->urlModel());

    //Create actions
    QAction * refreshAction = new QAction(tr("Refresh"),this);
    refreshAction->setShortcut(QKeySequence("F5"));

    QAction * nextTabAction = new QAction(tr("Next result"),this);
    nextTabAction->setShortcut(QKeySequence("Ctrl+Tab"));

    QAction * previousTabAction = new QAction(tr("Previous result"),this);
    previousTabAction->setShortcut(QKeySequence("Ctrl+Shift+Tab"));

    QAction * previousAction = new QAction(tr("Previous"),this);
    previousAction->setShortcut(QKeySequence(Qt::Key_Backspace));

    QAction * editAction = new QAction(tr("edit query"),this);
    editAction->setShortcut(QKeySequence(Qt::Key_Insert));

    addAction(refreshAction);
    addAction(nextTabAction);
    addAction(previousTabAction);
    addAction(previousAction);
    addAction(editAction);


    // Create Connection
    connect(refreshAction,SIGNAL(triggered()), mControlBar,SLOT(sendRequest()));
    connect(nextTabAction,SIGNAL(triggered()),mResponseWidget,SLOT(nextTab()));
    connect(previousTabAction,SIGNAL(triggered()),mResponseWidget,SLOT(previousTab()));
    connect(previousAction,SIGNAL(triggered()),mHistoryDock,SLOT(setBack()));
    connect(editAction,SIGNAL(triggered()),mControlBar,SLOT(edit()));


    connect(mControlBar,SIGNAL(requestTrigger(Request)),mManager,SLOT(sendRequest(Request)));
    connect(mControlBar,SIGNAL(favoriteTrigger(Request)),mFavoriteDock,SLOT(append(Request)));
    connect(mControlBar,SIGNAL(backTrigger()),mHistoryDock,SLOT(setBack()));
    connect(mControlBar,SIGNAL(forwardTrigger()),mHistoryDock,SLOT(setForward()));

    connect(mManager,SIGNAL(received(Response)),mResponseWidget, SLOT(setResponse(Response)));
    connect(mManager,SIGNAL(received(Response)),mHistoryDock,SLOT(append(Response)));
    connect(mManager,SIGNAL(sended(Request)),mConsoleDockWidget,SLOT(append(Request)));
    connect(mManager,SIGNAL(received(Response)),mConsoleDockWidget,SLOT(append(Response)));
    connect(mManager,SIGNAL(received(Response)),mStatusBar,SLOT(setResponse(Response)));
    connect(mFavoriteDock,SIGNAL(doubleClicked(Request)),mControlBar,SLOT(sendRequest(Request)));
    connect(mFavoriteDock,SIGNAL(clicked(Request)),mControlBar,SLOT(setRequest(Request)));

    connect(mHistoryDock,SIGNAL(doubleClicked(Request)),mControlBar,SLOT(sendRequest(Request)));
    connect(mResponseWidget,SIGNAL(requestTrigger(Request)),mControlBar,SLOT(setRequest(Request)));

    connect(mControlBar,SIGNAL(exportTrigger()),this,SLOT(exportFavorite()));
    connect(mControlBar,SIGNAL(importTrigger()),this,SLOT(importFavorite()));
    connect(mControlBar,SIGNAL(proxyTrigger()),this,SLOT(showSettings()));
    connect(mControlBar,SIGNAL(aboutTrigger()),this,SLOT(showAbout()));
    connect(mControlBar,SIGNAL(abortTrigger()),mManager,SLOT(abortRequest()));
    connect(mControlBar,SIGNAL(favoriteClicked(bool)),mFavoriteDock,SLOT(setVisible(bool)));
    connect(mControlBar,SIGNAL(historyClicked(bool)),mHistoryDock,SLOT(setVisible(bool)));
    connect(mControlBar,SIGNAL(consoleClicked(bool)),mConsoleDockWidget,SLOT(setVisible(bool)));



    connect(mManager,SIGNAL(loadingChanged(bool)),mStatusBar,SLOT(setLoading(bool)));
    connect(mManager,SIGNAL(loadingChanged(bool)),centralWidget(),SLOT(setDisabled(bool)));
    connect(mManager,SIGNAL(loadingChanged(bool)),mControlBar,SLOT(setLoading(bool)));
    connect(mManager,SIGNAL(error(QString)),this,SLOT(showError(QString)));




    resize(1024,640);
//    QAction * styleAction = new QAction(this);
//    styleAction->setShortcut(Qt::Key_F12);

//    addAction(styleAction);

//    connect(styleAction,SIGNAL(triggered()),this,SLOT(setStyle()));

    setWindowTitle(qApp->applicationName());
    setTabPosition(Qt::BottomDockWidgetArea,QTabWidget::North);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mManager;
    delete mResponseWidget;
    delete mFavoriteDock;
    delete mHistoryDock;
    delete mConsoleDockWidget ;
    delete mControlBar;
    delete mStatusBar;

}

void MainWindow::exportFavorite()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("Export favorite"),"",tr("Json file (*.json)"));
    mFavoriteDock->exportFavorite(filename);
}

void MainWindow::importFavorite()
{
    QString filename = QFileDialog::getOpenFileName(this,"Import favorite","",tr("Json file (*.json)"));
    mFavoriteDock->exportFavorite(filename);
}

void MainWindow::setStyle()
{
    if (!QFile::exists(mStyleFileName))
        mStyleFileName = QFileDialog::getOpenFileName(this,"qss file","","stylesheet (*.qss)");

    QFile styleFile(mStyleFileName );
    styleFile.open( QFile::ReadOnly );

    QString style( styleFile.readAll() );
    qApp->setStyleSheet(style);

}

void MainWindow::showSettings()
{
    SettingsDialog dialog(mManager,this);
    dialog.exec();
}

void MainWindow::showAbout()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::showError(const QString &error)
{

    QMessageBox box;
    box.setIcon(QMessageBox::Warning);
    box.setText(error);
    box.setWindowTitle(tr("Error"));
    box.exec();




}


