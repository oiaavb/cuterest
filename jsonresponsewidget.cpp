#include "jsonresponsewidget.h"
#include <QClipboard>
#include <QRegularExpression>
#include <QApplication>
JsonResponseWidget::JsonResponseWidget(QWidget * parent)
    :AbstractResponseWidget(parent)
{
    setWindowTitle("Tree");

    mView = new QTreeView;
    mModel = new QJsonModel;
    mSearchEdit = new FindBarWidget;
    mProxyModel = new TreeSortFilterProxyModel;

    mProxyModel->setSourceModel(mModel);

    QVBoxLayout * mainLayout = new QVBoxLayout;

    mainLayout->addWidget(mView);
    mainLayout->addWidget(mSearchEdit);

    mainLayout->setContentsMargins(0,0,0,0);

    //For stylesheet
    mView->setObjectName("Json");
    mView->header()->hide();
    mView->setModel(mProxyModel);
    mView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);


    mSearchEdit->setVisible(false);

    mModel->load("/home/sacha/test.json");

    setLayout(mainLayout);

    createActions();

    //double click.. for link ... !
    connect(mView,SIGNAL(doubleClicked(QModelIndex)), this,SLOT(doubleClicked(QModelIndex)));
    connect(mSearchEdit,SIGNAL(textChanged(QString)),mProxyModel,SLOT(setFilterFixedString(QString)));

}

JsonResponseWidget::~JsonResponseWidget()
{

}
void JsonResponseWidget::createActions()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* copyAction = new QAction(tr("Copy"),this);
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));
    addAction(copyAction);

}

void JsonResponseWidget::doubleClicked(const QModelIndex &index)
{
    // This methods allow to send new request from endpoint clicked from view..
    // That's mean all url "http://" and relative url..

    QString path = mModel->index(mProxyModel->mapToSource(index).row(),1, mProxyModel->mapToSource(index).parent()).data().toString();
    QRegularExpression regExp("^(https?://)?[\da-z\.-]+/");
    if (path.contains(regExp)){

        Request request = response().request();
        qDebug()<<response().request().verb();

        if (path.contains(QRegularExpression("^https?://")))
            request.setUrl(QUrl(path));
        else
        {
            QUrl url;
            url.setAuthority(request.url().authority());
            url.setPath(path);
            request.setUrl(url);
        }

        emit requestTrigger(request);
    }
}


void JsonResponseWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Find)){
        mSearchEdit->setVisible(!mSearchEdit->isVisible());

        if (mSearchEdit->isVisible()){
            mSearchEdit->setFocus();
            mView->expandAll();

        }
        else{
            setFocus();
            mProxyModel->setFilterFixedString(QString());
            mView->collapseAll();
        }
}




}

void JsonResponseWidget::setResponse(const Response &rep)
{

    mModel->loadJson(rep.body());


}

void JsonResponseWidget::copy()
{
    if (mView->selectionModel()->selectedRows().count() > 0) {
        int row = mProxyModel->mapToSource(mView->currentIndex()).row();
        QString value = mModel->index(row,1,mView->currentIndex().parent()).data().toString();
        qApp->clipboard()->setText(value);


    }


}



