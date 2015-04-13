#include "ui_recording_db.h"
#include "recording_db.h"

recording_db::recording_db(QWidget *parent) : QMainWindow(parent), newPrn(false), ui(new Ui::recording_db) {
    ui->setupUi(this);
    max = 1417;  //#pixles the cd is with 300dpi.  This should really be defined from the dpi, and CD width
    center_hole = 225; //dim of the inner circle
    QCoreApplication::setOrganizationName("Shepherd Community Church");
    QCoreApplication::setApplicationName("Recording :Database");
    setWindowTitle("Recording Database - " __GITREV__);
    sql_static = NULL;
    select_db(false);
    
    sql_static =  new QSqlTableModel(this, sql_db);
    sql_static->setTable("readonly");
    sql_static->select();
    QStringList labels = QStringList() << "Index" << "Recording ID" << "Speaker" << "Date" << "Passage" << "Comments";
    for(int i=0; i<labels.size(); i++)
      sql_static->setHeaderData(i, Qt::Horizontal, labels[i]);
    ui->readonly->setModel(sql_static);
    ui->readonly->show();
    ui->readonly->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    ui->readonly->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    while (sql_static->canFetchMore())   sql_static->fetchMore();
    for(int i=0; i<6; i++)
      ui->readonly->resizeColumnToContents(i);
    
    //connect add / reset / add & print actions
    connect(ui->actionChange,   SIGNAL(triggered()),                 this, SLOT(select_db()));
    connect(ui->actionSave,     SIGNAL(triggered()),                 this, SLOT(save()));
    connect(ui->actionPrint,    SIGNAL(triggered()),                 this, SLOT(print()));

    //connect text changes in add box to change image
    connect(ui->speaker,        SIGNAL(currentIndexChanged(QString)),this, SLOT(createImage()));
    connect(ui->series,         SIGNAL(currentIndexChanged(QString)),this, SLOT(createImage()));
    connect(ui->speaker,        SIGNAL(editTextChanged(QString)),    this, SLOT(createImage()));
    connect(ui->series,         SIGNAL(editTextChanged(QString)),    this, SLOT(createImage()));
    connect(ui->date,           SIGNAL(dateChanged(const QDate &)),  this, SLOT(createImage()));
    connect(ui->recording_item, SIGNAL(valueChanged(int)),           this, SLOT(createImage()));
    connect(ui->passage,        SIGNAL(textChanged(QString)),        this, SLOT(createImage()));
    connect(ui->message,        SIGNAL(textChanged(QString)),        this, SLOT(createImage()));

    //connect add buttons
    connect(ui->reset,          SIGNAL(clicked()),                   this, SLOT(reset()));
    connect(ui->add,            SIGNAL(clicked()),                   this, SLOT(add()));
    connect(ui->save,           SIGNAL(clicked()),                   this, SLOT(save()));
    connect(ui->print,          SIGNAL(clicked()),                   this, SLOT(print()));
    connect(ui->browsePrn,      SIGNAL(clicked()),                   this, SLOT(browsePrn()));
    connect(ui->savePrn,        SIGNAL(clicked()),                   this, SLOT(savePrn()));

    //connect treeview change selection
    connect(ui->readonly,       SIGNAL(activated(QModelIndex)),      this, SLOT(print_selection_changed(QModelIndex)));
    connect(ui->readonly,       SIGNAL(clicked(QModelIndex)),        this, SLOT(print_selection_changed(QModelIndex)));
    connect(ui->readonly,       SIGNAL(doubleClicked(QModelIndex)),  this, SLOT(print_selection_changed(QModelIndex)));
    connect(ui->readonly,       SIGNAL(entered(QModelIndex)),        this, SLOT(print_selection_changed(QModelIndex)));
    connect(ui->readonly,       SIGNAL(pressed(QModelIndex)),        this, SLOT(print_selection_changed(QModelIndex)));

    reset();
    createImage();
}
recording_db::~recording_db() {
    delete ui;
}
void recording_db::save() {
  /* Save the pixmap */
  QString fguess = ui->date->dateTime().toString("yyyy-MM-dd-%1.png").arg(ui->message->text().replace("\n","").replace(" ","-"));
  QString f = QFileDialog::getSaveFileName(this,"Where to save a CD image",fguess,"png image (*png);;jpg Images (*jpg)", 0, 0);
  if (f == "")
    return;
  if (f.endsWith("jpg") | f.endsWith("JPG") | f.endsWith("jpeg") | f.endsWith("JPEG"))
    ui->canvas->pixmap()->save(f, "JPG");
  else {
    if (!f.toLower().endsWith(".png")) //missing .png ending.  Add it anyway
      f+=".png";
    ui->canvas->pixmap()->save(f, "PNG");
  }
}
void recording_db::browsePrn() {
  /*Browse for a new PRN file*/
  QString f = QFileDialog::getOpenFileName(this, "Select a .prn file to impor", "", ".prn Files (*.prn);;All Files (*)");
  if (f != "") {
    newPrn = true;
    ui->prn->setText(f);
  }
}
void recording_db::savePrn() {
  /*Ask the user to save the .prn file somewhere and save it*/
  QString f = QFileDialog::getSaveFileName(this, "Save .prn file", "", ".prn Files (*.prn)", 0, QFileDialog::DontConfirmOverwrite);
  if (f == "")
    return;
  QSqlQuery query(sql_db);
  query.exec("SELECT prn FROM sermons WHERE recordingID=\"" + ui->recording->text() + "\"" );
  //qDebug() << query.lastError();
  while(query.next()) {
    QFile fo(f);
    if (!fo.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(this, "Could not save file", "Could not save selected .prn file to disk");
      return;
    }
    fo.write(query.value(0).toByteArray());
    fo.close();
  }
}
void recording_db::print_selection_changed(QModelIndex m) {
  /* We clicked on something.  Update the display*/
  int rowid = m.sibling(m.row(), 0).data().toInt();
  newPrn = false;  //if they change selection, load the new prn
  QSqlQuery query(sql_db);
  query.exec("SELECT recordingID, date, speaker, series, passage, message, comments, prnfilename FROM sermons WHERE rowid=" + QString::number(rowid));
  //qDebug() << query.lastError();
  while(query.next()) {
    ui->recording_item->setValue((query.value(0).toString().split(".").last().toInt()));
    ui->recording->setText(query.value(0).toString());
    ui->date->setDate(QDate::fromString(query.value(1).toString(), "MMM d yyyy"));
    //find the index of the combobox that has this value
    ui->speaker->setCurrentIndex(ui->speaker->findText(query.value(2).toString()));
    ui->series->setCurrentIndex(ui->series->findText(query.value(3).toString()));
    ui->passage->setText(query.value(4).toString());
    ui->message->setText(query.value(5).toString());
    ui->comments->setText(query.value(6).toString());
    ui->prn->setText(query.value(7).toString());
    ui->savePrn->setEnabled(false);
    if (ui->prn->text() != "")
      ui->savePrn->setEnabled(true);
    break;
  }
}
void recording_db::add() {
  /** Add parameters into the database*/
  QSqlQuery query(sql_db);
  //check if something with this ID already exists
  query.exec("SELECT rowid FROM sermons WHERE recordingID=\"" +  ui->recording->text() + "\"");
  QString old = "";
  while(query.next())
    old = query.value(0).toString();
  if (old == "")
    query.prepare("INSERT INTO sermons (rowid, recordingID, date, speaker, series, passage, message, comments) VALUES (NULL, :recordingID, :date, :speaker, :series, :passage, :message, :comments)");
  else
    query.prepare("UPDATE sermons SET recordingID=:recordingID, date=:date, speaker=:speaker, series=:series, passage=:passage, message=:message, comments=:comments WHERE rowid=" + old);
  query.bindValue(":recordingID", ui->recording->text());
  query.bindValue(":date", ui->date->text());
  query.bindValue(":speaker", ui->speaker->currentText());
  query.bindValue(":series", ui->series->currentText());
  query.bindValue(":passage", ui->passage->text());
  query.bindValue(":message", ui->message->text());
  query.bindValue(":comments", ui->comments->text());
  if (!query.exec())
    qDebug() << "Errors\n" << query.lastError() << "\n" << query.boundValues();
  if (newPrn) {
      query.prepare("UPDATE sermons SET prnfilename=:prnfilename, prn=:prn WHERE rowid=" + old);
      query.bindValue(":prnfilename", QFileInfo(ui->prn->text()).fileName());
      QFile f(ui->prn->text());
      if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Could not open selected .prn file", "Could not open selected .prn file for writing.  It will not be stored");
      } else {
        QVariant d = f.readAll();
        query.bindValue(":prn", d, QSql::In | QSql::Binary);
        if (!query.exec())
          QMessageBox::critical(this, "Could not open add selected .prn file", "Could not add selected .prn file for writing.  It will not be stored");
      }
  }
  update_db();
}
void recording_db::update_db() {
  QSqlQuery query(sql_db);
  QStringList speakers, series;
  query.exec("SELECT DISTINCT speaker FROM sermons");
  while(query.next())
    speakers << query.value(0).toString();
  query.exec("SELECT DISTINCT series FROM sermons");
  while(query.next())
    series << query.value(0).toString();
  series.sort(); speakers.sort();
  ui->speaker->clear();  ui->series->clear();
  ui->speaker->addItems(speakers); ui->speaker->setCurrentIndex(ui->speaker->count()-1);
  ui->series->addItems(series); ui->series->setCurrentIndex(ui->series->count()-1);
  sql_static->select();
  while (sql_static->canFetchMore())
    sql_static->fetchMore();
}
void recording_db::select_db() {
  select_db(true);
}
void recording_db::reset() {
  /*Reset's the text values of the text fields */
  ui->speaker->clear();
  ui->series->clear();
  ui->date->setDateTime(QDateTime::currentDateTime());
  ui->recording_item->setValue(1);
  ui->passage->setText("");
  ui->message->setText("");
  ui->prn->setText("");
  newPrn = false;
  ui->savePrn->setEnabled(false);
  update_db();
}
void recording_db::select_db(bool force = false) {
  //Selects a database
  QSettings cfg;
  if (force)
    cfg.setValue("db/db_path", "");
  while(1) {
    QString f = (cfg.value("db/db_path","").toString() == "") ? QFileDialog::getSaveFileName(this, "Select a database file to use", "", "Database files (*.db *dat);;All Files (*)", 0, 0): cfg.value("db/db_path").toString();
    if (f == "") { //only get this if the file dialog is opened
      QMessageBox::critical ( this, "No database selected", "No database was selected.  Exiting");
      exit(1);
    }
    if (!init_db(f)) {
      QMessageBox::critical ( this, "Error opening database", "Selected database file cannot be opened");
      cfg.setValue("db/db_path", "");
      continue;
    }
    ui->actionCurrent->setText("Current database: " + f);
    cfg.setValue("db/db_path", f);
    break;
  } 
}
bool recording_db::init_db(QString dbfilename) {
  /**Create / open the database.  We may get an old db, so we can reuse it*/
  bool rtn = true;
  if (sql_db.isOpen()) {
    //probably should close the old db if it is open here
    sql_db.close();
    QSqlDatabase::removeDatabase("sermonDB");
  }
  //open db
  sql_db = QSqlDatabase::addDatabase("QSQLITE", "sermonDB");
  sql_db.setDatabaseName(dbfilename);
  rtn &= sql_db.open();
  if(!rtn) {
    qDebug() << "init_db::Can not open database";
    return rtn;
  }
  QSqlQuery query(sql_db);
  /* Perform some pragma magic to make SQLite faster */
  rtn &= (query.exec("PRAGMA synchronous = OFF") && query.exec("PRAGMA journal_mode = OFF"));
  //create log table if is doesnt exist
  rtn &= query.exec("CREATE TABLE IF NOT EXISTS sermons (rowid INTEGER PRIMARY KEY ASC, recordingID TEXT,       date TEXT, speaker TEXT, series TEXT, passage TEXT, message TEXT, comments TEXT, prnfilename TEXT, prn BLOB)");
  rtn &= query.exec("CREATE TABLE IF NOT EXISTS printed (rowid INTEGER PRIMARY KEY ASC, recordingID TEXT, print_date TEXT, fileout TEXT, printer_info TEXT)");
  rtn &= query.exec("CREATE VIEW IF NOT EXISTS readonly AS SELECT rowid, recordingID, speaker, message, date, passage, comments FROM sermons");
  return rtn;
}
void recording_db::adaptFontSize(QPainter &painter, QRectF rect, QString text) {
    QRect fontBoundRect = painter.fontMetrics().boundingRect(rect.toRect(), Qt::AlignHCenter| Qt::AlignCenter|Qt::TextDontClip|Qt::TextWordWrap, text);
    float xFactor = rect.width() / fontBoundRect.width();
    float yFactor = rect.height() / fontBoundRect.height();
    float factor = xFactor < yFactor ? xFactor : yFactor;
    QFont f = painter.font();
    f.setPointSizeF(f.pointSizeF()*factor);
    painter.setFont(f);
    //painter.setBrush(QBrush(Qt::red)); painter.setBackground(QBrush(Qt::red)); painter.drawRect(rect);
    painter.drawText(rect, Qt::AlignHCenter| Qt::AlignCenter|Qt::TextDontClip|Qt::TextWordWrap, text);
}
void recording_db::createImage() {
  /*This does 2 things:  It is called every time anything is changed in the
    "Add Recording" text boxes.  It:
   - redraws the cd label with the new text
   - verifies all fields are filled in and enables/disabled the add button
  */
  //setup rectangles to be used later
  ui->recording->setText(ui->date->dateTime().toString("yyyy.MM.dd.") + QString::number(ui->recording_item->value()));
  if ( (ui->message->text() == "") | (ui->recording->text() == "") | (ui->date->text() == "") | (ui->speaker->currentText() == "") | (ui->passage->text() == "") | (ui->series->currentText() == "")) {
    ui->add->setEnabled(false);
  } else {
    ui->add->setEnabled(true);
  }
  ui->savePrn->setEnabled(false);  if (ui->prn->text() != "") ui->savePrn->setEnabled(true);
  QPixmap img(max+1, max+1); img.fill(Qt::transparent); //setup canvas
  QPainter painter(&img);
  paintImage(painter);
  ui->canvas->setPixmap(img);
}
void recording_db::print() {
  /*Send to a printer  - see http://doc.qt.digia.com/4.7/printing.html*/
  QList<QPrinterInfo> i =  QPrinterInfo::availablePrinters();
  foreach (QPrinterInfo ii, i)
    qDebug() << "\n\n\t" << ii.printerName() << "\n\tDefault: " << ii.isDefault();
  
  QPrinter printer;
  printer.setColorMode( QPrinter::Color );
  printer.setResolution(300); //300dpi
  printer.setFullPage(true);
  printer.setOrientation(QPrinter::Portrait);
  QPrintDialog *dialog = new QPrintDialog(&printer, this);
  dialog->setWindowTitle(tr("Print CD"));
  dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
  if (dialog->exec() != QDialog::Accepted)
    return;
  QPainter painter( &printer );
  paintImage(painter);
  painter.end();
}
void recording_db::paintImage(QPainter &painter) {
  /*Unified place to paint the CD image onto the canvas*/

  painter.setBackgroundMode(Qt::TransparentMode);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.translate(max/2, max/2);
  painter.setFont(QFont("Helvetica"));

  //draw CD outline
  QPainterPath cd_outline, cd_inner;
  cd_outline.addEllipse(-max/2, -max/2,max, max);
  cd_inner.addEllipse(-center_hole, -center_hole, 2*center_hole, 2*center_hole);
  cd_outline = cd_outline.subtracted(cd_inner);
  painter.setBrush(QBrush(Qt::white));
  painter.setPen(QPen(QBrush(Qt::black), 1));
  painter.drawPath(cd_outline);

  //now, draw text onto canvas
  painter.drawImage(     QRect(-490,  100, 917, 454), QImage(":/logo.png")); //draw the logo
  adaptFontSize(painter, QRect(-458, -525, 918, 220), ui->message->text().replace("\\n","\n")); //draw the title
  adaptFontSize(painter, QRect(-650,   30, 400,  30), ui->recording->text()); //draw rec #
  adaptFontSize(painter, QRect(-630,  -50, 350,  50), ui->date->text().replace("\\n","\n")); //draw date
  adaptFontSize(painter, QRect(250, -200,  390,  60), ui->speaker->currentText().replace("\\n","\n")); //draw author
  adaptFontSize(painter, QRect(250,  -120, 390, 120), ui->passage->text().replace("\\n","\n")); //draw passage
  adaptFontSize(painter, QRect(240,    30, 440, 120), ui->series->currentText().replace("\\n","\n")); //draw series
}
