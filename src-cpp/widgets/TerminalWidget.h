//
// Created by vasil on 02/11/2020.
//

#ifndef AVIONICS_PC_TOOLS_GUI_TERMINALWIDGET_H
#define AVIONICS_PC_TOOLS_GUI_TERMINALWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QSet>


class MyTextEdit: public QTextEdit
{
    Q_OBJECT
public:
    MyTextEdit(QWidget * parent = nullptr);
    void mousePressEvent(QMouseEvent *event) override;
    void append(const QString & text);
    void setBold(bool enable);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;

    void wheelEvent(QWheelEvent *e) override;

Q_SIGNALS:
    void onTextSelectionChanged(int position = -1);
    void onTextSelectionBegin(int position = -1);
    void onTextSelectionEnd(int position = -1);
    void onScrolledUp();
    void onScrolledDown();
private:
    bool isSelectionEnabled = false;
    QString mRegularStyle;
    QString mBoldStyle;
};

class TerminalWidget : public QWidget
{
    Q_OBJECT
public:
    TerminalWidget(QWidget * parent = nullptr);
    bool eventFilter(QObject *receiver, QEvent *event) override;
Q_SIGNALS:
    void onRequest(const QString & command);
    void onBeginResponse(const QString & data, bool isError = false);
    void onBeginResponse(QByteArray data, bool isError = false);
    void onEndResponse(bool insertNewLine = true);
    void resetRemoteTargetCommand ();
private:
    Q_SIGNAL void onEnterPressed(QKeyEvent * event);
    Q_SIGNAL void onCommandInput(QKeyEvent * event);
    void onZoomInClicked();
    void onZoomOutCLicked();
private:
    static bool isKnownCommand(const QString & command);
    void handleKnownCommands(const QString & command);
    void handleDownKeyEvent();
    void handleUpKeyEvent();
    bool handleLeftKeyEvent(QEvent *event);
    bool handleRightKeyEvent(QEvent *event);
    bool handleBackspaceKeyEvent(QEvent *event);
    void copyCurrentSelectionToTheClipBoard();
    bool handleKeyPressEvent(QEvent *event);
    void goToNextLine();

    int mCommandHistoryIndex;
    MyTextEdit * mConsoleText;
    QString mUser;
    QString mCommandDelimiter;
    QList<QString> mCommandHistory;
    QTextEdit::ExtraSelection mCustomSelection;
    QPair<int, int> mCustomSelectionIndices;
    QSet<int> mPressedKeys;
};


#endif //UNTITLED1_TERMINALWIDGET_H
