#ifndef AONELINETEXTEDIT_H
#define AONELINETEXTEDIT_H

#include <QPlainTextEdit>
#include <QSize>

class AOneLineTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    AOneLineTextEdit(QWidget * parent = nullptr);

    void setText(const QString & text);
    QString text() const;

protected:
    QSize sizeHint() const override;

    void keyPressEvent(QKeyEvent * event) override;
    void focusOutEvent(QFocusEvent * event) override;

signals:
    void editingFinished();
};

/*
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>
class QTextDocument;
struct AHighlightingRule
{
    QRegExp pattern;
    QTextCharFormat format;
};

class AShapeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    AShapeHighlighter(QTextDocument * parent = nullptr);

    QVector<AHighlightingRule> HighlightingRules;

protected:
    void highlightBlock(const QString & text) override;
};
*/

#endif // AONELINETEXTEDIT_H
