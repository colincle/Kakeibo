#include "EnveloppesUi.hpp"
#include "Assets.hpp"
#include "Globals.hpp"
#include "KakeiboScrollArea.hpp"

#include <QCheckBox>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QIntValidator>
#include <QLabel>
#include <QLayoutItem>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#define CARD_WIDTH 350
#define CARD_HEIGHT 380
#define CARD_SHADOW_OFFSET 4

EnveloppesUi::EnveloppesUi(QWidget *parent) : QWidget(parent)
{
	scrollContent = new QWidget(this);

	topInfoWidget = new QWidget(this);
	topInfoLayout = new QHBoxLayout(topInfoWidget);
	topInfoLayout->setContentsMargins(20, 10, 20, 10);
	topInfoLayout->setSpacing(10);

	auto *outerLayout = new QVBoxLayout(scrollContent);

	outerLayout->insertWidget(0, topInfoWidget);
	outerLayout->setContentsMargins(0, 0, 0, 0);
	outerLayout->setSpacing(0);

	gridLayout = new QGridLayout;
	gridLayout->setSpacing(10);
	gridLayout->setContentsMargins(10, 0, 10, 10);

	auto *centeredLayout = new QHBoxLayout;
	centeredLayout->addStretch();
	centeredLayout->addLayout(gridLayout);
	centeredLayout->addStretch();
	outerLayout->addLayout(centeredLayout);
	outerLayout->addStretch();

	scrollArea = createScrollArea(scrollContent);

	auto *wrapperLayout = new QVBoxLayout(this);
	wrapperLayout->setContentsMargins(0, 0, 0, 0);
	wrapperLayout->addWidget(scrollArea);
	setLayout(wrapperLayout);

	resizeDebounceTimer = new QTimer(this);
	resizeDebounceTimer->setSingleShot(true);
	resizeDebounceTimer->setInterval(50);
	connect(resizeDebounceTimer, &QTimer::timeout, this, &EnveloppesUi::showEnveloppes);

	showEnveloppes();
}

void EnveloppesUi::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);

	if ( resizeDebounceTimer )
		resizeDebounceTimer->start();
}

void EnveloppesUi::showEnveloppes()
{
	if ( !scrollArea || !gridLayout )
		return;

	scrollArea->saveScroll();
	clearGrid();

	showTopInfo();

	int columnCount = computeColumnCount();

	populateGrid(columnCount);

	scrollArea->restoreScroll();
}

void EnveloppesUi::clearGrid()
{
	QLayoutItem *item;

	while ( (item = gridLayout->takeAt(0)) != nullptr )
	{
		if ( item->widget() )
			item->widget()->deleteLater();

		delete item;
	}

	cloudCardWidgets.clear();
}

void EnveloppesUi::showTopInfo()
{
	topInfoLayout->setAlignment(Qt::AlignTop);
	topInfoLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	topInfoLayout->setSpacing(40);

	for ( QLayoutItem *item; (item = topInfoLayout->takeAt(0)); )
	{
		delete item->widget();
		delete item;
	}

	const auto &income = g_enveloppeManager.getIncomeEnveloppe();
	const auto &credit = g_enveloppeManager.getCreditEnveloppe();

	auto addEntry = [this](const Enveloppe &env)
	{
		QString name = QString::fromStdString(env.getName()).replace("\n", " ");

		QLocale jp(QLocale::Japanese, QLocale::Japan);
		QLocale fr(QLocale::French, QLocale::France);
		float   rate = getEurJpyRateCached();

		QString jpy = "¥ " + jp.toString(env.getAmount());
		QString eur = rate > 0 ? "€ " + fr.toString(env.getAmount() / rate, 'f', 2) : "-€";

		auto *label = new QLabel(name + " — " + jpy + " / " + eur);
		label->setStyleSheet(R"(
			font-family: 'Helvetica Neue';
			font-weight: 100;
			color: #E1E1E2;
			font-size: 18px;
		)");
		topInfoLayout->addWidget(label);
	};

	addEntry(income);
	addEntry(credit);
}

int EnveloppesUi::computeColumnCount() const
{
	const int cardMinWidth = CARD_WIDTH;
	const int spacing      = gridLayout->spacing();
	const int margin       = gridLayout->contentsMargins().left() + gridLayout->contentsMargins().right();
	int       totalWidth   = this->width() - margin;
	return std::max(1, totalWidth / (cardMinWidth + spacing));
}

