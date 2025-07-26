#include "EnveloppesUi.hpp"//WIP
#include "Globals.hpp"
#include "Assets.hpp"

#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QIntValidator>
#include <QString>
#include <QScrollArea>
#include <QTimer>
#include <QLocale>
#include <QDate>
#include <QPushButton>

EnveloppesUi::EnveloppesUi(QWidget* parent) : QWidget(parent)
{
	resizeDebounceTimer = new QTimer(this);
	resizeDebounceTimer->setSingleShot(true);
	resizeDebounceTimer->setInterval(50);
	connect(resizeDebounceTimer, &QTimer::timeout, this, &EnveloppesUi::showEnveloppes);
	
	showEnveloppes();
}

void EnveloppesUi::showEnveloppes()
{
	clearEnveloppes();

	auto* container = new QWidget;
	auto* outerLayout = new QVBoxLayout(container);
	outerLayout->setContentsMargins(0, 0, 0, 0);
	outerLayout->setSpacing(0);

	gridLayout = new QGridLayout;
	gridLayout->setSpacing(10);
	gridLayout->setContentsMargins(10, 10, 10, 10);
	outerLayout->addLayout(gridLayout);
	outerLayout->addStretch();

	auto* scrollArea = createScrollArea(container);

	auto* wrapperLayout = new QVBoxLayout(this);
	wrapperLayout->setContentsMargins(0, 0, 0, 0);
	wrapperLayout->addWidget(scrollArea, 1);
	setLayout(wrapperLayout);

	const int cardMinWidth = 200;
	const int spacing = gridLayout->spacing();
	const int margin = gridLayout->contentsMargins().left() + gridLayout->contentsMargins().right();

	int totalWidth = this->width() - margin;
	int columnCount = std::max(1, totalWidth / (cardMinWidth + spacing));

	const auto& enveloppes = g_enveloppeManager.getEnveloppes();
	for (std::size_t i = 0; i < enveloppes.size(); ++i)
	{
		std::size_t row = i / static_cast<std::size_t>(columnCount);
		std::size_t col = i % static_cast<std::size_t>(columnCount);
		gridLayout->addWidget(createCard(enveloppes[i]), static_cast<int>(row), static_cast<int>(col));
	}
}


QScrollArea* EnveloppesUi::createScrollArea(QWidget* content)
{
	auto* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidget(content);
	scrollArea->setStyleSheet(R"(
		QScrollBar:vertical {
			background: transparent;
			width: 8px;
			margin: 3px 0;
			border-radius: 4px;
		}
		QScrollBar::handle:vertical {
			background: #1B272A;
			min-height: 20px;
			border-radius: 4px;
		}
		QScrollBar::add-line:vertical,
		QScrollBar::sub-line:vertical {
			height: 0;
		}
		QScrollBar::add-page:vertical,
		QScrollBar::sub-page:vertical {
			background: none;
		}
	)");
	return scrollArea;
}


