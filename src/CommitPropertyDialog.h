#ifndef COMMITPROPERTYDIALOG_H
#define COMMITPROPERTYDIALOG_H

#include <QDialog>

#include "Git.h"

class MainWindow;
class RepositoryWrapperFrame;
class QLabel;
class SimpleImageWidget;

namespace Ui {
class CommitPropertyDialog;
}

class CommitPropertyDialog : public QDialog {
	Q_OBJECT
private:
	Ui::CommitPropertyDialog *ui;
	struct Private;
	Private *m;

	void init(MainWindow *mw);
	MainWindow *mainwindow();
	void setAvatar(const QImage &image, SimpleImageWidget *widget);
	void updateAvatar(bool request);
public:
	explicit CommitPropertyDialog(QWidget *parent, MainWindow *mw, Git::CommitItem const &commit);
	// explicit CommitPropertyDialog(QWidget *parent, MainWindow *mw, QString const &commit_id);
	~CommitPropertyDialog() override;

	void showCheckoutButton(bool f);
	void showJumpButton(bool f);
private slots:
	void avatarReady();
	void on_pushButton_checkout_clicked();
	void on_pushButton_details_clicked();
	void on_pushButton_explorer_clicked();
	void on_pushButton_jump_clicked();
};

#endif // COMMITPROPERTYDIALOG_H