void EnveloppesUi::populateGrid(int columnCount)
{
	const auto &enveloppes = g_enveloppeManager.getEnveloppes();
	int         row = 0, col = 0;

	for ( std::size_t i = 0; i < enveloppes.size(); ++i )
	{
		QWidget *card = createCard(enveloppes[i]);
		gridLayout->addWidget(card, row, col++);

		if ( enveloppes[i].isCloud() )
			cloudCardWidgets.append(card);

		if ( col == columnCount )
		{
			col = 0;
			++row;
		}
	}

	if ( col > 0 )
	{
		while ( col < columnCount )
		{
			QWidget *dummy = new QWidget;
			dummy->setFixedSize(CARD_WIDTH + CARD_SHADOW_OFFSET, CARD_HEIGHT + CARD_SHADOW_OFFSET);
			dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			dummy->setStyleSheet("background: transparent;");
			gridLayout->addWidget(dummy, row, col++);
		}
	}
}

QWidget *EnveloppesUi::createCard(const Enveloppe &env)
{
	int     amount   = env.getAmount();
	int     max      = env.getMaxAmount();
	int     percent  = (max > 0) ? static_cast<int>((100.0 * amount) / max) : 0;
	QString barColor = getProgressBarColor(percent, env);

	auto *wrapper = new QWidget;
	wrapper->setMinimumSize(CARD_WIDTH + CARD_SHADOW_OFFSET, CARD_HEIGHT + CARD_SHADOW_OFFSET);
	wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	auto *shadow = new QWidget(wrapper);
	shadow->setGeometry(CARD_SHADOW_OFFSET, CARD_SHADOW_OFFSET, CARD_WIDTH, CARD_HEIGHT);
	shadow->setStyleSheet(R"(
		background-color: #141d1f;
		border-radius: 8px;
	)");

	auto *card = new QWidget(wrapper);
	card->setGeometry(0, 0, CARD_WIDTH, CARD_HEIGHT);
	card->setStyleSheet(R"(
		background-color: #1B272A;
		border-radius: 8px;
		padding: 10px;
	)");

	auto *layout = setupCardLayout(card);
	addCardContent(layout, env, barColor, percent);

	return wrapper;
}

QVBoxLayout *EnveloppesUi::setupCardLayout(QWidget *card)
{
	auto *layout = new QVBoxLayout(card);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	return layout;
}

void EnveloppesUi::addCardContent(QVBoxLayout *layout, const Enveloppe &env, const QString &barColor, int percent)
{
	auto *nameLabel = new QLabel(QString::fromStdString(env.getName()));
	nameLabel->setStyleSheet(R"(
		font-family: 'Helvetica Neue';
		font-size: 28px;
		font-weight: 200;
		color: #E1E1E2;
		background: transparent;
		border: none;
	)");

	QLocale jpLocale(QLocale::Japanese, QLocale::Japan);
	QLocale frLocale(QLocale::French, QLocale::France);
	float   rate = getEurJpyRateCached();

	QString formattedAmount = "¥ " + jpLocale.toString(env.getAmount());
	QString eurAmount       = rate > 0 ? "€ " + frLocale.toString(env.getAmount() / rate, 'f', 2) : "-€";

	auto *amountLabel = new QLabel(QString("%1\n%2").arg(formattedAmount, eurAmount));
	amountLabel->setStyleSheet(QString(R"(
		font-family: 'Helvetica Neue';
		color: %1;
		font-size: 20px;
		background: transparent;
		border: none;
	)")
	                               .arg(barColor.mid(18, 7)));

	layout->addWidget(createCardButtons(env));
	layout->addWidget(nameLabel);
	layout->addWidget(amountLabel);
	layout->addWidget(createProgressLabel(barColor, percent));
	layout->addWidget(createGoalMaxLabel(env));
}

