#include "visualcalculator.h"
#include <QClipboard>
#include <ui_standartvisualcalculator.h>
#include <ui_integralvisualvalculator.h>

//you need to connect a third-party project with the CalculatorUtils namespace(https://github.com/YuarenArt/CalculatorUtils), 
//which defines functions for calculations and transformations that do not depend on the Visualcalculator class

#include "\QT Lib\CalculatorUtils\CalculatorUtils.cpp"
using namespace CalculatorUtils;

VisualCalculator::VisualCalculator(QWidget *parent) : QMainWindow(parent), ui(new Ui::standartVisualCalculator), ui_integral(nullptr)
{
    generateStandartInterface();
}

void VisualCalculator::generateStandartInterface()
{
    ui->setupUi(this);



    QMenu* menu = new QMenu(ui->menuChoice);

    // �������� �������� (������� ����)
    QAction* setDefault = new QAction("Default calculator", menu);
    QAction* setIntegral = new QAction("Numerical Integral Calculator", menu);

    // ���������� �������� � ����
    menu->addAction(setDefault);
    menu->addAction(setIntegral);
    // ��������� ���� ��� QToolButton
    ui->menuChoice->setMenu(menu);
    // ��������� ����� ������������ ����
    ui->menuChoice->setPopupMode(QToolButton::InstantPopup);

    connect(setDefault, &QAction::triggered, this, &VisualCalculator::loadStandardInterface);
    connect(setIntegral, &QAction::triggered, this, &VisualCalculator::updateInterfaceIntegral);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &VisualCalculator::updateDisplayText);
    connect(ui->equalBtn, &QPushButton::clicked, this, &VisualCalculator::calculateResult);
    connect(ui->copyBtn, &QPushButton::clicked, this, &VisualCalculator::copyExpressionToClipboard);
    connect(ui->clearBtn, &QPushButton::clicked, this, &VisualCalculator::clearExpression);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &VisualCalculator::handleEnterPressed);
    connect(ui->historyList, &QListWidget::itemDoubleClicked, this, &VisualCalculator::historyListDoubleClicked);
}

VisualCalculator::~VisualCalculator()
{
    delete ui;
}

// ������� ��������� ������� ��������� � ������ 
void VisualCalculator::updateHistoryList(const QString& text)
{
    ui->historyList->addItem(text);
}

// ������� ��������� ���������
void VisualCalculator::clearExpression()
{
    ui->lineEdit->clear();
    calculateResult();
}

//
void VisualCalculator::calculateResult()
{
    QString expression = ui->textShow->toPlainText();

    // ��������� ������ ����
    if (expression.isEmpty())
    {
        // ������� ���������
        ui->showResult->setText(" ");
        return;
    }
    // ��������� ���������
    double result = calculateExpression(expression);

    updateHistoryList(expression);

    // ��������� ����� � showResult
    ui->showResult->setText(QString::number(result));
}

// ������� ��������� ������������� ���������
void VisualCalculator::updateDisplayText(const QString& text)
{
    QString formattedText = text;

    // ��������� ����������� ����������� ������
    autoBalanceParentheses(formattedText);

    // ��������� ������� ����� ������� � ����������, ���� ��� �����������
    QRegularExpression spaceRegex("(\\d)([+\\-*/^sqrtloglnlgsincostancottgctg])");
    formattedText.replace(spaceRegex, "\\1 \\2 ");

    // ��������� ������� ����� � ����� ������
    formattedText.replace(QRegularExpression("\\("), " ( ");
    formattedText.replace(QRegularExpression("\\)"), " ) ");

    // ��������� ���� ���������, ���� ����� ������ � ������� ���� ������
    QRegularExpression numberBracketRegex("(\\d) *\\(");
    formattedText.replace(numberBracketRegex, "\\1 * (");
    QRegularExpression bracketNumberRegex("\\) (\\d)");
    formattedText.replace(bracketNumberRegex, ") * \\1");

    // ��������� ���� ���������, ���� ����� �������� ��� ����� ��������
    formattedText.replace(QRegularExpression("\\)\\s*\\("), ") * (");

    // ������� ������ ������� ����� �������
    QRegularExpression numberRegex("(\\d+)\\s*(\\d+)");
    formattedText.replace(numberRegex, "\\1\\2");

    // �������� ����� ���������� � �������
    formattedText.replace(QRegularExpression("([+\\-*/sqrtloglnsincostanlgcottgctg])(\\d+)"), "\\1 \\2");
    formattedText.replace(QRegularExpression("(\\d+)\\s+([+\\-*/sqrtloglnsinlgcostancottgctg])"), "\\1 \\2");
    formattedText.replace(QRegularExpression("([+\\-*/]sqrtloglnsincostancottgctglg)\\s*(\\d+)"), "\\1 \\2");

    formattedText.replace(QRegularExpression("([cossintgtancthcotloglnsqrtlg^])\\s+(\\d+)"), "\\1 ( \\2 )");

    // ������ ����� �� ������ 0.xxx, ���� ����� ���������� � ���� (021 = 0.21)
    formattedText.replace(QRegularExpression("(0)(\\d+)"), "\\1.\\2");

    // ������ ',' ��� ����� ����� �� '.' ��� ���������� ����������
    formattedText.replace(QRegularExpression(",") , ".");

    // ��������� ����� � textShow
    ui->textShow->setText(formattedText);
}

// ���� �������� ��������� � ����� ������
void VisualCalculator::copyExpressionToClipboard()
{
    QString expression = ui->showResult->text();

    QApplication::clipboard()->setText(expression);
}

// ���� ��� ��������� ������� ������� ������� Enter � lineEdit
void VisualCalculator::handleEnterPressed()
{
    calculateResult();
}

// ��� ������ ������ �� ����� ��������� � ����� �������, ��������� ��������� ��������� � lineEdit
void VisualCalculator::historyListDoubleClicked()
{
    QListWidgetItem* item = ui->historyList->currentItem();
    ui->lineEdit->setText(item->text());
}

// �������������� ��������� ���������� ��� ������� ��������� �������� ����������
void VisualCalculator::updateInterfaceIntegral()
{

    ui_integral = new Ui_integral::integralVisualCalculator;
    QWidget* integralWidget = new QWidget(this);
    ui_integral->setupUi(integralWidget);

    setCentralWidget(integralWidget);

}

// ������ ����������� �������� - ������� �����������
void VisualCalculator::loadStandardInterface()
{
    QSize windowSize = size();
    generateStandartInterface();
    resize(windowSize);

}

