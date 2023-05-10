#include "visualcalculator.h"

VisualCalculator::VisualCalculator(QWidget *parent) : QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    //�������� ������������ ������ ���� �� �����������
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    textShow = new QTextEdit(centralWidget);
    textShow->setReadOnly(true);
    mainLayout->addWidget(textShow);

    // ��������� ������� ������
    QFont font = textShow->font();
    font.setPointSize(font.pointSize() + 6);
    textShow->setFont(font);

    QHBoxLayout* inputLayout = new QHBoxLayout;
    mainLayout->addLayout(inputLayout);

    // �������� ���� �����
    lineEdit = new QLineEdit(centralWidget);
    inputLayout->addWidget(lineEdit);


    // �������� ������ "="
    QPushButton* equalsButton = new QPushButton("=");
    inputLayout->addWidget(equalsButton);

    QLabel* rez = new QLabel;
    mainLayout->addWidget(rez);

    connect(lineEdit, &QLineEdit::textChanged, this, &VisualCalculator::updateTextShow);
}

bool VisualCalculator::isValidInput(const QString& input)
{
    QRegularExpression validRegex("^[0-9()+\\-*/. ]+$");
    QRegularExpressionMatch match = validRegex.match(input);
    return match.hasMatch();
}


void VisualCalculator::updateTextShow(const QString& text)
{
    QString formattedText = text;

    if (!isValidInput(text))
    {
        QMessageBox::information(this, "Error", "Invalid input!");
    }

    // ��������� ������� ������� '/' � ������
    QRegularExpression divisionRegex("\\/");
    QRegularExpressionMatch divisionMatch = divisionRegex.match(formattedText);
    if (divisionMatch.hasMatch())
    {
        // ����������� ������� � ���� �����
        formattedText.replace(divisionMatch.capturedStart(), divisionMatch.capturedLength(), " / ");

        // ���������, ���� �� ����� � �������
        QRegularExpression bracketRegex("\\((\\d+)\\)");
        QRegularExpressionMatchIterator bracketIterator = bracketRegex.globalMatch(formattedText);
        while (bracketIterator.hasNext())
        {
            QRegularExpressionMatch bracketMatch = bracketIterator.next();
            QString numberInBrackets = bracketMatch.captured(1);
            formattedText.replace(bracketMatch.capturedStart(), bracketMatch.capturedLength(), numberInBrackets);
        }

        // ������������� ����� ��� �����
        formattedText = "<sup>" + formattedText + "</sup>";
    }

    // ��������� ����� � textShow
    textShow->setText(formattedText);
}

