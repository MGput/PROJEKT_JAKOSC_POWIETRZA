#include "settingsWindow.h"
#include "globalSettings.h"

void settingsWindow::initChoiceButtons() {
	buttonBox_->setOrientation(Qt::Vertical);
	buttonBox_->setStandardButtons(QDialogButtonBox::Ok);
}

void settingsWindow::initCheckboxes() {
	autoDownloadCheckBox_->setText(tr("Automatyczne pobieranie"));
	apiConnectCheckBox_->setText(tr("Połącz z usługą API"));

	autoDownloadCheckBox_->setChecked(GlobalSettings::isAutomaticDownloadEnabled());
	apiConnectCheckBox_->setChecked(GlobalSettings::areYouSureTheInternetIsEnabled());
	if (!GlobalSettings::isInternetEnabled()) {
		apiConnectCheckBox_->setChecked(false);
		apiConnectCheckBox_->setDisabled(true);
	}

	checkboxLayout->addWidget(autoDownloadCheckBox_);
	checkboxLayout->addWidget(apiConnectCheckBox_);

	connect(autoDownloadCheckBox_, &QCheckBox::toggled, this, &settingsWindow::onAutoDownloadChanged);
	connect(apiConnectCheckBox_, &QCheckBox::toggled, this, &settingsWindow::onApiConnectChanged);
}

void settingsWindow::enableInternetCheckbox() {
	apiConnectCheckBox_->setEnabled(true);
}

bool settingsWindow::dataExists() {
	DWORD attributes = GetFileAttributesA("projectData");
	return (attributes != INVALID_FILE_ATTRIBUTES and (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

void settingsWindow::initPushButtons() {
	QLabel* downloadLabel = new QLabel(tr("Pobierz wszystkie dane"), this);
	QLabel* deleteLabel = new QLabel(tr("Usuń wszystkie dane"), this);

	downloadButton_->setIcon(QIcon(":/images/download.png"));
	downloadLayout->addWidget(downloadButton_);
	downloadLayout->addWidget(downloadLabel);
	downloadLayout->addStretch();

	deleteButton_->setIcon(QIcon(":/images/delete.png"));
	if (!dataExists()) deleteButton_->setDisabled(true);
	deleteLayout->addWidget(deleteButton_);
	deleteLayout->addWidget(deleteLabel);
	deleteLayout->addStretch();
}

void settingsWindow::onAutoDownloadChanged(bool checked) {
	if (checked) {
		GlobalSettings::enableAutomaticDownload();
	}
	else {
		GlobalSettings::disableAutomaticDownload();
	}
}

void settingsWindow::onApiConnectChanged(bool checked) {
	if (checked) {
		//GlobalSettings::enableInternet();
		GlobalSettings::enableFakeInternet();
	}
	else {
		//GlobalSettings::disableInternet();
		GlobalSettings::disableFakeInternet();
	}
}

settingsWindow::settingsWindow(QWidget* parent) : QDialog(parent) {
	setWindowTitle("Ustawienia");
	resize(400, 119);

	initChoiceButtons();
	initCheckboxes();
	initPushButtons();

	rightLayout->addWidget(buttonBox_);
	rightLayout->addStretch();
	leftLayout->addLayout(downloadLayout);
	leftLayout->addLayout(deleteLayout);
	leftLayout->addStretch();
	leftLayout->addLayout(checkboxLayout);
	horizontalLayout->addLayout(leftLayout);
	horizontalLayout->addLayout(rightLayout);
	mainLayout->addLayout(horizontalLayout);
	//if (!dataExists()) deleteButton_->setDisabled(true);
	connect(buttonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
}