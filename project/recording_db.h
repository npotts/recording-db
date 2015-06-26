#ifndef recording_db_H
#define recording_db_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "ui_recording_db.h"

#ifndef __GITREV__
#define __GITREV__ "no-version-yet"
#endif

namespace Ui {
  class recording_db;
}

class recording_db : public QMainWindow {
    Q_OBJECT

public:
    explicit recording_db(QWidget *parent = 0);
    ~recording_db();

private:
    void adaptFontSize(QPainter &painter, QRectF, QString);
    bool init_db(QString dbfilename); /*Init's database.  returns true if it worked*/
    void paintImage(QPainter &);
    void select_db(bool);
    void update_db();
    bool newPrn;
    int max; //#pixles the cd is with 300dpi.  This should really be defined from the dpi, and CD width
    int center_hole; //dim of the inner circle
    QSqlDatabase sql_db;
    QSqlTableModel *sql_static;
    Ui::recording_db *ui;
    
private slots:
    void add();
    void browsePrn();
    void createImage(); //so we can paint circles, lines, etc
    void print();
    void print_selection_changed(QModelIndex);
    void reset();
    void save();
    void savePrn();
    void select_db();
};

#endif // recording_db_H
 