QWidget* EnveloppesUi::createCard(const Enveloppe& env)
{
	int amount = env.getAmount();
	int max = env.getMaxAmount();
	int percent = (max > 0) ? static_cast<int>((100.0 * amount) / max) : 0;
	QString barColor = env.isSavings() ? "background-color: #337BFF;" : getProgressBarColor(percent);

	QWidget* card = new QWidget;
	card->setMinimumSize(200, 270);
	card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	card->setStyleSheet(R"(
		background-color: #1B272A;
		border-radius: 8px;
		padding: 10px;
	)");

	auto* cardLayout = new QVBoxLayout(card);
	cardLayout->setContentsMargins(0, 0, 0, 0);
	cardLayout->setSpacing(2);

	auto* nameLabel = new QLabel(QString::fromStdString(env.getName()), card);
	nameLabel->setStyleSheet(R"(
		font-family: 'Helvetica Neue';
		font-size: 28px;
		font-weight: 200;
		color: #E1E1E2;
		background: transparent;
		border: none;
	)");

	QLocale jpLocale(QLocale::Japanese, QLocale::Japan);
	QString formattedAmount = jpLocale.toString(env.getAmount()) + " ¥";

	auto* amountLabel = new QLabel(formattedAmount, card);
	amountLabel->setStyleSheet(QString(R"(
		font-family: 'Helvetica Neue';
		color: %1;
		font-size: 20px;
		background: transparent;
		border: none;
	)").arg(barColor.mid(18, 7)));

	cardLayout->addWidget(createCardButtons(env));
	cardLayout->addWidget(nameLabel);
	cardLayout->addWidget(amountLabel);
	cardLayout->addWidget(createProgressLabel(barColor, percent));
	cardLayout->addWidget(createGoalMaxLabel(env));

	return card;
}

QWidget* EnveloppesUi::createCardButtons(const Enveloppe& env)
{
	auto* container = new QWidget;
	auto* layout = new QHBoxLayout(container);
	layout->setContentsMargins(5, 5, 5, 5);
	layout->setSpacing(5);

	auto* deleteBtn = new QPushButton;
	deleteBtn->setIcon(QIcon(CROSS_ICON));
	deleteBtn->setFixedSize(30, 30);

	auto* leftBtn = new QPushButton;
	leftBtn->setIcon(QIcon(LEFT_ICON));
	leftBtn->setFixedSize(30, 30);

	auto* rightBtn = new QPushButton;
	rightBtn->setIcon(QIcon(RIGHT_ICON));
	rightBtn->setFixedSize(30, 30);

	layout->addWidget(deleteBtn);
	layout->addStretch();
	layout->addWidget(leftBtn);
	layout->addWidget(rightBtn);

	QString btnStyle = R"(
		QPushButton {
			background-color: transparent;
			border: none;
		}
		QPushButton:hover {
			background-color: #242F32;
			border-radius: 4px;
		}
	)";
	deleteBtn->setStyleSheet(btnStyle);
	leftBtn->setStyleSheet(btnStyle);
	rightBtn->setStyleSheet(btnStyle);

	return container;
}


QWidget* EnveloppesUi::createGoalMaxLabel(const Enveloppe& env)
{
	auto* label = new QLabel(QString("Plafond / 上限額 = %1\nObjectif mensuel / 月間目標 = %2")
		.arg(env.getMaxAmount())
		.arg(env.getGoal()));
	label->setStyleSheet(R"(
		font-family: 'Helvetica Neue';
		color: #AAAAAA;
		font-size: 12px;
		background: transparent;
		border: none;
	)");
	return label;
}

QWidget* EnveloppesUi::createProgressLabel(QString barColor, int percent)
{
	auto* container = new QWidget;
	auto* layout = new QHBoxLayout(container);
	layout->setContentsMargins(10, 0, 0, 0);
	layout->setSpacing(5);

	auto* barContainer = new QWidget;
	barContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	barContainer->setMinimumHeight(2);
	barContainer->setMaximumHeight(2);
	barContainer->setStyleSheet("background-color: #444;");

	auto* barFill = new QWidget(barContainer);
	barFill->setStyleSheet(barColor);
	barFill->setGeometry(0, 0, 0, 2);
	barFill->show();

	QTimer::singleShot(0, [=]() {
		int fullWidth = barContainer->width();
		int fillWidth = (percent * fullWidth) / 100;
		barFill->setGeometry(0, 0, fillWidth, 2);
	});

	auto* percentLabel = new QLabel(QString::number(percent) + " %");
	percentLabel->setStyleSheet(R"(
		font-family: 'Helvetica Neue';
		color: #E1E1E2;
		font-size: 14px;
		background: transparent;
		border: none;
	)");

	layout->addWidget(barContainer);
	layout->addWidget(percentLabel);
	layout->setAlignment(percentLabel, Qt::AlignRight);

	return container;
}

QString EnveloppesUi::getProgressBarColor(int percent)
{
	QDate today = QDate::currentDate();
	int day = today.day();
	int daysInMonth = today.daysInMonth();
	double monthProgress = (static_cast<double>(day) / daysInMonth) * 100.0;

	double spentPercent = 100.0 - percent;
	double difference = spentPercent - monthProgress;

	qDebug() << "Today:" << day << "/" << daysInMonth
	         << "Month progress:" << monthProgress
	         << "Spent:" << spentPercent
	         << "Diff:" << difference;

	if (difference <= 0)
		return "background-color: #337BFF;";
	else if (difference >= 0 && difference <= 5)
		return "background-color: #FA9D09;";
	else
		return "background-color: #FA5E57;";
}





