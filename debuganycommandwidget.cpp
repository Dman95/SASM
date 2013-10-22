#include "debuganycommandwidget.h"

DebugAnyCommandWidget::DebugAnyCommandWidget(QWidget *parent) :
    QWidget(parent)
{
    currentCommandPos = -1;

    anyCommandLabel = new QLabel(tr("GDB command:"));
    command = new QLineEdit;
    performButton = new QPushButton(tr("Perform"));

    layout = new QHBoxLayout;
    layout->addWidget(anyCommandLabel);
    layout->addWidget(command);
    layout->addWidget(performButton);

    setLayout(layout);

    connect(performButton, SIGNAL(clicked()), this, SLOT(processCommand()));
}

void DebugAnyCommandWidget::showPreviousCommand()
{
    if (currentCommandPos > 0)
        currentCommandPos--;
    if (currentCommandPos >= 0 && currentCommandPos < commands.size())
        command->setText(commands[currentCommandPos]);
}

void DebugAnyCommandWidget::showNextCommand()
{
    if (currentCommandPos < commands.size() - 1)
        currentCommandPos++;
    if (currentCommandPos >= 0 && currentCommandPos < commands.size())
        command->setText(commands[currentCommandPos]);
}

void DebugAnyCommandWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        processCommand();
    if (event->key() == Qt::Key_Up)
        showPreviousCommand();
    if (event->key() == Qt::Key_Down)
        showNextCommand();
    QWidget::keyPressEvent(event);
}

void DebugAnyCommandWidget::processCommand()
{
    if (!command->text().isEmpty()) {
        commands.append(command->text());
        currentCommandPos = commands.size();
        emit performCommand(command->text());
        command->clear();
    } else {
        if (!commands.isEmpty())
            emit performCommand(commands.last());
    }
}

void DebugAnyCommandWidget::setFocusOnLineEdit()
{
    command->setFocus();
}

DebugAnyCommandWidget::~DebugAnyCommandWidget()
{
    delete anyCommandLabel;
    delete command;
    delete performButton;
    delete layout;
}
