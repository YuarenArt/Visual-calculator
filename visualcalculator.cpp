#include "visualcalculator.h"
#include <QStack>

int getOperatorPrecedence(const QString& op) {
    if (op == "+" || op == "-") {
        return 1;
    }
    else if (op == "*" || op == "/") {
        return 2;
    }
    else {
        // ����������� ��������, ����� ������� 3 ��� ������ �������� �� ���������
        return 3;
    }
}

VisualCalculator::VisualCalculator(QWidget *parent) : QMainWindow(parent)
{

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    //�������� ������������ ������ ���� �� �����������
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);


    QHBoxLayout* inputLayout = new QHBoxLayout;
    mainLayout->addLayout(inputLayout);

    // �������� ���� �����
    lineEdit = new QLineEdit(centralWidget);
    inputLayout->addWidget(lineEdit);


    // �������� ������ "="
    equalBtn = new QPushButton("=");
    inputLayout->addWidget(equalBtn);

    showResult = new QLineEdit(centralWidget);
    showResult->setReadOnly(true);
    mainLayout->addWidget(showResult);

    // ���������� ��� ���� ������� �������������
    textShow = new QTextEdit(centralWidget);
    textShow->setReadOnly(true);
    mainLayout->addWidget(textShow);

    // ��������� ������� ������
    QFont font = textShow->font();
    font.setPointSize(font.pointSize() + 12);
    textShow->setFont(font);

    connect(lineEdit, &QLineEdit::textChanged, this, &VisualCalculator::updateTextShow);
    connect(equalBtn, &QPushButton::clicked, this, &VisualCalculator::calculate);
}

bool VisualCalculator::isValidInput(const QString& input)
{
    QRegularExpression validRegex("^[0-9()+\\-*/. ]+$");
    QRegularExpressionMatch match = validRegex.match(input);
    return match.hasMatch();
}

bool VisualCalculator::isOperator(const QString& token)
{
    return token == "+" || token == "-" || token == "*" || token == "/";
}

void VisualCalculator::calculate()
{
    QString expression = textShow->toPlainText();

    // ��������� ������ ����
    if (expression.isEmpty())
    {
        // ������� ���������
        showResult->setText(" ");
        return;
    }

    double result = calculateExpression(expression);

    // ��������� ����� � showResult
    showResult->setText(QString::number(result));
}

double VisualCalculator::calculateExpression(const QString& expression)
{
    // �������� ������ ������� (����� � ���������)
    QStringList tokens = convertToRPN(expression);

    if (tokens.isEmpty())
    {
        return 0;
    }

    // ��������� ��� �����
    QStack<double> numbersStack;

    for (const QString& token : tokens) {
        if (!isOperator(token)) {
            // ������� ����� - �����
            numbersStack.push(token.toDouble());
        }
        else {
            // ������� ����� - ��������
            double operand2 = numbersStack.pop();
            double operand1 = numbersStack.pop();
            double result = performOperation(operand1, operand2, token);
            numbersStack.push(result);
        }
    }

    return numbersStack.pop();
}

QStringList VisualCalculator::convertToRPN(const QString& expression) {

    QStringList outputQueue;  // ������� ��� �������� �������� �������� ������
    QStack<QString> operatorStack;  // ���� ��� ����������

    QStringList tokens = expression.split(' ', Qt::SkipEmptyParts);

    for (const QString& token : tokens) {
        if (!isOperator(token)) {
            // ���������, �������� �� ����� ������
            bool isNumber;
            token.toDouble(&isNumber);
            if (isNumber) {
                // ������� ����� - �����, ��������� ��� � �������� �������
                outputQueue.append(token);
            }
            else {
                // ��������� ������ - ����� �� �������� ������
                QMessageBox::warning(nullptr, "Warning", "Unknown operand");
            }
        }
        else {
            // ������� ����� - ��������
            while (!operatorStack.isEmpty() && isOperator(operatorStack.top())) {

                QString topOperator = operatorStack.top();
                if (getOperatorPrecedence(token) <= getOperatorPrecedence(topOperator)) {
                    // ������� �������� ����� ������� ��� ������ ���������, ��� �������� �� ������� �����
                    // ���������� �������� � ������� ����� � �������� �������
                    outputQueue.append(operatorStack.pop());
                }
                else {
                    break;
                }
            }
            // ��������� ������� �������� � ����
            operatorStack.push(token);
        }
    }

    // ���������� ���������� ��������� �� ����� � �������� �������
    while (!operatorStack.isEmpty()) {
        outputQueue.append(operatorStack.pop());
    }

    return outputQueue;
}

double VisualCalculator::performOperation(double operand1, double operand2, const QString& operation)
{
    if (operation == "+") {
        return operand1 + operand2;
    }
    else if (operation == "-") {
        return operand1 - operand2;
    }
    else if (operation == "*") {
        return operand1 * operand2;
    }
    else if (operation == "/") {
        if (operand2 == 0)   {
            QMessageBox::warning(nullptr, "Warning", "You divide by zero");
        }
        return operand1 / operand2;
    }
    else {
        // ���������������� ��������
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void VisualCalculator::updateTextShow(const QString& text)
{
    QString formattedText = text;

    // ��������� ������� ����� ������� � ����������, ���� ��� �����������
    QRegularExpression spaceRegex("(\\d)([+\\-*/])");
    formattedText.replace(spaceRegex, "\\1 \\2 ");
    formattedText.replace("(", "( ");
    formattedText.replace(")", " )");

   

    // ��������� ����� � textShow
    textShow->setText(formattedText);
}

