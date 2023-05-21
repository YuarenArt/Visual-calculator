#include "visualcalculator.h"
#include <QClipboard>
#include <ui_visualcalculator.h>

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

bool isValidInput(const QString& input)
{
    QRegularExpression validRegex("^[0-9()+\\-*/. ]+$");
    QRegularExpressionMatch match = validRegex.match(input);
    return match.hasMatch();
}

bool isOperator(const QString& token)
{
    return token == "+" || token == "-" || token == "*" || token == "/";
}
// ��������� ����������� ����������� ������
void autoBalanceParentheses(QString& expression)
{
    QStack<QChar> parenthesesStack;

    for (int i = 0; i < expression.length(); ++i) {
        QChar ch = expression.at(i);

        if (ch == '(') {
            // ����������� ������, ��������� �� � ����
            parenthesesStack.push(ch);
        }
        else if (ch == ')') {
            // ����������� ������

            if (parenthesesStack.isEmpty()) {
                // ��� ��������������� ����������� ������, ������� ����������� ������
                expression.remove(i, 1);
                --i;
            }
            else {
                // ���� ��������������� ����������� ������, ������� �� �� �����
                parenthesesStack.pop();
            }
        }
    }

    // ��������� ����������� ����������� ������
    while (!parenthesesStack.isEmpty()) {
        expression.append(')');
        parenthesesStack.pop();
    }
}

// ������������ ��������� � ����� ���������
double performOperation(double operand1, double operand2, const QString& operation)
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
        if (operand2 == 0) {
            QMessageBox::warning(nullptr, "Warning", "You divide by zero");
        }
        return operand1 / operand2;
    }



    else {
        // ���������������� ��������
        return std::numeric_limits<double>::quiet_NaN();
    }
}

// ������� ������� ������ � �������� �������� �������
QStringList convertToRPN(const QString& expression) {

    QStringList outputQueue;  // ������� ��� �������� �������� �������� ������
    QStack<QString> operatorStack;  // ���� ��� ����������

    QStringList tokens = expression.split(' ', Qt::SkipEmptyParts);

    for (const QString& token : tokens) {
        if (!isOperator(token) && token != "(" && token != ")") {
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
        else if (token == "(") {
            // ������� ����� - ����������� ������, ��������� �� � ���� ����������
            operatorStack.push(token);
        }
        else if (token == ")") {
            // ������� ����� - ����������� ������

            // ���������� ��������� �� ����� � �������� ������� �� ��� ���,
            // ���� �� �������� ��������������� ����������� ������ ��� ���� �� ��������
            while (!operatorStack.isEmpty() && operatorStack.top() != "(") {
                outputQueue.append(operatorStack.pop());
            }

            // ���� ���� ������� � �� ���� ������� ����������� ������, ������ � ��������� ������ �� ��������������
            if (operatorStack.isEmpty()) {
                QMessageBox::warning(nullptr, "Warning", "Unbalanced parentheses");
                return QStringList();  // ���������� ������ ������, ����� ���������� ������
            }

            // ������� ����������� ������ �� �����
            operatorStack.pop();
        }
        else {
            // ������� ����� - ��������

            while (!operatorStack.isEmpty() && operatorStack.top() != "(" &&
                getOperatorPrecedence(token) <= getOperatorPrecedence(operatorStack.top())) {
                // ������� �������� ����� ������� ��� ������ ���������, ��� �������� �� ������� �����
                // ���������� �������� � ������� ����� � �������� �������
                outputQueue.append(operatorStack.pop());
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

double calculateExpression(const QString& expression)
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

            // ���� ����� ������ ������� ��� ����� ���������� 0
            if (numbersStack.isEmpty()) return 0.0;

            double operand2 = numbersStack.pop();
            double operand1 = numbersStack.pop();
            double result = performOperation(operand1, operand2, token);
            numbersStack.push(result);
        }
    }

    return numbersStack.pop();
}

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