QWidget *EnveloppesUi::createCardButtons(const Enveloppe &env)
{
	auto *container = new QWidget;
	auto *layout    = new QHBoxLayout(container);
	layout->setContentsMargins(5, 5, 5, 5);
	layout->setSpacing(5);

	auto buttons = createCardButtonList(env);
	layout->addWidget(buttons[0]);
	layout->addWidget(buttons[1]);
	layout->addStretch();
	layout->addWidget(buttons[2]);
	layout->addWidget(buttons[3]);
	layout->addWidget(buttons[4]);

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

	for ( auto *btn : buttons )
		btn->setStyleSheet(btnStyle);

	connectCardButtons(env, buttons);
	return container;
}

QList<QPushButton *> EnveloppesUi::createCardButtonList(const Enveloppe &env)
{
	auto makeBtn = [](const QString &iconPath)
	{
		auto *btn = new QPushButton;
		btn->setIcon(QIcon(iconPath));
		btn->setFixedSize(30, 30);
		return btn;
	};

	return {
	    makeBtn(CROSS_ICON),
	    makeBtn(MODIFY_ICON),
	    makeBtn(env.isCloud() ? CLOUD_BLUE_ICON : CLOUD_WHITE_ICON),
	    makeBtn(LEFT_ICON),
	    makeBtn(RIGHT_ICON)};
}

void EnveloppesUi::connectCardButtons(const Enveloppe &env, const QList<QPushButton *> &buttons)
{
	connect(buttons[0], &QPushButton::clicked, this, [this, env]()
	        { deleteEnveloppe(env); });
	connect(buttons[1], &QPushButton::clicked, this, [this, env]()
	        {
		addEnveloppe(env.getName());
		emit updateNeeded(); });
	connect(buttons[2], &QPushButton::clicked, this, [this, env]() mutable
	        {
		g_enveloppeManager.switchCloud(env.getName());
		emit updateNeeded(); });
	connect(buttons[3], &QPushButton::clicked, this, [this, env]()
	        {
		g_enveloppeManager.moveEnveloppe(env.getName(), true);
		emit updateNeeded(); });
	connect(buttons[4], &QPushButton::clicked, this, [this, env]()
	        {
		g_enveloppeManager.moveEnveloppe(env.getName(), false);
		emit updateNeeded(); });
}

QWidget *EnveloppesUi::createGoalMaxLabel(const Enveloppe &env)
{
	QLocale jp(QLocale::Japanese, QLocale::Japan);
	QLocale fr(QLocale::French, QLocale::France);
	float   rate = getEurJpyRateCached();

	QString maxYen  = jp.toString(env.getMaxAmount());
	QString goalYen = jp.toString(env.getGoal());

	QString maxEur  = rate > 0 ? fr.toString(env.getMaxAmount() / rate, 'f', 2) + "€" : "-€";
	QString goalEur = rate > 0 ? fr.toString(env.getGoal() / rate, 'f', 2) + "€" : "-€";

	auto *label = new QLabel(QString("Plafond / 上限額  ¥%1 | %2\nObjectif mensuel / 月間目標  ¥%3 | %4")
	                             .arg(maxYen, maxEur, goalYen, goalEur));

	label->setStyleSheet(R"(
		font-family: 'Helvetica Neue';
		color: #AAAAAA;
		font-size: 12px;
		background: transparent;
		border: none;
	)");
	return label;
}

