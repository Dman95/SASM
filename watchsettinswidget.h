#ifndef WATCHSETTINSWIDGET_H
#define WATCHSETTINSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>

class WatchSettinsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WatchSettinsWidget(QWidget *parent = 0);
    ~WatchSettinsWidget();
    int sumSize();
    QComboBox *typeComboBox;
    QComboBox *sizeComboBox;
    QLineEdit *arraySizeEdit;
    QCheckBox *addressCheckbox;
    QSize sizeHint() const;

private:
    QHBoxLayout *layout;

signals:
    void settingsChanged(void);
};

#endif // WATCHSETTINSWIDGET_H