void EnveloppesUi::clearEnveloppes()
{
	if (layout())
	{
		QLayoutItem* item;
		while ((item = layout()->takeAt(0)))
		{
			if (QWidget* w = item->widget())
				delete w;
			delete item;
		}
		delete layout();
	}
}

void EnveloppesUi::addEnveloppe()
{
	QDialog dialog(this);
	dialog.setWindowTitle("Ajouter une enveloppe");

	QFormLayout* formLayout = new QFormLayout(&dialog);
	EnveloppeFormFields fields;

	createFormInputs(&dialog, formLayout, fields);
	addDialogButtons(&dialog, formLayout);

	if(dialog.exec() == QDialog::Accepted)
		handleEnveloppeSubmission(fields);
}

void EnveloppesUi::createFormInputs(QDialog* dialog, QFormLayout* layout, EnveloppeFormFields& f)
{
	createFields(dialog, layout, f);
	QLineEdit* edits[] = { f.nameInput, f.amountInput, f.maxAmountInput, f.goalInput };

	for(QLineEdit * edit : edits)
		edit->setStyleSheet("color: black; background-color: white;");

	QLabel* labels[] =
	{
		qobject_cast<QLabel*>(layout->labelForField(f.nameInput)),
		qobject_cast<QLabel*>(layout->labelForField(f.amountInput)),
		qobject_cast<QLabel*>(layout->labelForField(f.maxAmountInput)),
		qobject_cast<QLabel*>(layout->labelForField(f.goalInput))
	};

	for(QLabel * label : labels)
		if(label)
			label->setStyleSheet("color: black;");

	f.savingsCheck->setStyleSheet("color: black;");
}

void EnveloppesUi::createFields(QDialog* dialog, QFormLayout* layout, EnveloppeFormFields& f)
{
	f.nameFrInput = new QLineEdit(dialog);
	f.nameJpInput = new QLineEdit(dialog);
	f.nameInput = new QLineEdit(dialog);
	f.nameInput->hide();

	f.amountInput = new QLineEdit(dialog);
	f.maxAmountInput = new QLineEdit(dialog);
	f.goalInput = new QLineEdit(dialog);
	f.savingsCheck = new QCheckBox("Épargne", dialog);

	QIntValidator* validator = new QIntValidator(0, 100000000, dialog);
	f.amountInput->setValidator(validator);
	f.maxAmountInput->setValidator(validator);
	f.goalInput->setValidator(validator);

	layout->addRow("Nom (FR)", f.nameFrInput);
	layout->addRow("Nom (JP)", f.nameJpInput);
	layout->addRow("Montant initial", f.amountInput);
	layout->addRow("Montant max", f.maxAmountInput);
	layout->addRow("Objectif", f.goalInput);
	layout->addRow(f.savingsCheck);

	auto updateCombinedName = [&f]() {
		f.nameInput->setText(f.nameFrInput->text() + "\n" + f.nameJpInput->text());
	};
	connect(f.nameFrInput, &QLineEdit::textChanged, updateCombinedName);
	connect(f.nameJpInput, &QLineEdit::textChanged, updateCombinedName);
}

void EnveloppesUi::addDialogButtons(QDialog* dialog, QFormLayout* layout)
{
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
	buttons->setStyleSheet("color: black;");
	layout->addWidget(buttons);

	connect(buttons, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
}

void EnveloppesUi::handleEnveloppeSubmission(const EnveloppeFormFields& f)
{
	QString name = f.nameInput->text();
	int amount = f.amountInput->text().toInt();
	int maxAmount = f.maxAmountInput->text().toInt();
	int goal = f.goalInput->text().toInt();
	bool savings = f.savingsCheck->isChecked();

	g_enveloppeManager.addEnveloppe(name.toStdString(), amount, maxAmount, goal, savings);
}

void EnveloppesUi::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	if (resizeDebounceTimer)
		resizeDebounceTimer->start();
}