QWidget *EnveloppesUi::createProgressLabel(QString barColor, int percent)
{
	auto *container = new QWidget;
	auto *layout    = new QHBoxLayout(container);
	layout->setContentsMargins(10, 0, 0, 0);
	layout->setSpacing(5);

	auto *barContainer = new QWidget;
	auto *barLayout    = new QHBoxLayout(barContainer);
	barLayout->setContentsMargins(0, 0, 0, 0);
	barLayout->setSpacing(0);

	auto *barFill = new QWidget;
	barFill->setStyleSheet(barColor);

	auto *barEmpty = new QWidget;
	barEmpty->setStyleSheet("background-color: transparent;");

	barLayout->addWidget(barFill, percent);
	barLayout->addWidget(barEmpty, 100 - percent);

	barContainer->setMinimumHeight(2);
	barContainer->setMaximumHeight(2);
	barContainer->setStyleSheet("background-color: #444;");

	auto *percentLabel = new QLabel(QString::number(percent) + " %");
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

QString EnveloppesUi::getProgressBarColor(int percent, const Enveloppe &env)
{
	if ( env.getAmount() <= 0 )
		return "background-color: #FA5E57;";
	else if ( env.isSavings() )
		return "background-color: #337BFF;";

	QDate  today         = QDate::currentDate();
	int    day           = today.day();
	int    daysInMonth   = today.daysInMonth();
	double monthProgress = (static_cast<double>(day) / daysInMonth) * 100.0;

	double spentPercent = 100.0 - percent;
	double difference   = spentPercent - monthProgress;

	if ( difference <= -5 )
		return "background-color: #337BFF;";
	else if ( difference > -5 && difference <= 0 )
		return "background-color: #FA9D09;";
	else
		return "background-color: #FA5E57;";
}

KakeiboScrollArea *EnveloppesUi::createScrollArea(QWidget *content)
{
	scrollArea = new KakeiboScrollArea(this);
	scrollArea->setWidget(content);
	return scrollArea;
}

void EnveloppesUi::addEnveloppe(std::string name)
{
	QDialog dialog(this);
	setupEnveloppeDialog(dialog);

	QFormLayout        *formLayout = new QFormLayout(&dialog);
	EnveloppeFormFields fields;
	createFields(&dialog, formLayout, fields);

	if ( !name.empty() )
		preFillFieldsFromName(name, fields);

	addDialogButtons(&dialog, formLayout);

	if ( dialog.exec() == QDialog::Accepted )
		handleEnveloppeSubmission(fields, name);
}

void EnveloppesUi::setupEnveloppeDialog(QDialog &dialog)
{
	dialog.setWindowTitle("Ajouter une enveloppe / 封筒を追加");
	dialog.setStyleSheet(R"(
		QDialog {
			background-color: #242F32;
			color: #E1E1E2;
			font-family: "Helvetica Neue";
		}
		QLabel {
			color: #E1E1E2;
		}
		QLineEdit {
			background-color: #1B272A;
			color: #E1E1E2;
			border: 1px solid #2F3D41;
			border-radius: 4px;
			padding: 4px;
		}
		QCheckBox {
			color: #E1E1E2;
		}
		QPushButton {
			background-color: #1B272A;
			color: #E1E1E2;
			border: none;
			padding: 6px 12px;
			border-radius: 4px;
			min-width: 80px;
		}
		QPushButton:hover {
			background-color: #2F3D41;
		}
	)");
}

void EnveloppesUi::preFillFieldsFromName(const std::string &name, EnveloppeFormFields &fields)
{
	for ( const auto &env : g_enveloppeManager.getEnveloppes() )
	{
		if ( env.getName() == name )
		{
			QStringList parts = QString::fromStdString(env.getName()).split('\n');

			if ( parts.size() == 2 )
			{
				fields.nameFrInput->setText(parts[0]);
				fields.nameJpInput->setText(parts[1]);
			}
			else
				fields.nameFrInput->setText(QString::fromStdString(env.getName()));

			fields.amountInput->setText(QString::number(env.getAmount()));
			fields.maxAmountInput->setText(QString::number(env.getMaxAmount()));
			fields.goalInput->setText(QString::number(env.getGoal()));
			fields.savingsCheck->setChecked(env.isSavings());
			break;
		}
	}
}

void EnveloppesUi::createFields(QDialog *dialog, QFormLayout *layout, EnveloppeFormFields &f)
{
	f.nameFrInput = new QLineEdit(dialog);
	f.nameJpInput = new QLineEdit(dialog);
	f.nameInput   = new QLineEdit(dialog);
	f.nameInput->hide();

	f.amountInput    = new QLineEdit(dialog);
	f.maxAmountInput = new QLineEdit(dialog);
	f.goalInput      = new QLineEdit(dialog);
	f.savingsCheck   = new QCheckBox("Épargne", dialog);

	QIntValidator *validator = new QIntValidator(0, 100000000, dialog);
	f.amountInput->setValidator(validator);
	f.maxAmountInput->setValidator(validator);
	f.goalInput->setValidator(validator);

	layout->addRow("Nom (FR) / フランス語名", f.nameFrInput);
	layout->addRow("Nom (JP) / 日本語名", f.nameJpInput);
	layout->addRow("Montant initial / 初期金額", f.amountInput);
	layout->addRow("Plafond / 上限額", f.maxAmountInput);
	layout->addRow("Objectif mensuel / 月間目標", f.goalInput);
	layout->addRow(f.savingsCheck);

	auto updateCombinedName = [&f]()
	{
		f.nameInput->setText(f.nameFrInput->text() + "\n" + f.nameJpInput->text());
	};
	connect(f.nameFrInput, &QLineEdit::textChanged, updateCombinedName);
	connect(f.nameJpInput, &QLineEdit::textChanged, updateCombinedName);
}

void EnveloppesUi::addDialogButtons(QDialog *dialog, QFormLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
	layout->addWidget(buttons);

	connect(buttons, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
}

void EnveloppesUi::handleEnveloppeSubmission(const EnveloppeFormFields &f, std::string oldName)
{
	QString name      = f.nameInput->text();
	int     amount    = f.amountInput->text().toInt();
	int     maxAmount = f.maxAmountInput->text().toInt();
	int     goal      = f.goalInput->text().toInt();
	bool    savings   = f.savingsCheck->isChecked();

	if ( !oldName.empty() )
		g_enveloppeManager.modifyEnveloppe(oldName, name.toStdString(), amount, maxAmount, goal, savings);
	else
		g_enveloppeManager.addEnveloppe(name.toStdString(), amount, maxAmount, goal, savings);
}

void EnveloppesUi::deleteEnveloppe(Enveloppe env)
{
	if ( env.getAmount() != 0 )
	{
		showNonEmptyWarning();
		return;
	}

	if ( confirmDeletion() )
	{
		g_enveloppeManager.deleteEnveloppe(env.getName());
		emit updateNeeded();
	}
}

void EnveloppesUi::showNonEmptyWarning()
{
	QDialog *warn = new QDialog(this);
	warn->setWindowTitle("Suppression impossible / 削除できません");
	warn->setStyleSheet(dialogStyleSheet());

	QVBoxLayout *warnLayout = new QVBoxLayout(warn);
	warnLayout->addWidget(new QLabel("Impossible de supprimer cette enveloppe car son montant n'est pas nul.\nこの封筒にはまだ残高があるため、削除できません。"));

	QHBoxLayout *btnLayout = new QHBoxLayout;
	btnLayout->addStretch();
	QPushButton *okBtn = new QPushButton("OK");
	btnLayout->addWidget(okBtn);
	warnLayout->addLayout(btnLayout);

	connect(okBtn, &QPushButton::clicked, warn, &QDialog::accept);
	warn->exec();
}

bool EnveloppesUi::confirmDeletion()
{
	QDialog *confirm = new QDialog(this);
	confirm->setWindowTitle("Confirmation / 確認");
	confirm->setStyleSheet(dialogStyleSheet());

	QVBoxLayout *layout = new QVBoxLayout(confirm);
	layout->addWidget(new QLabel("Êtes-vous sûr de vouloir supprimer cette enveloppe ?\nこの封筒を削除してもよろしいですか？"));

	QHBoxLayout *btnLayout = new QHBoxLayout();
	btnLayout->addStretch();
	QPushButton *noBtn  = new QPushButton("Non / いいえ");
	QPushButton *yesBtn = new QPushButton("Oui / はい");
	btnLayout->addWidget(noBtn);
	btnLayout->addWidget(yesBtn);
	layout->addLayout(btnLayout);

	connect(noBtn, &QPushButton::clicked, confirm, &QDialog::reject);
	connect(yesBtn, &QPushButton::clicked, confirm, &QDialog::accept);

	return confirm->exec() == QDialog::Accepted;
}

QString EnveloppesUi::dialogStyleSheet()
{
	return R"(
	       QDialog {
	       background-color: #242F32;
	       color: #E1E1E2;
	       font-family: "Helvetica Neue";
       }
	       QPushButton {
	       background-color: #1B272A;
	       color: #E1E1E2;
	       border: none;
	       padding: 6px 12px;
	       border-radius: 4px;
	       min-width: 80px;
       }
	       QPushButton:hover {
	       background-color: #2F3D41;
       }
	       )";
}

void EnveloppesUi::clearEnveloppes()
{
	if ( layout() )
	{
		QLayoutItem *item;

		while ( (item = layout()->takeAt(0)) )
		{
			if ( QWidget *w = item->widget() )
				delete w;

			delete item;
		}

		delete layout();
	}
}
