#include "visualcalculator.h"
#include <QClipboard>
#include <ui_visualcalculator.h>
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
}

VisualCalculator::~VisualCalculator()
{
    delete ui;
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

    double result = calculateExpression(expression);

    // ��������� ����� � showResult
    ui->showResult->setText(QString::number(result));
}


void VisualCalculator::updateDisplayText(const QString& text)
{
    QString formattedText = text;

    // ��������� ����������� ����������� ������
    int openBracketsCount = formattedText.count("(");
    int closeBracketsCount = formattedText.count(")");
    if (openBracketsCount > closeBracketsCount) {
        formattedText.append(") ");
    }

    // ������� ����������� ������, ���� ���� ������ �����������, ��� ��������
    if (closeBracketsCount > openBracketsCount) {
        formattedText.chop(1);
    }

    // ��������� ������� ����� ������� � ����������, ���� ��� �����������
    QRegularExpression spaceRegex("(\\d)([+\\-*/])");
    formattedText.replace(spaceRegex, "\\1 \\2 ");

    // ��������� ������� ����� � ����� ������
    formattedText.replace(QRegularExpression("\\("), " ( ");
    formattedText.replace(QRegularExpression("\\)"), " ) ");

    // ��������� ���� ���������, ���� ����� ������ � ������� ���� ������
    QRegularExpression numberBracketRegex("(\\d) \\(");
    formattedText.replace(numberBracketRegex, "\\1 * (");
    QRegularExpression bracketNumberRegex("\\) (\\d)");
    formattedText.replace(bracketNumberRegex, ") * \\1");

    // ��������� ���� ���������, ���� ����� �������� ��� ����� ��������
    formattedText.replace(QRegularExpression("\\)\\s*\\("), ") * (");

    // ������� ������ ������� ����� �������
    QRegularExpression numberRegex("(\\d) (\\d)");
    formattedText.replace(numberRegex, "\\1\\2");

    // ��������� ������ ����� ��������, ������� ������� ����� ����������� ������
    formattedText.replace(QRegularExpression("\\)([+\\-*/])"), ") \\1\\");

    // ���������� �����, ���� ����� ���� ���� ������, �� ��� ��������
    formattedText.replace(QRegularExpression("(\\d) + (\\d)"), "\\1\\2");

    // ��������� ����� � textShow
    ui->textShow->setText(formattedText);
}

// ���� �������� ��������� � ����� ������
void VisualCalculator::copyExpressionToClipboard()
{
    QString expression = ui->showResult->text();

    QApplication::clipboard()->setText(expression);
}

// ���� ��� ��������� ������� ������� ������� Enter
void VisualCalculator::handleEnterPressed()
{
    calculateResult();
}
