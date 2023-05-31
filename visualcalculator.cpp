#include "visualcalculator.h"
#include <QClipboard>
#include <ui_visualcalculator.h>

//you need to connect a third-party project with the CalculatorUtils namespace(https://github.com/YuarenArt/CalculatorUtils), 
//which defines functions for calculations and transformations that do not depend on the Visualcalculator class

#include "\QT Lib\CalculatorUtils\CalculatorUtils.cpp"
using namespace CalculatorUtils;

VisualCalculator::VisualCalculator(QWidget *parent) : QMainWindow(parent), ui(new Ui::VisualCalculator)
{
    ui->setupUi(this);

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

void VisualCalculator::updateHistoryList(const QString& text)
{
    ui->historyList->addItem(text);
}

void VisualCalculator::clearExpression()
{
    ui->lineEdit->clear();
    calculateResult();
}

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
    QRegularExpression spaceRegex("(\\d)([+\\-*/^sqrtloglnsincostancottgctg])");
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
    formattedText.replace(QRegularExpression("([+\\-*/^sqrtloglnsincostancottgctg])(\\d+)"), "\\1 \\2");
    formattedText.replace(QRegularExpression("(\\d+)\\s+([+\\-*/^sqrtloglnsincostancottgctg])"), "\\1 \\2");
    formattedText.replace(QRegularExpression("([+\\-*/]^sqrtloglnsincostancottgctg)\\s*(\\d+)"), "\\1 \\2");


    formattedText.replace(QRegularExpression(",") , ".");

    /*formattedText.replace(QRegularExpression("(cos)\\s+(\\d)"), "\\1\\2");
    formattedText.replace(QRegularExpression("(sqrt)\\s+(\\d)"), "\\1\\2");*/

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

void VisualCalculator::historyListDoubleClicked()
{
    QListWidgetItem* item = ui->historyList->currentItem();
    ui->lineEdit->setText(item->text());
}
